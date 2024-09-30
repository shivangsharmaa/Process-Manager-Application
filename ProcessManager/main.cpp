#include <QApplication>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QChart>
#include <QTimer>
#include <QLabel>
#include <QDebug>
#include <fstream>
#include <sstream>
#include <QTextStream>
#include <QHeaderView>
#include "processmanager.h"

using namespace QtCharts;  // Use the QtCharts namespace

void displaySubprocesses(const QStringList &subprocesses, int parentPid, ProcessManager &pm) {
    // Create a dialog to show the subprocesses
    QDialog *dialog = new QDialog();
    dialog->setWindowTitle(QString("Subprocesses for PID %1").arg(parentPid));

    QVBoxLayout *dialogLayout = new QVBoxLayout(dialog);

    // Add a label for the title
    QLabel *titleLabel = new QLabel(QString("Subprocesses for PID %1").arg(parentPid));
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: green;");
    dialogLayout->addWidget(titleLabel);

    // Create a table to display the subprocesses
    QTableWidget *subprocessTable = new QTableWidget(subprocesses.size(), 2);
    subprocessTable->setHorizontalHeaderLabels(QStringList() << "PID" << "Command");

    // Populate the table with subprocesses and their commands
    for (int i = 0; i < subprocesses.size(); ++i) {
        QString pid = subprocesses[i];
        subprocessTable->setItem(i, 0, new QTableWidgetItem(pid));

        // Fetch the command for each PID
        QString command = pm.getCommandForPid(pid.toInt()); // Convert QString to int for the function
        subprocessTable->setItem(i, 1, new QTableWidgetItem(command));
    }

    // Allow the table to stretch columns equally
    subprocessTable->horizontalHeader()->setStretchLastSection(true);
    subprocessTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    subprocessTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    dialogLayout->addWidget(subprocessTable);

    // Set the layout for the dialog and show it
    dialog->setLayout(dialogLayout);
    dialog->exec();  // Show the dialog
}



