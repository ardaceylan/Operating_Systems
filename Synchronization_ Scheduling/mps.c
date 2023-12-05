#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <getopt.h>
#include <string.h>
#include <math.h>


#include "process.h"
#include "ready_queue.h"
#include "scheduler.h"

struct timeval start_time;

int n = 2; // default value for -n
char sap = 'M'; // default value for -a
char* qs = "RM"; // default value for -a
char* alg = "RR"; // default value for -s
int q = 20; // default value for -s
char* infile = "in.txt"; // default value for -i
int outmode = 1; // default value for -m
char* outfile = NULL; // default value for -o
int randomize = 0; // flag for -r
int index_specified = 0;
int T = 200, T1 = 10, T2 = 1000, L = 100, L1 = 10, L2 = 500, PC = 10; // default values for -r

struct node** ready_queues_list;

struct node* finished_list = NULL;
pthread_mutex_t finished_lock;

pthread_mutex_t* mutex;
int i;


typedef struct {
    int i;
    char* alg;
    int q;
} thread_args;
const char* RR_STRING = "RR";
const char* FCFS_STRING = "FCFS";
const char* SJF_STRING = "SJF";
// function used by the threads, called from pthread_create()
void *schedule(void *args) {
    thread_args *targs = (thread_args *)args;

    int index = 0;
    if (sap == 'M'){
        index = targs->i;
    }

    // pick the correct scheduling algorithm function
    if (strcmp(targs->alg, RR_STRING) == 0){
        schedule_rr(targs->i, outmode, start_time, &finished_lock, &finished_list, &mutex[index], &ready_queues_list[index], targs->q);
    } else if (strcmp(targs->alg, FCFS_STRING) == 0){
        schedule_fcfs(targs->i, outmode, start_time, &finished_lock, &finished_list, &mutex[index], &ready_queues_list[index]);
    }else if (strcmp(targs->alg, SJF_STRING) == 0) {
        schedule_sjf(targs->i, outmode, start_time, &finished_lock, &finished_list, &mutex[index], &ready_queues_list[index]);
    }else {
        printf("ERROR: scheduling algo (%s) not recognized!\n", targs->alg);
    }
}

