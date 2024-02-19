#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VIRTUAL_MEMORY_SIZE 4 * 1024 * 1024 //bytes (4 MB)
#define PHYSICAL_MEMORY_SIZE 1 * 1024 * 1024 //bytes (1 MB)

#define PAGE_SIZE 4096 //bytes

// struct for page table
typedef struct {
  int valid;
  int frame_number;
} page_table_entry;

int *physical_memory;

page_table_entry *page_table;
page_table = malloc(VIRTUAL_MEMORY_SIZE / PAGE_SIZE * sizeof(page_table_entry));
physical_memory = malloc(PHYSICAL_MEMORY_SIZE);


// function prototypes
int translate_address(int virtual_address);
int allocate_page();
void deallocate_page(int frame_number);
void handle_page_fault(int virtual_address);