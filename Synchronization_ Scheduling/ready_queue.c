//
// Created by ohamdash on 4/16/23.
//

#include "ready_queue.h"

void insert_to_tail(struct node** head, struct process* proc){

    struct node* new_node = (struct node*) malloc(sizeof(struct node));
    new_node->proc = proc;
    new_node->prev = NULL;
    new_node->next = NULL;

    if ((*head) == NULL){
        (*head) =  new_node;
        (*head)->proc = proc;
        return;
    }

    struct node* curr;
    for (curr = (*head); curr->next != NULL; curr = curr->next)
        ;
    curr->next = new_node;
    new_node->prev = curr;
}

void retrieve_head(struct node** head, struct process** proc){
    if((*head) == NULL){
        printf("ERROR: Queue is empty, cannot retrieve\n");
        return;
    }

    (*proc) = (*head)->proc;
    if((*head)->next != NULL){
        (*head)->next->prev = NULL;
    }
    struct node* old_head = (*head);
    (*head) = (*head)->next;
    old_head->next = NULL;
    free(old_head);
    old_head = NULL;
}

void retrieve(struct node** head, int pid, struct process** proc){
    if((*head) == NULL){
        printf("ERROR: Queue is empty, cannot retrieve\n");
        return;
    }

    if ((*head)->proc->pid == pid){
        (*proc) = (*head)->proc;
        struct node* old_head = (*head);
        (*head) = (*head)->next;
        free(old_head);
        old_head = NULL;
        return;
    }

    struct node* curr;
    for (curr = (*head)->next; curr != NULL; curr = curr->next){
        struct process* curr_proc = curr->proc;
        if (curr_proc->pid == pid){
            (*proc) = curr_proc;
            curr->prev->next = curr->next;
            if (curr->next != NULL){
                curr->next->prev = curr->prev;
            }
            free(curr);
            curr = NULL;
            return;
        }
    }

    printf("process with pid %d was not found\n", pid);
}

void get_proc(struct node* head, int pid, struct process** proc){

    if(head == NULL){
        printf("ERROR: Queue is empty, cannot get process\n");
        return;
    }

    struct node* curr;
    for (curr = head; curr != NULL; curr = curr->next){
        struct process* curr_proc = curr->proc;
        if (curr_proc->pid == pid){
            (*proc) = curr_proc;
            return;
        }
    }

    printf("process with pid %d was not found\n", pid);
}

void get_head(struct node* head, struct process** proc){
    if(head == NULL){
        printf("ERROR: Queue is empty, cannot get process\n");
        return;
    }

    (*proc) = head->proc;
}

void send_head_to_tail(struct node** head){
    if (((*head) == NULL) || ((*head)->next == NULL)){
        printf("queue is empty or has only one element\n");
        return;
    }

    struct node* curr;
    for (curr = (*head); curr->next != NULL; curr = curr->next)
        ;

    curr->next = (*head);
    (*head) = (*head)->next;
    (*head)->prev = NULL;
    curr->next->prev = curr;
    curr->next->next = NULL;
}

void print_queue(struct node* head){
    if(head == NULL){
        printf("print_queue: queue is empty\n");
        return;
    }

    struct node* curr;
    int pid;
    int i;
    printf("[\n");
    for (curr = head, i = 1; curr != NULL; curr = curr->next, i++){
        pid = curr->proc->pid;

        printf("    %d. pid: %d\n",i, pid);
    }
    printf("]\n");

}

void print_queue_detailed(struct node* head){
    if(head == NULL){
        printf("print_queue: queue is empty\n");
        return;
    }

    struct node* curr;
    int pid;
    int burst_length;
    int arrival_time;
    int remaining_time;
    int finish_time;
    int turnaround_time;
    int processor_id;

    int i;
    printf("[\n");
    for (curr = head, i = 1; curr != NULL; curr = curr->next, i++){
        pid = curr->proc->pid;
        burst_length = curr->proc->burst_length;
        arrival_time = curr->proc->arrival_time;
        remaining_time = curr->proc->remaining_time;
        finish_time = curr->proc->finish_time;
        turnaround_time = curr->proc->turnaround_time;
        processor_id = curr->proc->processor_id;

        printf("    pid: %d\n"
               "    burst_length: %d\n"
               "    arrival_time: %d\n"
               "    remaining_time: %d\n"
               "    finish_time: %d\n"
               "    turnaround_time: %d\n"
               "    processor_id: %d\n\n"
               , pid, burst_length, arrival_time, remaining_time, finish_time, turnaround_time, processor_id);
    }
    printf("]\n");
}