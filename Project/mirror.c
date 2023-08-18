/*
COMP8567 SECTION: 3 | ADVANCED SYSTEM PROGRAMING
TEAM NUMBER: 23
TEAM MEMBER 1: ABHIRUP RANJAN (110091866) 
TEAM MEMBER 2: ROHIT KUMAR (110088741)
*/

// TOOK HELP FROM GOOGLE AND OTHER TECHNICAL WEBSITES.

#define _XOPEN_SOURCE 500

#include <utime.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SPARE_FDS 5
#define BUFSIZE 4096
#define ALLOWED_FILES_COUNT_LIMIT 20

// GLOBAL VARIBALE DECLARATION
int roabNumOfConnections = 0;
//char *roabFilePath;
char roabFilePath[250];
char *roabFileName;
int size1, size2;
time_t date1, date2;
int roabChildDescriptor;
char *roabHomeDirectory;
int roabLimit;
char *tempCommand[10] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

// THIS METHOD CONVERTS THE PASSED STRING(DATE) TO THE PROPER DATE FORMAT YEAR-MONTH-DATE
time_t convertStringToDate(char *date)
{
    struct tm tm = {0};
    if (strptime(date, "%Y-%m-%d", &tm) == NULL)
    {
        return -1;
    }
    tm.tm_isdst = -1;
    time_t temp;
    temp = mktime(&tm);
    return temp;
}
// -----------------------------------------------------------------------


// TAKES 2 ARGUMENTS AS INPUT. 1. AN EMPTY ARRAY OF CHARACTER POINTERS "outputArr" & 2. A CHARACTER POINTER "rawCommand" FROM THE CONSOLE.
// THE PURPOSE OF THIS FUNCTION IS TO DEFINE THE "rawCommand" STRING USING SPACE (" ") AS A DELIMITER.
// AND STORE THE RESULTING COMMAND IN THE "outputArr" ARRAY.
void breakRawCommand(char *outputArr[50], char *rawCommand)
{
    int length = 0;
    char *ptr = strtok(rawCommand, " ");
    while (1)
    {
        if (ptr == NULL)
        {
            outputArr[length] = NULL;
            length++;
            break;
        }
        if (strcmp(ptr, "\n") == 0)
        {
            continue;
        }
        outputArr[length] = ptr;
        length++;
        ptr = strtok(NULL, " ");
    }

    if (length < 1 || length > 7)
    {
        printf("OUTPUT ARRAY SHOULD HAVE ARGUMENTS BEWTEEN 1-6 MAXIMUM!!\n");
        exit(1);
    }
}
// -----------------------------------------------------------------------


// TAKES 4 ARGUMENTS AS INPUT. 1. "fpath" POINTER TO THE PATH OF THE FILE OR DIRECTORY CURRENTLY BEING CONSIDERED DURING TRAVERSAL
// 2. "sb" STRUCT STAT THAT HOLDS INFORMATION ABOUT THE FILE OR DIRECTORY.
// 3. "flag" AN INTEGER INDICATING THE TYPE OF THE FILE OR DIRECTORY (FTW_F MEANS REGULAR FILE, FTW_D FOR A DIRECTORY).
// 4. POINTER TO A STRUCT FTW "ftwbuf" THAT PROVIDES ADDITIONAL TRAVERSAL INFORMATION.
// METHOD DESIGNED TO BE USED WITH "nftw" TO SEARCH FOR A SPECIFIC FILE ("roabFilePath") IN A DIRECTORY TREE TRAVERSAL.
// "nftw" MEHTOD USED FOR TRAVERSING DIRECTORY TREES AND PERFORMING ACTIONS ON FILES OR DIRECTORIES ENCOUNTERED DURING TRAVERSAL
// RETURNS AN INTEGER VALUE THAT INDICATES WHETHER A FILE WITH A MATCHING NAME WAS FOUND DURING THE DIRECTORY TREE TRAVERSAL.
// RETURN VALUE OF 1: IF FILE WITH THE DESIRED NAME WAS FOUND.
// RETURN VALUE OF 0: NO FILE WITH THE DESIRED NAME WAS FOUND OR THE ITEM BEING CONSIDERED IS NOT A REGULAR FILE.
int fileSearch(const char *fpath, const struct stat *sb, int flag, struct FTW *ftwbuf)
{
    if (flag == FTW_F)
    {
        if (strcmp(basename((char*)fpath), roabFileName) == 0)
        {
            // IF FILE WITH SAME NAME IS FOUND IT WOULD RETURN 1
            sprintf(roabFilePath, "%s", (char*)fpath);
            return 1;
        }
    }
    // IF NO FILE MATCH FOUND IT WILL RETURN O
    return 0;
}
// -----------------------------------------------------------------------


