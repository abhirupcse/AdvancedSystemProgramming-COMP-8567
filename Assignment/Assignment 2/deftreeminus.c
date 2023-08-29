//@Author: Abhirup Ranjan
// Section: 3
// COMP8567

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

int child[265];
int bashPid;
int abhiPid;

// This method takes a pid as input and return the ParentProcess id of it
int fetchParentid(int pid) {
  char path[50];
  FILE *fd;
  int parentPpid;

  sprintf(path, "/proc/%d/stat", pid);
  fd = fopen(path, "r");
  if (fd == NULL) {
    printf("Error while opening the file!!");
    exit(0);
  }
  fscanf(fd, "%*d %*s %*c %d", &parentPpid);
  fclose(fd);
  return parentPpid;
}

// This method checks if the passed process id is a zombi process or not(Returns True or False)
bool checkZombie(int pid) {
  char path[50];
  FILE *fd;
  char statusLine[100];
  char *statusVar;
  bool result = false; // setting it to false
  sprintf(path, "/proc/%d/status", pid);
  fd = fopen(path, "r");
  if (fd == NULL) {
    printf("Error while opening the file!!");
    exit(0);
  }
  while (fgets(statusLine, sizeof(statusLine), fd) != NULL) {
    if (strncmp(statusLine, "State:", 6) == 0) {
      statusVar = statusLine + 7;
      if (strstr(statusVar, "Z (zombie)") != NULL) {
        result = true; // if this is defunct then set result to true
      }
      break;
    }
  }
  fclose(fd);
  return result;
}

// This methods finds all the child process for the given process pid
int *fetchChildProcess(int pid) {
  char path[100];
  DIR *directory;
  struct dirent *dirEntries;
  int childPid, childPpid;
  int counter = 0;

  sprintf(path, "/proc/");
  if ((directory = opendir(path)) == NULL) {
    printf("The passed Processid is not Correct %s\n", path);
    return 0;
  }

  while ((dirEntries = readdir(directory)) != NULL) {
    if (dirEntries->d_type == DT_DIR) {
      if (!isdigit(dirEntries->d_name[0]))
        continue; // skipping the folders which does not contain digits
      childPid = atoi(dirEntries->d_name);
      if (childPid == 1)
        continue; // skipping the main init folder
      if (childPid == pid)
        continue; // skipping the process
      // fetching the child's parentid
      childPpid = fetchParentid(childPid);
      if (childPpid == pid) {
        child[counter] = childPid;
        counter++;
      }
    }
  }
  return child;
  closedir(directory);
}

// This method fetches the elapsed duration in min for the passed pid
int fetchElapsedTime(int pid) {
  char fname[265], lines[265], *strVar;
  FILE *fd;
  int time;
  struct timespec tsStruct;

  sprintf(fname, "/proc/%d/stat", pid); //opening the proc file
  fd = fopen(fname, "r");
  if (fd == NULL) {
    printf("The passed ProcessID: %d is Invalid..Try AGAIN!!\n", pid);
    return -1;
  }

  // reading the start time from the file
  if (fgets(lines, 100, fd) == NULL) {
    printf("Error in ELAPSED TIME METHOD!!");
    fclose(fd);
    return -1;
  }
  strVar = strtok(lines, " ");
  for (int i = 1; i < 22; i++) {
    strVar = strtok(NULL, " ");
  }
  time = atoi(strVar);
  fclose(fd);

  // fetch the current time and calculating the elapsed time
  if (clock_gettime(CLOCK_MONOTONIC, &tsStruct) == -1) {
    printf("Error while getting the time!!");
    return -1;
  }
  return (int)(tsStruct.tv_sec - (time / sysconf(_SC_CLK_TCK))) / 60;
}

// This method check if the array contains values as 0
int checkChild(int childArr[]) {

  for (int i = 0; i < 265; i++) {
    if (childArr[i] > 0)
      return 1;
  }
  return 0;
}


//The method is used to kill the process
void killProcess(int pid) {
  if (pid == bashPid) { //global varibale
    printf("This has not been KILLED as its a BashProcess\n");
    return;
  }
  if (pid == abhiPid) { //global varibale value
    printf("\nNot killing as  passed pid is equal to passed processid %d\n", abhiPid);
    return;
  }
  printf("Killed PID- %d\n", pid);
  kill(pid, SIGKILL);//signal 9 used to kill
}

