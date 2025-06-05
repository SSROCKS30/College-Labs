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

set_workload:
	@read -p "Enter new workload in ms (e.g., 150): " workload; \
	if [ ! -z "$$workload" ]; then \
	    echo $$workload | sudo tee /sys/module/avionics_sim/parameters/simulated_workload_ms; \
	    echo "Workload set to $$workload ms."; \
	else \
	    echo "No workload entered. Keeping current value."; \
	fi

help:
	@echo "Available targets:"
	@echo "  make all          - Compile the kernel module."
	@echo "  make clean        - Remove compiled files."
	@echo "  make load         - Compile and load the module (requires sudo)."
	@echo "  make unload       - Unload the module (requires sudo)."
	@echo "  make status       - Display the content of /proc/avionics_status."
	@echo "  make log          - Show the last few kernel log messages (dmesg)."
	@echo "  make set_workload - Interactively set the simulated_workload_ms module parameter."
	@echo "  make help         - Show this help message."

.PHONY: all clean load unload status log set_workload help 