// CONVERT PASSED EPOCH TIME TO DATE/TIME FORMAT YEAR-MONTH-DATE HH:MM:SS
// METHOD TAKES 2 ARGUMENTS: 1. AN EPOCH TIME VALUE ("epochTime") AND 2. A CHARACTER POINTER ("timeStr")
// RESULTING FORMATTED DATE AND TIME STRING WILL BE STORED in "timeStr" YEAR-MONTH-DATE HH:MM:SS.
void convertEpochToNewDateTime(time_t epochTime, char *timeStr)
{
    struct tm *info;
    info = localtime(&epochTime);
    strftime(timeStr, 80, "%Y-%m-%d %H:%M:%S", info);
}
// -----------------------------------------------------------------------


// TAKES 2 ARGUMENTS AS INPUT. 1. SOURCE FILE PATH "fpath" AND 2. TEMPORARY DESTINATION PATH "desti"
// FUNCTION IS USED TO COPY THE CONTENTS OF A SOURCE FILE TO A DESTINATION FILE. 
// IT RETURNS 0 IF SUCCESS AND 1 FOR FAILURE.
int copyFileToTempPath(const char *fpath, char *desti)
{

    int fd1, fd2;
    char buffer[100];
    long int n1;

    strcat(desti, basename((char*)fpath));
    if ((fd1 = open(fpath, O_RDONLY)) == -1)
    {
        perror("ERROR OPENING FILE!!");
        return 1;
    }
    else
    {
        if ((fd2 = open(desti, O_CREAT | O_WRONLY | O_TRUNC, 0700)) == -1)
        {
            perror("FAILED OPENING FILE!!");
            return 1;
        }
    }

    while ((n1 = read(fd1, buffer, 100)) > 0)
    {
        if (write(fd2, buffer, n1) != n1)
        {
            perror("THERE IS SOME WRITING ISSUE....");
            return 1;
        }
        if (n1 == -1)
        {
            perror("THERE IS SOME READING ISSUE....");
            return 1;
        }
    }
    close(fd2);
    return 0;
}
// -----------------------------------------------------------------------


// METHOD RESPONSIBLE FOR CREATING A COMPRESSED TAR ARCHIVE ("".tar.gz" FORMAT).
// INPUT PARAMETER: A STRING SPECIFYING THE PATH TO THE DIRECTORY OR FILES THAT WE WANT TO INCLUDE IN THE TAR ARCHIVE.
// FUNCTION RETURNS AN INTEGER VALUE THAT INDICATES THE RESULT OF THE OPERATION.
// IF THE TAR ARCHIVE IS SUCCESSFULLY CREATED, THE FUNCTION RETURNS 1
// IN CASE OF AN ERROR DURING THE CREATION PROCESS THE FUNCTION RETURNS -1.
// int buildTarFile(char *tarName, char *destinationPath)
int buildTarFile(char *destinationPath)
{
    char roabCommand[100];
    fprintf(stderr, "CREATING TAR FILE!!\n");
    sprintf(roabCommand, "tar -czvf %s.tar.gz %s", "temp", destinationPath);
    if (system(roabCommand) == -1)
    {
        printf("ERROR WHILE CREATING 'tar.gz' TAR ARCHIVE FILE.......\n");
        return -1;
    }
    return 1;
}
// -----------------------------------------------------------------------


// THIS METHOD REMOVE A FILE OR FOLDER FROM THE FILESYSTEM.
// METHOD TAKE 1 ARGUMENT: "roabFileName" A STRING SPECIFYING THE PATH TO THE FILE OR FOLDER THAT YOU WANT TO REMOVE.  
// FUNCTION RETURNS AN INTEGER VALUE THAT INDICATES THE RESULT OF THE OPERATION.
// IF THE FILE OR FOLDER IS SUCCESSFULLY REMOVED, THE FUNCTION RETURNS 1.
// IN CASE OF AN ERROR DURING THE REMOVAL PROCESS THE FUNCTION RETURNS -1.
int removeFileFolder(char *roabFileName)
{
    char roabCmd[100];
    sprintf(roabCmd, "rm -rf %s", roabFileName);

    if (system(roabCmd) == -1)
    {
        printf("ERROR WHILE REMOVING THE FILE OR FOLDER....\n");
        return -1;
    }
    return 1;
}
// -----------------------------------------------------------------------


