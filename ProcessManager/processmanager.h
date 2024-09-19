#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include <QTableWidget>
#include <vector>
#include <string>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>

struct ProcessInfo {
    int pid;       // Process ID
    std::string name; // Process Name
    int priority;  // Process Priority
    int parentPid;
};

class ProcessManager : QObject{
	Q_OBJECT
public:
    ProcessManager();  // Constructor
    Q_INVOKABLE double getCpuUsage();  // Expose this function to QML
    void createProcessTree(QGraphicsScene *scene, const std::vector<ProcessInfo> &processes);
    void populateTable(QTableWidget *table);
    void reniceProcess(int pid, int priority);
    void addProcess(const ProcessInfo &process);
    void killProcess(int pid);  // Add this line
    void listProcesses();       // Add this line

    std::vector<ProcessInfo> processes; // List of processes
    signals:
    void cpuUsageChanged();  // Signal to notify QML
};

#endif // PROCESSMANAGER_H

