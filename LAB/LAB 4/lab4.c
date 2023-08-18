#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int pid_c1, pid_c2;
    pid_c1 = fork();
    pid_c2 = fork();

    if (pid_c1 == 0)
    {
        // If child C1 is exectuded
        printf("Process ID for the C1: %d\n", getpid());
        int pid_gc = fork();
        if (pid_gc == 0)
        {
            // If grandchild is executed inside C1
            printf("Process ID for the GC of C1: %d\n", getpid());
            chdir("/home/ranjan2/lab4");
            int fd = open("sample.txt", O_CREAT|O_RDWR, 0777);
            if (fd == -1)
            {
                printf("Error while creating the file\n");
                exit(0);
            }
            printf("Sample.txt File Created\n");
        }
        else if (pid_gc == -1)
        {
            printf("Error when the grandchild is created!!");
            exit(0);
        }
        else
        {
        //c1 process
        printf("Waiting for the complete execution of the grandchild process!!\n");
        int status;
        wait(&status);
        printf("Printing the c1 ProcessID: %d\n", getpid());
        char *arr[] = {"/bin/ls", "-1", "/home/ranjan2/lab4", NULL};
        execv("/bin/ls",arr);
        }
    }
    if (pid_c1 == -1)
    {
        printf("Error while creating the C1 child!!");
        exit(0);
    }
    else
    {
        //Parent Process is executed here
        printf("This is the Parent Process with pid as: %d\n", getpid());
        if (pid_c2 == 0)
        {
            // child 2 process
            execl("/bin/ls", "/bin/ls", "-1", "/home/ranjan2", NULL);
        }
        else if (pid_c2 == -1)
        {
            printf("Error while executing the child2 process");
            exit(0);
        }
        printf("Waiting for the completion of the 2nd process!!\n");
        int status;
        wait(&status);
        exit(0);
    }
return 0;
}