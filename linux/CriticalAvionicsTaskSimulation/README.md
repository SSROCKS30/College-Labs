# Multi-Task Avionics Simulator with Priority Scheduling

This project simulates a **realistic multi-task avionics system** using a Linux Kernel Module (LKM) and visualizes task performance with an enhanced GUI. The system demonstrates **priority-based scheduling** and **real-time task management** concepts used in actual aircraft systems.

## 🛩️ **What's New: Multi-Task System**

### **Five Concurrent Avionics Tasks:**
1. **Flight Attitude Monitor** (Priority 0 - Highest) 🔴
   - Critical for flight safety
   - Default: 100ms period, 50ms deadline, 30ms workload
   
2. **Engine Control** (Priority 1 - High) 🟠
   - Engine monitoring and control
   - Default: 200ms period, 100ms deadline, 60ms workload
   
3. **Navigation System** (Priority 2 - Medium) 🟡
   - GPS/INS updates
   - Default: 500ms period, 200ms deadline, 120ms workload
   
4. **Communication System** (Priority 3 - Lower) 🟢
   - Radio communication
   - Default: 1000ms period, 400ms deadline, 150ms workload
   
5. **Cabin Systems** (Priority 4 - Lowest) 🔵
   - Non-critical cabin functions
   - Default: 2000ms period, 800ms deadline, 200ms workload

### **Priority Scheduling Algorithm:**
- **Highest priority task** always executes first
- **Preemptive scheduling** with 10ms scheduler intervals
- **Real-time deadline monitoring** for each task
- **Spinlock protection** for thread-safe operations

## 📋 **Prerequisites**

1. **Linux Environment:** Ubuntu/Debian recommended (VM supported)
2. **Build Tools:** `sudo apt-get install build-essential linux-headers-$(uname -r)`
3. **Python 3 + Tkinter:** `sudo apt-get install python3 python3-tk`
4. **Root Access:** Required for kernel module operations

## 🚀 **Quick Start**

### **1. Complete Demo (Recommended)**
```bash
make demo
```
This runs a full demonstration: module loading, monitoring, stress testing, and cleanup.

### **2. Manual Setup**
```bash
# Compile and load the kernel module
make load

# Launch the enhanced GUI (with control capabilities)
make gui_sudo

# Or launch read-only GUI
make gui
```

### **3. Command Line Monitoring**
```bash
# View current status
make status

# Show all task parameters
make show_params

# Continuous monitoring
make monitor
```

## 🎛️ **Enhanced GUI Features**

### **Multi-Task Dashboard:**
- **Priority Color Coding:** Visual distinction of task priorities
- **Real-Time Status:** Live updates of task execution state
- **Deadline Compliance:** Green/Red indicators for met/missed deadlines
- **Statistics Tracking:** Met/Missed/Total execution counts
- **Individual Controls:** Per-task parameter adjustment

### **GUI Layout:**
```
┌─────────────────────────────────────────────┐
│         Multi-Task Avionics Simulator      │
├─────────────────────────────────────────────┤
│ Scheduler: RUNNING    Active Tasks: 5      │
├─────────────────────────────────────────────┤
│ Pri │ Task Name           │ Status & Timing │
├─────┼─────────────────────┼─────────────────┤
│ P0  │ Flight Attitude     │ READY  MET  30ms│
│ P1  │ Engine Control      │ EXEC   MET  58ms│
│ P2  │ Navigation System   │ READY  MET 118ms│
│ P3  │ Communication       │ READY  MET 145ms│
│ P4  │ Cabin Systems       │ READY  MET 198ms│
└─────┴─────────────────────┴─────────────────┘
```

## 🎯 **Learning Experiments**

### **1. Priority Inversion Demonstration**
```bash
# Increase low-priority task workload dramatically
echo 500 | sudo tee /sys/module/avionics_sim/parameters/cabin_workload_ms

# Observe how it affects higher priority tasks
make monitor
```

### **2. Deadline Stress Testing**
```bash
# Light stress (50% increase)
make stress_test_light

# Heavy stress (triple workloads) 
make stress_test_heavy

# Reset to defaults
make reset_defaults
```

### **3. Individual Task Tuning**
```bash
# Adjust specific tasks
make set_attitude_workload
make set_engine_workload  
make set_nav_workload
```

## 📊 **Understanding the Output**

### **Proc File Format (`/proc/avionics_status`):**
```
AvionicsSystem: Multi-Task Simulator
SchedulerStatus: RUNNING
ActiveTasks: 5
---
Task0_Name: Flight Attitude Monitor
Task0_Priority: 0
Task0_Period: 100
Task0_Deadline: 50
Task0_Workload: 30
Task0_Status: READY
Task0_LastExecTime: 28
Task0_LastDeadlineResult: MET
Task0_MetCount: 156
Task0_MissedCount: 0
Task0_TotalExecs: 156
Task0_Enabled: YES
---
[Additional tasks...]
```

### **Key Metrics to Watch:**
- **LastDeadlineResult:** MET vs MISSED
- **Status:** EXECUTING, READY, DISABLED
- **MetCount/MissedCount:** Deadline compliance ratio
- **LastExecTime vs Deadline:** Performance margin

