import tkinter as tk
from tkinter import ttk, messagebox
import os
import time
from collections import defaultdict

PROC_FILE_PATH = "/proc/avionics_status"
SYS_PARAM_BASE_PATH = "/sys/module/avionics_sim/parameters"

# Task parameter mappings
TASK_PARAMS = {
    "Flight Attitude Monitor": {
        "period": f"{SYS_PARAM_BASE_PATH}/attitude_period_ms",
        "deadline": f"{SYS_PARAM_BASE_PATH}/attitude_deadline_ms",
        "workload": f"{SYS_PARAM_BASE_PATH}/attitude_workload_ms"
    },
    "Engine Control": {
        "period": f"{SYS_PARAM_BASE_PATH}/engine_period_ms",
        "deadline": f"{SYS_PARAM_BASE_PATH}/engine_deadline_ms",
        "workload": f"{SYS_PARAM_BASE_PATH}/engine_workload_ms"
    },
    "Navigation System": {
        "period": f"{SYS_PARAM_BASE_PATH}/nav_period_ms",
        "deadline": f"{SYS_PARAM_BASE_PATH}/nav_deadline_ms",
        "workload": f"{SYS_PARAM_BASE_PATH}/nav_workload_ms"
    },
    "Communication System": {
        "period": f"{SYS_PARAM_BASE_PATH}/comm_period_ms",
        "deadline": f"{SYS_PARAM_BASE_PATH}/comm_deadline_ms",
        "workload": f"{SYS_PARAM_BASE_PATH}/comm_workload_ms"
    },
    "Cabin Systems": {
        "period": f"{SYS_PARAM_BASE_PATH}/cabin_period_ms",
        "deadline": f"{SYS_PARAM_BASE_PATH}/cabin_deadline_ms",
        "workload": f"{SYS_PARAM_BASE_PATH}/cabin_workload_ms"
    }
}

# Priority colors for visual distinction
PRIORITY_COLORS = {
    0: "#FF0000",  # Red - Highest priority (Critical)
    1: "#FF8C00",  # Orange - High priority
    2: "#FFD700",  # Gold - Medium priority
    3: "#32CD32",  # Lime - Lower priority
    4: "#87CEEB"   # Sky Blue - Lowest priority
}

