#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>

//#define NUM_THREADS 3  // Define the number of threads to create
//#define ARRAY_SIZE 10  // Define the size of the array to add
int K;
char outputFileName[64];
int NUM_THREADS;
#define MAX_WORD_LENGTH 64
//#define K 4
//char arrays[3][100] = {"example.txt", "yourfile.txt", "myfile.txt"};


struct node* topWords;



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





void insert(struct node** head, char w[64]){
    if ((*head) == NULL){
        (*head) = (struct node*) malloc(sizeof(struct node));
        strcpy((*head)->word, w);
        (*head)->ctr = 1;
        return;
    }

    struct node* curr = (*head);
    int break_status = 1;

    while ((strcmp(curr->word, w) != 0)) {
        if (curr->next == NULL){
            break_status = 0;
            break;
        }

        curr = curr->next;
    }



    if (break_status == 0){
        struct node* new_node = (struct node*) malloc(sizeof(struct node));
        strcpy(new_node->word, w);
        new_node->ctr = 1;
        new_node->prev = curr;
        new_node->next = NULL;

        curr->next = new_node;
    } else if (break_status == 1){
        curr->ctr++;
        if ((curr->prev == NULL) || !(curr->ctr > curr->prev->ctr)){
            return;
        }


        struct node* curr_itr = curr;
        while ((curr_itr->prev != NULL) && (curr->ctr > curr_itr->prev->ctr)){
            curr_itr = curr_itr->prev;
        }

        if (curr->next != NULL){
            curr->next->prev = curr->prev;
        }
        curr->prev->next = curr->next;

        if(curr_itr->prev == NULL){
            (*head) = curr;
            curr->next = curr_itr;
            curr->prev = NULL;
            curr_itr->prev = curr;
        } else {
            curr_itr->prev->next = curr;
            curr->prev = curr_itr->prev;
            curr->next = curr_itr;
            curr_itr->prev = curr;
        }

    } else {
        printf("------------ERROR------------\n");
        exit(0);
    }
}





/**Reads the file and calls add list function to add each word into the list in a sorted order*/


int read_from_file(char* filename){

    struct node* head;

    FILE *fp;
    char word[MAX_WORD_LENGTH];
    char character;
    int count = 0;

    fp = fopen(filename, "r");


    while ((character = fgetc(fp)) != EOF) {
        if (isspace(character)) {
            if (count > 0) {
                word[count] = '\0';
                //printf("%s\t\n ", word); // use '\t', '\n' and ' ' to separate words
                insert(&head, word);
                count = 0;
            }
        } else {
            word[count++] = character;
        }
    }

    //Add the last word to the list.
    //insert(head,word);



    //find the top k on the global variable
    struct node* temp = head;
    for (size_t i = 0; i < K && temp != NULL; i++)
    {
        struct node* curNode = (struct node*)malloc(sizeof(struct node));
        strcpy(curNode->word, temp->word);
        curNode->ctr = temp->ctr;

        curNode->next = topWords;
        topWords->prev = curNode;
        topWords = curNode;

        temp = temp->next;
    }

    fclose(fp);
    return 0;
}




/*void insertToFinalList(struct node** head, char word[64], int count){

    if ((*head) == NULL){
        (*head) = (struct node*) malloc(sizeof(struct node));
        strcpy((*head)->word, word);
        (*head)->ctr = count;
        return;
    }

    struct node* curr = (*head);
    int break_status = 1;
    int pos_found = 0;
    struct node* next = NULL;

    while ((strcmp(curr->word, word) != 0)) {
        if ((curr->ctr <= count) && (pos_found == 0)){
            next = curr;
            pos_found = 1;
        }

        if (curr->next == NULL){
            break_status = 0;
            break;
        }

        curr = curr->next;
    }

    if (break_status == 0){
        struct node* new_node = (struct node*) malloc(sizeof(struct node));
        strcpy(new_node->word, word);
        new_node->ctr = count;

        if (pos_found){
            new_node->next = next;
            new_node->prev = next->prev;
            if (next->prev != NULL){
                next->prev->next = new_node;
            }
            next->prev = new_node;
        } else {
            curr->next = new_node;
            new_node->prev = curr;
            new_node->next = NULL;
        }
    } else if (break_status == 1){
        curr->ctr += count;
        if ((curr->prev == NULL) || !(curr->ctr > curr->prev->ctr)){
            return;
        }

        struct node* curr_itr = curr;
        while ((curr_itr->prev != NULL) && (curr->ctr > curr_itr->prev->ctr)){
            curr_itr = curr_itr->prev;
        }

        if (curr->next != NULL){
            curr->next->prev = curr->prev;
        }
        curr->prev->next = curr->next;

        if(curr_itr->prev == NULL){
            (*head) = curr;
            curr->next = curr_itr;
            curr->prev = NULL;
            curr_itr->prev = curr;
        } else {
            curr_itr->prev->next = curr;
            curr->prev = curr_itr->prev;
            curr->next = curr_itr;
            curr_itr->prev = curr;
        }

    } else {
        printf("------------ERROR------------\n");
        exit(0);
    }
}*/