//fetch the processid inside the process tree
void fetchProcessTree(int pid, int depth, int passedElapsedTime,
                         int noZombie) {
  int *childArray;
  int localChildProcess[265], countZombie = 0, elapsedTime = 0;
  if (depth >= 265) {
    printf("ERROR LIMIT REACHED!!\n");
    return;
  }

  // fetching the elapse time of the parent
  elapsedTime = fetchElapsedTime(pid);

  // fetch the child processid for the parent
  childArray = fetchChildProcess(pid);
  memcpy(localChildProcess, childArray,
         265 * sizeof(int)); // making the copy of child process locally
  memset(child, 0, sizeof(child));    // resetting global array declared

  // check if its has child
  if (!checkChild(localChildProcess))
    return;

  // count the zombie childs
  if (0) {
    printf("Child process present for ProcessID: %d\n", pid);
  }
  for (int i = 0; i < 128; i++) {
    if (localChildProcess[i] == 0)
      continue; // skipping the null value
    if (0) {
      printf("%d", localChildProcess[i]);
    }

    // check if it is a zombie process if yes then count the zombie number
    if (checkZombie(localChildProcess[i])) {
      countZombie++;
      if (0) {
        printf(" Zombie---\n");
      }
    } 
    else {
      if (0) {
        printf("\n");
      }
    }
    // Recursivlly calling the method
    fetchProcessTree(localChildProcess[i], depth + 1, passedElapsedTime, noZombie);
  }

  if (0) {
    printf("The Processid %d has %d zombie or defunct childs with %d mins elapsed time\n",
           pid, countZombie, elapsedTime);
  }

  // kill the parentProcess when the elapsed time is greater than the passed PROC_ELTIME
  if (passedElapsedTime > 0 && countZombie > 0) {
    if (elapsedTime > passedElapsedTime) {
      killProcess(pid);
      return;
    }
  }

  // kill the parentProcess when no. of zombie child greater than equal to passed NO_OF_DFCS
  else if (noZombie > 0 && countZombie > 0) {
    if (countZombie >= noZombie) {
      killProcess(pid);
      return;
    }
  }

  // Kills all the present parentProcess
  else if (passedElapsedTime == 0 && noZombie == 0 && countZombie > 0) {
    killProcess(pid);
    return;
  }

  if (0) {
    printf("\nThe passed process id: %d is not killed!!\n", pid);
  }
}

int main(int argCount, char *argVar[]) {

  //varibale used as given in problem statement
  int PROC_ELTIME = 0, NO_OF_DFCS = 0; 
  char *option1;
  char *option2;
  int rootPid = 0;
  
  // check Synopsis
  if (argCount < 2 && argCount > 5) {
    printf("Synopsis: deftreeminus [root_process] [OPTION1] [OPTION2] " "[-processid]\n");
    return 1;
  }

  rootPid = atoi(argVar[1]);
  if (rootPid <= 0) {
    printf("Passed Root PID is not correct\n");
    return 1;
  }

  // checking the number of arguments here
  if (argCount == 3) { 
    abhiPid = atoi(argVar[2] + 1);
    if (abhiPid <= 0) {
      printf("Invalid Synopsis. Make sure to pass something with option 1!!!!Try AGAIN!!\n");
      return 1;
    }
  } else if (argCount == 4) {
    option1 = argVar[2];
    option2 = argVar[3];
  } else if (argCount == 5) {
    option1 = argVar[2];
    option2 = argVar[3];
    abhiPid = atoi(argVar[4] + 1);
    if (abhiPid <= 0) {
      printf("Invalid processID!!!!\n");
      return 1;
    }
  }

  if (argCount > 3) {
    if (0) {
      printf("Entered Option 1: %s\n", option1);
      printf("Entered Option 2: %s\n", option2);
    }

    if (strcmp(option1, "-t") == 0) {
      PROC_ELTIME = atoi(option2);
      if (PROC_ELTIME < 1) {
        printf("Incorrect PROC_ELTIME value has been entered.TRY AGAIN!!\n");
        return 1;
      }
    } else if (strcmp(option1, "-b") == 0) {
      NO_OF_DFCS = atoi(option2);
      if (NO_OF_DFCS < 1) {
        printf("Incorrect NO_OF_DFCS value has been entered.TRY AGAIN!!\n");
        return 1;
      }
    } else {
      printf("Invalid option1 has been entered.TRY AGAIN!!\n");
      return 1;
    }
  }

  // checking and stroing the bash PID here in global varibale
  int bashPid = getppid();

  // start searching process tree
  fetchProcessTree(rootPid, 0, PROC_ELTIME, NO_OF_DFCS);
  return 0;
}