class TaskWidget:
    """Widget for displaying and controlling individual tasks"""
    
    def __init__(self, parent, task_name, priority, row_start):
        self.parent = parent
        self.task_name = task_name
        self.priority = priority
        self.row_start = row_start
        
        # Task data variables
        self.task_data = {}
        self.control_vars = {
            'period': tk.StringVar(),
            'deadline': tk.StringVar(), 
            'workload': tk.StringVar()
        }
        
        self.create_widgets()
        
    def create_widgets(self):
        # Priority indicator and task name
        priority_color = PRIORITY_COLORS.get(self.priority, "#CCCCCC")
        
        # Priority badge
        self.priority_label = tk.Label(self.parent, text=f"P{self.priority}", 
                                      bg=priority_color, fg="white", 
                                      font=("Arial", 10, "bold"),
                                      width=3, relief="raised")
        self.priority_label.grid(row=self.row_start, column=0, padx=5, pady=2, sticky="nsew")
        
        # Task name
        self.name_label = tk.Label(self.parent, text=self.task_name, 
                                  font=("Arial", 12, "bold"),
                                  width=20, anchor="w")
        self.name_label.grid(row=self.row_start, column=1, padx=5, pady=2, sticky="w")
        
        # Status indicators frame
        self.status_frame = tk.Frame(self.parent)
        self.status_frame.grid(row=self.row_start, column=2, padx=5, pady=2, sticky="ew")
        
        # Status labels
        self.status_label = tk.Label(self.status_frame, text="READY", 
                                    font=("Arial", 10), width=10)
        self.status_label.pack(side=tk.LEFT, padx=2)
        
        self.deadline_label = tk.Label(self.status_frame, text="N/A", 
                                      font=("Arial", 10), width=8)
        self.deadline_label.pack(side=tk.LEFT, padx=2)
        
        self.exec_time_label = tk.Label(self.status_frame, text="0ms", 
                                       font=("Arial", 10), width=8)
        self.exec_time_label.pack(side=tk.LEFT, padx=2)
        
        # Statistics frame
        self.stats_frame = tk.Frame(self.parent)
        self.stats_frame.grid(row=self.row_start, column=3, padx=5, pady=2, sticky="ew")
        
        self.met_label = tk.Label(self.stats_frame, text="Met: 0", 
                                 font=("Arial", 9), fg="green")
        self.met_label.pack(side=tk.LEFT, padx=2)
        
        self.missed_label = tk.Label(self.stats_frame, text="Missed: 0", 
                                    font=("Arial", 9), fg="red")
        self.missed_label.pack(side=tk.LEFT, padx=2)
        
        self.total_label = tk.Label(self.stats_frame, text="Total: 0", 
                                   font=("Arial", 9))
        self.total_label.pack(side=tk.LEFT, padx=2)
        
        # Control frame
        self.control_frame = tk.Frame(self.parent)
        self.control_frame.grid(row=self.row_start, column=4, padx=5, pady=2, sticky="ew")
        
        # Period control
        tk.Label(self.control_frame, text="P:", font=("Arial", 8)).grid(row=0, column=0)
        self.period_entry = tk.Entry(self.control_frame, textvariable=self.control_vars['period'], 
                                    width=6, font=("Arial", 8))
        self.period_entry.grid(row=0, column=1, padx=1)
        
        # Deadline control
        tk.Label(self.control_frame, text="D:", font=("Arial", 8)).grid(row=0, column=2)
        self.deadline_entry = tk.Entry(self.control_frame, textvariable=self.control_vars['deadline'], 
                                      width=6, font=("Arial", 8))
        self.deadline_entry.grid(row=0, column=3, padx=1)
        
        # Workload control
        tk.Label(self.control_frame, text="W:", font=("Arial", 8)).grid(row=0, column=4)
        self.workload_entry = tk.Entry(self.control_frame, textvariable=self.control_vars['workload'], 
                                      width=6, font=("Arial", 8))
        self.workload_entry.grid(row=0, column=5, padx=1)
        
        # Update button
        self.update_button = tk.Button(self.control_frame, text="Set", 
                                      command=self.update_task_params,
                                      font=("Arial", 8))
        self.update_button.grid(row=0, column=6, padx=2)
        
    def update_display(self, task_data):
        """Update the display with new task data"""
        self.task_data = task_data
        
        # Update status
        status = task_data.get('Status', 'UNKNOWN')
        self.status_label.config(text=status)
        
        if status == "EXECUTING":
            self.status_label.config(bg="yellow", fg="black")
        elif status == "READY":
            self.status_label.config(bg="lightgreen", fg="black")
        elif status == "DISABLED":
            self.status_label.config(bg="lightgray", fg="black")
        else:
            self.status_label.config(bg="white", fg="black")
        
        # Update deadline result
        deadline_result = task_data.get('LastDeadlineResult', 'N/A')
        self.deadline_label.config(text=deadline_result)
        
        if deadline_result == "MISSED":
            self.deadline_label.config(fg="red", font=("Arial", 10, "bold"))
        elif deadline_result == "MET":
            self.deadline_label.config(fg="green", font=("Arial", 10))
        else:
            self.deadline_label.config(fg="black", font=("Arial", 10))
        
        # Update execution time
        exec_time = task_data.get('LastExecTime', '0')
        deadline = task_data.get('Deadline', '0')
        self.exec_time_label.config(text=f"{exec_time}ms")
        
        try:
            if int(exec_time) > int(deadline):
                self.exec_time_label.config(fg="red", font=("Arial", 10, "bold"))
            else:
                self.exec_time_label.config(fg="black", font=("Arial", 10))
        except ValueError:
            pass
        
        # Update statistics
        met_count = task_data.get('MetCount', '0')
        missed_count = task_data.get('MissedCount', '0') 
        total_count = task_data.get('TotalExecs', '0')
        
        self.met_label.config(text=f"Met: {met_count}")
        self.missed_label.config(text=f"Missed: {missed_count}")
        self.total_label.config(text=f"Total: {total_count}")
        
        # Update control fields with current values
        self.control_vars['period'].set(task_data.get('Period', ''))
        self.control_vars['deadline'].set(task_data.get('Deadline', ''))
        self.control_vars['workload'].set(task_data.get('Workload', ''))
        
    def update_task_params(self):
        """Update task parameters via sysfs"""
        if self.task_name not in TASK_PARAMS:
            messagebox.showerror("Error", f"Unknown task: {self.task_name}")
            return
            
        success_count = 0
        errors = []
        
        params = TASK_PARAMS[self.task_name]
        
        # Update period
        period_value = self.control_vars['period'].get().strip()
        if period_value and period_value.isdigit():
            if self._write_param(params['period'], period_value, "Period"):
                success_count += 1
            else:
                errors.append("Period")
        
        # Update deadline
        deadline_value = self.control_vars['deadline'].get().strip()
        if deadline_value and deadline_value.isdigit():
            if self._write_param(params['deadline'], deadline_value, "Deadline"):
                success_count += 1
            else:
                errors.append("Deadline")
        
        # Update workload
        workload_value = self.control_vars['workload'].get().strip()
        if workload_value and workload_value.isdigit():
            if self._write_param(params['workload'], workload_value, "Workload"):
                success_count += 1
            else:
                errors.append("Workload")
        
        # Show result
        if success_count > 0 and not errors:
            messagebox.showinfo("Success", f"{self.task_name}: Updated {success_count} parameter(s)")
        elif errors:
            messagebox.showwarning("Partial Success", 
                                  f"{self.task_name}: Failed to update: {', '.join(errors)}")
    
    def _write_param(self, path, value, param_name):
        """Write a parameter to sysfs"""
        try:
            with open(path, 'w') as f:
                f.write(value)
            return True
        except (FileNotFoundError, PermissionError, Exception) as e:
            return False

