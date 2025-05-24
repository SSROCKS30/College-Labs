# Avionics Task Simulator (LKM + GTK+ GUI)

This project simulates a critical avionics task using a Linux Kernel Module (LKM) and visualizes its performance with a GTK+ based C GUI.

## Prerequisites

1.  **Linux Environment:** A Linux system (VM recommended for kernel development).
2.  **Build Tools:** `sudo apt-get update && sudo apt-get install build-essential`
3.  **Linux Headers:** `sudo apt-get install linux-headers-$(uname -r)`
4.  **GTK+ 3 Development Libraries:** `sudo apt-get install libgtk-3-dev`

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

## Part 2: GTK+ GUI Application (`avionics_gui`)

The GUI application (`avionics_gui.c`) reads data from `/proc/avionics_status` and displays it.

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

## Development Notes

*   **Kernel Module:** Communication is one-way (kernel to userspace) via the `/proc` file.
*   **GUI:** Uses GTK+ 3. The UI is updated by polling the `/proc` file.
*   **Error Handling:** Basic error handling is in place (e.g., if `/proc/avionics_status` is not found).
*   **Styling:** Basic CSS is embedded in the C code for the GUI for better visual feedback (e.g., colors for deadline met/missed). 