int main(int argc, char *argv[]) {
    gettimeofday(&start_time, NULL);

   int opt;
    while ((opt = getopt(argc, argv, "n:a:s:i:m:o:r:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'a':

                sap = optarg[0];
                qs = argv[optind]; 
                optind++;
                break;
            case 's':
                alg = optarg;
                q = atoi(argv[optind]);
                optind++;
                break;

            case 'i':
                infile = optarg;
                index_specified = 1;
                break;
            case 'm':
                outmode = atoi(optarg);
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'r':
                randomize = 1;
                T = atoi(optarg);
                T1 = atoi(argv[optind]);
                T2 = atoi(argv[optind+1]);
                L = atoi(argv[optind+2]);
                L1 = atoi(argv[optind+3]);
                L2 = atoi(argv[optind+4]);
                PC = atoi(argv[optind+5]);
                break;
            default:
                //printf(stderr, "Usage: %s [-n N] [-a SAP QS] [-s ALG Q] [-i INFILE] [-m OUTMODE] [-o OUTFILE] [-r T T1 T2 L L1 L2 PC]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    printf("n=%d, sap=%c, qs=%s, alg=%s, q=%d, infile=%s, outmode=%d, outfile=%s, randomize=%d, T=%d, T1=%d, T2=%d, L=%d, L1=%d, L2=%d, PC=%d\n", n, sap, qs, alg, q, infile, outmode, outfile, randomize, T, T1, T2, L, L1, L2, PC);
    
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////

    pthread_t threads[n];
    int i;
    int temp_int = n;
    if (sap == 'S'){
        temp_int = 1;
    }

    ready_queues_list = (struct node**) malloc(temp_int * sizeof(struct node));
    mutex = (pthread_mutex_t*) malloc(temp_int * sizeof(pthread_mutex_t));

    for (i = 0; i < temp_int; i++){
        ready_queues_list[i] = NULL;

        if (pthread_mutex_init(&mutex[i], NULL) != 0) {
            printf("Mutex initialization failed.\n");
            return 1;
        }
    }


    thread_args* targs = (thread_args*) malloc(n * sizeof(thread_args));
    for (i = 0; i < n; i++){
        thread_args temp_args = { i, alg, q};
        targs[i] = temp_args;
        pthread_create(&threads[i], NULL, schedule, &targs[i]); // schedule will be implemented
    }

    ///////////////////////////////////////
    // Here will be the file reading code//
    ///////////////////////////////////////


    
    struct timeval curr_time;
    if (index_specified && randomize == 0)
    {
        FILE *fp;
        fp = fopen(infile, "r");
        if (fp == NULL) {
            printf("Error opening file\n");
            return 1;
        }

        // Read the lines and create the struct for PL
        char line[100];
        int iat;
        int pl = 0;
        int process_count = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "PL %d", &pl) == 1) {
                struct process* nproc = (struct process*)malloc(sizeof(struct process));
                nproc->burst_length = pl;
                gettimeofday(&curr_time, NULL);
                nproc->arrival_time = get_elapsed_time(start_time, curr_time);
                nproc->remaining_time = pl;
                process_count++;
                nproc->pid = process_count;


                //If it is singular
                if (sap == 'S')
                {
                    pthread_mutex_lock(&mutex[0]);
                    nproc->processor_id = 0;
                    insert_to_tail(&ready_queues_list[0], nproc);
                    
                    pthread_mutex_unlock(&mutex[0]);
                }
                //If it is multiple
                else
                {
                    //If RR is used
                    if (strcmp(qs, "RM") == 0)
                    {
                        pthread_mutex_lock(&mutex[(process_count-1)%n]);
                        nproc->processor_id = (process_count-1)%n;
                        insert_to_tail(&ready_queues_list[(process_count-1)%n], nproc);
                        pthread_mutex_unlock(&mutex[(process_count-1)%n]);
                    }
                    //If load balancing used
                    else if(strcmp(qs, "LM") == 0)
                    {
                        //Find the minumum length queue
                        int current_min_value = 1000000000;
                        int current_min_index = 0;
                        for (i = 0; i < n; i++)
                        {
                            struct node* temp = ready_queues_list[i];

                            //If one of the node is empty directly assign to the empty queue
                            if (temp == NULL)
                            {
                                current_min_index = i;
                                break;
                            }
                            
                            int count = 0;//keeps the length of bursts int the list.
                            while (temp != NULL)
                            {
                                count = count + temp->proc->remaining_time;
                                temp = temp->next;
                            }
                            if (count < current_min_value)
                            {
                                current_min_value = count;
                                current_min_index = i;
                            }
                        }
                        //insert the node into min queue
                        pthread_mutex_lock(&mutex[current_min_index]);
                        nproc->processor_id = current_min_index;
                        insert_to_tail(&ready_queues_list[current_min_index], nproc);
                        pthread_mutex_unlock(&mutex[current_min_index]);
                    }
                }
            } else if (sscanf(line, "IAT %d", &iat) == 1) {
                // Sleep for the value of IAT
                usleep(1000 * iat);
            }
        }


    }else
    {
        printf("index not specified\n");
        for (i = 0; i < PC; i++)
        {

            //Generate a random pl time
            double lambda = 1.0 / L;
            double x = 0.0;
            int pl = 0;
            while (1) {
                double u = (double) rand() / RAND_MAX;
                x = -log(1 - u) / lambda;
                if (x >= L1 && x <= L2) {
                    pl = (int) round(x);
                    break;
                }
                
            }
    
            //Create the process 
            struct process* nproc = (struct process*)malloc(sizeof(struct process));
            nproc->burst_length = pl;
            gettimeofday(&curr_time, NULL);
            nproc->arrival_time = get_elapsed_time(start_time, curr_time);
            nproc->remaining_time = pl;
            nproc->pid = i+1;


            //If it is singular
            if (sap == 'S')
            {
                pthread_mutex_lock(&mutex[0]);
                nproc->processor_id = 0;
                insert_to_tail(&ready_queues_list[0], nproc);
                
                pthread_mutex_unlock(&mutex[0]);
            }
            //If it is multiple
            else
            {
                //If RR is used
                if (strcmp(qs, "RM") == 0)
                {
                    pthread_mutex_lock(&mutex[(i)%n]);
                    nproc->processor_id = (i)%n;
                    insert_to_tail(&ready_queues_list[(i)%n], nproc);
                    pthread_mutex_unlock(&mutex[(i)%n]);
                }
                //If load balancing used
                else if(strcmp(qs, "LM") == 0)
                {
                    //Find the minumum length queue
                    int current_min_value = 1000000000;
                    int current_min_index = 0;
                    for (i = 0; i < n; i++)
                    {
                        struct node* temp = ready_queues_list[i];

                        //If one of the node is empty directly assign to the empty queue
                        if (temp == NULL)
                        {
                            current_min_index = i;
                            break;
                        }
                        
                        int count = 0;//keeps the length of bursts int the list.
                        while (temp != NULL)
                        {
                            count = count + temp->proc->remaining_time;
                            temp = temp->next;
                        }
                        if (count < current_min_value)
                        {
                            current_min_value = count;
                            current_min_index = i;
                        }
                    }
                    //insert the node into min queue
                    pthread_mutex_lock(&mutex[current_min_index]);
                    nproc->processor_id = current_min_index;
                    insert_to_tail(&ready_queues_list[current_min_index], nproc);
                    pthread_mutex_unlock(&mutex[current_min_index]);
                }
            }

            //Generate iat and sleep for the interarrvialtime
            if (i != PC-1)
            {
                double lambda = 1.0 / T;
                double x = 0.0;
                int iat = 0;
                while (1) {
                    double u = (double) rand() / RAND_MAX;
                    x = -log(1 - u) / lambda;
                    if (x >= T1 && x <= T2) {
                        iat = (int) round(x);
                        break;
                    }
                }
                usleep(1000 * iat);
            }
            
        }
    }
    int index;
    int arda_count = 1;
    if (sap == 'M'){
        arda_count = n;
    }
    //Add dummy node at the end
    for (i = 0; i < arda_count; i++)
    {
        struct process* nproc = (struct process*)malloc(sizeof(struct process));
        nproc->pid = -1;
        struct node* procnode = (struct node*)malloc(sizeof(struct node));
        procnode->proc = nproc;

        index = 0;
        if (sap == 'M'){
            index = i;
        }

        //printf("before lock\n");
        pthread_mutex_lock(&mutex[index]);
        //printf("after lock\n");
        insert_to_tail(&ready_queues_list[index], nproc);
        pthread_mutex_unlock(&mutex[index]);
    }

    ///////////////////////////////////////


    for (i = 0; i < n; i++){
        pthread_join(threads[i], NULL);
    }

    printf("\n\nlist of finished processes in the order they were terminated:\n");
    print_queue_detailed(finished_list);

    
    struct node* finished_sorted = NULL;
    struct node* curr_node = NULL;
    struct process* curr_proc = NULL;
    int list_size = 0;

    for (curr_node = finished_list; curr_node != NULL; curr_node = curr_node->next){
        list_size++;
    }

    
    for (i = 0; i < list_size; i++){
        retrieve(&finished_list, i + 1, &curr_proc);
        insert_to_tail(&finished_sorted, curr_proc);
    }

    int turnaround_tot = 0;
    printf("\npid  cpu  burstlen  arv    finish  waitingtime  turnaround\n");
    for (curr_node = finished_sorted; curr_node != NULL; curr_node = curr_node->next){
        curr_proc = curr_node->proc;
        turnaround_tot += curr_proc->turnaround_time;
        printf("%-5d%-5d%-10d%-7d%-8d%-13d%-10d\n", curr_proc->pid, curr_proc->processor_id, curr_proc->burst_length, curr_proc->arrival_time, curr_proc->finish_time, curr_proc->turnaround_time - curr_proc->burst_length, curr_proc->turnaround_time);
    }
    printf("average turnaround time: %d ms\n", turnaround_tot / list_size);

    return 0;
}
