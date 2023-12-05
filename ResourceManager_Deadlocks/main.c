#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s_mem.h"

int main(int argc, char* argv[]) {



    int K = atoi(argv[1]);
    char outputFileName[64] = argv[2];
    int N = atoi(argv[3]);

    const int WORD_SIZE = 64;
    const int BLOCK_SIZE = WORD_SIZE + sizeof(int);

    int i;

    //char** filenames[5] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt", "file5.txt"};


    // create shared mem
    const int SIZE = N * K * (WORD_SIZE + sizeof(int));
    char *name = "SHARED_MEM";

    void* ptr;
    allocate_mem(SIZE, name, &ptr);
    //munmap(ptr, SIZE);

    char* curr_filename;

    void* insert_ptr;
    int n;
    //int status;
    for (i=0; i<N; ++i) {
        curr_filename = argv[i+4];
        n = fork();
        if (n==0) {
            insert_ptr = ptr + (i * BLOCK_SIZE * K);
            struct node* head;
            read_from_file(curr_filename, &head);
            //print_list(head);
            insert_in_mem(insert_ptr, head, WORD_SIZE);
            head = NULL;
            munmap(ptr, SIZE);
            exit (0);
        }
    }


    for (i=0; i<N; ++i){
        wait(NULL);
    }

    munmap(ptr, SIZE);

    //printf("\n--------------------------\n\n");
    //printf("all children executed\n");

    struct node* final_head;
    void* ptr_c;
    open_mem(SIZE, name, &ptr_c);
    void* ptr_curr = ptr_c;

    retrieve_from_mem(ptr_c, SIZE, N, K, WORD_SIZE, &final_head);

    //print_first_k(final_head, K);

    munmap(ptr_c, SIZE);

    write_to_file(outputFileName, final_head, K);


    return 0;
}
