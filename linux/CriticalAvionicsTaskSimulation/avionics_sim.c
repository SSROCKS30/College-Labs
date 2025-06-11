#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/ktime.h>   // For ktime_get, ktime_sub, ktime_to_ns
#include <linux/delay.h>   // For mdelay
#include <linux/spinlock.h> // For spinlocks
#include <linux/list.h>    // For linked lists
#include <linux/string.h>  // For strcpy

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AI Assistant (Multi-Task Avionics LKM)");
MODULE_DESCRIPTION("Multi-Task Critical Avionics Simulator with Priority Scheduling");

#define PROC_FILENAME "avionics_status"
#define MAX_TASKS 5

// --- Task Types with Realistic Avionics Functions ---
typedef enum {
    TASK_FLIGHT_ATTITUDE = 0,    // Highest priority - Critical for flight safety
    TASK_ENGINE_CONTROL,         // High priority - Engine monitoring
    TASK_NAVIGATION,             // Medium priority - GPS/INS updates  
    TASK_COMMUNICATION,          // Lower priority - Radio communication
    TASK_CABIN_SYSTEMS,          // Lowest priority - Non-critical systems
    TASK_COUNT
} avionics_task_type_t;

// --- Task State Structure ---
typedef struct {
    avionics_task_type_t type;
    char name[32];
    unsigned int priority;          // 0 = highest priority
    unsigned int period_ms;         // How often task runs
    unsigned int deadline_ms;       // Deadline from start time
    unsigned int workload_ms;       // Simulated work time
    
    // Runtime state
    struct timer_list timer;
    ktime_t task_start_time;
    long long last_exec_time_ms;
    bool currently_running;
    bool enabled;
    bool ready_to_run;              // NEW: Indicates task period has expired
    
    // Statistics
    int deadline_met_count;
    int deadline_missed_count;
    int total_executions;
    
    // For future use
    struct list_head ready_list;    // For ready queue
    unsigned long next_deadline_jiffies;
} avionics_task_t;

// --- Execution Logging for Gantt Chart ---
#define MAX_EXEC_LOG_ENTRIES 1000

typedef struct {
    avionics_task_type_t task_type;
    ktime_t start_time;
    ktime_t end_time;
    long long start_time_ms;  // Relative to system start
    long long duration_ms;
    bool deadline_met;
} exec_log_entry_t;

static exec_log_entry_t exec_log[MAX_EXEC_LOG_ENTRIES];
static int exec_log_count = 0;
static ktime_t system_start_time;
static struct timer_list system_timer;
static unsigned int system_runtime_sec = 0;  // 0 = infinite, >0 = limited runtime
static bool system_finished = false;

// --- Global Variables ---
static avionics_task_t avionics_tasks[TASK_COUNT];
static LIST_HEAD(ready_queue);
static spinlock_t scheduler_lock;
static struct timer_list scheduler_timer;
static bool scheduler_running = false;

// Module parameters for task configuration
static unsigned int attitude_period_ms = 100;   // Very frequent - critical
static unsigned int attitude_deadline_ms = 50;
static unsigned int attitude_workload_ms = 30;

static unsigned int engine_period_ms = 200;     // High frequency
static unsigned int engine_deadline_ms = 100;
static unsigned int engine_workload_ms = 60;

static unsigned int nav_period_ms = 500;        // Medium frequency
static unsigned int nav_deadline_ms = 200;
static unsigned int nav_workload_ms = 120;

static unsigned int comm_period_ms = 1000;      // Low frequency
static unsigned int comm_deadline_ms = 400;
static unsigned int comm_workload_ms = 150;

static unsigned int cabin_period_ms = 2000;     // Lowest frequency
static unsigned int cabin_deadline_ms = 800;
static unsigned int cabin_workload_ms = 200;

