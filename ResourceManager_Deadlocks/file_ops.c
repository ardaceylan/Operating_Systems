#include "file_ops.h"
#define MAX_WORD_LENGTH 64

int read_from_file(char* filename, struct node** head){
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
                insert(head, word);
                count = 0;
            }
        } else {
            word[count++] = character;
        }
    }

    //Add the last word to the list.
    //insert(head,word);

    fclose(fp);
    return 0;
}

int write_to_file(char* filename, struct node* head, const int K){

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
