#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // For bool type
#include <ctype.h>   // For isspace

#define PROC_FILE_PATH "/proc/avionics_status"
#define GUI_UPDATE_INTERVAL_MS 250 // Update GUI every 250ms

// --- Struct to hold GUI widgets ---
typedef struct {
    GtkWidget *window;
    GtkWidget *grid;

    GtkWidget *task_name_label;
    GtkWidget *task_name_value;
    GtkWidget *period_label;
    GtkWidget *period_value;
    GtkWidget *deadline_label;
    GtkWidget *deadline_value;
    GtkWidget *workload_label;
    GtkWidget *workload_value;
    GtkWidget *task_status_label;
    GtkWidget *task_status_value;
    GtkWidget *last_exec_time_label;
    GtkWidget *last_exec_time_value;
    GtkWidget *last_deadline_result_label;
    GtkWidget *last_deadline_result_value;
    GtkWidget *met_count_label;
    GtkWidget *met_count_value;
    GtkWidget *missed_count_label;
    GtkWidget *missed_count_value;
    GtkWidget *module_status_label;
    GtkWidget *module_status_value;

    GtkWidget *status_bar; // For messages like "File not found"
    guint status_bar_context_id;

    GtkCssProvider *css_provider; // For styling
} AvionicsGUI;

// --- Helper function to trim leading/trailing whitespace ---
char *trim_whitespace(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

// --- Function to read /proc/avionics_status and update GUI ---
gboolean update_gui_from_proc(gpointer data) {
    AvionicsGUI *gui = (AvionicsGUI *)data;
    FILE *fp;
    char line[256];
    char key[64], value[192]; // Increased value buffer

    char task_name[128] = "N/A";
    char period_ms[32] = "N/A";
    char deadline_ms[32] = "N/A";
    char current_workload_ms[32] = "N/A";
    char task_status[64] = "N/A";
    char last_exec_time_ms[32] = "N/A";
    char last_deadline_result[32] = "N/A";
    char met_count[32] = "N/A";
    char missed_count[32] = "N/A";
    char module_status[64] = "Waiting..."; // Default if file not found or empty

    fp = fopen(PROC_FILE_PATH, "r");
    if (fp == NULL) {
        gtk_statusbar_push(GTK_STATUSBAR(gui->status_bar), gui->status_bar_context_id, "Error: /proc/avionics_status not found. Is the LKM loaded?");
        // Keep previous values or set to N/A if first time
        gtk_label_set_text(GTK_LABEL(gui->task_name_value), task_name);
        gtk_label_set_text(GTK_LABEL(gui->period_value), period_ms);
        gtk_label_set_text(GTK_LABEL(gui->deadline_value), deadline_ms);
        gtk_label_set_text(GTK_LABEL(gui->workload_value), current_workload_ms);
        gtk_label_set_text(GTK_LABEL(gui->task_status_value), task_status);
        gtk_label_set_text(GTK_LABEL(gui->last_exec_time_value), last_exec_time_ms);
        gtk_label_set_text(GTK_LABEL(gui->last_deadline_result_value), last_deadline_result);
        gtk_label_set_text(GTK_LABEL(gui->met_count_value), met_count);
        gtk_label_set_text(GTK_LABEL(gui->missed_count_value), missed_count);
        gtk_label_set_text(GTK_LABEL(gui->module_status_value), module_status); // Show "Waiting..." or specific error
        return G_SOURCE_CONTINUE; // Keep timer running
    }

    gtk_statusbar_pop(GTK_STATUSBAR(gui->status_bar), gui->status_bar_context_id); // Clear previous messages

    bool proc_file_empty = true;
    while (fgets(line, sizeof(line), fp)) {
        proc_file_empty = false;
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0'; // Split key and value
            strncpy(key, line, sizeof(key) - 1);
            key[sizeof(key) - 1] = '\0';

            strncpy(value, colon + 1, sizeof(value) - 1);
            value[sizeof(value) - 1] = '\0';
            char *trimmed_value = trim_whitespace(value); // Trim newline and spaces

            if (strcmp(key, "TaskName") == 0) strncpy(task_name, trimmed_value, sizeof(task_name) -1);
            else if (strcmp(key, "PeriodMS") == 0) strncpy(period_ms, trimmed_value, sizeof(period_ms) -1);
            else if (strcmp(key, "DeadlineMS") == 0) strncpy(deadline_ms, trimmed_value, sizeof(deadline_ms) -1);
            else if (strcmp(key, "CurrentWorkloadMS") == 0) strncpy(current_workload_ms, trimmed_value, sizeof(current_workload_ms) -1);
            else if (strcmp(key, "TaskStatus") == 0) strncpy(task_status, trimmed_value, sizeof(task_status) -1);
            else if (strcmp(key, "LastExecTimeMS") == 0) strncpy(last_exec_time_ms, trimmed_value, sizeof(last_exec_time_ms) -1);
            else if (strcmp(key, "LastDeadlineResult") == 0) strncpy(last_deadline_result, trimmed_value, sizeof(last_deadline_result) -1);
            else if (strcmp(key, "MetCount") == 0) strncpy(met_count, trimmed_value, sizeof(met_count) -1);
            else if (strcmp(key, "MissedCount") == 0) strncpy(missed_count, trimmed_value, sizeof(missed_count) -1);
            else if (strcmp(key, "ModuleStatus") == 0) strncpy(module_status, trimmed_value, sizeof(module_status) -1);
        }
    }
    fclose(fp);

    if (proc_file_empty) {
         gtk_statusbar_push(GTK_STATUSBAR(gui->status_bar), gui->status_bar_context_id, "Warning: /proc/avionics_status is empty or unreadable.");
         // Set to N/A if file was empty
        gtk_label_set_text(GTK_LABEL(gui->module_status_value), "Module Data Missing");
    } else {
        gtk_label_set_text(GTK_LABEL(gui->module_status_value), module_status);
    }


    // Update GUI labels
    gtk_label_set_text(GTK_LABEL(gui->task_name_value), task_name);
    gtk_label_set_text(GTK_LABEL(gui->period_value), period_ms);
    gtk_label_set_text(GTK_LABEL(gui->deadline_value), deadline_ms);
    gtk_label_set_text(GTK_LABEL(gui->workload_value), current_workload_ms);
    gtk_label_set_text(GTK_LABEL(gui->task_status_value), task_status);
    gtk_label_set_text(GTK_LABEL(gui->last_exec_time_value), last_exec_time_ms);
    gtk_label_set_text(GTK_LABEL(gui->last_deadline_result_value), last_deadline_result);
    gtk_label_set_text(GTK_LABEL(gui->met_count_value), met_count);
    gtk_label_set_text(GTK_LABEL(gui->missed_count_value), missed_count);

    // Dynamically style the deadline result
    const char *style_class = "default-text"; // Default style
    if (strcmp(last_deadline_result, "MET") == 0) {
        style_class = "deadline-met";
    } else if (strcmp(last_deadline_result, "MISSED") == 0) {
        style_class = "deadline-missed";
    } else if (strcmp(last_deadline_result, "N/A") == 0) {
         style_class = "deadline-na";
    }

    GtkStyleContext *context = gtk_widget_get_style_context(gui->last_deadline_result_value);
    // Remove previous classes before adding a new one
    if (gtk_style_context_has_class(context, "deadline-met")) {
        gtk_style_context_remove_class(context, "deadline-met");
    }
    if (gtk_style_context_has_class(context, "deadline-missed")) {
        gtk_style_context_remove_class(context, "deadline-missed");
    }
     if (gtk_style_context_has_class(context, "deadline-na")) {
        gtk_style_context_remove_class(context, "deadline-na");
    }
    gtk_style_context_add_class(context, style_class);


    // Style for Task Status
    GtkStyleContext *status_context = gtk_widget_get_style_context(gui->task_status_value);
    const char *status_style_class = "status-idle"; // Default
    if (strcmp(task_status, "EXECUTING") == 0) {
        status_style_class = "status-executing";
    }
     // Remove previous classes
    if (gtk_style_context_has_class(status_context, "status-idle")) {
        gtk_style_context_remove_class(status_context, "status-idle");
    }
    if (gtk_style_context_has_class(status_context, "status-executing")) {
        gtk_style_context_remove_class(status_context, "status-executing");
    }
    gtk_style_context_add_class(status_context, status_style_class);


    return G_SOURCE_CONTINUE; // Keep timer running
}