// Module parameter declarations
module_param(attitude_period_ms, uint, 0644);
MODULE_PARM_DESC(attitude_period_ms, "Flight Attitude Monitor period in ms");
module_param(attitude_deadline_ms, uint, 0644);
MODULE_PARM_DESC(attitude_deadline_ms, "Flight Attitude Monitor deadline in ms");
module_param(attitude_workload_ms, uint, 0644);
MODULE_PARM_DESC(attitude_workload_ms, "Flight Attitude Monitor workload in ms");

module_param(engine_period_ms, uint, 0644);
MODULE_PARM_DESC(engine_period_ms, "Engine Control period in ms");
module_param(engine_deadline_ms, uint, 0644);
MODULE_PARM_DESC(engine_deadline_ms, "Engine Control deadline in ms");
module_param(engine_workload_ms, uint, 0644);
MODULE_PARM_DESC(engine_workload_ms, "Engine Control workload in ms");

module_param(nav_period_ms, uint, 0644);
MODULE_PARM_DESC(nav_period_ms, "Navigation System period in ms");
module_param(nav_deadline_ms, uint, 0644);
MODULE_PARM_DESC(nav_deadline_ms, "Navigation System deadline in ms");
module_param(nav_workload_ms, uint, 0644);
MODULE_PARM_DESC(nav_workload_ms, "Navigation System workload in ms");

module_param(comm_period_ms, uint, 0644);
MODULE_PARM_DESC(comm_period_ms, "Communication System period in ms");
module_param(comm_deadline_ms, uint, 0644);
MODULE_PARM_DESC(comm_deadline_ms, "Communication System deadline in ms");
module_param(comm_workload_ms, uint, 0644);
MODULE_PARM_DESC(comm_workload_ms, "Communication System workload in ms");

module_param(cabin_period_ms, uint, 0644);
MODULE_PARM_DESC(cabin_period_ms, "Cabin Systems period in ms");
module_param(cabin_deadline_ms, uint, 0644);
MODULE_PARM_DESC(cabin_deadline_ms, "Cabin Systems deadline in ms");
module_param(cabin_workload_ms, uint, 0644);
MODULE_PARM_DESC(cabin_workload_ms, "Cabin Systems workload in ms");

// System runtime parameter
module_param(system_runtime_sec, uint, 0644);
MODULE_PARM_DESC(system_runtime_sec, "System runtime in seconds (0=infinite)");

// Helper function to log task execution
static void log_task_execution(avionics_task_t *task, ktime_t start, ktime_t end, bool deadline_met) {
    unsigned long flags;
    long long start_ms, duration_ms;
    
    if (exec_log_count >= MAX_EXEC_LOG_ENTRIES || system_finished) {
        return;
    }
    
    start_ms = div_s64(ktime_to_ns(ktime_sub(start, system_start_time)), 1000000);
    duration_ms = div_s64(ktime_to_ns(ktime_sub(end, start)), 1000000);
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    exec_log[exec_log_count].task_type = task->type;
    exec_log[exec_log_count].start_time = start;
    exec_log[exec_log_count].end_time = end;
    exec_log[exec_log_count].start_time_ms = start_ms;
    exec_log[exec_log_count].duration_ms = duration_ms;
    exec_log[exec_log_count].deadline_met = deadline_met;
    exec_log_count++;
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
}

// --- System Timer Callback ---
static void system_timer_callback(struct timer_list *timer) {
    unsigned long flags;
    int i;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    system_finished = true;
    scheduler_running = false;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Stop all task timers
    for (i = 0; i < TASK_COUNT; i++) {
        del_timer_sync(&avionics_tasks[i].timer);
    }
    del_timer_sync(&scheduler_timer);
    
    printk(KERN_INFO "AvionicsSim: System runtime completed. %d execution events logged.\n", exec_log_count);
}

