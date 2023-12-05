#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dl_list.h"

#define MAX_WORD_LENGTH 64

int read_from_file(char* filename, struct node** head);

int write_to_file(char* filename, struct node* head, const int K);
