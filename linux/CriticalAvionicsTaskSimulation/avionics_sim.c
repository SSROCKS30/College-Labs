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
    }
}

// --- Task Execution Function ---
static void execute_task(avionics_task_t *task) {
    ktime_t current_time;
    long long exec_duration_ns;
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    if (!task->enabled || task->currently_running || !task->ready_to_run) {
        spin_unlock_irqrestore(&scheduler_lock, flags);
        return;
    }
    
    task->currently_running = true;
    task->ready_to_run = false;  // Task is now executing
    task->task_start_time = ktime_get();
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
    if (task->last_exec_time_ms <= task->deadline_ms) {
        task->deadline_met_count++;
    } else {
        task->deadline_missed_count++;
        printk(KERN_WARNING "AvionicsSim: %s DEADLINE MISSED! Time: %lld ms, Deadline: %u ms\n", 
               task->name, task->last_exec_time_ms, task->deadline_ms);
    }
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
}

// --- Priority Scheduler ---
static void priority_scheduler(struct timer_list *timer) {
    avionics_task_t *highest_priority_task = NULL;
    avionics_task_t *current_task;
    unsigned long flags;
    int i;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
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
    if (scheduler_running) {
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
    char buffer[2048];  // Increased buffer size for multiple tasks
    int len = 0;
    int i;
    unsigned long flags;
    char *deadline_result_str;
    
    if (*ppos > 0) {
        return 0;
    }
    
    spin_lock_irqsave(&scheduler_lock, flags);
    
    len += scnprintf(buffer + len, sizeof(buffer) - len, "AvionicsSystem: Multi-Task Simulator\n");
    len += scnprintf(buffer + len, sizeof(buffer) - len, "SchedulerStatus: %s\n", 
                     scheduler_running ? "RUNNING" : "STOPPED");
    len += scnprintf(buffer + len, sizeof(buffer) - len, "ActiveTasks: %d\n", TASK_COUNT);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "---\n");
    
    // Output each task's status
    for (i = 0; i < TASK_COUNT; i++) {
        avionics_task_t *task = &avionics_tasks[i];
        
        if (task->last_exec_time_ms < 0) {
            deadline_result_str = "N/A";
        } else if (task->last_exec_time_ms <= task->deadline_ms) {
            deadline_result_str = "MET";
        } else {
            deadline_result_str = "MISSED";
        }
        
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Name: %s\n", i, task->name);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Priority: %u\n", i, task->priority);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Period: %u\n", i, task->period_ms);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Deadline: %u\n", i, task->deadline_ms);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Workload: %u\n", i, task->workload_ms);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Status: %s\n", i, 
                         task->currently_running ? "EXECUTING" : (task->enabled ? "READY" : "DISABLED"));
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_LastExecTime: %lld\n", i, 
                         task->last_exec_time_ms < 0 ? 0 : task->last_exec_time_ms);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_LastDeadlineResult: %s\n", i, deadline_result_str);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_MetCount: %d\n", i, task->deadline_met_count);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_MissedCount: %d\n", i, task->deadline_missed_count);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_TotalExecs: %d\n", i, task->total_executions);
        len += scnprintf(buffer + len, sizeof(buffer) - len, "Task%d_Enabled: %s\n", i, task->enabled ? "YES" : "NO");
        
        if (i < TASK_COUNT - 1) {
            len += scnprintf(buffer + len, sizeof(buffer) - len, "---\n");
        }
    }
    
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    if (copy_to_user(usr_buf, buffer, len)) {
        return -EFAULT;
    }
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
    
    // Initialize task timers
    timer_setup(&avionics_tasks[TASK_FLIGHT_ATTITUDE].timer, attitude_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_ENGINE_CONTROL].timer, engine_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_NAVIGATION].timer, nav_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_COMMUNICATION].timer, comm_task_timer, 0);
    timer_setup(&avionics_tasks[TASK_CABIN_SYSTEMS].timer, cabin_task_timer, 0);
    
    // Initialize and start scheduler
    timer_setup(&scheduler_timer, priority_scheduler, 0);
    scheduler_running = true;
    
    // Start all task timers
    mod_timer(&avionics_tasks[TASK_FLIGHT_ATTITUDE].timer, jiffies + msecs_to_jiffies(100));
    mod_timer(&avionics_tasks[TASK_ENGINE_CONTROL].timer, jiffies + msecs_to_jiffies(200));
    mod_timer(&avionics_tasks[TASK_NAVIGATION].timer, jiffies + msecs_to_jiffies(300));
    mod_timer(&avionics_tasks[TASK_COMMUNICATION].timer, jiffies + msecs_to_jiffies(400));
    mod_timer(&avionics_tasks[TASK_CABIN_SYSTEMS].timer, jiffies + msecs_to_jiffies(500));
    
    // Start scheduler
    mod_timer(&scheduler_timer, jiffies + msecs_to_jiffies(10));
    
    printk(KERN_INFO "AvionicsSim: Multi-Task Module Loaded. %d tasks initialized.\n", TASK_COUNT);
    return 0;
}

static void __exit avionics_sim_exit(void) {
    int i;
    unsigned long flags;
    
    spin_lock_irqsave(&scheduler_lock, flags);
    scheduler_running = false;
    spin_unlock_irqrestore(&scheduler_lock, flags);
    
    // Stop all timers
    del_timer_sync(&scheduler_timer);
    for (i = 0; i < TASK_COUNT; i++) {
        del_timer_sync(&avionics_tasks[i].timer);
    }
    
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "AvionicsSim: Multi-Task Module Unloaded.\n");
}

module_init(avionics_sim_init);
module_exit(avionics_sim_exit); 