class MultiTaskAvionicsGUI:
    def __init__(self, master):
        self.master = master
        master.title("Multi-Task Avionics Simulator with Priority Scheduling")
        master.geometry("1200x700")
        
        # Data storage
        self.system_data = {}
        self.task_widgets = {}
        
        self.create_widgets()
        self.update_data()  # Initial data load
        
    def create_widgets(self):
        # Main title
        title_frame = tk.Frame(self.master, bg="#2E3440", height=60)
        title_frame.pack(fill=tk.X, padx=5, pady=5)
        title_frame.pack_propagate(False)
        
        title_label = tk.Label(title_frame, 
                              text="Multi-Task Avionics Simulator", 
                              font=("Arial", 16, "bold"),
                              bg="#2E3440", fg="white")
        title_label.pack(expand=True)
        
        # System status frame
        self.system_frame = tk.LabelFrame(self.master, text="System Status", 
                                         font=("Arial", 12, "bold"))
        self.system_frame.pack(fill=tk.X, padx=10, pady=5)
        
        # Add internal padding using a frame
        system_padding_frame = tk.Frame(self.system_frame)
        system_padding_frame.pack(fill=tk.X, padx=10, pady=10)
        
        # System status labels
        self.system_status_frame = tk.Frame(system_padding_frame)
        self.system_status_frame.pack(fill=tk.X)
        
        self.scheduler_status_var = tk.StringVar(value="Scheduler: UNKNOWN")
        self.scheduler_label = tk.Label(self.system_status_frame, 
                                       textvariable=self.scheduler_status_var,
                                       font=("Arial", 12, "bold"))
        self.scheduler_label.pack(side=tk.LEFT, padx=10)
        
        self.active_tasks_var = tk.StringVar(value="Active Tasks: 0")
        self.active_tasks_label = tk.Label(self.system_status_frame,
                                          textvariable=self.active_tasks_var,
                                          font=("Arial", 12))
        self.active_tasks_label.pack(side=tk.LEFT, padx=10)
        
        self.status_message_var = tk.StringVar(value="Ready")
        self.status_message_label = tk.Label(self.system_status_frame,
                                            textvariable=self.status_message_var,
                                            font=("Arial", 10),
                                            fg="blue")
        self.status_message_label.pack(side=tk.RIGHT, padx=10)
        
        # Tasks frame with scrollbar
        self.tasks_main_frame = tk.LabelFrame(self.master, text="Avionics Tasks", 
                                             font=("Arial", 12, "bold"))
        self.tasks_main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        
        # Add internal padding using a frame
        tasks_padding_frame = tk.Frame(self.tasks_main_frame)
        tasks_padding_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Canvas and scrollbar for tasks
        self.canvas = tk.Canvas(tasks_padding_frame)
        self.scrollbar = ttk.Scrollbar(tasks_padding_frame, orient="vertical", command=self.canvas.yview)
        self.scrollable_frame = tk.Frame(self.canvas)
        
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )
        
        self.canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        # Headers
        self.create_headers()
        
        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")
        
        # Control buttons frame
        self.control_buttons_frame = tk.Frame(self.master)
        self.control_buttons_frame.pack(fill=tk.X, padx=10, pady=5)
        
        self.refresh_button = tk.Button(self.control_buttons_frame, text="Refresh Now",
                                       command=self.force_refresh,
                                       font=("Arial", 10, "bold"),
                                       bg="lightblue")
        self.refresh_button.pack(side=tk.LEFT, padx=5)
        
        self.stress_test_button = tk.Button(self.control_buttons_frame, text="Quick Stress Test",
                                           command=self.quick_stress_test,
                                           font=("Arial", 10, "bold"),
                                           bg="orange")
        self.stress_test_button.pack(side=tk.LEFT, padx=5)
        
        self.reset_stats_button = tk.Button(self.control_buttons_frame, text="Reset All Stats",
                                           command=self.reset_all_stats,
                                           font=("Arial", 10, "bold"),
                                           bg="lightcoral")
        self.reset_stats_button.pack(side=tk.LEFT, padx=5)
        
        # Add 10-second simulation button
        self.simulation_button = tk.Button(self.control_buttons_frame, text="10-Second Simulation",
                                          command=self.start_10_second_simulation,
                                          font=("Arial", 10, "bold"),
                                          bg="lightgreen")
        self.simulation_button.pack(side=tk.LEFT, padx=5)
        
        # Add Gantt chart button
        self.gantt_button = tk.Button(self.control_buttons_frame, text="Show Gantt Chart",
                                     command=self.show_gantt_chart,
                                     font=("Arial", 10, "bold"),
                                     bg="lightyellow")
        self.gantt_button.pack(side=tk.LEFT, padx=5)
        
        # Simulation status
        self.simulation_status_var = tk.StringVar(value="")
        self.simulation_status_label = tk.Label(self.control_buttons_frame,
                                               textvariable=self.simulation_status_var,
                                               font=("Arial", 10, "italic"),
                                               fg="green")
        self.simulation_status_label.pack(side=tk.RIGHT, padx=10)
        
    def create_headers(self):
        """Create column headers for task display"""
        headers_frame = tk.Frame(self.scrollable_frame, bg="#D3D3D3", relief="raised", bd=1)
        headers_frame.grid(row=0, column=0, columnspan=5, sticky="ew", padx=2, pady=2)
        
        tk.Label(headers_frame, text="Pri", font=("Arial", 10, "bold"), 
                bg="#D3D3D3", width=3).grid(row=0, column=0, padx=5, pady=5)
        tk.Label(headers_frame, text="Task Name", font=("Arial", 10, "bold"),
                bg="#D3D3D3", width=20).grid(row=0, column=1, padx=5, pady=5)
        tk.Label(headers_frame, text="Status & Timing", font=("Arial", 10, "bold"),
                bg="#D3D3D3", width=25).grid(row=0, column=2, padx=5, pady=5)
        tk.Label(headers_frame, text="Statistics", font=("Arial", 10, "bold"),
                bg="#D3D3D3", width=20).grid(row=0, column=3, padx=5, pady=5)
        tk.Label(headers_frame, text="Controls (P/D/W ms)", font=("Arial", 10, "bold"),
                bg="#D3D3D3", width=25).grid(row=0, column=4, padx=5, pady=5)
        
    def read_proc_file(self):
        """Read and parse the multi-task proc file"""
        data = {}
        try:
            with open(PROC_FILE_PATH, 'r') as f:
                content = f.read()
                
            # Parse system-level data
            lines = content.strip().split('\n')
            current_task = None
            
            for line in lines:
                if ':' in line:
                    key, value = line.strip().split(':', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    if key.startswith('Task') and key.endswith('_Name'):
                        # Extract task number and start new task
                        task_num = key.split('_')[0].replace('Task', '')
                        current_task = f"Task{task_num}"
                        if current_task not in data:
                            data[current_task] = {}
                        data[current_task]['Name'] = value
                        data[current_task]['TaskNumber'] = task_num
                    elif key.startswith('Task') and current_task:
                        # Add task-specific data
                        param_name = key.split('_', 1)[1]  # Remove TaskX_ prefix
                        data[current_task][param_name] = value
                    else:
                        # System-level data
                        data[key] = value
                        
        except FileNotFoundError:
            self.status_message_var.set("Error: Module not loaded or proc file not found")
            return None
        except Exception as e:
            self.status_message_var.set(f"Error reading proc file: {e}")
            return None
            
        return data
        
    def update_task_widgets(self, task_data):
        """Update or create task widgets based on current data"""
        current_tasks = set()
        
        for task_key, task_info in task_data.items():
            if not task_key.startswith('Task'):
                continue
                
            task_name = task_info.get('Name', f'Unknown Task {task_key}')
            current_tasks.add(task_name)
            priority = int(task_info.get('Priority', 99))
            
            if task_name not in self.task_widgets:
                # Create new task widget
                row = len(self.task_widgets) + 1  # +1 for header row
                self.task_widgets[task_name] = TaskWidget(
                    self.scrollable_frame, task_name, priority, row
                )
            
            # Update task widget
            self.task_widgets[task_name].update_display(task_info)
        
        # Remove widgets for tasks that no longer exist
        for task_name in list(self.task_widgets.keys()):
            if task_name not in current_tasks:
                # TODO: Implement widget removal if needed
                pass
                
    def update_system_status(self, system_data):
        """Update system-level status display"""
        scheduler_status = system_data.get('SchedulerStatus', 'UNKNOWN')
        self.scheduler_status_var.set(f"Scheduler: {scheduler_status}")
        
        if scheduler_status == "RUNNING":
            self.scheduler_label.config(fg="green")
        else:
            self.scheduler_label.config(fg="red")
            
        active_tasks = system_data.get('ActiveTasks', '0')
        self.active_tasks_var.set(f"Active Tasks: {active_tasks}")
        
    def update_data(self):
        """Main data update method"""
        proc_data = self.read_proc_file()
        if proc_data:
            self.system_data = proc_data
            self.update_system_status(proc_data)
            self.update_task_widgets(proc_data)
            self.status_message_var.set("Data updated successfully")
        
        # Schedule next update
        self.master.after(1000, self.update_data)  # Update every 1 second
        
    def force_refresh(self):
        """Force immediate data refresh"""
        self.update_data()
        self.status_message_var.set("Manual refresh completed")
        
    def quick_stress_test(self):
        """Apply a quick stress test by increasing workloads"""
        if messagebox.askyesno("Stress Test", 
                              "This will temporarily increase all task workloads to test deadline compliance. Continue?"):
            # Store original values and increase workloads
            for task_name, widget in self.task_widgets.items():
                if task_name in TASK_PARAMS:
                    try:
                        current_workload = int(widget.control_vars['workload'].get() or "0")
                        stress_workload = min(current_workload * 3, 1000)  # Triple workload, max 1000ms
                        widget.control_vars['workload'].set(str(stress_workload))
                        widget.update_task_params()
                    except ValueError:
                        pass
            
            self.status_message_var.set("Stress test applied - monitor deadline compliance!")
            
    def reset_all_stats(self):
        """Reset statistics by reloading the module (requires user confirmation)"""
        if messagebox.askyesno("Reset Statistics", 
                              "This will restart the kernel module to reset all statistics. Continue?"):
            self.status_message_var.set("Please manually reload the kernel module to reset stats")

    def start_10_second_simulation(self):
        """Start a 10-second simulation"""
        if messagebox.askyesno("10-Second Simulation", 
                              "This will restart the module with 10-second timer.\nContinue?"):
            try:
                # Restart module with 10-second timer
                import subprocess
                import time
                
                self.simulation_status_var.set("Stopping current module...")
                self.master.update()
                
                # Stop current module
                subprocess.run(['sudo', 'rmmod', 'avionics_sim'], capture_output=True)
                time.sleep(1)
                
                # Load module with 10-second runtime
                self.simulation_status_var.set("Starting 10-second simulation...")
                self.master.update()
                
                result = subprocess.run(['sudo', 'insmod', 'avionics_sim.ko', 'system_runtime_sec=10'], 
                                      capture_output=True, text=True)
                
                if result.returncode == 0:
                    self.simulation_status_var.set("Simulation running... (10 seconds)")
                    # Start countdown timer
                    self.countdown_simulation(10)
                else:
                    self.simulation_status_var.set("Failed to start simulation")
                    messagebox.showerror("Error", f"Failed to load module: {result.stderr}")
                    
            except Exception as e:
                self.simulation_status_var.set("Error during simulation")
                messagebox.showerror("Error", f"Simulation failed: {e}")
                
    def countdown_simulation(self, seconds_left):
        """Countdown timer for simulation"""
        if seconds_left > 0:
            self.simulation_status_var.set(f"Simulation running... ({seconds_left}s left)")
            self.master.after(1000, lambda: self.countdown_simulation(seconds_left - 1))
        else:
            self.simulation_status_var.set("Simulation completed! Check Gantt Chart")
            self.simulation_button.config(bg="lightblue", text="Restart 10s Sim")
            messagebox.showinfo("Simulation Complete", 
                              "10-second simulation completed!\nClick 'Show Gantt Chart' to see results.")

    def show_gantt_chart(self):
        """Show the Gantt chart visualization"""
        try:
            # Read execution log from proc file
            exec_log = self.parse_execution_log()
            
            if not exec_log:
                messagebox.showwarning("No Data", "No execution log found. Run a 10-second simulation first.")
                return
                
            # Create Gantt chart window
            self.create_gantt_window(exec_log)
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to show Gantt chart: {e}")
            
    def parse_execution_log(self):
        """Parse execution log from proc file"""
        try:
            with open(PROC_FILE_PATH, 'r') as f:
                content = f.read()
                
            lines = content.strip().split('\n')
            exec_log = []
            in_log_section = False
            
            for line in lines:
                if line.strip() == "EXECUTION_LOG:":
                    in_log_section = True
                    continue
                    
                if in_log_section and line.startswith("EXEC:"):
                    # Parse: EXEC:task_type,start_time_ms,duration_ms,deadline_result
                    parts = line[5:].split(',')  # Remove "EXEC:" prefix
                    if len(parts) == 4:
                        exec_log.append({
                            'task_type': int(parts[0]),
                            'start_time': int(parts[1]),
                            'duration': int(parts[2]),
                            'deadline_met': parts[3] == 'MET'
                        })
            
            return exec_log
            
        except Exception as e:
            print(f"Error parsing execution log: {e}")
            return []
            
    def create_gantt_window(self, exec_log):
        """Create and display Gantt chart window"""
        # Create new window
        gantt_window = tk.Toplevel(self.master)
        gantt_window.title("Gantt Chart - Task Execution Timeline")
        gantt_window.geometry("1000x600")
        
        # Create main frame with scrollbars
        main_frame = tk.Frame(gantt_window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Canvas for Gantt chart
        canvas = tk.Canvas(main_frame, bg="white", scrollregion=(0, 0, 2000, 400))
        v_scrollbar = tk.Scrollbar(main_frame, orient="vertical", command=canvas.yview)
        h_scrollbar = tk.Scrollbar(main_frame, orient="horizontal", command=canvas.xview)
        canvas.configure(yscrollcommand=v_scrollbar.set, xscrollcommand=h_scrollbar.set)
        
        # Pack scrollbars and canvas
        v_scrollbar.pack(side="right", fill="y")
        h_scrollbar.pack(side="bottom", fill="x")
        canvas.pack(side="left", fill="both", expand=True)
        
        # Draw Gantt chart
        self.draw_gantt_chart(canvas, exec_log)
        
        # Add statistics
        stats_frame = tk.Frame(gantt_window)
        stats_frame.pack(fill=tk.X, padx=10, pady=5)
        
        self.show_execution_statistics(stats_frame, exec_log)
        
    def draw_gantt_chart(self, canvas, exec_log):
        """Draw the actual Gantt chart with improved visualization"""
        if not exec_log:
            canvas.create_text(400, 200, text="No execution data available", 
                             font=("Arial", 16), fill="red")
            canvas.create_text(400, 250, text="Run a 10-second simulation first", 
                             font=("Arial", 12), fill="gray")
            return
            
        # Enhanced colors for each task (base colors)
        task_base_colors = {
            0: "#FF6B6B",  # Flight Attitude - Red
            1: "#FFA500",  # Engine Control - Orange  
            2: "#4ECDC4",  # Navigation - Teal
            3: "#45B7D1",  # Communication - Blue
            4: "#96CEB4"   # Cabin Systems - Green
        }
        
        task_names = {
            0: "P0: Flight Attitude Monitor",
            1: "P1: Engine Control", 
            2: "P2: Navigation System",
            3: "P3: Communication System",
            4: "P4: Cabin Systems"
        }
        
        # Chart parameters - improved sizing
        chart_start_x = 200
        chart_start_y = 80
        row_height = 60  # Increased for better visibility
        chart_width = 1200  # Fixed width for better control
        
        # Find max time and calculate scale
        max_time = max(entry['start_time'] + entry['duration'] for entry in exec_log) if exec_log else 10000
        time_scale = chart_width / max_time  # Dynamic scaling
        
        # Clear canvas and set scroll region
        canvas.delete("all")
        canvas.configure(scrollregion=(0, 0, chart_start_x + chart_width + 300, chart_start_y + 5 * row_height + 150))
        
        # Title
        canvas.create_text(chart_start_x + chart_width//2, 30, 
                          text=f"Task Execution Timeline - {len(exec_log)} executions over {max_time}ms", 
                          font=("Arial", 16, "bold"), fill="black")
        
        # Draw task rows with labels and grid
        for task_id in range(5):
            y_pos = chart_start_y + task_id * row_height
            
            # Task label background
            canvas.create_rectangle(10, y_pos - 5, chart_start_x - 10, y_pos + 40, 
                                  fill="#F0F0F0", outline="black", width=1)
            
            # Priority indicator
            priority_color = task_base_colors.get(task_id, "gray")
            canvas.create_rectangle(15, y_pos, 45, y_pos + 30, 
                                  fill=priority_color, outline="black", width=2)
            canvas.create_text(30, y_pos + 15, text=f"P{task_id}", 
                             font=("Arial", 12, "bold"), fill="white")
            
            # Task name
            canvas.create_text(55, y_pos + 15, 
                             text=task_names.get(task_id, f"Task {task_id}"), 
                             font=("Arial", 11, "bold"), anchor="w", fill="black")
            
            # Horizontal grid line for timeline
            canvas.create_line(chart_start_x, y_pos + 35, chart_start_x + chart_width, y_pos + 35, 
                             fill="lightgray", width=1, dash=(2, 2))
            
            # Task row background
            canvas.create_rectangle(chart_start_x, y_pos, chart_start_x + chart_width, y_pos + 35,
                                  fill="white", outline="lightgray", width=1)
        
        # Draw time axis with better markings
        time_axis_y = chart_start_y + 5 * row_height + 10
        canvas.create_line(chart_start_x, time_axis_y, chart_start_x + chart_width, time_axis_y, 
                         fill="black", width=2)
        
        # Time markers - every 1000ms (1 second)
        time_interval = 1000  # 1 second intervals
        for time_ms in range(0, max_time + time_interval, time_interval):
            x_pos = chart_start_x + time_ms * time_scale
            if x_pos <= chart_start_x + chart_width:  # Only draw if within bounds
                canvas.create_line(x_pos, time_axis_y, x_pos, time_axis_y + 8, fill="black", width=1)
                canvas.create_text(x_pos, time_axis_y + 20, text=f"{time_ms//1000}s", 
                                 font=("Arial", 9), anchor="n")
        
        # Sort execution log by start time for better visualization
        sorted_exec_log = sorted(exec_log, key=lambda x: x['start_time'])
        
        # Draw execution bars with enhanced information
        execution_count = {}  # Track executions per task for positioning
        for task_id in range(5):
            execution_count[task_id] = 0
            
        for entry in sorted_exec_log:
            task_id = entry['task_type']
            start_time = entry['start_time']
            duration = entry['duration']
            deadline_met = entry['deadline_met']
            
            # Calculate position
            y_pos = chart_start_y + task_id * row_height
            x_start = chart_start_x + start_time * time_scale
            x_end = chart_start_x + (start_time + duration) * time_scale
            
            # Ensure minimum width for visibility
            min_width = 3
            if x_end - x_start < min_width:
                x_end = x_start + min_width
            
            # Color coding: base color for deadline met, red for missed
            if deadline_met:
                color = task_base_colors.get(task_id, "gray")
                outline_color = "darkgreen"
                outline_width = 1
            else:
                color = "#FF4444"  # Bright red for deadline misses
                outline_color = "darkred"
                outline_width = 2
            
            # Draw execution bar
            bar_height = 25
            canvas.create_rectangle(x_start, y_pos + 5, x_end, y_pos + 5 + bar_height, 
                                  fill=color, outline=outline_color, width=outline_width)
            
            # Add execution number and duration text
            bar_center_x = (x_start + x_end) / 2
            bar_center_y = y_pos + 5 + bar_height // 2
            
            execution_count[task_id] += 1
            
            # Add text if bar is wide enough
            bar_width = x_end - x_start
            if bar_width > 40:  # If bar is wide enough for text
                canvas.create_text(bar_center_x, bar_center_y - 6, 
                                 text=f"#{execution_count[task_id]}", 
                                 font=("Arial", 7, "bold"), fill="white")
                canvas.create_text(bar_center_x, bar_center_y + 4, 
                                 text=f"{duration}ms", 
                                 font=("Arial", 7), fill="white")
            elif bar_width > 20:  # Just duration
                canvas.create_text(bar_center_x, bar_center_y, 
                                 text=f"{duration}", 
                                 font=("Arial", 6), fill="white")
            
            # Add deadline status indicator (small dot)
            status_color = "green" if deadline_met else "red"
            canvas.create_oval(x_start - 2, y_pos + 2, x_start + 2, y_pos + 6, 
                             fill=status_color, outline=status_color)
        
        # Enhanced legend
        legend_x = chart_start_x + chart_width + 20
        legend_y = chart_start_y
        
        # Legend background
        canvas.create_rectangle(legend_x - 5, legend_y - 10, legend_x + 250, legend_y + 300, 
                              fill="#F8F8F8", outline="black", width=1)
        
        canvas.create_text(legend_x + 10, legend_y, text="Legend", font=("Arial", 14, "bold"), anchor="nw")
        
        # Task colors legend
        canvas.create_text(legend_x + 10, legend_y + 25, text="Tasks:", font=("Arial", 12, "bold"), anchor="nw")
        for i, (task_id, color) in enumerate(task_base_colors.items()):
            y_pos = legend_y + 45 + i * 30
            canvas.create_rectangle(legend_x + 10, y_pos, legend_x + 25, y_pos + 15, 
                                  fill=color, outline="black")
            canvas.create_text(legend_x + 35, y_pos + 7, 
                             text=task_names.get(task_id, f'Task {task_id}'), 
                             font=("Arial", 10), anchor="w")
        
        # Status indicators legend
        canvas.create_text(legend_x + 10, legend_y + 200, text="Status:", font=("Arial", 12, "bold"), anchor="nw")
        
        # Deadline met indicator
        canvas.create_rectangle(legend_x + 10, legend_y + 220, legend_x + 60, legend_y + 235, 
                              fill=task_base_colors[0], outline="darkgreen", width=1)
        canvas.create_text(legend_x + 70, legend_y + 227, text="Deadline Met", 
                         font=("Arial", 10), anchor="w")
        
        # Deadline missed indicator
        canvas.create_rectangle(legend_x + 10, legend_y + 245, legend_x + 60, legend_y + 260, 
                              fill="#FF4444", outline="darkred", width=2)
        canvas.create_text(legend_x + 70, legend_y + 252, text="Deadline MISSED", 
                         font=("Arial", 10, "bold"), anchor="w", fill="red")
        
        # Status dots legend
        canvas.create_oval(legend_x + 10, legend_y + 270, legend_x + 14, legend_y + 274, 
                         fill="green", outline="green")
        canvas.create_text(legend_x + 20, legend_y + 272, text="Met", 
                         font=("Arial", 9), anchor="w")
        
        canvas.create_oval(legend_x + 70, legend_y + 270, legend_x + 74, legend_y + 274, 
                         fill="red", outline="red")
        canvas.create_text(legend_x + 80, legend_y + 272, text="Missed", 
                         font=("Arial", 9), anchor="w")
        
        # Instructions
        canvas.create_text(legend_x + 10, legend_y + 290, 
                         text="• Numbers show execution order\n• Duration shown in milliseconds\n• Red bars = deadline violations", 
                         font=("Arial", 8), anchor="nw", justify="left")
        
    def show_execution_statistics(self, parent_frame, exec_log):
        """Show execution statistics summary with enhanced details"""
        # Calculate comprehensive statistics
        task_stats = {}
        total_executions = len(exec_log)
        total_deadline_misses = 0
        
        # Calculate timeline span
        if exec_log:
            timeline_start = min(entry['start_time'] for entry in exec_log)
            timeline_end = max(entry['start_time'] + entry['duration'] for entry in exec_log)
            timeline_duration = timeline_end - timeline_start
        else:
            timeline_duration = 0
        
        for entry in exec_log:
            task_id = entry['task_type']
            if task_id not in task_stats:
                task_stats[task_id] = {
                    'count': 0, 
                    'total_time': 0, 
                    'misses': 0, 
                    'min_duration': float('inf'),
                    'max_duration': 0,
                    'total_cpu_time': 0
                }
                
            stats = task_stats[task_id]
            stats['count'] += 1
            stats['total_time'] += entry['duration']
            stats['total_cpu_time'] += entry['duration']
            stats['min_duration'] = min(stats['min_duration'], entry['duration'])
            stats['max_duration'] = max(stats['max_duration'], entry['duration'])
            
            if not entry['deadline_met']:
                stats['misses'] += 1
                total_deadline_misses += 1
        
        # Create enhanced statistics display
        stats_text = f"📊 EXECUTION ANALYSIS REPORT\n"
        stats_text += f"{'='*60}\n"
        stats_text += f"Total Executions: {total_executions}\n"
        stats_text += f"Timeline Duration: {timeline_duration:.0f} ms ({timeline_duration/1000:.1f} seconds)\n"
        stats_text += f"Total Deadline Violations: {total_deadline_misses} ({total_deadline_misses/total_executions*100:.1f}%)\n"
        stats_text += f"System Health: {'🔴 CRITICAL' if total_deadline_misses > total_executions*0.2 else '🟡 WARNING' if total_deadline_misses > 0 else '🟢 HEALTHY'}\n"
        stats_text += f"\n{'TASK BREAKDOWN:'}\n"
        stats_text += f"{'-'*60}\n"
        
        task_names = ["Flight Attitude Monitor", "Engine Control", "Navigation System", "Communication System", "Cabin Systems"]
        
        for task_id in range(5):
            task_name = task_names[task_id]
            stats_text += f"\nP{task_id}: {task_name}\n"
            
            if task_id in task_stats:
                stats = task_stats[task_id]
                avg_time = stats['total_time'] / stats['count'] if stats['count'] > 0 else 0
                miss_rate = (stats['misses'] / stats['count'] * 100) if stats['count'] > 0 else 0
                cpu_utilization = (stats['total_cpu_time'] / timeline_duration * 100) if timeline_duration > 0 else 0
                
                # Fix min_duration display for tasks that didn't run
                min_dur = stats['min_duration'] if stats['min_duration'] != float('inf') else 0
                
                stats_text += f"  ✓ Executions: {stats['count']}\n"
                stats_text += f"  ⏱️  Avg Duration: {avg_time:.1f} ms\n"
                stats_text += f"  📏 Duration Range: {min_dur} - {stats['max_duration']} ms\n"
                stats_text += f"  🎯 Deadline Compliance: {stats['count'] - stats['misses']}/{stats['count']} ({100-miss_rate:.1f}%)\n"
                stats_text += f"  ⚠️  Deadline Misses: {stats['misses']} ({miss_rate:.1f}%)\n"
                stats_text += f"  💻 CPU Utilization: {cpu_utilization:.1f}%\n"
                
                # Status indicator
                if stats['misses'] == 0:
                    stats_text += f"  Status: 🟢 OPTIMAL\n"
                elif miss_rate < 10:
                    stats_text += f"  Status: 🟡 ACCEPTABLE\n"
                else:
                    stats_text += f"  Status: 🔴 CRITICAL\n"
            else:
                stats_text += f"  ❌ STARVED - No executions recorded\n"
                stats_text += f"  Status: 🔴 CRITICAL - Task not running\n"
        
        # Add priority inversion analysis
        stats_text += f"\n{'PRIORITY ANALYSIS:'}\n"
        stats_text += f"{'-'*60}\n"
        
        total_high_priority_execs = sum(task_stats.get(i, {'count': 0})['count'] for i in [0, 1])
        total_low_priority_execs = sum(task_stats.get(i, {'count': 0})['count'] for i in [2, 3, 4])
        
        if total_low_priority_execs == 0 and total_high_priority_execs > 0:
            stats_text += f"🚨 PRIORITY INVERSION DETECTED!\n"
            stats_text += f"High-priority tasks (P0, P1) monopolizing CPU\n"
            stats_text += f"Low-priority tasks (P2, P3, P4) starved\n"
        elif total_high_priority_execs > total_low_priority_execs * 3:
            stats_text += f"⚠️  HIGH PRIORITY DOMINANCE\n"
            stats_text += f"High-priority: {total_high_priority_execs} executions\n"
            stats_text += f"Low-priority: {total_low_priority_execs} executions\n"
        else:
            stats_text += f"✅ BALANCED EXECUTION\n"
            stats_text += f"Priority scheduling working correctly\n"
        
        # Display enhanced statistics
        stats_frame = tk.Frame(parent_frame, relief="sunken", bd=2)
        stats_frame.pack(fill=tk.X, padx=5, pady=5)
        
        # Add scrollable text widget for long statistics
        text_frame = tk.Frame(stats_frame)
        text_frame.pack(fill=tk.BOTH, expand=True)
        
        stats_text_widget = tk.Text(text_frame, font=("Courier", 9), 
                                   height=12, wrap=tk.WORD, 
                                   bg="#F5F5F5", fg="black")
        stats_scrollbar = tk.Scrollbar(text_frame, orient="vertical", command=stats_text_widget.yview)
        stats_text_widget.configure(yscrollcommand=stats_scrollbar.set)
        
        stats_text_widget.pack(side="left", fill="both", expand=True)
        stats_scrollbar.pack(side="right", fill="y")
        
        stats_text_widget.insert("1.0", stats_text)
        stats_text_widget.config(state="disabled")  # Make read-only

if __name__ == "__main__":
    # Check for display
    if os.environ.get('DISPLAY', '') == '':
        print("No display found. Attempting console output...")
        try:
            with open(PROC_FILE_PATH, 'r') as f:
                print(f"--- Contents of {PROC_FILE_PATH} ---")
                print(f.read())
                print("------------------------------------")
        except Exception as e:
            print(f"Could not read {PROC_FILE_PATH}: {e}")
    else:
        root = tk.Tk()
        gui = MultiTaskAvionicsGUI(root)
        root.mainloop() 