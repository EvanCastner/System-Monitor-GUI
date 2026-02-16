# System-Monitor-GUI
A lightweight system monitoring GUI application written in C++ that provides real-time insights into your machine’s CPU usage, memory usage, and network activity. The project focuses on performance, clean design, and modern C++ practices, with an interactive graphical interface suitable for Linux-based systems.

Features
CPU Usage Monitoring
Displays current CPU utilization in real time.

Memory Usage Tracking
Shows used vs available system memory, updated continuously.

Network Activity Monitoring
Tracks incoming and outgoing network traffic.

Graphical User Interface
Clean, responsive GUI for visualizing system metrics.

Efficient & Lightweight
Designed with low overhead to minimize performance impact.

Technologies Used

- C++ (C++17)
- ImGui for the GUI rendering
- GLFW for window and context management
- OpenGL for graphics rendering
- CMake for build configuration

How It Works
- CPU Usage is calculated by sampling system CPU statistics and computing active vs idle time.
- Memory Usage is read directly from system memory information provided by the OS.
- Network Usage is derived from system network interface counters.
- All metrics are refreshed at regular intervals and rendered in real time within the GUI.
