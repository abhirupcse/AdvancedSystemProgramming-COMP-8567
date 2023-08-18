#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int pid1, pid2, pid3, grandChildFlag = 0, grandChildFlag2 = 0;

void signalHandler(int sig)
{
    if (grandChildFlag == 0)
    {
        // terminate grandchildren using pgid.
        killpg(getpid(), SIGUSR1);
        kill(pid1, SIGUSR1);
        kill(pid2, SIGUSR1);
        kill(pid3, SIGUSR1);
        grandChildFlag = 1;
    }
}

//print the statement before terminating the grandchilds
void signalHandlerPrint(int sig)
{
    printf("Grandchild process terminated with PID= %d & PPID= %d\n ", getpid(), getppid());
    exit(0);
}

//flag to check for the grandchild
void grandchildFlagFun(int sig)
{
    grandChildFlag2 = 1;
}

int main()
{
	int mainProgpid = getpid();
	
	//3 consecutive fork calls
    pid1 = fork();
    pid2 = fork();
    pid3 = fork();
    
    //checking if any of the forking fails
    if (pid1 < 0 || pid2 < 0 || pid3 < 0)
    {
        printf("ERROR while executing the fork()");
        exit(1);
    }
    
    //child processes
    if (getpid() != mainProgpid)
    {
        // ignoring the ctrl + C in child
        signal(SIGINT, SIG_IGN);
        // using the SIGUSR1 for interprocess communication
        signal(SIGUSR1, signalHandlerPrint);
        // checking the direct descendent of the parent.
        if (getppid() == mainProgpid)
        {
            signal(SIGUSR1, grandchildFlagFun);
			int a = setpgid(getpid(), getpid());
            if (a == -1)
            {
                printf("ERROR while getting the PG id!!");
                exit(1);
            }
        }
        
        //running the INFINITE LOOP for the CHILD
        while (1)
        {
            printf("This is from process (process id) %d\n", getpid());
            if (grandChildFlag2 == 0)
            {
            	//making it sleep for 2 sec as per requirement 
                sleep(2);
            }
            else
            {
                sleep(2);
            }
            
        }
    }
    
    // This is a parent process
    else
    {
        signal(SIGINT, signalHandler);
        // ignore signal
        signal(SIGUSR1, SIG_IGN);
        signal(SIGCONT, SIG_IGN);
        
        //running the INFINITE LOOP for the PARENT
        while (1)
        {
        	//making it sleep for 2 sec as per requirement
            sleep(2);
            if (grandChildFlag != 0)
            {
                // grandchild to be killed
                printf("This is from process (process id) %d\n", getpid());
                //using the concept for the STOP(ctrl+S) and Continue(ctrl+Q) SIGNAL
                kill(pid1, SIGCONT);
                kill(pid2, SIGCONT);
                kill(pid3, SIGCONT);
            }
        }
    }
    return 0;
}