// METHOD HELPS IN SENDING THE CONTENTS OF A FILE TO A CLIENT SOCKET. 
// THE FUNCTION CREATES A CHILD PROCESS USING fork() TO HANDLE THE SENDING OPERATION, WHILE THE PARENT PROCESS WAITS FOR THE CHILD PROCESS TO END.
// METHOD TAKE 2 ARGUMENTS: 1. "fptr" STRING SPECIFYING THE PATH TO THE FILE THAT IS REQUIRED TO BE SENT.
// 2. "socketfd" AN INTEGER REPRESENTING THE SOCKET FILE DESCRIPTOR TO WHICH THE FILE CONTENTS SHOULD BE SENT.
// FUNCTION RETURNS AN INTEGER VALUE THAT INDICATES THE RESULT OF THE OPERATION.
// IF THE FILE CONTENTS ARE SUCCESSFULLY SENT, THE FUNCTION RETURNS 1.
// IN CASE OF AN ERROR DURING THE SENDING FILE PROCESS THE FUNCTION RETURNS -1.
int transferFileToClientSocket(char *fptr, int socketfd)
{
    int pid = fork();
    if (pid == 0) // CHILD PROCESS
    {
        int fd, roabLength;
        printf("TRANSFERING FILES TO CLIENT...\n");
        fd = open(fptr, O_RDONLY);
        char buffer[BUFSIZE];

        while ((roabLength = read(fd, buffer, BUFSIZE)) > 0)
        {
            if (send(socketfd, buffer, roabLength, 0) == -1)
            {
                perror("send");
                return -1;
            }
        }
        sleep(1);
        printf("FILE TRANSFERED TO CLIENT SUCCESSFULLY!!\n");
        close(fd);
        close(socketfd);
        exit(0);
    }

    wait(NULL); // PARENT WAIT HERE FOR CHILD TO END
    return 1;
}
// -----------------------------------------------------------------------


// THE METHOD HANDLE THE PROCESS OF SENDING FILES TO CLIENT, PERFORMING NECESSARY ACTIONS SUCH AS 
// CREATING A TAR ARCHIVE, SENDING THE FILE OVER THE SOCKET, AND REMOVING TEMPORARY FILES.
// METHOD TAKE ONLY 1 ARGUMENT: "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES
// DOES NOT RETURN ANYTHINF BUT THE "roabResult" PARAMETER IS USED TO COMMUNICATE THE SUCCESS OR FAILURE OF THESE ACTIONS TO THE CALLER.
void sendFileToClient(char *roabResult)
{
    char roabBuffer[150];
    char roabDir[50];
    char roabTarFileName[40];
    int roabVar;
    sprintf(roabDir, "%d", roabChildDescriptor);
    roabVar = sprintf(roabTarFileName, "temp%d", roabChildDescriptor);
    roabTarFileName[roabVar] = '\0';
    if (buildTarFile(roabDir) == -1)
    {
        roabVar = sprintf(roabBuffer, "SOMETHING WENT WRONG...\n");
        roabBuffer[roabVar] = '\0';
        strcpy(roabResult, roabBuffer);
        fprintf(stderr, "TAR FILE CREATION HAS BEEN FAILED FOR CLIENT: %d\n", roabChildDescriptor);
        return;
    }
    sprintf(roabDir, "%s.tar.gz", roabTarFileName);
    roabVar = sprintf(roabBuffer, "TAR FILE SENT");
    roabBuffer[roabVar] = '\0';
    write(roabChildDescriptor, roabBuffer, strlen(roabBuffer) + 1);

    if (transferFileToClientSocket(roabDir, roabChildDescriptor) == -1)
    {
        roabVar = sprintf(roabBuffer, "SOMETHING WENT WRONG...\n");
        roabBuffer[roabVar] = '\0';
        strcpy(roabResult, roabBuffer);
        fprintf(stderr, "SOME ISSUE WHILE SENDING THE FILE TO CLIENT: %d\n", roabChildDescriptor);
        return;
    }

    removeFileFolder(roabDir);
    sprintf(roabDir, "%d", roabChildDescriptor);
    removeFileFolder(roabDir);
    roabVar = sprintf(roabBuffer, "REQUEST FOR THE CLIENT HAS BEEN PROCESSED SUCCESSFULLY!!");
    roabBuffer[roabVar] = '\0';
    strcpy(roabResult, roabBuffer);
}
// -----------------------------------------------------------------------


