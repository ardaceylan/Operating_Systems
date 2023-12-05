#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "dl_list.h"

// returns -1 if failed and 0 if memory was created successfully
int allocate_mem(int size, char* name, void **ptr);

// returns -1 if failed and 0 if memory was created successfully
int open_mem(int size, char* name, void** ptr);
int open_mem_rdwr(int size, char* name, void** ptr);

void insert_in_mem(void* ptr, struct node* head, const int WORD_SIZE);

void retrieve_from_mem(void* ptr, const int SIZE, const int N, const int K, const int WORD_SIZE, struct node** head);
