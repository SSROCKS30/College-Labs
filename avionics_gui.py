import tkinter as tk
from tkinter import ttk
import os
import time

PROC_FILE_PATH = "/proc/avionics_status"
SYS_PARAM_WORKLOAD_PATH = "/sys/module/avionics_sim/parameters/simulated_workload_ms"

class AvionicsGUI:
    def __init__(self, master):
        self.master = master
        master.title("Avionics Task Simulator")

        self.data_labels = {}
        self.create_widgets()
        self.update_data() # Initial data load

    def create_widgets(self):
        frame = ttk.Frame(self.master, padding="10")
        frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

        # Labels for displaying data from /proc/avionics_status
        # These will be populated from the actual keys found in the file
        # For now, creating a placeholder title
        ttk.Label(frame, text="Avionics Status:", font=("Arial", 16)).grid(row=0, column=0, columnspan=2, pady=10)

        # Input for simulated_workload_ms
        ttk.Label(frame, text="Set Simulated Workload (ms):").grid(row=1, column=0, sticky=tk.W, padx=5, pady=5)
        self.workload_var = tk.StringVar()
        self.workload_entry = ttk.Entry(frame, textvariable=self.workload_var, width=10)
        self.workload_entry.grid(row=1, column=1, sticky=tk.W, padx=5, pady=5)
        self.set_workload_button = ttk.Button(frame, text="Set Workload", command=self.set_workload)
        self.set_workload_button.grid(row=1, column=2, sticky=tk.W, padx=5, pady=5)
        
        self.status_message = tk.StringVar()
        ttk.Label(frame, textvariable=self.status_message, font=("Arial", 10)).grid(row=2, column=0, columnspan=3, pady=5)


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
        current_row = 3 # Start placing data labels below the workload controls
        
        # Clear old data labels if any (e.g., if a key disappears)
        # This simple version just re-grids, a more robust one might destroy old ones.
        for key, value in proc_data.items():
            if key not in self.data_labels:
                # Create label for the key (e.g., "TaskName:")
                key_label = ttk.Label(self.master.winfo_children()[0], text=f"{key}:", font=("Arial", 12, "bold"))
                key_label.grid(row=current_row, column=0, sticky=tk.W, padx=5, pady=2)
                
                # Create label for the value
                value_label_var = tk.StringVar()
                value_label = ttk.Label(self.master.winfo_children()[0], textvariable=value_label_var, font=("Arial", 12))
                value_label.grid(row=current_row, column=1, columnspan=2, sticky=tk.W, padx=5, pady=2)
                self.data_labels[key] = value_label_var
            
            self.data_labels[key].set(value) # Update the value
            
            # Style "MISSED" and "EXECUTING" for emphasis
            if key == "LastDeadlineResult" and value == "MISSED":
                 self.data_labels[key].set(f"{value} <!>") # Add visual cue
                 # Potentially change color here if we had access to the label widget itself
            elif key == "TaskStatus" and value == "EXECUTING":
                 self.data_labels[key].set(f"{value}...")
            current_row +=1
        self.master.winfo_children()[0].grid_rowconfigure(current_row, weight=1) # Add padding at the bottom


    def update_data(self):
        proc_data = self.read_proc_file()
        if proc_data:
            self.status_message.set("Data loaded successfully.")
            self.update_data_labels(proc_data)
        else:
            # If proc_data is None, it means an error occurred and status_message is already set.
            # We might want to clear old data if the file is not found.
            for key_var in self.data_labels.values():
                key_var.set("N/A")

        # Schedule the next update
        self.master.after(1000, self.update_data) # Update every 1 second

    def set_workload(self):
        new_workload = self.workload_var.get()
        if not new_workload.isdigit():
            self.status_message.set("Error: Workload must be a number.")
            return

        try:
            # Writing to the sysfs parameter requires root privileges typically,
            # or specific udev rules. For simplicity, we'll assume the user handles permissions.
            # A more robust solution might involve 'sudo tee' or a helper script.
            with open(SYS_PARAM_WORKLOAD_PATH, 'w') as f:
                f.write(new_workload)
            self.status_message.set(f"Simulated workload set to {new_workload} ms.")
            # The module should pick this up, and the next GUI refresh will show it.
        except FileNotFoundError:
            self.status_message.set(f"Error: {SYS_PARAM_WORKLOAD_PATH} not found. Is module loaded and parameter exposed?")
        except PermissionError:
            self.status_message.set(f"Error: Permission denied writing to {SYS_PARAM_WORKLOAD_PATH}. Try with sudo or check permissions.")
        except Exception as e:
            self.status_message.set(f"Error setting workload: {e}")


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