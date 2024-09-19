#include "processlist.h"
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <cctype>  // For isdigit()
#include <QDebug>

// Function to list all running processes and add them to the ProcessManager
void listProcesses(ProcessManager &manager) {
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
                    manager.addProcess(processInfo);
                }
            }
        }
    }

    closedir(dir);
}
