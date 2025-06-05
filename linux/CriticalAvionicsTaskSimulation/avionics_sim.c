#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // For copy_to_user
#include <linux/ktime.h>   // For ktime_get, ktime_sub, ktime_to_ns
#include <linux/delay.h>   // For mdelay

// Module metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("AI Assistant (C LKM Version)");
MODULE_DESCRIPTION("Simple Critical Avionics Task Simulator (LKM in C)");

// --- Task Configuration ---
// #define TASK_PERIOD_MS 1000     // Run task every 1 second - Will become a module parameter
// #define TASK_DEADLINE_MS 200    // Task must complete within 200ms - Will become a module parameter
#define PROC_FILENAME "avionics_status"

// --- Global Variables for Task State ---
static struct timer_list avionics_timer;
static ktime_t task_start_time;
static long long last_exec_time_ms = -1; // -1 indicates not run yet or reset
static int deadline_met_count = 0;
static int deadline_missed_count = 0;
static bool task_currently_running = false;

// Module parameter for configurable workload
static unsigned int simulated_workload_ms = 100; // Default workload
module_param(simulated_workload_ms, uint, 0644);
MODULE_PARM_DESC(simulated_workload_ms, "Simulated workload in milliseconds for the avionics task (e.g., 50, 100, 250).");

// Module parameters for configurable period and deadline
static unsigned int task_period_ms = 1000; // Default period: 1 second
module_param(task_period_ms, uint, 0644);
MODULE_PARM_DESC(task_period_ms, "Task period in milliseconds (e.g., 500, 1000, 2000). Module reload might be needed for immediate effect if changed while timer is active.");

static unsigned int task_deadline_ms = 200; // Default deadline: 200ms
module_param(task_deadline_ms, uint, 0644);
MODULE_PARM_DESC(task_deadline_ms, "Task deadline in milliseconds (e.g., 100, 200, 500).");

// --- Timer Callback Function (The Simulated Avionics Task) ---
void avionics_task_callback(struct timer_list *timer) {
    ktime_t current_time;
    long long exec_duration_ns;

    task_currently_running = true;
    task_start_time = ktime_get();

    if (simulated_workload_ms > 0) {
        mdelay(simulated_workload_ms);
    }

    current_time = ktime_get();
    task_currently_running = false;

    exec_duration_ns = ktime_to_ns(ktime_sub(current_time, task_start_time));
    last_exec_time_ms = div_s64(exec_duration_ns, 1000000); // Convert ns to ms

    if (last_exec_time_ms <= task_deadline_ms) {
        deadline_met_count++;
    } else {
        deadline_missed_count++;
        // Optionally, printk for misses to dmesg for easier kernel-side debugging
        // printk(KERN_WARNING "AvionicsSim: Task Time: %lld ms. DEADLINE MISSED! (Deadline: %u ms)\n", last_exec_time_ms, task_deadline_ms);
    }

    mod_timer(&avionics_timer, jiffies + msecs_to_jiffies(task_period_ms));
}

// --- /proc File Implementation ---
static ssize_t proc_read_avionics_status(struct file *file_ptr, char __user *usr_buf, size_t count, loff_t *ppos) {
    char buffer[512];
    int len = 0;
    char *deadline_result_str;

    if (*ppos > 0) {
        return 0;
    }

    if (last_exec_time_ms < 0) {
        deadline_result_str = "N/A";
    } else if (last_exec_time_ms <= task_deadline_ms) {
        deadline_result_str = "MET";
    } else {
        deadline_result_str = "MISSED";
    }

    len += scnprintf(buffer + len, sizeof(buffer) - len, "TaskName: Flight Attitude Monitor\n");
    len += scnprintf(buffer + len, sizeof(buffer) - len, "PeriodMS: %u\n", task_period_ms);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "DeadlineMS: %u\n", task_deadline_ms);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "CurrentWorkloadMS: %u\n", simulated_workload_ms);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "TaskStatus: %s\n", task_currently_running ? "EXECUTING" : "IDLE");
    len += scnprintf(buffer + len, sizeof(buffer) - len, "LastExecTimeMS: %lld\n", last_exec_time_ms < 0 ? 0 : last_exec_time_ms);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "LastDeadlineResult: %s\n", deadline_result_str);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "MetCount: %d\n", deadline_met_count);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "MissedCount: %d\n", deadline_missed_count);
    len += scnprintf(buffer + len, sizeof(buffer) - len, "ModuleStatus: Loaded\n");

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

    timer_setup(&avionics_timer, avionics_task_callback, 0);
    mod_timer(&avionics_timer, jiffies + msecs_to_jiffies(task_period_ms));

    printk(KERN_INFO "AvionicsSim: Module Loaded. /proc/%s created. Period: %u ms, Deadline: %u ms, Workload: %u ms.\n",
           PROC_FILENAME, task_period_ms, task_deadline_ms, simulated_workload_ms);
    return 0;
}

static void __exit avionics_sim_exit(void) {
    del_timer_sync(&avionics_timer);
    remove_proc_entry(PROC_FILENAME, NULL);
    printk(KERN_INFO "AvionicsSim: Module Unloaded.\n");
}

module_init(avionics_sim_init);
module_exit(avionics_sim_exit); 