// TAKES 4 ARGUMENTS AS INPUT. 1. "fpath" POINTER TO THE PATH OF THE FILE OR DIRECTORY CURRENTLY BEING CONSIDERED DURING TRAVERSAL
// 2. "sb" STRUCT STAT THAT HOLDS INFORMATION ABOUT THE FILE OR DIRECTORY.
// 3. "flag" AN INTEGER INDICATING THE TYPE OF THE FILE OR DIRECTORY (FTW_F MEANS REGULAR FILE, FTW_D FOR A DIRECTORY).
// 4. POINTER TO A STRUCT FTW "ftwbuf" THAT PROVIDES ADDITIONAL TRAVERSAL INFORMATION.
// METHOD DESIGNED TO BE USED WITH "nftw" TO SEARCH FOR A SPECIFIC FILE ("roabFilePath") IN A DIRECTORY TREE TRAVERSAL.
// "nftw" MEHTOD USED FOR TRAVERSING DIRECTORY TREES AND PERFORMING ACTIONS ON FILES OR DIRECTORIES ENCOUNTERED DURING TRAVERSAL
// RETURNS AN INTEGER VALUE THAT INDICATES WHETHER A FILE WITH A MATCHING NAME WAS FOUND DURING THE DIRECTORY TREE TRAVERSAL.
// RETURN VALUE OF 1: IF FILE WITH THE DESIRED NAME WAS FOUND.
// RETURN VALUE OF 0: NO FILE WITH THE DESIRED NAME WAS FOUND OR THE ITEM BEING CONSIDERED IS NOT A REGULAR FILE.
int processFgetsCommandfstw(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    {
        int i = 0;
        while (tempCommand[i] != NULL)
        {
            if (strcmp(basename((char*)fpath), tempCommand[i]) == 0)
            {
                roabLimit--;
                char dir[42];
                sprintf(dir, "./%d/", roabChildDescriptor);
                mkdir(dir, 0700);
                copyFileToTempPath(fpath, dir);
                if (roabLimit == 0)
                {
                    return 1;
                }
            }
            i++;
        }
    }
    return 0;
}
// -----------------------------------------------------------------------


// THE METHOD HANDLE THE PROCESS THE "fgets" COMMAND SENT FROM CLIENT, PERFORMING NECESSARY ACTIONS SUCH AS 
// IT SEARCHES FOR SPECIFIC FILES IN A DIRECTORY HIERARCHY AND PERFORMS ACTIONS ON THOSE FILES.
// METHOD TAKE 2 ARGUMENT: 1. CONTAIN THE COMMAND LINE ARGUMENT
// 2. "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES
// DOES NOT RETURN ANYTHINF BUT THE "roabResult" PARAMETER IS USED TO COMMUNICATE THE RESULT MESSAGE BACK TO THE CLIENT.
void processFgetsCommand(char *roabCmd[10], char *roabResult)
{
    roabLimit = ALLOWED_FILES_COUNT_LIMIT;
    long nfds = sysconf(_SC_OPEN_MAX) - SPARE_FDS;
    int i = 0;
    while (roabCmd[i] != NULL)
    {
        tempCommand[i] = roabCmd[i];
        i++;
    }
    tempCommand[i] = NULL;
    nftw(roabHomeDirectory, processFgetsCommandfstw, nfds, FTW_PHYS);
    if (roabLimit == ALLOWED_FILES_COUNT_LIMIT)
    {
        strcpy(roabResult, "NO FILE FOUND!!");
        return;
    }
    sendFileToClient(roabResult);
}
// -----------------------------------------------------------------------