void insertToFinalList(struct node **head, char str[MAX_WORD_LENGTH], int count){


    if((*head)->ctr == -1)
    {
        (*head) = (struct node*)malloc(sizeof(struct node));
        (*head)->ctr = count;
        strcpy((*head)->word, str);
    }
    else{


        struct node *currentNode = *head;
        while (currentNode != NULL)
        {

            //if the word already exits in the list
            if (!(strcmp(currentNode->word, str)))
            {
                currentNode->ctr = currentNode->ctr + count;
            
                //Reorder the nodes according to new counts
                struct node *reverseNode = currentNode->prev;
                while (reverseNode != NULL && reverseNode->ctr < currentNode->ctr)
                {
                    //switch to one step back
                    reverseNode->next = currentNode->next;
                    currentNode->prev = reverseNode->prev;
                    currentNode->next = reverseNode;
                    reverseNode->prev = currentNode;

                    if (currentNode->prev == NULL)
                    {
                        *head = currentNode;
                    }
                    
                    //update reverseNode
                    reverseNode = currentNode->prev;
                }

                break;
            }

            //if the word does not exits add a new node to the end of the list
            if (currentNode->next == NULL)
            {
                struct node *newNode = (struct node*)malloc(sizeof(struct node));
                newNode->ctr = count;
                strcpy(newNode->word, str);
                currentNode->next = newNode;
                newNode->prev = currentNode;
                currentNode = newNode;




                //Reorder the nodes according to new counts
                struct node *reverseNode = currentNode->prev;
                while (reverseNode != NULL && reverseNode->ctr < currentNode->ctr)
                {
                    //switch to one step back
                    reverseNode->next = currentNode->next;
                    currentNode->prev = reverseNode->prev;
                    currentNode->next = reverseNode;
                    reverseNode->prev = currentNode;

                    if (currentNode->prev == NULL)
                    {
                        *head = currentNode;
                    }
                    
                    //update reverseNode
                    reverseNode = currentNode->prev;
                }
                break;
            }

            //update currentNode
            currentNode = currentNode->next;
        }
    }
    //return head;
}








int write_to_file(char* filename, struct node* head){

    FILE * outfile;
    char line[100];

    /* Open the file for writing */
    outfile = fopen(filename, "w");

    /* Check if the file is opened successfully */
    if (outfile == NULL) {
        printf("Unable to open the file\n");
        return 1;
    }

    /* Write each line to the file using fprintf */

    struct node* currentNode = head;
    int i = K;
    while ((i > 0) && (currentNode != NULL))
    {
        fprintf(outfile, " %s %d\n", currentNode->word, currentNode->ctr);
        currentNode = currentNode->next;
        i--;
    }

    fclose(outfile);

    return 0;
}



/*void* readFile(void* arg) {
    FILE *fp;
    char filename[] = "example.txt";
    char word[MAX_WORD_LENGTH];
    char charecter;
    int count = 0;


    struct node *head;
    head = (struct node*)malloc(sizeof(struct node));
    head->ctr = -1;

    fp = fopen(filename, "r");


    while ((charecter = fgetc(fp)) != EOF) {
        if (isspace(charecter)) {
            if (count > 0) {
                word[count] = '\0';
                //printf("%s\t\n ", word); // use '\t', '\n' and ' ' to separate words
                insertToList(&head,word);
                count = 0;
            }
        } else {
            word[count++] = charecter;
        }
    }

    //Add the last word to the list.
    insertToList(&head,word);

    fclose(fp);

    //find the top k on the global variable
    struct Node* temp = head;
    for (size_t i = 0; i < K && temp != NULL; i++)
    {
        struct Node* curNode = (struct Node*)malloc(sizeof(struct Node));
        strcpy(curNode->word, temp->word);
        curNode->number = temp->number;
        topWords->next = curNode;
        temp = temp->next;
    }
    
    return 0;
}*/


/*int main(int argc, char* argv[]) {

    int K = atoi(argv[1]);
    char outputFileName[64] = argv[2];
    int N = atoi(argv[3]);
    atoi(argv[1]);//k
    atoi(argv[1]);//n

    //change outputfilename
    //change iteration filenames + 4*/



int main(int argc, char* argv[]) {


    K = atoi(argv[1]);
    outputFileName[64] = argv[2];
    NUM_THREADS = atoi(argv[3]);


    pthread_t threads[NUM_THREADS];  // Define an array of thread identifiers
    topWords = (struct node*)malloc(sizeof(struct node));

    for (int i = 0; i < NUM_THREADS; i++) {
        
        pthread_create(&threads[i], NULL, read_from_file, (void*) argv[i+4]);
        //pthread_create(&threads[i], NULL, read_from_file, (void*) arrays[i]);  // Create a new thread for each integer array
        //printf("name of the files: %s\n", argv[i+4]);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);  // Wait for all threads to finish
    }


    struct node* finalHead = (struct node*)malloc(sizeof(struct node));
    finalHead->ctr = -1;
    struct node* currNode = topWords;
    while (currNode->next != NULL)
    {
        //printf("worrd: %s , number: %d \n" , currNode->word, currNode->ctr);
        insertToFinalList(&finalHead, currNode->word, currNode->ctr);
        currNode = currNode->next;
    }

    write_to_file(outputFileName, finalHead);
    pthread_exit(NULL);
}