// Helper function to get current task parameters (reads from module params)
static void update_task_params_from_sysfs(avionics_task_t *task) {
    switch (task->type) {
        case TASK_FLIGHT_ATTITUDE:
            task->period_ms = attitude_period_ms;
            task->deadline_ms = attitude_deadline_ms;
            task->workload_ms = attitude_workload_ms;
            break;
        case TASK_ENGINE_CONTROL:
            task->period_ms = engine_period_ms;
            task->deadline_ms = engine_deadline_ms;
            task->workload_ms = engine_workload_ms;
            break;
        case TASK_NAVIGATION:
            task->period_ms = nav_period_ms;
            task->deadline_ms = nav_deadline_ms;
            task->workload_ms = nav_workload_ms;
            break;
        case TASK_COMMUNICATION:
            task->period_ms = comm_period_ms;
            task->deadline_ms = comm_deadline_ms;
            task->workload_ms = comm_workload_ms;
            break;
        case TASK_CABIN_SYSTEMS:
            task->period_ms = cabin_period_ms;
            task->deadline_ms = cabin_deadline_ms;
            task->workload_ms = cabin_workload_ms;
            break;
        case TASK_COUNT:
        default:
            // No action needed for TASK_COUNT or invalid values
            break;
    }
}

// --- Task Execution Function ---
static void execute_task(avionics_task_t *task) {
    ktime_t current_time, exec_start_time;
    long long exec_duration_ns;
    unsigned long flags;
    bool deadline_met;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    if (!task->enabled || task->currently_running || !task->ready_to_run || system_finished) {
        spin_unlock_irqrestore(&scheduler_lock, flags);
        return;
    }
    
    task->currently_running = true;
    task->ready_to_run = false;  // Task is now executing
    exec_start_time = ktime_get();
    task->task_start_time = exec_start_time;
    task->total_executions++;
    
    // Update parameters from sysfs before execution
    update_task_params_from_sysfs(task);
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Simulate task-specific work
    if (task->workload_ms > 0) {
        mdelay(task->workload_ms);
    }
    
    current_time = ktime_get();
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    task->currently_running = false;
    exec_duration_ns = ktime_to_ns(ktime_sub(current_time, task->task_start_time));
    task->last_exec_time_ms = div_s64(exec_duration_ns, 1000000); // Convert ns to ms
    
    // Check deadline compliance
    deadline_met = (task->last_exec_time_ms <= task->deadline_ms);
    if (deadline_met) {
        task->deadline_met_count++;
    } else {
        task->deadline_missed_count++;
        printk(KERN_WARNING "AvionicsSim: %s DEADLINE MISSED! Time: %lld ms, Deadline: %u ms\n", 
               task->name, task->last_exec_time_ms, task->deadline_ms);
    }
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Log execution for Gantt chart
    log_task_execution(task, exec_start_time, current_time, deadline_met);
}

// --- Priority Scheduler ---
static void priority_scheduler(struct timer_list *timer) {
    avionics_task_t *highest_priority_task = NULL;
    avionics_task_t *current_task;
    unsigned long flags;
    int i;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    if (system_finished) {
        spin_unlock_irqrestore(&scheduler_lock, flags);
        return;
    }
    
    // Find highest priority ready task
    for (i = 0; i < TASK_COUNT; i++) {
        current_task = &avionics_tasks[i];
        
        if (current_task->enabled && !current_task->currently_running && current_task->ready_to_run) {
            if (!highest_priority_task || 
                current_task->priority < highest_priority_task->priority) {
                highest_priority_task = current_task;
            }
        }
    }
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Execute highest priority task
    if (highest_priority_task) {
        execute_task(highest_priority_task);
    }
    
    // Schedule next scheduler run
    if (scheduler_running && !system_finished) {
        mod_timer(&scheduler_timer, jiffies + msecs_to_jiffies(10)); // Schedule every 10ms
    }
}