int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create the main window widget
    QWidget window;

    // Create a horizontal layout for the main window
    QHBoxLayout *mainLayout = new QHBoxLayout(&window);

    // Create a vertical layout for the process table on the left
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // Create a table widget to display processes
    QTableWidget *table = new QTableWidget();
    leftLayout->addWidget(table);

    // Instantiate the process manager
    ProcessManager pm;

    // Populate the processes and table
    auto updateTable = [&]() {
        pm.processes.clear();
        pm.listProcesses();
        pm.populateTable(table);
    };

    updateTable();

    // Add buttons
    
    QLineEdit *pidInputField = new QLineEdit();
    pidInputField->setPlaceholderText("Enter PID to view subprocesses");
    leftLayout->addWidget(pidInputField);

    // Create a button to view subprocesses
    QPushButton *viewSubprocessButton = new QPushButton("View Subprocesses");
    leftLayout->addWidget(viewSubprocessButton);

    
    QPushButton *killButton = new QPushButton("Kill Process");
    leftLayout->addWidget(killButton);

    QPushButton *reniceButton = new QPushButton("Renice Process");
    leftLayout->addWidget(reniceButton);

    QLineEdit *pidInput = new QLineEdit();
    pidInput->setPlaceholderText("Enter PID");
    leftLayout->addWidget(pidInput);

    QPushButton *searchButton = new QPushButton("Find Process");
    leftLayout->addWidget(searchButton);

    QPushButton *refreshButton = new QPushButton("Refresh");
    leftLayout->addWidget(refreshButton);

    mainLayout->addLayout(leftLayout);

    // Create a widget to hold the CPU and memory charts
    QWidget *chartsWidget = new QWidget();
    QVBoxLayout *chartsLayout = new QVBoxLayout(chartsWidget);

    // Create CPU usage graph using QtCharts
    QChart *cpuChart = new QChart();
    cpuChart->setTitle("CPU Usage Over Time");
    cpuChart->setBackgroundBrush(QBrush(QColor(30, 30, 30))); // Dark background
    cpuChart->setBackgroundPen(QPen(Qt::NoPen)); // No border

    QLineSeries *cpuSeries = new QLineSeries();
    cpuSeries->setColor(QColor(0, 255, 0)); // Green line for CPU usage
    cpuSeries->setPen(QPen(QBrush(Qt::green), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    cpuChart->addSeries(cpuSeries);

    QChartView *cpuChartView = new QChartView(cpuChart);
    cpuChartView->setRenderHint(QPainter::Antialiasing);  // Smoother rendering
    chartsLayout->addWidget(cpuChartView);

    QValueAxis *cpuAxisX = new QValueAxis();
    cpuAxisX->setTitleText("Time (seconds)");
    cpuAxisX->setLabelFormat("%d");
    cpuAxisX->setGridLineVisible(true);
    cpuAxisX->setGridLineColor(Qt::gray);
    cpuAxisX->setLabelsBrush(QBrush(Qt::white));  // White text on dark background

    cpuChart->addAxis(cpuAxisX, Qt::AlignBottom);
    cpuSeries->attachAxis(cpuAxisX);

    QValueAxis *cpuAxisY = new QValueAxis();
    cpuAxisY->setRange(0, 100);
    cpuAxisY->setTitleText("CPU Usage (%)");
    cpuAxisY->setLabelFormat("%d%%");
    cpuAxisY->setGridLineVisible(true);
    cpuAxisY->setGridLineColor(Qt::gray);
    cpuAxisY->setLabelsBrush(QBrush(Qt::white));  // White text on dark background

    cpuChart->addAxis(cpuAxisY, Qt::AlignLeft);
    cpuSeries->attachAxis(cpuAxisY);

    // Create Memory usage graph using QtCharts
    QChart *memoryChart = new QChart();
    memoryChart->setTitle("Memory Usage Over Time");
    memoryChart->setBackgroundBrush(QBrush(QColor(30, 30, 30))); // Dark background
    memoryChart->setBackgroundPen(QPen(Qt::NoPen)); // No border

    QLineSeries *memorySeries = new QLineSeries();
    memorySeries->setColor(QColor(0, 0, 255)); // Blue line for memory usage
    memorySeries->setPen(QPen(QBrush(Qt::blue), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    memoryChart->addSeries(memorySeries);

    QChartView *memoryChartView = new QChartView(memoryChart);
    memoryChartView->setRenderHint(QPainter::Antialiasing);  // Smoother rendering
    chartsLayout->addWidget(memoryChartView);

    QValueAxis *memoryAxisX = new QValueAxis();
    memoryAxisX->setTitleText("Time (seconds)");
    memoryAxisX->setLabelFormat("%d");
    memoryAxisX->setGridLineVisible(true);
    memoryAxisX->setGridLineColor(Qt::gray);
    memoryAxisX->setLabelsBrush(QBrush(Qt::white));  // White text on dark background

    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    memorySeries->attachAxis(memoryAxisX);

    QValueAxis *memoryAxisY = new QValueAxis();
    memoryAxisY->setRange(0, 10000); // Adjust range based on expected memory usage (e.g., 10000 MB)
    memoryAxisY->setTitleText("Memory Usage (MB)");
    memoryAxisY->setLabelFormat("%d MB");
    memoryAxisY->setGridLineVisible(true);
    memoryAxisY->setGridLineColor(Qt::gray);
    memoryAxisY->setLabelsBrush(QBrush(Qt::white));  // White text on dark background

    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);
    memorySeries->attachAxis(memoryAxisY);

    // Add the charts widget to the main layout
    mainLayout->addWidget(chartsWidget);

    // Function to get CPU usage
    auto getCpuUsage = [&]() -> float {
        std::ifstream file("/proc/stat");
        std::string line;
        if (file.is_open() && std::getline(file, line)) {
            std::istringstream ss(line);
            std::string cpu;
            long user, nice, system, idle;
            ss >> cpu >> user >> nice >> system >> idle;
            long totalTime = user + nice + system + idle;
            long busyTime = user + nice + system;
            file.close();
            return (float)busyTime / totalTime * 100.0f;
        }
        return 0.0f;
    };

    // Function to get Memory usage
    auto getMemoryUsage = [&]() -> float {
        std::ifstream file("/proc/meminfo");
        std::string line;
        long totalMemory = 0;
        long freeMemory = 0;
        if (file.is_open()) {
            while (std::getline(file, line)) {
                std::istringstream ss(line);
                std::string key;
                long value;
                std::string unit;
                ss >> key >> value >> unit;
                if (key == "MemTotal:") {
                    totalMemory = value;
                } else if (key == "MemFree:") {
                    freeMemory = value;
                }
            }
            file.close();
        }
        // Add debug information
        //qDebug() << "Total Memory: " << totalMemory << " Free Memory: " << freeMemory;
        return (float)(totalMemory - freeMemory) / 1024.0f; // Convert to MB
    };


    // Update CPU and Memory usage graphs dynamically
    QTimer *timer = new QTimer(&window);
    QObject::connect(timer, &QTimer::timeout, [&]() {
        static int timeCounter = 0;
        float cpuUsage = getCpuUsage();
        float memoryUsage = getMemoryUsage();

        cpuSeries->append(timeCounter++, cpuUsage);
        memorySeries->append(timeCounter, memoryUsage);

        cpuAxisX->setMax(timeCounter);
        cpuAxisX->setMin(timeCounter - 20);  // Keep the graph scrolling

        memoryAxisX->setMax(timeCounter);
        memoryAxisX->setMin(timeCounter - 20);  // Keep the graph scrolling
    });
    timer->start(1000);  // Update every second

    // Button actions
    QObject::connect(killButton, &QPushButton::clicked, [&]() {
        int currentRow = table->currentRow();
        if (currentRow >= 0) {
            int pid = table->item(currentRow, 0)->text().toInt();
            pm.killProcess(pid);
            table->removeRow(currentRow);
        }
    });

    QObject::connect(reniceButton, &QPushButton::clicked, [&]() {
        int currentRow = table->currentRow();
        if (currentRow >= 0) {
            int pid = table->item(currentRow, 0)->text().toInt();
            int newPriority = QInputDialog::getInt(&window, "Renice Process", "New Priority:");
            pm.reniceProcess(pid, newPriority);
        }
    });

    QObject::connect(refreshButton, &QPushButton::clicked, [&]() {
        updateTable();
    });

    QObject::connect(searchButton, &QPushButton::clicked, [&]() {
        bool found = false;
        QString pidText = pidInput->text();
        if (pidText.isEmpty()) {
            QMessageBox::warning(&window, "Error", "PID cannot be empty.");
            return;
        }

        int searchPid = pidText.toInt();
        for (int row = 0; row < table->rowCount(); ++row) {
            int pid = table->item(row, 0)->text().toInt();
            if (pid == searchPid) {
                table->selectRow(row);
                table->scrollToItem(table->item(row, 0));
                found = true;
                break;
            }
        }

        if (!found) {
            QMessageBox::information(&window, "Not Found", "Process with PID " + pidText + " not found.");
        }
    });

QObject::connect(viewSubprocessButton, &QPushButton::clicked, [&]() {
    // Get the PID from the input field
    bool ok;
    int pid = pidInputField->text().toInt(&ok);

    if (!ok || pid <= 0) {
        QMessageBox::warning(&window, "Error", "Please enter a valid PID.");
        return;
    }

    // Fetch the subprocesses
    QStringList subprocesses = pm.getSubprocesses(pid);

    // Check if subprocesses are found
    if (subprocesses.isEmpty()) {
        QMessageBox::information(&window, "No Subprocesses", "No subprocesses found for this PID.");
        return;
    }

    // Display the subprocesses
    displaySubprocesses(subprocesses, pid, pm);
});


    window.setLayout(mainLayout);
    window.show();

    return app.exec();
}
