//
// Created by ohamdash on 4/23/23.
//

#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "ready_queue.h"

#ifndef CS342_PROJECT2_SCHEDULER_H
#define CS342_PROJECT2_SCHEDULER_H

int get_ms(struct timeval time);
int get_elapsed_time(struct timeval start_time, struct timeval curr_time);

// 3 functions, one for each scheduling algo

void schedule_rr(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head, int q);
void schedule_fcfs(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head);
void schedule_sjf(int processor_id, int outmode, struct timeval start_time, pthread_mutex_t* finished_lock, struct node** finished_list, pthread_mutex_t* lock, struct node** ready_queue_head);

#endif //CS342_PROJECT2_SCHEDULER_H
