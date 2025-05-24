import tkinter as tk
from tkinter import ttk
import os
import time

PROC_FILE_PATH = "/proc/avionics_status"
SYS_PARAM_BASE_PATH = "/sys/module/avionics_sim/parameters"
SYS_PARAM_WORKLOAD_PATH = f"{SYS_PARAM_BASE_PATH}/simulated_workload_ms"
SYS_PARAM_PERIOD_PATH = f"{SYS_PARAM_BASE_PATH}/task_period_ms"
SYS_PARAM_DEADLINE_PATH = f"{SYS_PARAM_BASE_PATH}/task_deadline_ms"

class AvionicsGUI:
    def __init__(self, master):
        self.master = master
        master.title("Avionics Task Simulator")
        master.resizable(False, False) # Prevent resizing

        self.data_labels = {}
        self.control_frame = ttk.LabelFrame(master, text="Controls", padding="10")
        self.control_frame.grid(row=0, column=0, padx=10, pady=10, sticky=(tk.W, tk.E))
        
        self.status_frame = ttk.LabelFrame(master, text="Status", padding="10")
        self.status_frame.grid(row=1, column=0, padx=10, pady=10, sticky=(tk.W, tk.E))
        
        self.create_widgets()
        self.update_data() # Initial data load

    def create_widgets(self):
        # --- Control Widgets ---
        current_control_row = 0
        # Workload
        ttk.Label(self.control_frame, text="Simulated Workload (ms):").grid(row=current_control_row, column=0, sticky=tk.W, padx=5, pady=5)
        self.workload_var = tk.StringVar()
        self.workload_entry = ttk.Entry(self.control_frame, textvariable=self.workload_var, width=10)
        self.workload_entry.grid(row=current_control_row, column=1, sticky=tk.W, padx=5, pady=5)
        self.set_workload_button = ttk.Button(self.control_frame, text="Set Workload", command=self.set_workload)
        self.set_workload_button.grid(row=current_control_row, column=2, sticky=tk.W, padx=5, pady=5)
        current_control_row += 1

        # Period
        ttk.Label(self.control_frame, text="Task Period (ms):").grid(row=current_control_row, column=0, sticky=tk.W, padx=5, pady=5)
        self.period_var = tk.StringVar()
        self.period_entry = ttk.Entry(self.control_frame, textvariable=self.period_var, width=10)
        self.period_entry.grid(row=current_control_row, column=1, sticky=tk.W, padx=5, pady=5)
        self.set_period_button = ttk.Button(self.control_frame, text="Set Period", command=self.set_period)
        self.set_period_button.grid(row=current_control_row, column=2, sticky=tk.W, padx=5, pady=5)
        current_control_row += 1

        # Deadline
        ttk.Label(self.control_frame, text="Task Deadline (ms):").grid(row=current_control_row, column=0, sticky=tk.W, padx=5, pady=5)
        self.deadline_var = tk.StringVar()
        self.deadline_entry = ttk.Entry(self.control_frame, textvariable=self.deadline_var, width=10)
        self.deadline_entry.grid(row=current_control_row, column=1, sticky=tk.W, padx=5, pady=5)
        self.set_deadline_button = ttk.Button(self.control_frame, text="Set Deadline", command=self.set_deadline)
        self.set_deadline_button.grid(row=current_control_row, column=2, sticky=tk.W, padx=5, pady=5)
        current_control_row += 1
        
        self.status_message = tk.StringVar()
        ttk.Label(self.control_frame, textvariable=self.status_message, font=("Arial", 10), wraplength=380).grid(row=current_control_row, column=0, columnspan=3, pady=10, sticky=tk.W)

        # --- Status Display Widgets ---
        ttk.Label(self.status_frame, text="Avionics Status Data:", font=("Arial", 14, "bold")).grid(row=0, column=0, columnspan=2, pady=(0,10), sticky=tk.W)
        # Data labels will be populated by update_data_labels method in status_frame

    def _write_sys_param(self, path, value, param_name):
        if not value.isdigit() or int(value) <= 0:
            self.status_message.set(f"Error: {param_name} must be a positive number.")
            return False
        try:
            with open(path, 'w') as f:
                f.write(value)
            self.status_message.set(f"{param_name} set to {value} ms.")
            return True
        except FileNotFoundError:
            self.status_message.set(f"Error: {path} not found. Is module loaded and parameter exposed?")
        except PermissionError:
            self.status_message.set(f"Error: Permission denied writing to {path}. Try with sudo.")
        except Exception as e:
            self.status_message.set(f"Error setting {param_name}: {e}")
        return False

    def set_workload(self):
        self._write_sys_param(SYS_PARAM_WORKLOAD_PATH, self.workload_var.get(), "Simulated Workload")

    def set_period(self):
        self._write_sys_param(SYS_PARAM_PERIOD_PATH, self.period_var.get(), "Task Period")

    def set_deadline(self):
        self._write_sys_param(SYS_PARAM_DEADLINE_PATH, self.deadline_var.get(), "Task Deadline")

    def read_proc_file(self):
        data = {}
        try:
            with open(PROC_FILE_PATH, 'r') as f:
                for line in f:
                    if ':' in line:
                        key, value = line.strip().split(':', 1)
                        data[key.strip()] = value.strip()
        except FileNotFoundError:
            self.status_message.set(f"Error: {PROC_FILE_PATH} not found. Is the kernel module loaded?")
            return None
        except Exception as e:
            self.status_message.set(f"Error reading {PROC_FILE_PATH}: {e}")
            return None
        return data

    def update_data_labels(self, proc_data):
        # Dynamically create/update labels based on proc_data
        # Start placing data labels from row 1 in status_frame, as row 0 is title
        current_status_row = 1 
        
        for key, value in proc_data.items():
            if key not in self.data_labels:
                key_label_widget = ttk.Label(self.status_frame, text=f"{key}:", font=("Arial", 12, "bold"))
                key_label_widget.grid(row=current_status_row, column=0, sticky=tk.W, padx=5, pady=2)
                
                value_label_var = tk.StringVar()
                value_label_widget = ttk.Label(self.status_frame, textvariable=value_label_var, font=("Arial", 12))
                value_label_widget.grid(row=current_status_row, column=1, sticky=tk.W, padx=5, pady=2)
                self.data_labels[key] = {"var": value_label_var, "widget": value_label_widget}
            
            self.data_labels[key]["var"].set(value)
            
            # Reset style
            self.data_labels[key]["widget"].configure(foreground='black', font=("Arial", 12))

            if key == "LastDeadlineResult":
                if value == "MISSED":
                    self.data_labels[key]["widget"].configure(foreground='red', font=("Arial", 12, "bold"))
                    self.data_labels[key]["var"].set(f"{value} <!>")
                elif value == "MET":
                    self.data_labels[key]["widget"].configure(foreground='green')
            elif key == "TaskStatus" and value == "EXECUTING":
                self.data_labels[key]["widget"].configure(font=("Arial", 12, "italic"))
                self.data_labels[key]["var"].set(f"{value}...")
            elif key == "MissedCount" and value != "0":
                 self.data_labels[key]["widget"].configure(foreground='orange', font=("Arial", 12, "bold"))

            current_status_row +=1
        # Ensure status_frame expands if necessary
        self.status_frame.grid_rowconfigure(current_status_row, weight=1)

    def update_data(self):
        proc_data = self.read_proc_file()
        if proc_data:
            self.status_message.set("Data loaded successfully.")
            self.update_data_labels(proc_data)
        else:
            # If proc_data is None, it means an error occurred and status_message is already set.
            # We might want to clear old data if the file is not found.
            for key_var in self.data_labels.values():
                key_var["var"].set("N/A")

        # Schedule the next update
        self.master.after(1000, self.update_data) # Update every 1 second


if __name__ == "__main__":
    # This check is important for when running on Linux.
    # The GUI should only start if not on a headless system or if X is available.
    if os.environ.get('DISPLAY', '') == '':
        print("No display found. Skipping GUI launch. (Is X server running or SSH forwarding enabled?)")
        # Attempt to read proc file once for console output if GUI doesn't launch
        try:
            with open(PROC_FILE_PATH, 'r') as f:
                print(f"--- Contents of {PROC_FILE_PATH} ---")
                print(f.read())
                print("------------------------------------")
        except Exception as e:
            print(f"Could not read {PROC_FILE_PATH}: {e}")

    else:
        root = tk.Tk()
        gui = AvionicsGUI(root)
        root.mainloop() 