#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

//Global Declaration
int abhirupCounter = 0;
time_t abhirupInterrupt = 0;
int abhirupCounterT = 0;
int abhirupFlag = 0;

void abhiSignalHandler(int Signum)
{
    // this gives the current time
    time_t currentTime = time(NULL);
    if (Signum == SIGINT)
    {
        if(currentTime-abhirupInterrupt<=5)
        {
            //printf("Value in Counter = %d\n",abhirupCounter);
            exit(0);
        }
        else
        {
            abhirupCounter = 0;
        }
        abhirupInterrupt = currentTime;
        abhirupCounter++;
        printf("Value in Counter = %d\n",abhirupCounter);
    }
}

int main()
{
    signal(SIGINT, abhiSignalHandler);
    while (1)
    {
        printf("Welcome\n");
        sleep(1);
    }
    return 0;
}