// TAKES 4 ARGUMENTS AS INPUT. 1. "fpath" POINTER TO THE PATH OF THE FILE OR DIRECTORY CURRENTLY BEING CONSIDERED DURING TRAVERSAL
// 2. "sb" STRUCT STAT THAT HOLDS INFORMATION ABOUT THE FILE OR DIRECTORY.
// 3. "flag" AN INTEGER INDICATING THE TYPE OF THE FILE OR DIRECTORY (FTW_F MEANS REGULAR FILE, FTW_D FOR A DIRECTORY).
// 4. POINTER TO A STRUCT FTW "ftwbuf" THAT PROVIDES ADDITIONAL TRAVERSAL INFORMATION.
// METHOD DESIGNED TO BE USED WITH "nftw" TO SEARCH FOR A SPECIFIC FILE ("roabFilePath") IN A DIRECTORY TREE TRAVERSAL.
// "nftw" MEHTOD USED FOR TRAVERSING DIRECTORY TREES AND PERFORMING ACTIONS ON FILES OR DIRECTORIES ENCOUNTERED DURING TRAVERSAL
// RETURNS AN INTEGER VALUE THAT INDICATES WHETHER A FILE WITH A MATCHING NAME WAS FOUND DURING THE DIRECTORY TREE TRAVERSAL.
// RETURN VALUE OF 1: IF FILE WITH THE DESIRED NAME WAS FOUND.
// RETURN VALUE OF 0: NO FILE WITH THE DESIRED NAME WAS FOUND OR THE ITEM BEING CONSIDERED IS NOT A REGULAR FILE.
int processTarfgetzCommandfstw(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    {
        struct stat st;
        if (stat(fpath, &st) == 0)
        {
            if (st.st_size >= size1 && st.st_size <= size2)
            {
                roabLimit--;
                char dir[42];
                sprintf(dir, "./%d/", roabChildDescriptor);
                mkdir(dir, 0700);
                copyFileToTempPath(fpath, dir);
                if (roabLimit == 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// -----------------------------------------------------------------------


// THE METHOD HANDLE THE PROCESS THE "tarfgetz" COMMAND SENT FROM CLIENT, PERFORMING NECESSARY ACTIONS SUCH AS 
// IT SEARCHES FOR SPECIFIC FILES FOR DEFINED SIZE LIMIT IN A DIRECTORY HIERARCHY AND PERFORMS ACTIONS ON THOSE FILES.
// METHOD TAKE 2 ARGUMENT: 1. CONTAIN THE COMMAND LINE ARGUMENT
// 2. "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES
// DOES NOT RETURN ANYTHINF BUT THE "roabResult" PARAMETER IS USED TO COMMUNICATE THE RESULT MESSAGE BACK TO THE CLIENT.
void processTarfgetzCommand(char *roabCmd[10], char *roabResult)
{
    roabLimit = ALLOWED_FILES_COUNT_LIMIT;
    size1 = atoi(roabCmd[1]);
    size2 = atoi(roabCmd[2]);

    long nfds = sysconf(_SC_OPEN_MAX) - SPARE_FDS;
    nftw(roabHomeDirectory, processTarfgetzCommandfstw, nfds, FTW_PHYS);
    if (roabLimit == ALLOWED_FILES_COUNT_LIMIT)
    {
        strcpy(roabResult, "FILE NOT FOUND");
        return;
    }
    sendFileToClient(roabResult);
}
// -----------------------------------------------------------------------


// METHOD HANDLES A FILE SEARCH "filesrch" COMMAND LOGIC BY USING THE "nftw" FUNCTION TO SEARCH FOR A SPECIFIED FILE NAME IN A SPECIFIED DIRECTORY.
// METHOD TAKE 2 ARGUMENT: 1. CONTAIN THE COMMAND LINE ARGUMENT
// 2. "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES 
// IF THE FILE IS FOUND, IT EXTRACTS INFORMATION ABOUT THE FILE AND FORMATS IT INTO A STRING, WHICH IS THEN RETURNED AS THE "roabResult". 
// IF THE FILE IS NOT FOUND, "FILE NOT FOUND" IS RETURNED AS THE "roabResult".
void processFileSrchCommand(char *roabCmd[10], char *roabResult)
{
    long nfds = sysconf(_SC_OPEN_MAX) - SPARE_FDS;
    char roabTimeString[80];
    struct stat st;
    char roabBuffer[150];

    roabFileName = roabCmd[1];
    int status = nftw(roabHomeDirectory, fileSearch, nfds, FTW_PHYS);
    if (status != 1)
    {
        strcpy(roabResult, "FILE NOT FOUND");
        return;
    }
    stat(roabFilePath, &st);
    convertEpochToNewDateTime(st.st_mtime, roabTimeString);
    int n = sprintf(roabBuffer, "FILE HAS BEEN FOUND WITH BELOW DETAILS:-\n1. NAME OF THE FILE - %s\n2. SIZE OF THE FILE - %lld\n3. MODIFIED AT - %s", basename(roabFilePath), st.st_size, roabTimeString);
    roabBuffer[n] = '\0';
    strcpy(roabResult, roabBuffer);
}
// -----------------------------------------------------------------------


// TAKES 4 ARGUMENTS AS INPUT. 1. "fpath" POINTER TO THE PATH OF THE FILE OR DIRECTORY CURRENTLY BEING CONSIDERED DURING TRAVERSAL
// 2. "sb" STRUCT STAT THAT HOLDS INFORMATION ABOUT THE FILE OR DIRECTORY.
// 3. "flag" AN INTEGER INDICATING THE TYPE OF THE FILE OR DIRECTORY (FTW_F MEANS REGULAR FILE, FTW_D FOR A DIRECTORY).
// 4. POINTER TO A STRUCT FTW "ftwbuf" THAT PROVIDES ADDITIONAL TRAVERSAL INFORMATION.
// METHOD DESIGNED TO BE USED WITH "nftw" TO SEARCH FOR A SPECIFIC FILE ("roabFilePath") IN A DIRECTORY TREE TRAVERSAL.
// "nftw" MEHTOD USED FOR TRAVERSING DIRECTORY TREES AND PERFORMING ACTIONS ON FILES OR DIRECTORIES ENCOUNTERED DURING TRAVERSAL
// RETURNS AN INTEGER VALUE THAT INDICATES WHETHER A FILE WITH A MATCHING NAME WAS FOUND DURING THE DIRECTORY TREE TRAVERSAL.
// RETURN VALUE OF 1: IF FILE WITH THE DESIRED NAME WAS FOUND.
// RETURN VALUE OF 0: NO FILE WITH THE DESIRED NAME WAS FOUND OR THE ITEM BEING CONSIDERED IS NOT A REGULAR FILE.
int processTargzfCommandfstw(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    {
        int i = 0;
        char *extension = strrchr(fpath, '.');
        while (tempCommand[i] != NULL)
        {
            if (strcmp(extension, tempCommand[i]) == 0)
            {
                roabLimit--;
                char dir[42];
                sprintf(dir, "./%d/", roabChildDescriptor);
                mkdir(dir, 0700);
                copyFileToTempPath(fpath, dir);
                if (roabLimit == 0)
                {
                    return 1;
                }
            }
            i++;
        }
    }
    return 0;
}
// -----------------------------------------------------------------------


// THE METHOD HANDLE THE PROCESS THE "targzf" COMMAND SENT FROM CLIENT, PERFORMING NECESSARY ACTIONS SUCH AS 
// IT SEARCHES FOR SPECIFIC FILES WITH DEFINED EXTENSION IN A DIRECTORY HIERARCHY AND PERFORMS ACTIONS ON THOSE FILES.
// METHOD TAKE 2 ARGUMENT: 1. CONTAIN THE COMMAND LINE ARGUMENT
// 2. "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES
// DOES NOT RETURN ANYTHINF BUT THE "roabResult" PARAMETER IS USED TO COMMUNICATE THE RESULT MESSAGE BACK TO THE CLIENT.
void processTargzfCommand(char *roabCmd[10], char *roabResult)
{
    roabLimit = ALLOWED_FILES_COUNT_LIMIT;
    long nfds = sysconf(_SC_OPEN_MAX) - SPARE_FDS;
    int i = 0;
    while (roabCmd[i] != NULL)
    {
        tempCommand[i] = roabCmd[i];
        i++;
    }
    tempCommand[i] = NULL;
    nftw(roabHomeDirectory, processTargzfCommandfstw, nfds, FTW_PHYS);
    if (roabLimit == ALLOWED_FILES_COUNT_LIMIT)
    {
        strcpy(roabResult, "NO FILE FOUND!!");
        return;
    }
    sendFileToClient(roabResult);
}
// -----------------------------------------------------------------------


// TAKES 4 ARGUMENTS AS INPUT. 1. "fpath" POINTER TO THE PATH OF THE FILE OR DIRECTORY CURRENTLY BEING CONSIDERED DURING TRAVERSAL
// 2. "sb" STRUCT STAT THAT HOLDS INFORMATION ABOUT THE FILE OR DIRECTORY.
// 3. "flag" AN INTEGER INDICATING THE TYPE OF THE FILE OR DIRECTORY (FTW_F MEANS REGULAR FILE, FTW_D FOR A DIRECTORY).
// 4. POINTER TO A STRUCT FTW "ftwbuf" THAT PROVIDES ADDITIONAL TRAVERSAL INFORMATION.
// METHOD DESIGNED TO BE USED WITH "nftw" TO SEARCH FOR A SPECIFIC FILE ("roabFilePath") IN A DIRECTORY TREE TRAVERSAL.
// "nftw" MEHTOD USED FOR TRAVERSING DIRECTORY TREES AND PERFORMING ACTIONS ON FILES OR DIRECTORIES ENCOUNTERED DURING TRAVERSAL
// RETURNS AN INTEGER VALUE THAT INDICATES WHETHER A FILE WITH A MATCHING NAME WAS FOUND DURING THE DIRECTORY TREE TRAVERSAL.
// RETURN VALUE OF 1: IF FILE WITH THE DESIRED NAME WAS FOUND.
// RETURN VALUE OF 0: NO FILE WITH THE DESIRED NAME WAS FOUND OR THE ITEM BEING CONSIDERED IS NOT A REGULAR FILE.
int processGetdirfCommandfstw(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F)
    {
        struct stat st;
        if (stat(fpath, &st) == 0)
        {
            // time_t creation_time = st.st_birthtimespec.tv_sec;
            time_t creation_time = st.st_mtime;
            printf("%s %ld\n", fpath, creation_time);
            if (creation_time >= date1 && creation_time < (date2 + 86400))
            {
                roabLimit--;
                char dir[42];
                sprintf(dir, "./%d/", roabChildDescriptor);
                mkdir(dir, 0700);
                copyFileToTempPath(fpath, dir);
                if (roabLimit == 0)
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}
// -----------------------------------------------------------------------


// THE METHOD HANDLE THE PROCESS THE "getdirf" COMMAND SENT FROM CLIENT, PERFORMING NECESSARY ACTIONS SUCH AS 
// IT SEARCHES FOR SPECIFIC FILES BETWEEN 2 DATES IN A DIRECTORY HIERARCHY AND PERFORMS ACTIONS ON THOSE FILES.
// METHOD TAKE 2 ARGUMENT: 1. CONTAIN THE COMMAND LINE ARGUMENT
// 2. "roabResult" AN EMPTY ARRAY WHERE THE FINAL STATUS MESSAGE WILL BE STORED AFTER THE FUNCTION COMPLETES
// DOES NOT RETURN ANYTHINF BUT THE "roabResult" PARAMETER IS USED TO COMMUNICATE THE RESULT MESSAGE BACK TO THE CLIENT.
void processGetdirfCommand(char *roabCmd[10], char *roabResult)
{
    roabLimit = ALLOWED_FILES_COUNT_LIMIT;
    long nfds = sysconf(_SC_OPEN_MAX) - SPARE_FDS;
    date1 = convertStringToDate(roabCmd[1]);
    date2 = convertStringToDate(roabCmd[2]);

    printf("SEARCHING THE FILES BETWEEN DATES: %ld & %ld\n", date1, date2);
    nftw(roabHomeDirectory, processGetdirfCommandfstw, nfds, FTW_PHYS);
    if (roabLimit == ALLOWED_FILES_COUNT_LIMIT)
    {
        strcpy(roabResult, "FILE NOT FOUND");
        return;
    }
    sendFileToClient(roabResult);
}
// -----------------------------------------------------------------------


// THE PURPOSE OF THIS METHOD IS TO HANDLE CLIENT SENT DEFINED COMMANDS AND PROCESS THE LOGIC
// METHOD TAKE 2 ARGUMENTS: 1. "rawCommand" COMMAND RECEIVED FROM THE CLIENT
// 2. A EMPTY BUFFER "roabResult" WHERE THE RESULT OR RESPONSE MESSAGE OF PROCESSING THE COMMAND WILL BE STORED. 
// THERE IS NO RETURN TYPE OF THIS FUNCTION BUT THE RESPONSE MESSAGE IS STORED IN THE "roabResult" PARAMETER.
void processUsrDefinedCommands(char *rawCommand, char *roabResult)
{
    char *roabCmd[10];
    breakRawCommand(roabCmd, rawCommand);

    if (strcmp(roabCmd[0], "fgets") == 0)
    {
        processFgetsCommand(roabCmd, roabResult);
        return;
    }
    if (strcmp(roabCmd[0], "tarfgetz") == 0)
    {
        processTarfgetzCommand(roabCmd, roabResult);
        return;
    }
    if (strcmp(roabCmd[0], "filesrch") == 0)
    {
        processFileSrchCommand(roabCmd, roabResult);
        return;
    }
    if (strcmp(roabCmd[0], "targzf") == 0)
    {
        processTargzfCommand(roabCmd, roabResult);
        return;
    }
    if (strcmp(roabCmd[0], "getdirf") == 0)
    {
        processGetdirfCommand(roabCmd, roabResult);
        return;
    }
    if (strcmp(roabCmd[0], "quit") == 0)
    {
        strcpy(roabResult, "Exit"); // MESSAGE SENT FROM SERVER
        return;
    }
    strcpy(roabResult, "BAD REQUEST!!!!\nINVALID COMMAND OR COMMAND NOT SUPPORTED BY SERVER...KINDLY CHECK AGAIN...\n");
}
// -----------------------------------------------------------------------


// THE PURPOSE OF THIS METHOD IS TO HANDLE CLIENT REQUESTS. IT READS RAW COMMAND STRINGS SENT BY CLIENTS,
// PROCESSES THESE COMMANDS USING THE PROCESSUSRDEFINEDCOMMANDS FUNCTION AND SENDS BACK THE RESULTS TO THE CLIENT.
// METHOD TAKE ONLY 1 ARGUMENT: THE SOCKET DESCRIPTOR "sd" OF THE CLIENT CONNECTION
void processClientRequest(int sd)
{
    roabChildDescriptor = sd;
    char rawCommand[255];
    while (1)
    {
        int n = read(sd, rawCommand, 255);
        if (n <= 0)
        {
            printf("Bye, CLIENT IS KILLED/TERMINATED NOW\nWAITING FOR A NEW CLIENT TO STABLISH CONNECTION\n");
            printf("-----------------------------------------------------------------------------------------------\n");
            close(sd);
            exit(0);
        }
        rawCommand[n] = '\0';
        printf("COMMAND RECEIVED AS: %s,\nNOW PROCESSING THE REQUEST IN MIRROR....\n", rawCommand);
        char roabResult[255];
        processUsrDefinedCommands(rawCommand, roabResult);
        write(sd, roabResult, strlen(roabResult) + 1);
        printf("MIRROR SERVER MESSAGE-> %s\n",roabResult);
        printf("++++++++++++++++++\n");
    }
}
// -----------------------------------------------------------------------


// MAIN METHOD BEGIN HERE 
int main(int argc, char *argv[])
{
    int sd, clientDescriptor, portNumber;
    struct sockaddr_in servAdd;

    if (argc != 2)
    {
        fprintf(stderr, "INVALID MIRROR START COMMAND...\nFORMAT-> %s <PORT>\n",argv[0]);
        exit(0);
    }

    if (argv[1][strlen(argv[1]) - 1] == '\n')
    {
        argv[1][strlen(argv[1]) - 1] = '\0';
    }

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "UNABLE TO CREATE SOCKET. EXITING NOW...\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);
    bind(sd, (struct sockaddr *)&servAdd, sizeof(servAdd));
    listen(sd, 5);
    printf("MIRROR IS LISTENING ON PORT NUMBER: %d\n", portNumber);
    roabHomeDirectory = strcat(getenv("HOME"), "/roab");
    //roabHomeDirectory = getenv("HOME");
    
    char buffer[100];
    printf("WAITING FOR THE CLIENT TO CONNECT!!\n");
  while (1)
    {
        roabNumOfConnections++;
        printf("<<-------------------------------------------------------------->>\n");
        clientDescriptor = accept(sd, (struct sockaddr *)NULL, NULL);
        printf("CLIENT CONNECTION ESTABLISHED SUCCESSFULLY!!\n");
        int n = sprintf(buffer, "success");
        buffer[n] = '\0';
        write(clientDescriptor, buffer, strlen(buffer) + 1);
        if (!fork())
            processClientRequest(clientDescriptor);
        close(clientDescriptor);
        printf("WAITING FOR THE OTHER NEW CLIENT TO CONNECT!!\n");
    }
    return 0;
}
