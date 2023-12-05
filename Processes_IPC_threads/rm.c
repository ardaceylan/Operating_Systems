#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "rm.h"


// global variables

int DA;  // indicates if deadlocks will be avoided or not
int N;  // number of processes
int M;   // number of resource types
int ExistingRes[MAXR]; // Existing resources vector

int **allocation;
int **need;
int **max;
int **request_matrix;
int *avaliable;
pthread_t *thread_id_array;//the index of the tid is matched with the internal id of the thread
pthread_mutex_t lock; 
pthread_cond_t cond;


//..... other definitions/variables .....
//.....
//.....

// end of global variables

void print_n(){
    printf("%%%%%%%%%%%%%%%%%%%%%%%%%%\nN: %d\n%%%%%%%%%%%%%%%%%%%%%%%%\n", N);
}

/**
 * Checks whether requests can be satisifed by the avaliable resources
 */
int compare(int request[], int available[]) {
    int safe = 1;
    for (int i = 0; i < M; i++) {
        if (request[i] > available[i]) {
            safe = 0;
        }
    }
    return safe;
}

int compare_equality(int request[], int available[]) {
    int safe = 1;
    for (int i = 0; i < M; i++) {
        if (request[i] != available[i]) {
            safe = 0;
        }
    }
    return safe;
}

/**
 * This function is used for allocating a matrix
 * 
*/
void create_matrix(int*** matrix, int N, int M) {//N =>rows
    *matrix = malloc(N * sizeof(int*));
    for (int i = 0; i < N; i++) {
        (*matrix)[i] = malloc(M * sizeof(int));
        for (int j = 0; j < M; j++) {
            (*matrix)[i][j] = 0;
        }
    }
}

