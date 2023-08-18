#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

void main()
{    
    int num1 = 10, num2 = 20, num3 = 30;
    umask(0);
    int fd = open("sample.txt", O_CREAT | O_RDWR, 0777);

    if (fd == -1) {
        printf("Error while opening the file!!\n");
        exit(1);
    }
    
    int pid_1 = fork();
    int pid_2 = fork();

    if(pid_1 == 0){
        if(pid_2 == 0)
        {
            printf("The pid of Grandchild PROCESS: %d\n", getpid());
            char* message = "COMP 8567\n";
            write(fd, message, strlen(message));
            for (int i = 0; i < 15; i++) 
            {
                printf("Child Process: PID -> %d, PPID -> %d\n", getpid(), getppid());
                sleep(1);
            }
            num1= num1+10;
            num2= num2+10;
            num3= num3+10;

            printf("Child Process: num1 = %d, num2 = %d, num3 = %d\n", num1, num2, num3);
            exit(0);
        }
        else
        {
            printf("The pid of 1st CHILD PROCESS: %d\n", getpid());
            char* message = "COMP 8567\n";
            write(fd, message, strlen(message));
            for (int i = 0; i < 15; i++) {
                printf("Child Process: PID -> %d, PPID -> %d\n", getpid(), getppid());
                sleep(1);
            }
            num1= num1+10;
            num2= num2+10;
            num3= num3+10;
            printf("Child Process: num1 = %d, num2 = %d, num3 = %d\n", num1, num2, num3);
            exit(0);
        }
    } else
    {
        if(pid_2 == 0)
        {
            printf("The pid of 2nd CHILD PROCESS: %d\n", getpid());
            char* message = "COMP 8567\n";
            write(fd, message, strlen(message));
            for (int i = 0; i < 15; i++) 
            {
                printf("Child Process: PID -> %d, PPID -> %d\n", getpid(), getppid());
                sleep(1);
            }
            num1= num1+10;
            num2= num2+10;
            num3= num3+10;
            printf("Child Process: num1 = %d, num2 = %d, num3 = %d\n", num1, num2, num3);
            exit(0);
        }
        else
        {
            
            int status;
        	wait(&status);
        	if (WIFEXITED(status))
        	{
            // IF CHILD process end NATURALLY
            printf("CHILD process has exits NORMALLY with exit status as:- %d\n", WEXITSTATUS(status));
        	}
        	else if (WIFSIGNALED(status))
        	{
            	// IF CHILD process ends ABNORMALLY
            	printf("CHILD process has been TERMINATED ABNORMALLY or FORCIBLY by signal %d\n", WTERMSIG(status));
        	}
            printf("The pid of MAIN PROCESS is: %d\n", getpid());
            char* message = "HELLO! FROM PARENT\n";
            write(fd, message, strlen(message));
            num1= num1+25;
            num2= num2+25;
            num3= num3+25;
            printf("Parent Process: num1 = %d, num2 = %d, num3 = %d\n", num1, num2, num3);
            exit(0);
         }
    }
    exit(0);
}