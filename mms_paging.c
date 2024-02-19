#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mt19937.h>

#define VIRTUAL_MEMORY_SIZE 4 * 1024 * 1024 //bytes (4 MB)
#define PHYSICAL_MEMORY_SIZE 1 * 1024 * 1024 //bytes (1 MB)

#define PAGE_SIZE 4096 //bytes

// function prototypes
int translate_address(int virtual_address);
int allocate_page();
void deallocate_page(int frame_number);
void handle_page_fault(int virtual_address);

// struct for page table
typedef struct {
  int valid;
  int frame_number;
} page_table_entry;

int *physical_memory;

page_table_entry *page_table;
page_table = malloc(VIRTUAL_MEMORY_SIZE / PAGE_SIZE * sizeof(page_table_entry));
physical_memory = malloc(PHYSICAL_MEMORY_SIZE);
free_frames = physical_memory_size / PAGE_SIZE;

int translate_address(int virtual_address) {
  int page_number = virtual_address / PAGE_SIZE;
  int offset = virtual_address % PAGE_SIZE;

  if (page_table[page_number].valid) {
    int physical_address = page_table[page_number].frame_number * PAGE_SIZE + offset;
    return physical_address; // Successfully translated address
  } else {
    // Handle page fault here (e.g., call handle_page_fault(virtual_address))
    return -1; // Indicate error or incomplete address
  }
}

int allocate_page() {
  if (free_frames <= 0) {
    // Handle no free frames (e.g., implement page replacement)
    return -1; // Indicate failure
  }

  // Find a free frame and mark it as occupied
  for (int i = 0; i < PHYSICAL_MEMORY_SIZE / PAGE_SIZE; i++) {
    if (!page_table[i].valid) {
      free_frames--;
      return i; // Return frame number
    }
  }

  // No free frames found (shouldn't reach here)
  return -1;
}

void deallocate_page(int frame_number) {
  page_table[frame_number].valid = 0;
  page_table[frame_number].frame_number = -1; // Optional: clear frame number
  free_frames++;
}

void handle_page_fault(int virtual_address) {
  printf("Page fault for virtual address: %d\n", virtual_address);

  // Select a victim frame for page replacement (consider your chosen algorithm)
  int victim_frame = select_victim_frame();

  // If replacement is necessary, evict the victim page
  if (victim_frame != -1) {
    if (page_table[victim_frame].dirty) {
      // Simulate writing dirty page back to secondary storage
      printf("Writing dirty page %d back to storage\n", victim_frame);
    }
    deallocate_page(victim_frame);
  }

  // Allocate a new frame for the faulted page and update page table
  int new_frame = allocate_page();
  if (new_frame == -1) {
    // Handle memory allocation failure
    return;
  }
  page_table[virtual_address / PAGE_SIZE].valid = 1;
  page_table[virtual_address / PAGE_SIZE].frame_number = new_frame;

  // Simulate loading the page from secondary storage
  printf("Loading page into frame: %d\n", new_frame);

  // Retry memory access using the translated address
  translate_address(virtual_address);
}

void create_process(int process_id, int memory_size) {
  // Check if sufficient memory available for new process
  if (memory_size > free_frames * PAGE_SIZE) {
    printf("Insufficient memory for process %d\n", process_id);
    return;
  }

  // Allocate memory for process pages
  int allocated_pages = 0;
  while (allocated_pages < memory_size / PAGE_SIZE) {
    int frame_number = allocate_page();
    if (frame_number == -1) {
      printf("Error allocating memory for process %d\n", process_id);
      break; // Handle allocation failure
    }
    // Update page table entry for the process
    page_table[process_id * memory_size / PAGE_SIZE + allocated_pages].valid = 1;
    page_table[process_id * memory_size / PAGE_SIZE + allocated_pages].frame_number = frame_number;
    allocated_pages++;
  }

  // Simulate process execution (e.g., random memory accesses)
  for (int i = 0; i < 100; i++) {
    int virtual_address = process_id * memory_size / PAGE_SIZE * PAGE_SIZE + rand() % (memory_size / PAGE_SIZE) * PAGE_SIZE;
    translate_address(virtual_address);
  }

  // Deallocate process memory
  for (int i = 0; i < allocated_pages; i++) {
    int frame_number = page_table[process_id * memory_size / PAGE_SIZE + i].frame_number;
    deallocate_page(frame_number);
  }
}