// --- Individual Task Timer Callbacks (FIXED) ---
static void attitude_task_timer(struct timer_list *timer) {
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    avionics_tasks[TASK_FLIGHT_ATTITUDE].ready_to_run = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Schedule next period
    mod_timer(&avionics_tasks[TASK_FLIGHT_ATTITUDE].timer, 
              jiffies + msecs_to_jiffies(attitude_period_ms));
}

static void engine_task_timer(struct timer_list *timer) {
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    avionics_tasks[TASK_ENGINE_CONTROL].ready_to_run = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    mod_timer(&avionics_tasks[TASK_ENGINE_CONTROL].timer, 
              jiffies + msecs_to_jiffies(engine_period_ms));
}

static void nav_task_timer(struct timer_list *timer) {
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    avionics_tasks[TASK_NAVIGATION].ready_to_run = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    mod_timer(&avionics_tasks[TASK_NAVIGATION].timer, 
              jiffies + msecs_to_jiffies(nav_period_ms));
}

static void comm_task_timer(struct timer_list *timer) {
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    avionics_tasks[TASK_COMMUNICATION].ready_to_run = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    mod_timer(&avionics_tasks[TASK_COMMUNICATION].timer, 
              jiffies + msecs_to_jiffies(comm_period_ms));
}

static void cabin_task_timer(struct timer_list *timer) {
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    avionics_tasks[TASK_CABIN_SYSTEMS].ready_to_run = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    mod_timer(&avionics_tasks[TASK_CABIN_SYSTEMS].timer, 
              jiffies + msecs_to_jiffies(cabin_period_ms));
}

// --- Initialize Task Configurations ---
static void init_avionics_tasks(void) {
    // Flight Attitude Monitor - Highest Priority
    avionics_tasks[TASK_FLIGHT_ATTITUDE] = (avionics_task_t) {
        .type = TASK_FLIGHT_ATTITUDE,
        .priority = 0,
        .period_ms = attitude_period_ms,
        .deadline_ms = attitude_deadline_ms,
        .workload_ms = attitude_workload_ms,
        .enabled = true,
        .currently_running = false,
        .ready_to_run = false,
        .last_exec_time_ms = -1,
        .deadline_met_count = 0,
        .deadline_missed_count = 0,
        .total_executions = 0
    };
    strcpy(avionics_tasks[TASK_FLIGHT_ATTITUDE].name, "Flight Attitude Monitor");
    
    // Engine Control - High Priority  
    avionics_tasks[TASK_ENGINE_CONTROL] = (avionics_task_t) {
        .type = TASK_ENGINE_CONTROL,
        .priority = 1,
        .period_ms = engine_period_ms,
        .deadline_ms = engine_deadline_ms,
        .workload_ms = engine_workload_ms,
        .enabled = true,
        .currently_running = false,
        .ready_to_run = false,
        .last_exec_time_ms = -1,
        .deadline_met_count = 0,
        .deadline_missed_count = 0,
        .total_executions = 0
    };
    strcpy(avionics_tasks[TASK_ENGINE_CONTROL].name, "Engine Control");
    
    // Navigation System - Medium Priority
    avionics_tasks[TASK_NAVIGATION] = (avionics_task_t) {
        .type = TASK_NAVIGATION,
        .priority = 2,
        .period_ms = nav_period_ms,
        .deadline_ms = nav_deadline_ms,
        .workload_ms = nav_workload_ms,
        .enabled = true,
        .currently_running = false,
        .ready_to_run = false,
        .last_exec_time_ms = -1,
        .deadline_met_count = 0,
        .deadline_missed_count = 0,
        .total_executions = 0
    };
    strcpy(avionics_tasks[TASK_NAVIGATION].name, "Navigation System");
    
    // Communication System - Lower Priority
    avionics_tasks[TASK_COMMUNICATION] = (avionics_task_t) {
        .type = TASK_COMMUNICATION,
        .priority = 3,
        .period_ms = comm_period_ms,
        .deadline_ms = comm_deadline_ms,
        .workload_ms = comm_workload_ms,
        .enabled = true,
        .currently_running = false,
        .ready_to_run = false,
        .last_exec_time_ms = -1,
        .deadline_met_count = 0,
        .deadline_missed_count = 0,
        .total_executions = 0
    };
    strcpy(avionics_tasks[TASK_COMMUNICATION].name, "Communication System");
    
    // Cabin Systems - Lowest Priority
    avionics_tasks[TASK_CABIN_SYSTEMS] = (avionics_task_t) {
        .type = TASK_CABIN_SYSTEMS,
        .priority = 4,
        .period_ms = cabin_period_ms,
        .deadline_ms = cabin_deadline_ms,
        .workload_ms = cabin_workload_ms,
        .enabled = true,
        .currently_running = false,
        .ready_to_run = false,
        .last_exec_time_ms = -1,
        .deadline_met_count = 0,
        .deadline_missed_count = 0,
        .total_executions = 0
    };
    strcpy(avionics_tasks[TASK_CABIN_SYSTEMS].name, "Cabin Systems");
}