void free_matrix(int* matrix[], int N) {
    for (int i = 0; i < N; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int find_id(pthread_t tid){
    int i;
    for (i = 0; i < N; i++){
        if (tid == thread_id_array[i]){
            break;
        }
    }

    int o;
    
    if (i == N){
        printf("\nthread not found\n");
        i = -1;
    }

    return i;
}

// compares 2 arrays and returns 1 if arr1 <= arr2, else it returns 0
int compare_arrays (int* arr1, int* arr2, int size){
    int i;
    for (i = 0; i < size; i++){
        if (arr1[i] > arr2[i]){
            return 0;
        }
    }

    return 1;
}




int rm_thread_started(int tid)
{
    int ret = 0;
    thread_id_array[tid] = pthread_self();
    return (ret);
}


int rm_thread_ended()
{
    pthread_mutex_lock(&lock);
    int ret = 0;
    int i;
    pthread_t tid = pthread_self();
    int internal_id = find_id(tid);

    for (i = 0; i < M; i++){
        need[internal_id][i] = 0;
        max[internal_id][i] = 0;
        request_matrix[internal_id][i] = 0;
        avaliable[i] += allocation[internal_id][i];
        allocation[internal_id][i] = 0;
    }
    

    
    pthread_mutex_unlock(&lock);
    return (ret);
}



int rm_claim (int claim[])
{
    if (DA == 0){
        printf("rm_claim will not run, deadlock avoidance is not desired\n");
        return -1;
    }
    int ret = 0;



    pthread_t tid = pthread_self();
    int internal_id = find_id(tid);

    int i;

    pthread_mutex_lock(&lock);
    for (i = 0; i < M; i++){
        if (claim[i] > ExistingRes[i]){
            printf("ERROR: claiming more resorces than existing\n");
            return -1;
        }
    }

    for (i = 0; i < M; i++){
        max[internal_id][i] = claim[i];
        need[internal_id][i] = claim[i];
    }

    pthread_mutex_unlock(&lock);
    return(ret);
}

int rm_init(int p_count, int r_count, int r_exist[],  int avoid)
{
    if (p_count > MAXP || r_count > MAXR)
    {
        return -1;
    }
    
    int i;
    int ret = 0;
    
    DA = avoid;
    N = p_count;
    M = r_count;
    // initialize (create) resources
    for (i = 0; i < M; ++i)
        ExistingRes[i] = r_exist[i];
    // resources initialized (created)
    
    //....

    avaliable = (int*) malloc(M * sizeof(int));
    for (int i = 0; i < M; i++) {
        (avaliable)[i] = r_exist[i];
    }
    create_matrix(&allocation, N, M);
    create_matrix(&need, N, M);
    create_matrix(&max, N, M);
    create_matrix(&request_matrix, N, M);

    thread_id_array = (pthread_t*) malloc(N * sizeof(pthread_t));
    lock = *((pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)));
    cond = *((pthread_cond_t*) malloc(sizeof(pthread_cond_t)));
  
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    rm_print_state("after init");

    return  (ret);
}



int rm_request (int request[])
{
    pthread_mutex_lock(&lock);

    pthread_t tid = pthread_self();
    int internal_id = find_id(tid);

    int p;
    for (p = 0; p < M; ++p) {
        request_matrix[internal_id][p] += request[p];
    }

    int i;

    ///////////////////////////
    // avoidance is not used //
    ///////////////////////////
    
    if (DA == 0){
        while (1){
            int can_allocate = 1;

            for (i = 0; i < M; i++){
                if (request[i] > avaliable[i]){
                    can_allocate = 0;
                    break;
                }
            }

            if (can_allocate){
                for (i = 0; i < M; i++){
                    avaliable[i] -= request[i];
                    allocation[internal_id][i] += request[i];
                    request_matrix[internal_id][i] -= request[i];
                }
                pthread_mutex_unlock(&lock);
                return 0;
            }else {
                pthread_cond_wait(&cond, &lock);
            }
        }
    }

    ///////////////////////
    // avoidance is used //
    ///////////////////////

    // step I: check that the request does not result in exceeding the claim
    if (compare_arrays(request, need[internal_id], M) == 0){
        printf("ERROR: process exceeded max claim\n");

        for (p = 0; p < M; ++p) {
            request_matrix[internal_id][p] -= request[p];

        }
        pthread_mutex_unlock(&lock);

        //print_n();

        return -1;
    }

    // step II: if resources are not available, wait
    while (1){
        int can_allocate = 1;

        for (i = 0; i < M; i++){
            if (request[i] > avaliable[i]){
                can_allocate = 0;
                break;
            }
        }

        if (can_allocate == 0){
            pthread_cond_wait(&cond, &lock);
        }
        else {
            break;
        }
    }

    // step III: check if state is safe
    int* original_available = (int*) malloc(M * sizeof(int));
    int* original_allocation = (int*) malloc(M * sizeof(int));;
    int* original_need = (int*) malloc(N * sizeof(int));;

    while (1){
        for (i = 0; i < M; i++){
            // back the values up
            original_available[i] = avaliable[i];
            original_allocation[i] = allocation[internal_id][i];
            original_need[i] = need[internal_id][i];

            // pretend to allocate the resources
            avaliable[i] -= request[i];
            allocation[internal_id][i] += request[i];
            need[internal_id][i] -= request[i];
        }


        int state_is_safe = 1;

        // step1
        int* work = (int*) malloc(M * sizeof(int));;
        int* finish = (int*) malloc(N * sizeof(int));;

        for (i = 0; i < M; i++){
            work[i] = avaliable[i];
        }

        for (i = 0; i < N; i++){
            finish[i] = 0;
        }

        step2:
        for (i = 0; i < N; i++){
            if ((finish[i] == 0) && compare_arrays(need[i], work, M)){
                break;
            }
        }

        if (i == N){
            goto step4;
        }

        // step3
        int found_index = i;
        for (i = 0; i < M; i++){
            work[i] += allocation[found_index][i];
        }
        finish[found_index] = 1;
        goto step2;

        step4:
        for (i = 0; i < N; i++){
            if (finish[i] == 0){
                state_is_safe = 0;
                break;
            }
        }

        // after state check
        if (state_is_safe){
            for (p = 0; p < M; ++p) {
                request_matrix[internal_id][p] -= request[p];
            }
            pthread_mutex_unlock(&lock);

            //print_n();

            return 0;
        } else {
            // restore the values
            for (i = 0; i < M; i++){
                avaliable[i] = original_available[i];
                allocation[internal_id][i] = original_allocation[i];
                need[internal_id][i] = original_need[i];
            }

            pthread_cond_wait(&cond, &lock);
        }
    }
}

int rm_release (int release[])
{
    pthread_mutex_lock(&lock);

    int ret = 0;

    pthread_t tid = pthread_self();
    int internal_id = find_id(tid);

    int i;
    for (i = 0; i < M; i++){
        if(allocation[internal_id][i] < release[i]){
            printf("ERROR: release is larger than allocation");
            pthread_mutex_unlock(&lock);
            return -1;
        }
        if ((avaliable[i] + release[i]) > ExistingRes[i]){
            printf("ERROR: available + releases larger than existing");
            pthread_mutex_unlock(&lock);
            return -1;
        }
    }

    for (i = 0; i < M; i++){
        allocation[internal_id][i] -= release[i];
        if (DA){
            need[internal_id][i] += release[i];
        }
        avaliable[i] += release[i];
    }

    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&cond);

    return (ret);
}



