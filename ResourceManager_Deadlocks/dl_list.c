#include "dl_list.h"

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


void insertToFinalList(struct node*** head, char word[64], int count){

    if ((**head) == NULL){
        (**head) = (struct node*) malloc(sizeof(struct node));
        strcpy((**head)->word, word);
        (**head)->ctr = count;
        return;
    }

    struct node* curr = (**head);
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
            (**head) = curr;
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


/*
void insertToFinalList(struct node **head, char str[64], int count){


    if((*head) == NULL)
    {

        (*head)->ctr = 1;
        strcpy((*head)->word, str);
    }
    else{


        struct node *currentNode = (*head);
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
                        (*head) = currentNode;
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
                        (*head) = currentNode;
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
}
*/

void print_list(struct node* head){
    struct node* curr;
    for (curr = head; curr != NULL; curr = curr->next){
        if (curr->prev == NULL){
            printf("\n[\n");
        }
        printf("    %s: %d", curr->word, curr->ctr);
        if (curr->next != NULL){
            printf(",\n");
        }
        else{
            printf("\n]\n\n");
        }
    }
}

void print_first_k(struct node* head, const int K){
    struct node* curr;
    int i = 0;
    for (curr = head; (curr != NULL) && (i < K); curr = curr->next){
        if (curr->prev == NULL){
            printf("\n[\n");
        }

        printf("    %s: %d", curr->word, curr->ctr);
        if ((curr->next != NULL) && (i < (K - 1))){
            printf(",\n");
        }
        else{
            printf("\n]\n\n");
            return;
        }

        i++;
    }
}
