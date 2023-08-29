//@Author: Abhirup Ranjan
// Section: 3
// COMP8567

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

//Methods
int fetchParentProcessID(int pids) 
{
    char procPathStore[50]; //array to store path for process status file 
    //format the path string by substituting the process ID into the /proc/%d/status format.
    snprintf(procPathStore, sizeof(procPathStore), "/proc/%d/status", pids);

	//opening the file in Read mode
    FILE *fd = fopen(procPathStore, "r");
    if (fd == NULL) {
        printf("\nError .....NO SUCH PROCESS ID EXISTS!!\n");
        return 1;
    }
	
	//store each line read from the process status file.
    char arr[128];
    int ppid = 0;

	// fgets() read line from text file or input stream. 
    // It stands for "file get string."
    while (fgets(arr, sizeof(arr), fd) != NULL) {
    	//strncmp function checks if line starts with "PPid:"
        if (strncmp(arr, "PPid:", 5) == 0) {
        	//extracts the parent process ID from the line and stores it in the ppid variable.
            sscanf(arr + 5, "%d", &ppid);
            break;
        }
    }
    fclose(fd);
    return ppid;
}

int findProcessId(int rootProcess, int pids)
{   
    int childPid = pids;
    int count = 0;
    while (childPid != 1) {
        if (childPid == rootProcess){
            printf("ProcessID = %d          ",pids);
            printf("Parent ProcesssID = %d\n",fetchParentProcessID(pids));
            count = count + 1;
            break;
        }
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", childPid);
    FILE *fd = fopen(path, "r");
    if (fd == NULL) {
        printf("Passed Process id = %d is not part of rootProcess = %d\n", childPid,rootProcess);
        return 1;
    }
    char lines[256];
    char ppidString[256];
    int ppid = -1;

    while (fgets(lines, sizeof(lines), fd) != NULL) {
        if (strncmp(lines, "PPid:", 5) == 0) {
            sscanf(lines, "PPid:\t%s", ppidString);
            ppid = atoi(ppidString);
            break;
        }
    }
    fclose(fd);
    if (ppid != -1)
    {
        //printf("childProcess = %d its Parent Process = %d\n", childPid, ppid);
        childPid = ppid;
        }
    else
    {
        printf("Unable to retrive the parent id for child process = %d.\n", childPid);
        return 0;
    }
    }
    if (count ==0)
    {
        printf("This passed Processid = %d is not under RootProcess = %d\n",pids,rootProcess);
        return 1;
    }
    return 0;
}

// Method returns the parentprocessid
int fetchPpid(int pids)
{
    char path[50];
    FILE *fd;
    int ppid;

    sprintf(path, "/proc/%d/stat", pids);
    fd = fopen(path, "r");
    if (fd == NULL)
    {
        printf("Error while opening the file.!!!");
        exit(0);
    }

    fscanf(fd, "%*d %*s %*c %d", &ppid);
    fclose(fd);
    return ppid;
}

void fetchNonDirectDescendants(int pids, int oldpids){
    DIR *directory;
    struct dirent *dirEntries;
    directory = opendir("/proc");
    while ((dirEntries = readdir(directory)) != NULL)
    {
        if (dirEntries->d_type == DT_DIR)
        {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/stat", dirEntries->d_name);
            FILE *fd = fopen(path, "r");
            int dumyPid, dumyPpid;
            if (fd != NULL)
            {
                if (fscanf(fd, "%d %*s %*c %d", &dumyPid, &dumyPpid) == 2)
                {
                    if (dumyPpid == pids)
                    {
                        int dumyPpid1 = fetchPpid(dumyPid);
                        if (dumyPpid1 == oldpids)
                        {
                            printf("PID: %d\n", dumyPid);
                        }
                        //calling the function recurrssivly
                        fetchNonDirectDescendants(dumyPid, oldpids);
                    }
                }
                fclose(fd);
            }
        }
    }
    closedir(directory);
}

