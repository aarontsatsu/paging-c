/**
 * @authors Aaron Tsatsu Tamakloe, John Manful, Bernd Opoku-Boadu
 * @file paging.c
 * @brief Implements a simple paging system simulation.
 *
 * This program simulates a paging system with virtual memory and physical memory.
 * It includes functions to create processes, allocate memory to processes, access memory,
 * and deallocate processes. The program allows the user to choose between two options:
 * 1. Create processes and allocate memory.
 * 2. Simulate memory accesses.
 *
 * The program uses a page table to map virtual addresses to physical addresses,
 * and a frame table to keep track of the allocation status of physical memory frames.
 * Page replacement is triggered when there are no free frames available for allocation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define VIRTUAL_MEMORY_SIZE 65536
#define PHYSICAL_MEMORY_SIZE 16384
#define PAGE_SIZE 1024
#define NUM_PAGES (VIRTUAL_MEMORY_SIZE / PAGE_SIZE)
#define NUM_FRAMES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)
#define TABLE_SIZE 128
#define MAX_PROCESSES 10
#define MAX_PAGES_PER_PROCESS 5
#define NUM_MEMORY_ACCESSES 10
#define TLB_SIZE 16

typedef struct PageTableEntry {
    int virtualPageNumber;
    int frameNumber;
    struct PageTableEntry* next;
} PageTableEntry;

typedef struct HashedPageTable {
    PageTableEntry* buckets[TABLE_SIZE];
} HashedPageTable;

typedef struct Frame {
    int occupied;
} Frame;

typedef struct ProcessStats {
    int pageFaults;
    int totalAccesses;
    float hitRate;
    int tlbHits;
} ProcessStats;

typedef struct Process {
    int pid;
    int numPages;
    int memorySize; // Added memory size for each process
    ProcessStats stats;
} Process;

typedef struct TLBEntry{
    int pageNumber;
    int frameNumber;
    bool valid;
} TLBEntry;

Frame frames[NUM_FRAMES];
TLBEntry tlb[TLB_SIZE];
int tlbIndex = 0;



// Function prototypes
unsigned int hashFunction(int virtualPageNumber);
void initializeFrames();
void initializeTLB();
void initializePageTable(HashedPageTable* pageTable);
void allocatePage(HashedPageTable* pageTable, int virtualPageNumber, int frameNumber);
int translateAddress(HashedPageTable* pageTable, int virtualAddress, int* physicalAddress, Process* process);
void handlePageFault(HashedPageTable* pageTable, int virtualPageNumber, Process* process);
int findFreeFrame();
void createProcesses(Process processes[], int numProcesses, HashedPageTable pageTables[], int option);
void simulateMemoryAccess(Process* processes, int numProcesses, HashedPageTable* pageTables);
void displayStatistics(Process* processes, int numProcesses);
int checkTLB(int pageNumber, Process* process);
void updateTLB(int pageNumber, int frameNumber);


unsigned int hashFunction(int virtualPageNumber) {
    return virtualPageNumber % TABLE_SIZE;
}

void initializePageTable(HashedPageTable* pageTable) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        pageTable->buckets[i] = NULL;
    }
}

void allocatePage(HashedPageTable* pageTable, int virtualPageNumber, int frameNumber) {
    frames[frameNumber].occupied = 1;
    unsigned int index = hashFunction(virtualPageNumber);
    PageTableEntry* newEntry = (PageTableEntry*)malloc(sizeof(PageTableEntry));
    newEntry->virtualPageNumber = virtualPageNumber;
    newEntry->frameNumber = frameNumber;
    newEntry->next = pageTable->buckets[index];
    pageTable->buckets[index] = newEntry;
    // frames[frameNumber].occupied = 1;
}

void deallocateProcessPages(HashedPageTable* pageTable) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        PageTableEntry* entry = pageTable->buckets[i];
        while (entry != NULL) {
            frames[entry->frameNumber].occupied = 0; // Mark the frame as free
            PageTableEntry* temp = entry;
            entry = entry->next;
            free(temp); // Free the page table entry
        }
        pageTable->buckets[i] = NULL; // Reset the bucket pointer
    }
    printf("Pages for process deallocated.\n");
}


int translateAddress(HashedPageTable* pageTable, int virtualAddress, int* physicalAddress, Process* process) {
    int pageNumber = virtualAddress / PAGE_SIZE;
    int offset = virtualAddress % PAGE_SIZE;

    // Check the TLB first
    int frameNumber = checkTLB(pageNumber, process);
    if (frameNumber != -1) { // TLB Hit
        *physicalAddress = (frameNumber * PAGE_SIZE) + offset;
        return 1; // Success
    } else { // TLB Miss, check the page table
        unsigned int index = hashFunction(pageNumber);
        PageTableEntry* entry = pageTable->buckets[index];

        while (entry) {
            if (entry->virtualPageNumber == pageNumber) {
                *physicalAddress = (entry->frameNumber * PAGE_SIZE) + offset;
                updateTLB(pageNumber, entry->frameNumber); // Update TLB with new entry
                return 1; // Success
            }
            entry = entry->next;
        }
          process->stats.pageFaults++;
    }
    return 0; // Page fault
}

int findFreeFrame() {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (!frames[i].occupied) {
            return i; // Return the index of the first unoccupied frame
        }
    }
    return -1; // Indicate that no free frames are available
}

void handlePageFault(HashedPageTable* pageTable, int virtualPageNumber, Process* process) {
    int frameNumber = findFreeFrame();
    if (frameNumber == -1) {
        printf("No free frames available.\n");
    } else {
        allocatePage(pageTable, virtualPageNumber, frameNumber);
        printf("Handled page fault for virtual page %d, allocated frame %d\n", virtualPageNumber, frameNumber);
    }
}


void createProcesses(Process processes[], int numProcesses, HashedPageTable pageTables[], int option) {
    for (int i = 0; i < numProcesses; i++) {
        processes[i].pid = i;
        if (option == 1) {
            printf("Enter memory size for process %d (in bytes): ", i);
            scanf("%d", &processes[i].memorySize);
            // Ensure at least one page is allocated to each process
            processes[i].numPages = processes[i].memorySize / PAGE_SIZE;
            if (processes[i].numPages < 1) {
                processes[i].numPages = 1; // Minimum of one page
            }
        } else {
            processes[i].memorySize =rand() % PHYSICAL_MEMORY_SIZE; // Default memory size for option 2
            processes[i].numPages = processes[i].memorySize / PAGE_SIZE;
             if (processes[i].numPages < 1) {
                processes[i].numPages = 1; // Minimum of one page
            }
        }
        processes[i].stats = (ProcessStats){0, 0, 0.0, 0}; // Initialize stats to zero

        initializePageTable(&pageTables[i]);

        for (int j = 0; j < processes[i].numPages; j++) {
            int virtualPageNumber = i * MAX_PAGES_PER_PROCESS + j;
            int frameNumber = findFreeFrame();
            allocatePage(&pageTables[i], virtualPageNumber, frameNumber);
        }
        
        printf("Process %d created with %d bytes requiring %d pages.\n", i, processes[i].memorySize, processes[i].numPages);
    }
}


void simulateMemoryAccess(Process* processes, int numProcesses, HashedPageTable* pageTables) {
    for (int i = 0; i < numProcesses; i++) {
        printf("Simulating memory accesses for process %d\n", i);
        for (int j = 0; j < NUM_MEMORY_ACCESSES; j++) { 
            int virtualPage = rand() % processes[i].numPages;
            int offset = rand() % PAGE_SIZE;
            int virtualAddress = virtualPage * PAGE_SIZE + offset;
            processes[i].stats.totalAccesses++;

            int physicalAddress;
            if (!translateAddress(&pageTables[i], virtualAddress, &physicalAddress, &processes[i])) {
                handlePageFault(&pageTables[i], virtualPage, &processes[i]);
                if (!translateAddress(&pageTables[i], virtualAddress, &physicalAddress, &processes[i])) {
                    printf("Process %d: Page fault could not be resolved for virtual address %d\n", i, virtualAddress);
                    continue;
                }
            }

            printf("Process %d: Virtual Address %d -> Physical Address %d\n", i, virtualAddress, physicalAddress);
        }
            deallocateProcessPages(&pageTables[i]);

    }
    
}



void displayStatistics(Process* processes, int numProcesses) {
    int totalPageFaults = 0;
    int totalAccesses = 0;

    printf("\nMemory Access Simulation Statistics:\n");
    for (int i = 0; i < numProcesses; ++i) {
        // Avoid division by zero by checking if totalAccesses is zero

        printf("Process %d: TLB Hits = %d\n", processes[i].pid, processes[i].stats.tlbHits);
    
        if (processes[i].stats.totalAccesses > 0) {
            processes[i].stats.hitRate = (float)(processes[i].stats.totalAccesses - processes[i].stats.pageFaults) / processes[i].stats.totalAccesses;
        } else {
            processes[i].stats.hitRate = 0; // If no accesses were made, set hit rate to 0
        }
        
        printf("Process %d: Page Faults = %d, Total Accesses = %d, Hit Rate = %.2f%%\n",
               processes[i].pid, processes[i].stats.pageFaults, processes[i].stats.totalAccesses, processes[i].stats.hitRate * 100);

        totalPageFaults += processes[i].stats.pageFaults;
        totalAccesses += processes[i].stats.totalAccesses;
    }

    float totalHitRate = totalAccesses > 0 ? (float)(totalAccesses - totalPageFaults) / totalAccesses : 0;
    printf("Total: Page Faults = %d, Total Accesses = %d, Hit Rate = %.2f%%\n", totalPageFaults, totalAccesses, totalHitRate * 100);
}

void initializeTLB() {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = false;
    }
}

void initializeFrames(){
     for (int i = 0; i < NUM_FRAMES; i++) {
        frames[i].occupied = 0;
    }
   
}

int checkTLB(int pageNumber, Process* process) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].pageNumber == pageNumber) {
            process->stats.tlbHits++;
            return tlb[i].frameNumber;
        }
    }
    return -1; // Not found
}

void updateTLB(int pageNumber, int frameNumber) {
    tlb[tlbIndex].pageNumber = pageNumber;
    tlb[tlbIndex].frameNumber = frameNumber;
    tlb[tlbIndex].valid = true;
    tlbIndex = (tlbIndex + 1) % TLB_SIZE; // Simple circular buffer for replacement
}



void option1_CreateAndAllocateProcesses(int numProcesses, HashedPageTable* pageTables) {
    Process* processes = (Process*)malloc(numProcesses * sizeof(Process));
    createProcesses(processes, numProcesses, pageTables, 1);
    simulateMemoryAccess(processes, numProcesses, pageTables);
    displayStatistics(processes, numProcesses);
    free(processes);
}

void option2_SimulateMemoryAccesses(int numProcesses, HashedPageTable* pageTables) {
    Process* processes = (Process*)malloc(numProcesses * sizeof(Process));
    createProcesses(processes, numProcesses, pageTables, 2);
    simulateMemoryAccess(processes, numProcesses, pageTables);
    displayStatistics(processes, numProcesses);
    free(processes);
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <option> <number_of_processes>\n", argv[0]);
        printf("Option 1: Create processes and allocate memory.\n");
        printf("Option 2: Simulate memory accesses.\n");
        return 1; // Exit with an error code if the correct number of arguments is not provided
    }

    srand(time(NULL));

    HashedPageTable pageTables[MAX_PROCESSES];
    initializeTLB();
    initializeFrames();

    int choice = atoi(argv[1]); // Convert the first command-line argument to an integer
    int numProcesses = atoi(argv[2]); // Convert the second command-line argument to an integer

    if (choice == 1) {
        option1_CreateAndAllocateProcesses(numProcesses, pageTables);
    } else if (choice == 2) {
        option2_SimulateMemoryAccesses(numProcesses, pageTables);
    } else {
        printf("Invalid option selected.\n");
        return -1;
    }

    return 0;
}