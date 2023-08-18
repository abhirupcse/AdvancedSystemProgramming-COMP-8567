#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/signal.h>
#include <pthread.h>

// GLOBAL VARIABLES ARE DECLARED
int abhiCounter = 6;
pthread_mutex_t lock;

// A COMMON THREAD FUNCTION
void* abhiFunc(void* arg) {
    int checkDetached = 0;
    pthread_t abhiTid = pthread_self();

// CHECK IF THREAD IS CREATED AS DETACH
    if (pthread_detach(abhiTid) == 0) {
        checkDetached = 1;}

// APPLY LOCK AS DECREMENTING THE COUNTER VARIABLE
    pthread_mutex_lock(&lock);
    if (checkDetached) {
        printf("THIS IS DETACHED WITH THREAD ID: %lu, AND COUNTER: %d\n", abhiTid, abhiCounter);
        abhiCounter--; // DECREMENTING COUNTER
        }
    else {
        printf("THIS IS JOIN WITH THREAD ID: %lu, AND COUNTER: %d\n", abhiTid, abhiCounter); 
        }         
    pthread_mutex_unlock(&lock); // UNLOCKING
    pthread_exit(NULL);
} // FUNCTION ENDS HERE

int main() {
    pthread_t abhiThreads[6];
    pthread_attr_t attr; // DECLARE ATTRIBUTE
    int i, abhiDetachState;

// INITIALIZING THE VARIABLE
    pthread_mutex_init(&lock, NULL);
    for (i = 0; i < 6; i++) {
        pthread_attr_init(&attr);
// VARIBALE TO USE FOR JOINABLE AND DETACHED
        if (i < 3) {
            abhiDetachState = PTHREAD_CREATE_JOINABLE;
            } 
        else {
            abhiDetachState = PTHREAD_CREATE_DETACHED;
            }

        pthread_attr_setdetachstate(&attr, abhiDetachState);
        pthread_create(&abhiThreads[i], &attr, abhiFunc, NULL); //WILL CREATE 1-3 AS JOINABLE AND LAST 3 AS DETACHED
        //sleep(1);
        }

// CREATING THE JOIN STATEMENT
    for (i = 0; i < 6; i++) {
        pthread_join(abhiThreads[i], NULL);
    }
    pthread_mutex_destroy(&lock);
    return 0;
}