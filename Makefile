CC = gcc
CFLAGS = -Wall
TARGET = final_mmu_paging

# Default target
# all: final_mmu_paging

# Linking the final executable
final_mmu_paging: final_mmu_paging.c
	$(CC) $(CFLAGS) -o final_mmu_paging final_mmu_paging.c

# Clean up generated files
clean:
	rm -f final_mmu_paging

# Example targets for running the program with different workloads
run_option1_small:
	./final_mmu_paging 1 5 # Option 1 with 5 processes

run_option1_large:
	./final_mmu_paging 1 10 # Option 1 with 10 processes

run_option2_small:
	./$(TARGET) 2 5 # Option 2 with 5 processes

run_option2_large:
	./$(TARGET) 2 10 # Option 2 with 10 processes