// --- Function to apply CSS ---
void apply_css(GtkWidget *widget, GtkCssProvider *provider) {
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    if (GTK_IS_CONTAINER(widget)) {
        gtk_container_forall(GTK_CONTAINER(widget), (GtkCallback)apply_css, provider);
    }
}

// --- Callback for window destroy ---
static void on_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}


// --- Main function ---
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AvionicsGUI gui;

    // Create the main window
    gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gui.window), "Avionics Task Monitor");
    gtk_window_set_default_size(GTK_WINDOW(gui.window), 450, 450); // Adjusted size
    gtk_container_set_border_width(GTK_CONTAINER(gui.window), 15);
    g_signal_connect(gui.window, "destroy", G_CALLBACK(on_destroy), NULL);

    // Create a grid to arrange widgets
    gui.grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(gui.grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(gui.grid), 8); // Slightly reduced row spacing
    gtk_container_add(GTK_CONTAINER(gui.window), gui.grid);

    // --- Create and add labels and value placeholders ---
    // Row 0
    gui.task_name_label = gtk_label_new("Task Name:");
    gui.task_name_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.task_name_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.task_name_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.task_name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.task_name_value, 1, 0, 1, 1);

    // Row 1
    gui.period_label = gtk_label_new("Period (ms):");
    gui.period_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.period_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.period_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.period_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.period_value, 1, 1, 1, 1);

    // Row 2
    gui.deadline_label = gtk_label_new("Deadline (ms):");
    gui.deadline_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.deadline_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.deadline_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.deadline_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.deadline_value, 1, 2, 1, 1);

    // Row 3
    gui.workload_label = gtk_label_new("Current Workload (ms):");
    gui.workload_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.workload_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.workload_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.workload_label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.workload_value, 1, 3, 1, 1);

    // Row 4
    gui.task_status_label = gtk_label_new("Task Status:");
    gui.task_status_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.task_status_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.task_status_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.task_status_label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.task_status_value, 1, 4, 1, 1);
    gtk_widget_set_name(gui.task_status_value, "task_status_value"); // For CSS

    // Row 5
    gui.last_exec_time_label = gtk_label_new("Last Execution Time (ms):");
    gui.last_exec_time_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.last_exec_time_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.last_exec_time_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.last_exec_time_label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.last_exec_time_value, 1, 5, 1, 1);

    // Row 6
    gui.last_deadline_result_label = gtk_label_new("Last Deadline Result:");
    gui.last_deadline_result_value = gtk_label_new("N/A");
    gtk_widget_set_halign(gui.last_deadline_result_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.last_deadline_result_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.last_deadline_result_label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.last_deadline_result_value, 1, 6, 1, 1);
    gtk_widget_set_name(gui.last_deadline_result_value, "deadline_result_value"); // For CSS

    // Row 7
    gui.met_count_label = gtk_label_new("Deadlines Met:");
    gui.met_count_value = gtk_label_new("0");
    gtk_widget_set_halign(gui.met_count_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.met_count_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.met_count_label, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.met_count_value, 1, 7, 1, 1);

    // Row 8
    gui.missed_count_label = gtk_label_new("Deadlines Missed:");
    gui.missed_count_value = gtk_label_new("0");
    gtk_widget_set_halign(gui.missed_count_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.missed_count_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.missed_count_label, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.missed_count_value, 1, 8, 1, 1);

    // Row 9
    gui.module_status_label = gtk_label_new("Module Status:");
    gui.module_status_value = gtk_label_new("Waiting...");
    gtk_widget_set_halign(gui.module_status_label, GTK_ALIGN_START);
    gtk_widget_set_halign(gui.module_status_value, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.module_status_label, 0, 9, 1, 1);
    gtk_grid_attach(GTK_GRID(gui.grid), gui.module_status_value, 1, 9, 1, 1);


    // Add a separator
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(gui.grid), separator, 0, 10, 2, 1); // Span 2 columns

    // Add status bar
    gui.status_bar = gtk_statusbar_new();
    gui.status_bar_context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(gui.status_bar), "AvionicsStatus");
    gtk_grid_attach(GTK_GRID(gui.grid), gui.status_bar, 0, 11, 2, 1); // Span 2 columns

    // Load CSS
    gui.css_provider = gtk_css_provider_new();
    const char *css_data =
        "label { font-size: 11pt; }"
        "#deadline_result_value.deadline-met { color: #2ECC71; font-weight: bold; }"      // Green
        "#deadline_result_value.deadline-missed { color: #E74C3C; font-weight: bold; }"  // Red
        "#deadline_result_value.deadline-na { color: #95A5A6; font-style: italic; }"    // Gray
        "#task_status_value.status-executing { color: #F39C12; font-weight: bold; }" // Orange
        "#task_status_value.status-idle { color: #3498DB; }"        // Blue
        "grid label:first-child { font-weight: bold; padding-right: 10px; }" // Make key labels bold
        "window { background-color: #F4F6F6; }" // Light background for the window
        "grid { margin: 5px; }";


    GError *error = NULL;
    gtk_css_provider_load_from_data(gui.css_provider, css_data, -1, &error);
    if (error) {
        g_warning("Failed to load CSS: %s", error->message);
        g_error_free(error);
    } else {
        // Apply CSS to the main window and its children recursively
        GdkScreen *screen = gtk_widget_get_screen(gui.window); // Deprecated in GTK4, use GdkDisplay
        gtk_style_context_add_provider_for_screen(screen,
                                                  GTK_STYLE_PROVIDER(gui.css_provider),
                                                  GTK_STYLE_PROVIDER_PRIORITY_USER);
        // For GTK4, you might use:
        // GdkDisplay *display = gtk_widget_get_display(gui.window);
        // gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(gui.css_provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }


    // Initial call to update GUI and start timer
    update_gui_from_proc(&gui);
    g_timeout_add(GUI_UPDATE_INTERVAL_MS, update_gui_from_proc, &gui);

    gtk_widget_show_all(gui.window);
    gtk_main();

    // Clean up CSS provider
    g_object_unref(gui.css_provider);

    return 0;
} 