// --- /proc File Implementation ---
static ssize_t proc_read_avionics_status(struct file *file_ptr, char __user *usr_buf, size_t count, loff_t *ppos) {
    char *buffer;
    int len = 0;
    int i;
    unsigned long flags;
    const char *deadline_result_str;
    const char *status_str;
    
    if (*ppos > 0) {
        return 0;
    }
    
    // Allocate buffer dynamically to reduce stack usage
    buffer = kmalloc(4096, GFP_KERNEL);  // Increased size for execution log
    if (!buffer) {
        return -ENOMEM;
    }
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    len += scnprintf(buffer + len, 4096 - len, "AvionicsSystem: Multi-Task Simulator\n");
    len += scnprintf(buffer + len, 4096 - len, "SchedulerStatus: %s\n", 
                     scheduler_running ? "RUNNING" : "STOPPED");
    len += scnprintf(buffer + len, 4096 - len, "SystemFinished: %s\n", 
                     system_finished ? "YES" : "NO");
    len += scnprintf(buffer + len, 4096 - len, "ExecutionLogCount: %d\n", exec_log_count);
    len += scnprintf(buffer + len, 4096 - len, "SystemRuntimeSec: %u\n", system_runtime_sec);
    len += scnprintf(buffer + len, 4096 - len, "ActiveTasks: %d\n", TASK_COUNT);
    len += scnprintf(buffer + len, 4096 - len, "---\n");
    
    // Output each task's status
    for (i = 0; i < TASK_COUNT; i++) {
        avionics_task_t *task = &avionics_tasks[i];
        
        deadline_result_str = (task->last_exec_time_ms < 0) ? "N/A" : 
                             (task->last_exec_time_ms <= task->deadline_ms) ? "MET" : "MISSED";
        
        status_str = task->currently_running ? "EXECUTING" : (task->enabled ? "READY" : "DISABLED");
        
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Name: %s\n", i, task->name);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Priority: %u\n", i, task->priority);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Period: %u\n", i, task->period_ms);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Deadline: %u\n", i, task->deadline_ms);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Workload: %u\n", i, task->workload_ms);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Status: %s\n", i, status_str);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_LastExecTime: %lld\n", i, 
                         task->last_exec_time_ms < 0 ? 0 : task->last_exec_time_ms);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_LastDeadlineResult: %s\n", i, deadline_result_str);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_MetCount: %d\n", i, task->deadline_met_count);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_MissedCount: %d\n", i, task->deadline_missed_count);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_TotalExecs: %d\n", i, task->total_executions);
        len += scnprintf(buffer + len, 4096 - len, "Task%d_Enabled: %s\n", i, task->enabled ? "YES" : "NO");
        
        if (i < TASK_COUNT - 1) {
            len += scnprintf(buffer + len, 4096 - len, "---\n");
        }
    }
    
    // Add execution log for Gantt chart (last 50 entries to avoid overflow)
    if (exec_log_count > 0) {
        int start_idx = (exec_log_count > 50) ? exec_log_count - 50 : 0;
        len += scnprintf(buffer + len, 4096 - len, "---\nEXECUTION_LOG:\n");
        for (i = start_idx; i < exec_log_count && len < 3800; i++) {
            len += scnprintf(buffer + len, 4096 - len, "EXEC:%d,%lld,%lld,%s\n",
                            exec_log[i].task_type,
                            exec_log[i].start_time_ms,
                            exec_log[i].duration_ms,
                            exec_log[i].deadline_met ? "MET" : "MISSED");
        }
    }
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    if (copy_to_user(usr_buf, buffer, len)) {
        kfree(buffer);
        return -EFAULT;
    }
    
    kfree(buffer);
    *ppos = len;
    return len;
}