void fetchImmChildProcess(int pids){
    DIR *directory = opendir("/proc");
    if (directory == NULL) {
        printf("Fail to open the /proc directory hence try again.\n");
        return;
    }
    struct dirent* dirEntries;
    printf("Current Process ID= %d\n",pids);
    printf("Immediate Descendants PIDs of the above Current Process:\n");
    while ((dirEntries = readdir(directory)) != NULL) {
        if (dirEntries->d_type == DT_DIR) {
            char path[256];
            snprintf(path, sizeof(path), "/proc/%s/status", dirEntries->d_name);
            FILE* fd = fopen(path, "r");
            if (fd != NULL) {
                int childPid = -1;
                char lines[256];
                // Reading processStatus file
                while (fgets(lines, sizeof(lines), fd) != NULL) {
                    // Finding the line which contains PPid field
                    if (strncmp(lines, "PPid:", 5) == 0) {
                        sscanf(lines + 6, "%d", &childPid);
                        break;
                    }
                }
                fclose(fd);
                if (childPid == pids) {
                    printf("%s\n", dirEntries->d_name);
                }
            }
        }
    }
    closedir(directory);
}

void getSiblingProcesses(int pids) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pids);

    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        printf("Fail to open the /proc file hence try again.\n");
        return;
    }

    int ppid = -1;
    char lines[256];

    // Reading processStatus file
    while (fgets(lines, sizeof(lines), fd) != NULL) {
        // Finding the line which contains PPid field
        if (strncmp(lines, "PPid:", 5) == 0) {
            sscanf(lines + 6, "%d", &ppid);
            break;
        }
    }
    fclose(fd);
    if (ppid == -1) {
        printf("Fail to get parent PID.\n");
        return;
    }
    DIR* directory = opendir("/proc");
    if (directory == NULL) {
        printf("Fail to open the /proc directory hence try again.\n");
        return;
    }

    struct dirent* dirEntries;
    printf("Current Process ID= %d\n",pids);
    printf("Siblings PIDs for the above Current Process:\n");

    while ((dirEntries = readdir(directory)) != NULL) {
        if (dirEntries->d_type == DT_DIR) {
            int pid = atoi(dirEntries->d_name);
            if (pid != 0 && pid != pids && pid != ppid) {
                char statusPath[256];
                snprintf(statusPath, sizeof(statusPath), "/proc/%s/status", dirEntries->d_name);

                FILE* statusFile = fopen(statusPath, "r");
                if (statusFile == NULL) {
                    continue;
                }

                int currentPpid = -1;
                char currentLines[256];
                // Reading processStatus file
                while (fgets(currentLines, sizeof(currentLines), statusFile) != NULL) {
                    // Finding the line which contains PPid field
                    if (strncmp(currentLines, "PPid:", 5) == 0) {
                        sscanf(currentLines + 6, "%d", &currentPpid);
                        break;
                    }
                }
                fclose(statusFile);

                if (currentPpid == ppid) {
                    printf("%d\n", pid);
                }
            }
        }
    }
    closedir(directory);
}

void fetchDefunSiblingProcesses(int pids) {
     char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pids);

    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
        printf("Fail to open the /proc directory hence try again.\n");
        return;
    }

    int ppid = -1;
    char lines[256];

    // Reading processStatus file
    while (fgets(lines, sizeof(lines), fd) != NULL) {
        // Finding the line which contains PPid field
        if (strncmp(lines, "PPid:", 5) == 0) {
            sscanf(lines + 6, "%d", &ppid);
            break;
        }
    }
    fclose(fd);
    if (ppid == -1) {
        printf("Failed to get parent PID.\n");
        return;
    }
    DIR* directory = opendir("/proc");
    if (directory == NULL) {
        printf("Failed to open the /proc directory. Please try again.\n");
        return;
    }

    struct dirent* dirEntries;
    printf("Current Process ID: %d\n", pids);
    printf("Zombie or Defunct Sibling PIDs for the above Current Process is/are below:\n");

    while ((dirEntries = readdir(directory)) != NULL) {
        if (dirEntries->d_type == DT_DIR) {
            int pid = atoi(dirEntries->d_name);
            if (pid != 0 && pid != pids && pid != ppid) {
                char statusPath[256];
                snprintf(statusPath, sizeof(statusPath), "/proc/%s/status", dirEntries->d_name);

                FILE* statusFile = fopen(statusPath, "r");
                if (statusFile == NULL) {
                    continue;
                }

                int currentPpid = -1;
                char currentLines[256];
                bool zombie = false;
                // Reading processStatus file
                while (fgets(currentLines, sizeof(currentLines), statusFile) != NULL) {
                    // Finding the line which contains PPid field
                    if (strncmp(currentLines, "PPid:", 5) == 0) {
                        sscanf(currentLines + 6, "%d", &currentPpid);
                    }
                    // Finding the line which contains State field
                    if (strncmp(currentLines, "State:", 6) == 0) {
                        if (strstr(currentLines, "Z (zombie)") != NULL || strstr(currentLines, "Z (defunct)") != NULL) {
                            zombie = true;
                        }
                    }
                }
                fclose(statusFile);

                if (currentPpid == ppid && zombie) {
                    printf("%d\n", pid);
                }
            }
        }
    }
    closedir(directory);
}

