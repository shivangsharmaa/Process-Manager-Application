# System Monitor Application

## Overview

The **System Monitor Application** is a robust tool developed in C++ using the Qt framework that provides real-time monitoring and management of system resources. This application is designed to visualize CPU and memory utilization and manage system processes efficiently. It leverages Qt's powerful graphical capabilities to offer a user-friendly interface for system monitoring and process management.

## Features

1. **Real-Time CPU Usage Monitoring:**
   - Displays a dynamic line chart showing CPU usage over time.
   - The chart updates every second to reflect the current CPU load.

2. **Memory Utilization Visualization:**
   - Shows real-time statistics of system memory, including total and free memory.
   - Provides a graphical representation of memory usage for easy interpretation.

3. **Process Management:**
   - View and manage system processes through a comprehensive table interface.
   - Features include:
     - **Search:** Locate processes by PID.
     - **Refresh:** Update the process list to reflect the current state.
     - **Kill Process:** Terminate a selected process.
     - **Renice Process:** Adjust the priority of a selected process.

4. **Dynamic Data Updates:**
   - Regularly updates CPU and memory metrics to provide live system insights.
   - Ensures that the displayed data is current and relevant.

## Technologies Used

- **Programming Languages:** C++ 
- **Framework:** Qt (Qt5), QtCharts, QML
- **Libraries & Tools:**
  - **QtCharts:** For creating and displaying interactive charts.
  - **QML:** For designing and implementing user interfaces.
- **Operating System:** Linux Kernel

## Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/shivangsharmaa/system-monitor.git
   ```
2. **Navigate to the Project Directory:**

   ```bash
   cd system-monitor
   ```
3. **Build the Project:**
Ensure you have Qt installed. You can build the project using qmake and make:
   ```bash
    qmake
    make
   ```
 4. **Run the Application:**

   ```bash
   ./systemmonitor
   ```
## Usage
- **View CPU Usage:** The CPU usage graph displays real-time data. It updates every second to reflect the current CPU load.
- **Monitor Memory Usage:** The memory utilization widget shows total and free memory, updated periodically.
- **Process Management Table:**
  - **Search Process:** Enter a PID to find and highlight a specific process.
  - **Refresh List:** Update the process list to reflect current active processes.
  - **Kill Process:** Terminate a selected process by PID.  
  - **Renice Process:** Change the priority of a selected process by PID.