static const struct proc_ops avionics_proc_ops = {
    .proc_read = proc_read_avionics_status,
};

// --- Module Initialization and Exit ---
static int __init avionics_sim_init(void) {
    if (!proc_create(PROC_FILENAME, 0444, NULL, &avionics_proc_ops)) {
        printk(KERN_ALERT "AvionicsSim: Failed to create /proc/%s\n", PROC_FILENAME);
        return -ENOMEM;
    }
    
    spin_lock_init(&scheduler_lock);
    init_avionics_tasks();
    
    // Initialize execution logging
    exec_log_count = 0;
    system_finished = false;
    system_start_time = ktime_get();
    
    // Initialize task timers
    timer_setup(&avionics_tasks[TASK_FLIGHT_ATTITUDE].timer, attitude_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_ENGINE_CONTROL].timer, engine_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_NAVIGATION].timer, nav_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_COMMUNICATION].timer, comm_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_CABIN_SYSTEMS].timer, cabin_task_timer, 0);
    
    // Initialize and start scheduler
    timer_setup(&scheduler_timer, priority_scheduler, 0);
    
    // Initialize system timer if runtime is specified
    timer_setup(&system_timer, system_timer_callback, 0);
    
    scheduler_running = true;
    
    // Start all task timers
    mod_timer(&avionics_tasks[TASK_FLIGHT_ATTITUDE].timer, jiffies + msecs_to_jiffies(100));
    mod_timer(&avionics_tasks[TASK_ENGINE_CONTROL].timer, jiffies + msecs_to_jiffies(200));
    mod_timer(&avionics_tasks[TASK_NAVIGATION].timer, jiffies + msecs_to_jiffies(300));
    mod_timer(&avionics_tasks[TASK_COMMUNICATION].timer, jiffies + msecs_to_jiffies(400));
    mod_timer(&avionics_tasks[TASK_CABIN_SYSTEMS].timer, jiffies + msecs_to_jiffies(500));
    
    // Start scheduler
    mod_timer(&scheduler_timer, jiffies + msecs_to_jiffies(10));
    
    // Start system timer if runtime specified
    if (system_runtime_sec > 0) {
        mod_timer(&system_timer, jiffies + msecs_to_jiffies(system_runtime_sec * 1000));
        printk(KERN_INFO "AvionicsSim: System will run for %u seconds\n", system_runtime_sec);
    }
    
    printk(KERN_INFO "AvionicsSim: Multi-Task Module Loaded. %d tasks initialized.\n", TASK_COUNT);
    return 0;
}

static void __exit avionics_sim_exit(void) {
    int i;
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    scheduler_running = false;
    system_finished = true;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Stop all timers
    del_timer_sync(&scheduler_timer);
    del_timer_sync(&system_timer);
    for (i = 0; i < TASK_COUNT; i++) {
        del_timer_sync(&avionics_tasks[i].timer);
    }
    
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "AvionicsSim: Multi-Task Module Unloaded. Logged %d executions.\n", exec_log_count);
}

module_init(avionics_sim_init);
module_exit(avionics_sim_exit); 