void fetchGrandchildrenProcesses(int pids) {
     DIR *directory;
    struct dirent *dirEntries;
    int childPid, childPpid;
    directory = opendir("/proc");

    printf("Below are the Grandchild Processes\n%d\n", pids);
    while ((dirEntries = readdir(directory)) != NULL)
    {
        if (dirEntries->d_type == DT_DIR)
        {
            // skipping folder which does not contain digits
            if (!isdigit(dirEntries->d_name[0]))
                continue;
            // skipping the main INIT process     
            childPid = atoi(dirEntries->d_name);
            if (childPid == 1)
                continue;
            // skipping the process if child matches    
            if (childPid == pids)
                continue;

            // fetch the child's parent id
            childPpid = fetchPpid(childPid);
            if (childPpid > 1 && fetchPpid(childPpid) == pids)
            {
                printf("Child's ProcessID: %d Its Parent ProcessPID: %d\n", childPid, childPpid);
            }
        }
    }
    closedir(directory);
}

void printStatus(int pids) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pids);

    FILE* fd = fopen(path, "r");
    if (fd == NULL) {
         printf("Fail to open the processStatus file hence try again.\n");
        return;
    }
    char lines[256];
    int a = 0;
    // Read the process status file line by line
    while (fgets(lines, sizeof(lines), fd) != NULL) {
        // Find the line containing the "State" field
        if (strncmp(lines, "State:", 6) == 0) {
            // Checks if process is defunct or Zombie
            if (strstr(lines, "Z (zombie)") != NULL) {
                a = 1;
                break;
            }
        }
    }
    fclose(fd);
    printf("ProcessID: %d    ", pids);
    printf("Status is: %s\n", a ? "Defunct or Zombie" : "Normal / Non Defunct");
}

void fetchDirectZombieDescendants(int pids) {
    DIR* directory = opendir("/proc");
    if (directory == NULL) {
        printf("Fail to open the /proc directory hence try again.\n");
        return;
    }

    struct dirent* dirEntries;
    printf("Current Process ID= %d\n",pids);
    printf("PIDs of all the direct descendants of Current Process that are Zombie\n");

    while ((dirEntries = readdir(directory)) != NULL) {
        if (dirEntries->d_type == DT_DIR) {
            int pid = atoi(dirEntries->d_name);
            if (pid != 0 && pid != pids) {
                char statusPath[256];
                snprintf(statusPath, sizeof(statusPath), "/proc/%s/status", dirEntries->d_name);
                FILE* statusFile = fopen(statusPath, "r");
                if (statusFile == NULL) {
                    continue;
                }
                int ppid = -1;
                int a = 0;
                char lines[256];

                // Reading Status file
                while (fgets(lines, sizeof(lines), statusFile) != NULL) {
                    // Finding the line which contains PPid and State field
                    if (strncmp(lines, "PPid:", 5) == 0) {
                        sscanf(lines + 6, "%d", &ppid);
                    } else if (strncmp(lines, "State:", 6) == 0) {
                        // Check if process is Zombie
                        if (strstr(lines, "Z (zombie)") != NULL) {
                            a = 1;
                        }
                    }
                    // just out of the loop if both fields are found
                    if (ppid != -1 && a) {
                        break;
                    }
                }
                fclose(statusFile);
                if (ppid == pids && a) {
                    printf("%d\n", pid);
                }
            }
        }
    }
    closedir(directory);
}

