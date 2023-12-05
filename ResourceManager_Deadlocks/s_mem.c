#include "s_mem.h"
#define MAX_WORD_LENGTH 64

// returns -1 if failed and 0 if memory was created successfully
int allocate_mem(int size, char* name, void **ptr){
    int shm_fd;;

	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd,size);

	(*ptr) = mmap(0,size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);


	if ((*ptr) == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
	}
	return 0;
}

// returns -1 if failed and 0 if memory was created successfully
int open_mem(int size, char* name, void** ptr){

    int shm_fd = shm_open(name, O_RDONLY, 0666);

    if (shm_fd == -1) {
        printf("shared memory failed\n"); exit(-1);
    }

    (*ptr) = mmap(0,size, PROT_READ, MAP_SHARED, shm_fd, 0);


    if ((*ptr) == MAP_FAILED) {
        printf("Map failed\n"); exit(-1);
    }

    if (shm_unlink(name) == -1) {
        printf("Error removing %s\n",name);
        return -1;
    }

    return 0;
}

int open_mem_rdwr(int size, char* name, void** ptr){

    int shm_fd = shm_open(name, O_RDWR, 0666);

    if (shm_fd == -1) {
        printf("shared memory failed\n"); exit(-1);
    }

    (*ptr) = mmap(0,size, PROT_READ, MAP_SHARED, shm_fd, 0);

    if ((*ptr) == MAP_FAILED) {
        printf("Map failed\n"); exit(-1);
    }

    if (shm_unlink(name) == -1) {
        printf("Error removing %s\n",name);
        return -1;
    }

    return 0;
}


void insert_in_mem(void* ptr, struct node* head, const int WORD_SIZE){

    int* shared_ctr;
    char word[WORD_SIZE];
    struct node* curr;
    for(curr = head; curr != NULL; curr = curr->next){
        stpcpy(word, curr->word);
        sprintf(ptr, "%s", word);

        ptr += WORD_SIZE;

        shared_ctr = (int*) ptr;
        (*shared_ctr) = curr->ctr;
        ptr += sizeof(int);
    }

}

void retrieve_from_mem(void* ptr, const int SIZE, const int N, const int K, const int WORD_SIZE, struct node** head){

    char curr_word[WORD_SIZE];
    int* curr_ctr;
    int i;
    for(i = 0; i < K * N; i++){
        snprintf(curr_word, WORD_SIZE, "%s", (char*) ptr);
        ptr = ptr + WORD_SIZE;

        curr_ctr = (int*) ptr;

        ptr += sizeof(int);
        if ((*curr_ctr) != 0){
            insertToFinalList(&head, curr_word, (*curr_ctr));
        }
    }
}
