obj-m += avionics_sim.o

KVERSION ?= $(shell uname -r)
KDIR := /lib/modules/$(KVERSION)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

load: all
	sudo insmod avionics_sim.ko

unload:
	sudo rmmod avionics_sim || true # Ignore error if not loaded

status:
	cat /proc/avionics_status || echo "Module not loaded or /proc file not found."

log:
	dmesg | tail

# Individual task parameter controls
set_attitude_workload:
	@read -p "Enter new Flight Attitude Monitor workload in ms (current: $$(cat /sys/module/avionics_sim/parameters/attitude_workload_ms 2>/dev/null || echo 'N/A')): " workload; \
	if [ ! -z "$$workload" ] && [ "$$workload" -gt 0 ] 2>/dev/null; then \
	    echo $$workload | sudo tee /sys/module/avionics_sim/parameters/attitude_workload_ms > /dev/null; \
	    echo "Flight Attitude Monitor workload set to $$workload ms."; \
	else \
	    echo "Invalid input. Please enter a positive number."; \
	fi

set_engine_workload:
	@read -p "Enter new Engine Control workload in ms (current: $$(cat /sys/module/avionics_sim/parameters/engine_workload_ms 2>/dev/null || echo 'N/A')): " workload; \
	if [ ! -z "$$workload" ] && [ "$$workload" -gt 0 ] 2>/dev/null; then \
	    echo $$workload | sudo tee /sys/module/avionics_sim/parameters/engine_workload_ms > /dev/null; \
	    echo "Engine Control workload set to $$workload ms."; \
	else \
	    echo "Invalid input. Please enter a positive number."; \
	fi

set_nav_workload:
	@read -p "Enter new Navigation System workload in ms (current: $$(cat /sys/module/avionics_sim/parameters/nav_workload_ms 2>/dev/null || echo 'N/A')): " workload; \
	if [ ! -z "$$workload" ] && [ "$$workload" -gt 0 ] 2>/dev/null; then \
	    echo $$workload | sudo tee /sys/module/avionics_sim/parameters/nav_workload_ms > /dev/null; \
	    echo "Navigation System workload set to $$workload ms."; \
	else \
	    echo "Invalid input. Please enter a positive number."; \
	fi

# Stress testing commands
stress_test_light:
	@echo "Applying light stress test (increase workloads by 50%)..."
	@current_attitude=$$(cat /sys/module/avionics_sim/parameters/attitude_workload_ms 2>/dev/null || echo 30); \
	current_engine=$$(cat /sys/module/avionics_sim/parameters/engine_workload_ms 2>/dev/null || echo 60); \
	current_nav=$$(cat /sys/module/avionics_sim/parameters/nav_workload_ms 2>/dev/null || echo 120); \
	new_attitude=$$((current_attitude * 3 / 2)); \
	new_engine=$$((current_engine * 3 / 2)); \
	new_nav=$$((current_nav * 3 / 2)); \
	echo $$new_attitude | sudo tee /sys/module/avionics_sim/parameters/attitude_workload_ms > /dev/null; \
	echo $$new_engine | sudo tee /sys/module/avionics_sim/parameters/engine_workload_ms > /dev/null; \
	echo $$new_nav | sudo tee /sys/module/avionics_sim/parameters/nav_workload_ms > /dev/null; \
	echo "Light stress test applied. Monitor /proc/avionics_status for deadline misses."

stress_test_heavy:
	@echo "Applying heavy stress test (triple all workloads)..."
	@current_attitude=$$(cat /sys/module/avionics_sim/parameters/attitude_workload_ms 2>/dev/null || echo 30); \
	current_engine=$$(cat /sys/module/avionics_sim/parameters/engine_workload_ms 2>/dev/null || echo 60); \
	current_nav=$$(cat /sys/module/avionics_sim/parameters/nav_workload_ms 2>/dev/null || echo 120); \
	new_attitude=$$((current_attitude * 3)); \
	new_engine=$$((current_engine * 3)); \
	new_nav=$$((current_nav * 3)); \
	echo $$new_attitude | sudo tee /sys/module/avionics_sim/parameters/attitude_workload_ms > /dev/null; \
	echo $$new_engine | sudo tee /sys/module/avionics_sim/parameters/engine_workload_ms > /dev/null; \
	echo $$new_nav | sudo tee /sys/module/avionics_sim/parameters/nav_workload_ms > /dev/null; \
	echo "Heavy stress test applied. Expect deadline misses!"

reset_defaults:
	@echo "Resetting all task parameters to defaults..."
	@echo 30 | sudo tee /sys/module/avionics_sim/parameters/attitude_workload_ms > /dev/null
	@echo 60 | sudo tee /sys/module/avionics_sim/parameters/engine_workload_ms > /dev/null
	@echo 120 | sudo tee /sys/module/avionics_sim/parameters/nav_workload_ms > /dev/null
	@echo 150 | sudo tee /sys/module/avionics_sim/parameters/comm_workload_ms > /dev/null
	@echo 200 | sudo tee /sys/module/avionics_sim/parameters/cabin_workload_ms > /dev/null
	@echo "All parameters reset to default values."