int rm_detection()
{

    pthread_mutex_lock(&lock);
    int ret = 0;
    int i;

    int* work;//[M]
    int* finish;//[N]
    finish = (int*) malloc(N * sizeof(int));
    work = (int*) malloc(M * sizeof(int));

    for (i = 0; i < M; i++){
        work[i] = avaliable[i];
    }

    int *check;
    check = (int*) malloc(sizeof(int));
    for ( i = 0; i < M; i++)
    {
        check[i] = 0; 
    }

    for (i = 0; i < N; i++){
        finish[i] = 0;
    }

    /*
    //Set the finish values
    for ( i = 0; i < N; i++)
    {
        if (compare_equality(request_matrix[i], check))//make sure that it is NULL!!
        {
           finish[i] = 1;
        }else{
            finish[i] = 0;
        }
    }
    */

    
    //Findanindex i such that
    
    int exists = 1;

    while(exists == 1){
        exists = 0;
        for ( i = 0; i < N; i++)
        {
            if (finish[i] == 0 && compare(request_matrix[i], work))//Make sure that it compares one by one!!!
            {
                //work = work + allocation[i]; //Make sure that you execute the calculation
                for (int j = 0; j < M; j++) {
                    (work)[j] += allocation[i][j];
                }
                finish[i] = 1;
                exists = 1;
            }   
        }
    }

    pthread_mutex_unlock(&lock);
    //Find the deadlock count
    int deadlock_count = 0;
    for ( i = 0; i < N; i++)
    {
        if (finish[i] == 0)
        {
            deadlock_count++;
        }    
    }

    return (deadlock_count);
}

void rm_print_state (char hmsg[])
{
    pthread_mutex_lock(&lock);

    printf("##########################\n");
    printf("%s", hmsg);
    printf("\n##########################\n");

    int i;
    int j;

    printf("Exist:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    printf("\n     ");
    for (i = 0; i < M; i++){
        printf("%-3d", ExistingRes[i]);
    }

    printf("\n\nAvailable:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    printf("\n     ");
    for (i = 0; i < M; i++){
        printf("%-3d", avaliable[i]);
    }


    printf("\n\nAllocation:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    for (i = 0; i < N; i++){
        printf("\nT%d:  ", i);
        for (j = 0; j < M; j++) {
            printf("%-3d", allocation[i][j]);
        }
    }


    printf("\n\nRequest:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    for (i = 0; i < N; i++){
        printf("\nT%d:  ", i);
        for (j = 0; j < M; j++) {
            printf("%-3d", request_matrix[i][j]);
        }
    }


    printf("\n\nMax Demand:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    for (i = 0; i < N; i++){
        printf("\nT%d:  ", i);
        for (j = 0; j < M; j++) {
            printf("%-3d", max[i][j]);
        }
    }

    printf("\n\nNeed:\n     ");
    for (i = 0; i < M; i++){
        printf("R%d ", i);
    }
    for (i = 0; i < N; i++){
        printf("\nT%d:  ", i);
        for (j = 0; j < M; j++) {
            printf("%-3d", need[i][j]);
        }
    }

    printf("\n###########################\n");

    pthread_mutex_unlock(&lock);
}


