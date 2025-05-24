# Avionics Task Simulator (LKM + C/Python GUI)

This project simulates a critical avionics task using a Linux Kernel Module (LKM) and visualizes its performance with a GUI. Two GUI options are available:
1. A GTK+ based C GUI (`avionics_gui.c`).
2. A Tkinter based Python GUI (`avionics_gui.py`).

## Prerequisites

1.  **Linux Environment:** A Linux system (VM recommended for kernel development).
2.  **Build Tools:** `sudo apt-get update && sudo apt-get install build-essential`
3.  **Linux Headers:** `sudo apt-get install linux-headers-$(uname -r)`
4.  **GTK+ 3 Development Libraries (for C GUI):** `sudo apt-get install libgtk-3-dev`
5.  **Python 3 and Tkinter (for Python GUI):** Most systems have this. If not: `sudo apt-get install python3 python3-tk` (Debian/Ubuntu) or equivalent for your distribution.

## Part 1: Kernel Module (`avionics_sim.ko`)

The kernel module (`avionics_sim.c`) simulates a periodic task and reports its status via `/proc/avionics_status`.

### Compilation and Usage:

Navigate to the project directory where `avionics_sim.c` and `Makefile` are located.

*   **Compile the module:**
    ```bash
    make
    ```
*   **Load the module (requires sudo):**
    ```bash
    sudo make load
    # Or: sudo insmod avionics_sim.ko
    ```
*   **Check module status and proc file:**
    ```bash
    lsmod | grep avionics_sim
    cat /proc/avionics_status
    dmesg | tail # To see kernel messages from the module
    ```
*   **Change simulated workload (e.g., to 150ms) (requires sudo):**
    The `simulated_workload_ms` parameter defaults to 100ms.
    ```bash
    echo 150 | sudo tee /sys/module/avionics_sim/parameters/simulated_workload_ms
    # Or use the interactive make target:
    make set_workload
    ```
    Verify the change with `cat /proc/avionics_status`.
*   **Unload the module (requires sudo):**
    ```bash
    sudo make unload
    # Or: sudo rmmod avionics_sim
    ```
*   **Clean compiled files:**
    ```bash
    make clean
    ```
*   **Show help for Makefile targets:**
    ```bash
    make help
    ```

## Part 2: GTK+ GUI Application (C - `avionics_gui`)

The C GUI application (`avionics_gui.c`) reads data from `/proc/avionics_status` and displays it using GTK+ 3.

### Compilation and Usage:

Navigate to the project directory where `avionics_gui.c` is located.

*   **Compile the GUI (ensure GTK+ 3 dev libraries are installed):**
    ```bash
    gcc avionics_gui.c -o avionics_gui $(pkg-config --cflags --libs gtk+-3.0)
    ```
*   **Run the GUI:**
    First, ensure the `avionics_sim.ko` kernel module is loaded (`sudo make load`).
    Then, run the GUI application:
    ```bash
    ./avionics_gui
    ```
    The GUI will periodically refresh with data from the kernel module.

## Part 3: Tkinter GUI Application (Python - `avionics_gui.py`)

The Python GUI application (`avionics_gui.py`) reads data from `/proc/avionics_status` and displays it using Tkinter.

### Prerequisites for Python GUI:

*   Ensure Python 3 and Tkinter are installed:
    ```bash
    # For Debian/Ubuntu based systems
    sudo apt-get update
    sudo apt-get install python3 python3-tk
    # For Fedora
    # sudo dnf install python3-tkinter
    # For CentOS/RHEL
    # sudo yum install python3-tkinter
    ```

### Usage:

Navigate to the project directory where `avionics_gui.py` is located.

1.  **Ensure the Kernel Module is Loaded:**
    The `avionics_sim.ko` module must be compiled and loaded first (see Part 1).
    ```bash
    # If not already done from Part 1
    # make
    # sudo make load
    ```

2.  **Run the Python GUI:**
    *   To view data:
        ```bash
        python3 avionics_gui.py
        ```
    *   To also enable setting the workload via the GUI (requires permission to write to sysfs):
        ```bash
        sudo python3 avionics_gui.py
        ```
    The GUI window will appear, displaying live data from the kernel module. It will refresh automatically. If no display/X server is detected (e.g., on a headless system), it will attempt to print the `/proc/avionics_status` contents to the console once.

## Development Notes

*   **Kernel Module:** Communication is one-way (kernel to userspace) via the `/proc` file. The `simulated_workload_ms` can be changed via a sysfs parameter (`/sys/module/avionics_sim/parameters/simulated_workload_ms`).
*   **C GUI:** Uses GTK+ 3. The UI is updated by polling the `/proc` file.
*   **Python GUI:** Uses Tkinter. The UI is updated by polling the `/proc` file. It can also modify `simulated_workload_ms` if run with appropriate permissions.
*   **Error Handling:** Basic error handling is in place (e.g., if `/proc/avionics_status` is not found).
*   **Styling:** Basic CSS is embedded in the C code for the C GUI. The Python GUI has basic visual cues for task status. 