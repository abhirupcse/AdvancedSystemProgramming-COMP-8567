#include <stdio.h> 
#include <pthread.h>  
#include <stdlib.h>  
#include <stdint.h> 
#include <time.h>  
 
// METHOD TO CREATE RANDOM INTERGER VALUE RANGING 10-20 
int generate_random() {  
    return rand() % 11 + 10;  
}  

// A THREAD METHOD TO CALCULATE THE SUM OF ELEMENTS IN AN ARRAY
void* getSumofArray(void* arg) {  
    int* arr = (int*)arg;  
    int calculateSum = 0;  
    for (int i = 0; i < 100; ++i) {  
        calculateSum += arr[i];  
    }  
    // EXIT THE THREAD AND RETURNS THE SUM AS A POINTER(void *)  
    pthread_exit((void*)(intptr_t)calculateSum);  
}  

// MAIN BODY  
int main() {
	//SEED THE RANDOM NUMBER GENERATOR WITH CURRENT TIME
    srand(time(NULL));  
    // INITIALIZING ALL THE 3 ARRAY OF SIZE 100  
    int ia1[100], ia2[100], ia3[100];  
    for (int i = 0; i < 100; ++i) {  
        ia1[i] = generate_random();  
        ia2[i] = generate_random();  
        ia3[i] = generate_random();  
    }  

    // CREATE 3 THREADS AND PASS THE ABOVE 3 CREATED ARRAY ONE BY ONE  
    pthread_t sum_1, sum_2, sum_3;  
    pthread_create(&sum_1, NULL, getSumofArray, (void*)ia1);  
    pthread_create(&sum_2, NULL, getSumofArray, (void*)ia2);  
    pthread_create(&sum_3, NULL, getSumofArray, (void*)ia3);  

    // VARIABLE TO STORE THE RETURN VALUE FROM JOIN THREADS  
    void* returnValue1;  
    void* returnValue2;  
    void* returnValue3;  

    // WAITS FOR THE THREADS TO END AND COLLECTS THE RETURN VALUES  
    pthread_join(sum_1, &returnValue1);  
    pthread_join(sum_2, &returnValue2);  
    pthread_join(sum_3, &returnValue3);  

    // COVERTING THE VALUES TO INTEGER  
    int total1 = (int)(intptr_t)returnValue1;  
    int total2 = (int)(intptr_t)returnValue2;  
    int total3 = (int)(intptr_t)returnValue3;  

    // DISPLAY THE SUMS  
    printf("SUM OF THE FIRST ARRAY1: %d\n", total1);  
    printf("SUM OF THE SECOND ARRAY2: %d\n", total2);  
    printf("SUM OF THE THIRD ARRAY3: %d\n", total3);  

    // CALCULATE THE SUM OF EACH ARRAY  
    int totalSum = total1 + total1 + total3;  
    printf("SUM OF ALL 3 ARRARYS: %d\n", totalSum);  
    return 0;  
}  