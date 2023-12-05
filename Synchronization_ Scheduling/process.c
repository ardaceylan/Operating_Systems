//
// Created by ohamdash on 4/17/23.
//

#include "process.h"

void print_proc(struct process* proc){
    printf("\n---------\n");
    printf("pid: %d\n", proc->pid);
    printf("burst_length: %d\n", proc->burst_length);
    printf("arrival_time: %d\n", proc->arrival_time);
    printf("remaining_time: %d\n", proc->remaining_time);
    printf("finish_time: %d\n", proc->finish_time);
    printf("turnaround_time: %d\n", proc->turnaround_time);
    printf("processor_id: %d\n", proc->processor_id);
    printf("---------\n");
}