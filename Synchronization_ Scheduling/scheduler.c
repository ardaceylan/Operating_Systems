//
// Created by ohamdash on 4/23/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "scheduler.h"

int get_ms(struct timeval time){
    return (time.tv_usec / 1000) + (time.tv_sec * 1000);
}

int get_elapsed_time(struct timeval start_time, struct timeval curr_time){
    double total_time_us = 0;
    if (curr_time.tv_sec == start_time.tv_sec){
        total_time_us = curr_time.tv_usec - start_time.tv_usec;
    }else{
        total_time_us += 1000000 - start_time.tv_usec;
        total_time_us += (curr_time.tv_sec - start_time.tv_sec - 1) * 1000000;
        total_time_us += curr_time.tv_usec;
    }

    return total_time_us / 1000;
}

int get_shortest_id(struct node* head){
    if (head == NULL){
        printf("queue empty\n");
        return 1;
    }

    int shortest_burst = head->proc->burst_length;
    int shortest_id = head->proc->pid;

    struct node* curr;
    for (curr = head->next; curr != NULL; curr = curr->next){
        if ((curr->proc->pid != -1) && (curr->proc->burst_length < shortest_burst)){
            shortest_burst = curr->proc->burst_length;
            shortest_id = curr->proc->pid;
        }
    }

    return shortest_id;
}

void schedule_rr(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head, int q){
    printf("RR algo Scheduling\n");
    struct timeval curr_time;

    while (1){

        pthread_mutex_lock(lock);
        while ((*ready_queue_head) == NULL){
            pthread_mutex_unlock(lock);
            usleep(1000);
            pthread_mutex_lock(lock);
        }

        struct process* curr_proc;
        get_head((*ready_queue_head), &curr_proc);
        if (curr_proc->pid == -1) {
            if ((*ready_queue_head)->next == NULL){
                pthread_mutex_unlock(lock);
                break;
            }else{
                send_head_to_tail(ready_queue_head);
            }
        }

        retrieve_head(ready_queue_head, &curr_proc);
        pthread_mutex_unlock(lock);

        curr_proc->processor_id = processor_id;

        if (outmode == 2){
            gettimeofday(&curr_time, NULL);
            printf("time=%d, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d\n\n", 
                    get_elapsed_time(start_time, curr_time), curr_proc->processor_id, curr_proc->pid, curr_proc->burst_length, curr_proc->remaining_time);
        }
        if (outmode == 3){
            printf("process with pid %d picked by cpu %d\n", curr_proc->pid, curr_proc->processor_id);
        }

        if (curr_proc->remaining_time <= q){
            usleep(1000 * curr_proc->remaining_time);

            gettimeofday(&curr_time, NULL);
            curr_proc->finish_time = get_elapsed_time(start_time, curr_time);
            curr_proc->remaining_time = 0;
            curr_proc->turnaround_time = curr_proc->finish_time - curr_proc->arrival_time;

            if (outmode == 3){
                printf("process with pid %d has finished\n", curr_proc->pid);
            } 

            pthread_mutex_lock(finished_lock);
            insert_to_tail(finished_list, curr_proc);
            pthread_mutex_unlock(finished_lock);
        }else {
            usleep(1000 * q);
            curr_proc->remaining_time -= q;
            
            if (outmode == 3){
                printf("time slice expired for process with pid %d, %d ms remaining\n", curr_proc->pid, curr_proc->remaining_time);
            }            
            
            pthread_mutex_lock(lock);
            insert_to_tail(ready_queue_head, curr_proc);
            pthread_mutex_unlock(lock);
        }
    }
}

void schedule_fcfs(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head){
    printf("FCFS algo Scheduling\n");
    struct timeval curr_time;
    while (1){
        pthread_mutex_lock(lock);
        while ((*ready_queue_head) == NULL){
            pthread_mutex_unlock(lock);
            usleep(1000);
            pthread_mutex_lock(lock);
        }
        
        struct process* curr_proc;
        get_head((*ready_queue_head), &curr_proc);
        if (curr_proc->pid == -1) {
            if ((*ready_queue_head)->next == NULL){
                pthread_mutex_unlock(lock);
                break;
            }else{
                send_head_to_tail(ready_queue_head);
            }
        }

        retrieve_head(ready_queue_head, &curr_proc);
        pthread_mutex_unlock(lock);

        curr_proc->processor_id = processor_id;

        if (outmode == 2){
            gettimeofday(&curr_time, NULL);
            printf("time=%d, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d\n\n", 
                    get_elapsed_time(start_time, curr_time), curr_proc->processor_id, curr_proc->pid, curr_proc->burst_length, curr_proc->remaining_time);
        }
        if (outmode == 3){
            printf("process with pid %d picked by cpu %d\n", curr_proc->pid, curr_proc->processor_id);
        }

        usleep(1000 * curr_proc->burst_length);

        gettimeofday(&curr_time, NULL);
        curr_proc->finish_time = get_elapsed_time(start_time, curr_time);
        curr_proc->remaining_time = 0;
        curr_proc->turnaround_time = curr_proc->finish_time - curr_proc->arrival_time;

        if (outmode == 3){
            printf("process with pid %d has finished\n", curr_proc->pid);
        } 

        pthread_mutex_lock(finished_lock);
        insert_to_tail(finished_list, curr_proc);
        pthread_mutex_unlock(finished_lock);
    }
}

void schedule_sjf(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head){
    printf("SJF algo Scheduling\n");
    struct timeval curr_time;
    int shortest_id;
    while (1){
        pthread_mutex_lock(lock);
        while ((*ready_queue_head) == NULL){
            pthread_mutex_unlock(lock);
            usleep(1000);
            pthread_mutex_lock(lock);
        }
        
        struct process* curr_proc;
        get_head((*ready_queue_head), &curr_proc);
        if (curr_proc->pid == -1) {
            if ((*ready_queue_head)->next == NULL){
                pthread_mutex_unlock(lock);
                break;
            }else{
                send_head_to_tail(ready_queue_head);
            }
        }

        shortest_id = get_shortest_id((*ready_queue_head));

        retrieve(ready_queue_head, shortest_id, &curr_proc);
        pthread_mutex_unlock(lock);

        curr_proc->processor_id = processor_id;

        if (outmode == 2){
            gettimeofday(&curr_time, NULL);
            printf("time=%d, cpu=%d, pid=%d, burstlen=%d, remainingtime=%d\n\n", 
                    get_elapsed_time(start_time, curr_time), curr_proc->processor_id, curr_proc->pid, curr_proc->burst_length, curr_proc->remaining_time);
        }
        if (outmode == 3){
            printf("process with pid %d picked by cpu %d\n", curr_proc->pid, curr_proc->processor_id);
        }

        usleep(1000 * curr_proc->burst_length);

        gettimeofday(&curr_time, NULL);
        curr_proc->finish_time = get_elapsed_time(start_time, curr_time);
        curr_proc->remaining_time = 0;
        curr_proc->turnaround_time = curr_proc->finish_time - curr_proc->arrival_time;

        if (outmode == 3){
            printf("process with pid %d has finished\n", curr_proc->pid);
        }

        pthread_mutex_lock(finished_lock);
        insert_to_tail(finished_list, curr_proc);
        pthread_mutex_unlock(finished_lock);
    }
}