void optionFun(int rootProcess, int pids, char*option)
{
    int ppids = pids;
    //Lists PIDs for all Non-Direct descendants of 1st processid
    if (strcmp(option, "-nd") == 0)
    {
        printf("Lists PIDs for all Non-Direct descendants of 1st processid(-nd)\n");
        printf("-------------------------------------------------------------------\n");
        fetchNonDirectDescendants(pids,pids);
    }
    //Lists PIDs for all immediate descendants of 1st processid
    else if (strcmp(option, "-dd") == 0)
    {
        printf("Lists PIDs for all immediate descendants of 1st processid(-dd)\n");
        printf("-------------------------------------------------------------------\n");
        fetchImmChildProcess(pids);
    }
    //Lists PIDs for all sibling of 1st processid
    else if (strcmp(option, "-sb") == 0)
    {
        printf("Lists PIDs for all Siblings process of 1st processid(-sb)\n");
        printf("-------------------------------------------------------------------\n");
        getSiblingProcesses(pids);
    }
    //Lists PIDs for all sibling of 1st processid that are defunct
    else if (strcmp(option, "-sz") == 0)
    {
        printf("Lists PIDs for all Zombie Siblings process of 1st processid(-sz)\n");
        printf("-------------------------------------------------------------------\n");
        fetchDefunSiblingProcesses(pids);
    }
    //Lists PIDs for all grandchildren of 1st processid
    else if (strcmp(option, "-gc") == 0)
    {
        printf("Lists PIDs for all Grandchildren process of 1st processid(-gc)\n");
        printf("-------------------------------------------------------------------\n");
        fetchGrandchildrenProcesses(pids);
    }
    //Prints the status of 1st processid for both (Defunct/ Not Defunct)
    else if (strcmp(option, "-zz") == 0)
    {
        printf("Print status of 1st processid(-zz)\n");
        printf("-------------------------------------------------------------------\n");
        printStatus(pids);
    }
    //Lists PIDs of all the direct descendants of 1st processid that are currently in the defunct state
    else //if (strcmp(option, "-zc") == 0)
    {
        printf("Lists PIDs for all direct descendants of 1st processid which are in defunct(-zc)\n");
        printf("-------------------------------------------------------------------\n");
        fetchDirectZombieDescendants(pids);
    }
}

int main(int argCount, char *argVar[])
{
    //built-in atoi() function to convert a string of characters to an integer value
    int rootProcess = atoi(argVar[1]);
    int processID1 = atoi(argVar[2]);
    char *options = argVar[argCount-1];
    int storePid[5];
    //int countPid = 0;

    if (argCount < 3)
    {
        printf("KINDLY use Synopsis format: %s [root_process] [1_process_id]....[n_process_id] [OPTION]\n", argVar[0]);
        return 1;
    }
	
    // Get the desired extensions
    if (argCount >= 3)
    {
        if(strcmp(options,"-nd") == 0||strcmp(options, "-dd") == 0||strcmp(options, "-sb") == 0||strcmp(options, "-sz") == 0||strcmp(options, "-gc") == 0||strcmp(options, "-zz") == 0||strcmp(options, "-zc") == 0){
            int j =0;
            int arrStatus[5];
            // UPTO 5 PROCESS ID's CAN BE PROVIDED AS A LIST
            for (int i=2; i<argCount-1 && i < 7; i++)
            {
                storePid[j]=atoi(argVar[i]);
                int k = findProcessId(rootProcess, storePid[j]);
                arrStatus[j]=k;
                j++;
                //countPid = countPid+1;
            }
            if(arrStatus[0]==0){   
                optionFun(rootProcess, processID1, options); 
                return 0;
            }
            else{
                printf("The 1st passed Processid = %d is not the decendent of the rootProcess id = %d\n",processID1,rootProcess);
                return 0;
            }
        }
        else{
            printf("Option is either not passed for this case or is incorrect!!!\nMake sure to choose either 1 of the below mentioned options if required \n1. -nd\n2. -dd\n3. -sb\n4. -sz\n5. -gc\n6. -zz\n7. -zc\n");
            int j =0;
            // UPTO 5 PROCESS ID's CAN BE PROVIDED AS A LIST
            for (int i=2; i<argCount && i < 7; i++)
            {
                storePid[j]=atoi(argVar[i]);
                findProcessId(rootProcess, storePid[j]);
                j++;
                //countPid = countPid+1;
            }       
            return 0;
        }
    }
}
