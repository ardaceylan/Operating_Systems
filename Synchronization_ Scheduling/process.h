//
// Created by ohamdash on 4/16/23.
//

#ifndef CS342_PROJECT2_PROCESS_H
#define CS342_PROJECT2_PROCESS_H

#include <stdlib.h>
#include <stdio.h>

struct process
{
    int pid;
    int burst_length;
    int arrival_time;
    int remaining_time;
    int finish_time;
    int turnaround_time;
    int processor_id;
};

void print_proc(struct process* proc);

#endif //CS342_PROJECT2_PROCESS_H