## 🔧 **Advanced Configuration**

### **Module Parameters (via sysfs):**
Each task has three configurable parameters:

```bash
# Flight Attitude Monitor
/sys/module/avionics_sim/parameters/attitude_period_ms
/sys/module/avionics_sim/parameters/attitude_deadline_ms
/sys/module/avionics_sim/parameters/attitude_workload_ms

# Engine Control
/sys/module/avionics_sim/parameters/engine_period_ms
/sys/module/avionics_sim/parameters/engine_deadline_ms
/sys/module/avionics_sim/parameters/engine_workload_ms

# [Similar patterns for nav, comm, cabin tasks]
```

### **Real-Time Modifications:**
```bash
# Example: Make attitude monitor very aggressive
echo 50 | sudo tee /sys/module/avionics_sim/parameters/attitude_period_ms
echo 25 | sudo tee /sys/module/avionics_sim/parameters/attitude_deadline_ms
echo 20 | sudo tee /sys/module/avionics_sim/parameters/attitude_workload_ms
```

## 🏗️ **System Architecture**

### **Kernel Space (avionics_sim.ko):**
- **Priority Scheduler:** 10ms intervals, preemptive
- **Task Management:** Individual timers per task
- **Statistics Tracking:** Per-task performance metrics
- **Thread Safety:** Spinlock-protected data structures

### **User Space (avionics_gui.py):**
- **Multi-Task Visualization:** Priority-coded display
- **Real-Time Updates:** 1-second refresh intervals  
- **Parameter Control:** Direct sysfs manipulation
- **Stress Testing:** Built-in workload manipulation

## 🎓 **Educational Value**

This enhanced system teaches:

### **Real-Time Systems Concepts:**
- **Priority Scheduling:** How higher priority tasks preempt lower ones
- **Deadline Management:** Critical timing requirements in avionics
- **Resource Contention:** When multiple tasks compete for CPU time
- **Performance Analysis:** Measuring and optimizing task execution

### **Avionics-Specific Learning:**
- **Task Hierarchy:** Why flight attitude has highest priority
- **Safety Criticality:** Different priority levels for different functions
- **System Integration:** How multiple subsystems coordinate
- **Fault Tolerance:** Monitoring and responding to deadline misses

### **Linux Kernel Programming:**
- **Multi-Timer Management:** Coordinating multiple kernel timers
- **Synchronization:** Using spinlocks for thread safety
- **Parameter Interfaces:** Runtime configuration via sysfs
- **Proc Filesystem:** Complex data output formatting

## ⚡ **Performance Characteristics**

### **Default System Load:**
- **Total CPU Usage:** ~60-70% of available compute
- **Scheduler Overhead:** ~1-2ms per 10ms interval
- **Task Execution:** Deterministic with mdelay() simulation
- **Memory Usage:** <1MB kernel memory

### **Stress Test Scenarios:**
1. **Light Stress:** 50% workload increase → Some deadline pressure
2. **Heavy Stress:** 300% workload increase → Guaranteed deadline misses
3. **Priority Inversion:** Low-priority task blocking high-priority tasks

## 🐛 **Troubleshooting**

### **Common Issues:**
```bash
# Module won't load
sudo dmesg | tail                    # Check kernel messages
lsmod | grep avionics               # Verify module status

# GUI won't start
echo $DISPLAY                       # Check X11 forwarding
python3 -m tkinter                  # Test Tkinter

# Permission errors
sudo python3 avionics_gui.py        # Run with elevated privileges
ls -la /sys/module/avionics_sim/    # Verify sysfs paths
```

### **Performance Issues:**
```bash
# High deadline miss rate
make show_params                    # Check current configuration
make reset_defaults                 # Return to safe values
make monitor                        # Watch real-time performance
```

## 📚 **Further Exploration**

### **Next Steps:**
1. **Add More Tasks:** Extend with additional avionics functions
2. **Implement EDF Scheduling:** Earliest Deadline First algorithm
3. **Add Network Simulation:** Inter-system communication
4. **Hardware Integration:** Connect real sensors
5. **Safety Analysis:** DO-178B/C compliance checking

### **Related Concepts:**
- **Rate Monotonic Scheduling**
- **ARINC 653 Partitioning**
- **DO-178C Software Standards**
- **Real-Time Operating Systems (RTOS)**
- **Avionics System Architecture**

## 🔗 **Makefile Commands Reference**

```bash
make help                    # Complete command reference
make demo                    # Full demonstration
make load/unload            # Module management
make monitor                 # Live monitoring
make stress_test_light       # Performance testing
make gui_sudo               # Enhanced GUI launch
make show_params            # Configuration display
```

---

**This multi-task simulator provides a realistic foundation for understanding the complex real-time systems that keep modern aircraft safely in the sky!** ✈️

## 🏆 **Achievement Unlocked**
You now have a sophisticated avionics simulator that demonstrates the same scheduling concepts used in real aircraft flight computers. Experiment with different configurations and observe how priority scheduling ensures that critical flight safety tasks always get the CPU time they need! 