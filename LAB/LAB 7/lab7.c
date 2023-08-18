#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    // Program would behave same as command--> cat input.txt|wc|wc -w>>output.txt
    // fileDescriptor(read/write) for 2 pipes passed
    int fd[2][2];

    // Create and Check pipe if in case of issue
    for (int i = 0; i < 2; i++)
    {
        if (pipe(fd[i]) == -1)
        {
            printf("ERROR OCCURRED WHILE PIPE CREATED!!");
            exit(1); // EXIT WHITH AN ERROR
        }
    }

    // 1st FORK to handle the cat command
    if (fork() == 0)
    { 
		// CREATE a duplicate copy for the standard output and point to the fd for write
		// fd[0][1] represent fileDescriptor for write end of pipe
		// STDOUT_FILENO file descriptor for the standard output = 1
        dup2(fd[0][1], STDOUT_FILENO);
        close(fd[0][0]); // Closing the read end for 1st Pipe
        close(fd[0][1]); // Closing the write end for 1st Pipe
        execlp("cat", "cat", "input.txt", NULL); // Replace cat with this fork or program body        
        printf("SOME ERROR OCCURED WHILE THE EXECTING 1st FORK()");
        exit(1); // EXIT WHITH AN ERROR
    }

    // 2nd FORK which will handle the wc command
    if (fork() == 0)
    {
    	// CREATE a duplicate copy for the standard input and point to the 1st pipe fd
		// STDIN_FILENO file descriptor for the standard input = 0
        dup2(fd[0][0], STDIN_FILENO);
		
		// CREATE a duplicate copy for the standard output and point to the 2nd pipe fd
		// STDOUT_FILENO file descriptor for the standard output = 1
        dup2(fd[1][1], STDOUT_FILENO);

        close(fd[0][0]); // Closing the read end for 1st Pipe
        close(fd[0][1]); // Closing the write end for 1st Pipe
        close(fd[1][0]); // Closing the read end for 2nd Pipe
        close(fd[1][1]); // Closing the write end for 2nd Pipe
        execlp("wc", "wc", NULL); // Replace wc with this fork or program body
        printf("SOME ERROR OCCURED WHILE THE EXECTING 2nd FORK()");
        exit(1);
    }

    // 3rd FORK which will handle the wc -w command
    if (fork() == 0)
    {
		// CREATE a duplicate copy for the standard input and point to the 2nd pipe fd
		// STDIN_FILENO file descriptor for the standard input = 0
        dup2(fd[1][0], STDIN_FILENO);
        FILE *fd1=fopen("output.txt", "w");
		dup2(fileno(fd1), STDOUT_FILENO); //changing the consol standard output to fd1 which is a file.

        close(fd[0][0]); // Closing the read end for 1st Pipe
        close(fd[0][1]); // Closing the write end for 1st Pipe
        close(fd[1][0]); // Closing the read end for 2nd Pipe
        close(fd[1][1]); // Closing the write end for 2nd Pipe
        execlp("wc", "wc", "-w", NULL); // Replace wc -w with this fork or program body

        printf("SOME ERROR OCCURED WHILE THE EXECTING 3rd FORK()");
        exit(1);
    }

    return 0;
}