#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char word[64];
    int ctr;
    struct node *prev;
    struct node *next;
};

void insert(struct node** head, char w[64]);
void insertToFinalList(struct node*** head, char word[64], int count);
void print_list(struct node* head);
void print_first_k(struct node* head, const int K);
