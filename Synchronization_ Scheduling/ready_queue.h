//
// Created by ohamdash on 4/15/23.
//

#ifndef CS342_PROJECT2_LINKED_LIST_H
#define CS342_PROJECT2_LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "process.h"


struct node
{
    struct process* proc;
    struct node* prev;
    struct node* next;
};

/*
 * Inserts a process to the tail of the ready queue.
 * In the case where the queue is empty, it assigns the new process as the head.
 */
void insert_to_tail(struct node** head, struct process* proc);

/*
 * Removes the head process from the queue and returns it in the
 * proc parameter (by reference)
 */
void retrieve_head(struct node** head, struct process** proc);

/*
 * Removes the process indicated by pid from the queue and returns it in the
 * proc parameter (by reference). If the process with the indicated pid does
 * not exist it prints an error message.
 */
void retrieve(struct node** head, int pid, struct process** proc);

/*
 * Same as retrieve, but it doesn't remove it from the queue.
 */
void get_proc(struct node* head, int pid, struct process** proc);

/*
 * Same as retrieve_head, but it doesn't remove it from the queue.
 */
void get_head(struct node* head, struct process** proc);

/*
 * Sends the head of the queue to the tail and assigns the head to the next node in the queue.
 */
void send_head_to_tail(struct node** head);

/*
 * Prints the ready queue starting from head to tail.
 */
void print_queue(struct node* head);

/*
 * Prints the ready queue starting from head to tail with all proc details.
 */
void print_queue_detailed(struct node* head);

#endif //CS342_PROJECT2_LINKED_LIST_H

