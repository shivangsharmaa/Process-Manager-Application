#include "processmanager.h"
#include <QTableWidget>
#include <QHeaderView>
#include <sstream>
#include <string>
#include <QInputDialog>
#include <QDebug>
#include <dirent.h>
#include <sys/types.h>
#include <cstdlib>  // For system calls
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cctype>  // For isdigit()
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QMap>
#include <QVector>
#include <QDir>
#include <QProcess>
#include <QStringList>


// Constructor for ProcessManager
ProcessManager::ProcessManager() {
    // Constructor implementation (if needed)
}

// Renice process

void ProcessManager::reniceProcess(int pid, int priority) {
    // Validate priority range (optional, but recommended)
    if (priority < -20 || priority > 19) {
        qDebug() << "Invalid priority value:" << priority;
        return;
    }

    // Create the renice command
    QString command = QString("renice -n %1 -p %2").arg(priority).arg(pid);

    // Execute the command
    int result = system(command.toStdString().c_str());

    // Check if the command was successful
    if (result == 0) {
        qDebug() << "Successfully changed priority of process with PID:" << pid << "to" << priority;
    } else {
        qDebug() << "Failed to change priority of process with PID:" << pid;
    }
}


// Add process information
void ProcessManager::addProcess(const ProcessInfo &process) {
    processes.push_back(process);
}

void ProcessManager::populateTable(QTableWidget *table) {
    qDebug() << "Number of processes: " << processes.size(); // Debug statement

    table->setRowCount(processes.size());
    table->setColumnCount(3); // Example column count

    // Set column headers
    table->setHorizontalHeaderLabels({"Process ID", "Process Name", "Priority"});

    // Populate the table with process data
    for (std::vector<ProcessInfo>::size_type i = 0; i < processes.size(); ++i) {
        const ProcessInfo &process = processes[i];
        //qDebug() << "Process ID: " << process.pid << ", Name: " << QString::fromStdString(process.name) << ", Priority: " << process.priority; // Debug statement
        table->setItem(i, 0, new QTableWidgetItem(QString::number(process.pid)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(process.name)));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(process.priority)));
    }

    // Stretch last section to fill available space
    QHeaderView *header = table->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
    table->horizontalHeader()->setStretchLastSection(true);
}

// Kill process
void ProcessManager::killProcess(int pid) {
    // Example implementation using a system call (platform dependent)
    QString command = QString("kill %1").arg(pid);
    system(command.toStdString().c_str());
    qDebug() << "Killed process with PID:" << pid;
}

void ProcessManager::listProcesses() {
    std::string procDir = "/proc";
    DIR* dir = opendir(procDir.c_str());

    if (dir == nullptr) {
        std::cerr << "Error opening /proc directory" << std::endl;
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            std::string dirName(entry->d_name);
            if (isdigit(dirName[0])) {
                int pid = std::stoi(dirName);

                // Read process name
                std::string commFile = procDir + "/" + dirName + "/comm";
                std::ifstream file(commFile);
                std::string processName;
                if (file.is_open()) {
                    getline(file, processName);
                    file.close();

                    // Create ProcessInfo and add it to the manager
                    ProcessInfo processInfo;
                    processInfo.pid = pid;
                    processInfo.name = processName;
                    processInfo.priority = 0; // Default priority, or set accordingly
                    processes.push_back(processInfo);
                }
            }
        }
    }

    closedir(dir);
}

double ProcessManager::getCpuUsage() {
    std::ifstream file("/proc/stat");
    std::string line;

    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string cpu;
        long user, nice, system, idle;
        iss >> cpu >> user >> nice >> system >> idle;
        long total = user + nice + system + idle;

        static long prevTotal = total, prevIdle = idle;

        long deltaTotal = total - prevTotal;
        long deltaIdle = idle - prevIdle;

        prevTotal = total;
        prevIdle = idle;

        return (100.0 * (deltaTotal - deltaIdle) / deltaTotal);
    }

    return 0.0;  // In case of error
}


void ProcessManager::createProcessTree(QGraphicsScene *scene, const std::vector<ProcessInfo> &processes) {
    QMap<int, QGraphicsEllipseItem*> processNodes;

    // Create nodes for each process
    for (const auto& process : processes) {
        QGraphicsEllipseItem *node = new QGraphicsEllipseItem();
        node->setRect(0, 0, 100, 50); // Size of the node
        node->setBrush(Qt::blue);
        node->setToolTip(QString::fromStdString(process.name));
        scene->addItem(node);
        processNodes[process.pid] = node;
    }

    // Create edges between parent and child processes
    for (const auto& process : processes) {
        if (process.parentPid != -1) { // Assume -1 means no parent
            if (processNodes.contains(process.parentPid) && processNodes.contains(process.pid)) {
                QGraphicsLineItem *line = new QGraphicsLineItem();
                line->setLine(
                    processNodes[process.parentPid]->x() + 50, processNodes[process.parentPid]->y() + 25, // Parent center
                    processNodes[process.pid]->x() + 50, processNodes[process.pid]->y() + 25              // Child center
                );
                line->setPen(QPen(Qt::black, 2));
                scene->addItem(line);
            }
        }
    }

    scene->setSceneRect(scene->itemsBoundingRect());
}

QStringList ProcessManager::getSubprocesses(int parentPid) {
    QStringList subprocesses;

    // Prepare the command to execute
    QString command = QString("ps --ppid %1 -o pid=").arg(parentPid); // Get PIDs of subprocesses with parent PID

    QProcess process;
    process.start(command);
    process.waitForFinished(); // Wait for the process to finish

    // Read the output of the command
    QString output = process.readAllStandardOutput();
    QStringList outputLines = output.split('\n', QString::SkipEmptyParts); // Split output into lines

    for (const QString &line : outputLines) {
        if (!line.trimmed().isEmpty()) {
            subprocesses << line.trimmed(); // Add each PID to the list
        }
    }

    return subprocesses; // Return the list of subprocess IDs
}

QString ProcessManager::getCommandForPid(int pid) {
    QString cmdFilePath = QString("/proc/%1/cmdline").arg(pid);
    QFile cmdFile(cmdFilePath);
    if (cmdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString command = cmdFile.readAll();
        cmdFile.close();
        // Replace null characters with spaces for better readability
        return command.replace('\0', ' ').trimmed();
    }
    return "Unknown"; // Return "Unknown" if unable to read
}