# System monitoring
show_params:
	@echo "=== Current Task Parameters ==="
	@echo "Flight Attitude Monitor:"
	@echo "  Period: $$(cat /sys/module/avionics_sim/parameters/attitude_period_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Deadline: $$(cat /sys/module/avionics_sim/parameters/attitude_deadline_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Workload: $$(cat /sys/module/avionics_sim/parameters/attitude_workload_ms 2>/dev/null || echo 'N/A') ms"
	@echo ""
	@echo "Engine Control:"
	@echo "  Period: $$(cat /sys/module/avionics_sim/parameters/engine_period_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Deadline: $$(cat /sys/module/avionics_sim/parameters/engine_deadline_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Workload: $$(cat /sys/module/avionics_sim/parameters/engine_workload_ms 2>/dev/null || echo 'N/A') ms"
	@echo ""
	@echo "Navigation System:"
	@echo "  Period: $$(cat /sys/module/avionics_sim/parameters/nav_period_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Deadline: $$(cat /sys/module/avionics_sim/parameters/nav_deadline_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Workload: $$(cat /sys/module/avionics_sim/parameters/nav_workload_ms 2>/dev/null || echo 'N/A') ms"
	@echo ""
	@echo "Communication System:"
	@echo "  Period: $$(cat /sys/module/avionics_sim/parameters/comm_period_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Deadline: $$(cat /sys/module/avionics_sim/parameters/comm_deadline_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Workload: $$(cat /sys/module/avionics_sim/parameters/comm_workload_ms 2>/dev/null || echo 'N/A') ms"
	@echo ""
	@echo "Cabin Systems:"
	@echo "  Period: $$(cat /sys/module/avionics_sim/parameters/cabin_period_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Deadline: $$(cat /sys/module/avionics_sim/parameters/cabin_deadline_ms 2>/dev/null || echo 'N/A') ms"
	@echo "  Workload: $$(cat /sys/module/avionics_sim/parameters/cabin_workload_ms 2>/dev/null || echo 'N/A') ms"

monitor:
	@echo "Starting continuous monitoring (Press Ctrl+C to stop)..."
	@while true; do \
		clear; \
		echo "=== Avionics Multi-Task Monitor ==="; \
		echo "Time: $$(date)"; \
		echo ""; \
		cat /proc/avionics_status 2>/dev/null || echo "Module not loaded"; \
		echo ""; \
		echo "=== Recent Kernel Messages ==="; \
		dmesg | grep AvionicsSim | tail -3; \
		sleep 2; \
	done

# GUI shortcuts
gui:
	python3 avionics_gui.py

gui_sudo:
	sudo python3 avionics_gui.py

# Quick demo sequence
demo:
	@echo "=== Avionics Multi-Task Demo ==="
	@echo "1. Loading module..."
	@make load
	@sleep 2
	@echo ""
	@echo "2. Initial status:"
	@make status
	@echo ""
	@echo "3. Applying light stress test in 3 seconds..."
	@sleep 3
	@make stress_test_light
	@echo ""
	@echo "4. Checking status after stress test..."
	@make status
	@echo ""
	@echo "5. Resetting to defaults..."
	@make reset_defaults
	@echo ""
	@echo "Demo complete! Use 'make gui' to launch the graphical interface."
	@echo "To continuously monitor, run: make monitor"

help:
	@echo "Available targets for Multi-Task Avionics Simulator:"
	@echo ""
	@echo "Build & Module Management:"
	@echo "  make all              - Compile the kernel module"
	@echo "  make clean            - Remove compiled files"
	@echo "  make load             - Compile and load the module (requires sudo)"
	@echo "  make unload           - Unload the module (requires sudo)"
	@echo ""
	@echo "Monitoring:"
	@echo "  make status           - Display current task status"
	@echo "  make show_params      - Show all task parameters"
	@echo "  make monitor          - Continuous monitoring (Ctrl+C to stop)"
	@echo "  make log              - Show recent kernel messages"
	@echo ""
	@echo "Task Control:"
	@echo "  make set_attitude_workload - Set Flight Attitude Monitor workload"
	@echo "  make set_engine_workload   - Set Engine Control workload"
	@echo "  make set_nav_workload      - Set Navigation System workload"
	@echo ""
	@echo "Testing:"
	@echo "  make stress_test_light - Apply 50% workload increase"
	@echo "  make stress_test_heavy - Triple all workloads"
	@echo "  make reset_defaults    - Reset all parameters to defaults"
	@echo "  make demo              - Run complete demo sequence"
	@echo ""
	@echo "GUI:"
	@echo "  make gui               - Launch Python GUI (read-only)"
	@echo "  make gui_sudo          - Launch Python GUI with parameter control"
	@echo ""
	@echo "Help:"
	@echo "  make help              - Show this help message"

.PHONY: all clean load unload status log set_attitude_workload set_engine_workload set_nav_workload stress_test_light stress_test_heavy reset_defaults show_params monitor gui gui_sudo demo help 