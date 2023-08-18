/*
    COMP 8567 SECTION: 3 | ADVANCED SYSTEM PROGRAMING
    TEAM NUMBER: 23
    TEAM MEMBER 1: ABHIRUP RANJAN (110091866)
    TEAM MEMBER 2: ROHIT KUMAR (110088741)
*/

// TOOK HELP FROM GOOGLE AND OTHER TECHNICAL WEBSITES.

#define _XOPEN_SOURCE 500

// INCLUDE HEADER FILES
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <utime.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// PREPROCESSOR DIRECTIVE TO DEFINE CONSTANT MACROS
#define MAX_BUFFER_SIZE 1024
#define MAX_COMMAND_LENGTH 1000

// GLOBAL VARIBALE DECLARATION
typedef struct File File;
int isUnzip = 0; // UNZIP FLAG FILE -> 0 - UNZIP, 1 - NO UNZIP

// BELOW METHOD CONVERTS STRING DATE TO DATE FORMAT(YYYY-MM-DD)
// IT TAKES INPUT AS CHARACTER POINTER AND RETURNS TO AS DATE FORMAT
time_t convertStringToDateFormat(char *charDate)
{
    struct tm tm = {0};
    if (strptime(charDate, "%Y-%m-%d", &tm) == NULL)
    {
        return -1;
    }
    tm.tm_isdst = -1;
    time_t tempDate = mktime(&tm);
    return tempDate;
}

// BELOW ARE THE CLIENT COMMAND PROVIDED BY THE USER IN THE CLENT TERMINAL
//   1 - fgets file1 file2 file3 file4
//   2 - tarfgetz size1 size2 <-u>
//   3 - filesrch filename
//   4 - targzf <extension list> <-u> //up to 4 different file types
//   5 - getdirf date1 date2 <-u>
//   6 - quit-  The command is transferred to the server and the client process is terminated
// BELOW METHOD VALIDATES COMMAND PROVIDED BY CLIENT/USER ARE CORRECT OR NOT
// INCASE OF INCORRECT COMMAND POPULATE THE APPROPRIATE ERROR MESSAGE TO CLIENT TERMINAL
int invokeValidateclientCommand(char *roabRawClientCommand)
{
    char *ptr = strtok(roabRawClientCommand, " ");
    char *local[50];
    int cnt = 0;
    int i = 0;
    while (1)
    {
        char *ptr1 = strtok(NULL, " ");
        if (ptr1 == NULL)
        {
            break;
        }
        if (strcmp(ptr1, "\n") == 0)
        {
            continue;
        }
        local[i] = ptr1;
        i++;
        cnt++;
    }

    if (cnt > 0 && strcmp(local[cnt - 1], "-u\n") == 0)
    {
        isUnzip = 1;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "filesrch" IS IN CORRECT FORMAT
    if (strcmp(ptr, "filesrch") == 0)
    {
        if (cnt != 1)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- filesrch `filename`\n");
            return -1;
        }
        return 1;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "tarfgetz" IS IN CORRECT FORMAT
    // VALIDATE THE OTHER BASIC CONDITIONS
    if (strcmp(ptr, "tarfgetz") == 0)
    {
        if (cnt < 2 || cnt > 3)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- tarfgetz size1 size2 <-u>\n");
            return -1;
        }

        char *errsize1, *errsize2;

        long int size1 = strtol(local[0], &errsize1, 10);
        if (*errsize1 != '\0')
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- tarfgetz size1 size2 <-u>: [Size1] SHOULD BE INTEGER\n");
            return -1;
        }

        long int size2 = strtol(local[1], &errsize2, 10);
        if (*errsize2 != '\0')
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- tarfgetz size1 size2 <-u>: [Size2] SHOULD BE INTEGER\n");
            return -1;
        }

        if (size1 < 0 || size2 < 0)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- tarfgetz size1 size2 <-u>: [Size1, Size2] >= 0\n");
            return -1;
        }

        if (size2 < size1)
        {
            fprintf(stderr,
                    "INVALID CLIENT COMMAND ENTERED- tarfgetz size1 size2 <-u>: SIZE 1 SHOULD BE LESS THAN EQUAL TO SIZE 2\n");
            return -1;
        }

        return 2;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "getdirf" IS IN CORRECT FORMAT
    // VALIDATE THE OTHER BASIC CONDITIONS
    if (strcmp(ptr, "getdirf") == 0)
    {
        if (cnt < 2 || cnt > 3)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- getdirf size1 size2 <-u>\n");
            return -1;
        }

        time_t date1, date2;
        date1 = convertStringToDateFormat(local[0]);
        date2 = convertStringToDateFormat(local[1]);
        if (date1 == -1 || date2 == -1)
        {
            fprintf(stderr, "INVALID DATE-FORMAT, IT SHOULD BE IN YYYY-MM-DD\n");
            return -1;
        }

        if (date2 < date1)
        {
            fprintf(stderr, "INVALID DATE RANGE- DATE 2 SHOULD BE GREATER THAN EQUAL TO DATE 1\n");
            return -1;
        }
        return 2;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "fgets" IS IN CORRECT FORMAT
    // VALIDATE THE OTHER BASIC CONDITIONS
    if (strcmp(ptr, "fgets") == 0)
    {
        isUnzip = 0;
        if (isUnzip == 0 && cnt > 4)
        {
            fprintf(stderr,
                    "INVALID CLIENT COMMAND ENTERED- fgets file1 file2 file3 file4(file 1 ..up to file4)\n");
            return -1;
        }

        if (cnt < 1 || cnt > 4)
        {
            fprintf(stderr,
                    "INVALID CLIENT COMMAND ENTERED- fgets file1 file2 file3 file4(file 1 ..up to file4)\n");
            return -1;
        }

        return 4;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "targzf" IS IN CORRECT FORMAT
    // VALIDATE THE OTHER BASIC CONDITIONS
    if (strcmp(ptr, "targzf") == 0)
    {
        if (isUnzip == 0 && cnt > 4)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- targzf <extension list> <-u> //up to 4 different file types\n");
            return -1;
        }

        if (cnt < 1 || cnt > 5)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- targzf <extension list> <-u> //up to 4 different file types\n");
            return -1;
        }
        return 5;
    }

    // VALIDATE IF CLIENT ENTERED COMMAND "quit" IS IN CORRECT FORMAT
    // VALIDATE THE OTHER BASIC CONDITIONS
    if (strcmp(ptr, "quit\n") == 0)
    {
        if (cnt)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND ENTERED- quit\n");
            return -1;
        }
        return 6;
    }

    fprintf(stderr, "COMMAND NOT SUPPORTED, ENTER CORRECT COMMAND!!\n");
    return -1;
}

// BELOW METHOD IS UNZIP THE TAR.GZ FILE AND KEEP THE FILE AT THE ROOT LOCATION
// THIS METHOD TAKES THE FILENAME AS POINTER TO CHARACTER AND UNZIP THE FILE USING "tar -xzvf" SYSTEM CALL
int invokeUnzipTarFile(char *roabTarFileName)
{
    printf("UNZIPING THE FILE %s\n", roabTarFileName);
    char buffer[100];
    int n = sprintf(buffer, "tar -xzvf %s", roabTarFileName);
    buffer[n] = '\0';
    int roabResult = system(buffer); // run the tar command to unzip the files
    if (roabResult != 0)
    {
        printf("ERROR OCCURED!! WHILE EXTRACTING THE FILES\n");
        return -1;
    }
    printf("TAR FILE %s IS UNZIPPED SUCCESSFULLY, CHECK IN THE ROOT FOLDER..\n", roabTarFileName);
    return 1;
}

// BELOW METHOD INVOKE THE UNZIP METHOD TO EXTRACT THE FILES RECEIVED FROM THE SERVER
// THIS METHODS TAKES ONE INPUT, 1- unzip FLAG
// BASED ON THE FLAG, IT IS UNZIPPING THE FILES
void invokeUnzipFileFromServer(int doUnzip)
{
    sleep(5); // it will wait for 5 secs, to let server socket connection to transfer the file to client
    printf("TAR FILE HAS BEEN CREATED AT ROOT LOCATION\n");
    if (doUnzip == 1)
    {
        invokeUnzipTarFile("temp.tar.gz");
    }
}

// BELOW METHOD CONNECTS TO THE SERVER USING SOCKETS
// IT REQUIRES TWO PARAMETER TO CONNECT THE SERVER 1. host-ip AND 2. port number
int invokeConnectClientToServer(char *host, char *port)
{
    int server, portNumber;
    struct sockaddr_in servAdd;
    if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        fprintf(stderr, "ERROR !! CANNOT CREATE SOCKET AT THE MOMENT\n");
        exit(1);
    }
    servAdd.sin_family = AF_INET;
    sscanf(port, "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber); // to add the port from host to server

    // BELOW LOGIC IS USED TO CONVERTS THE IPv4 or IPv6 ADDRESS FROM IT'S HUMAN-READABLE MANNER USING inet_pton
    if (inet_pton(AF_INET, host, &servAdd.sin_addr) < 0)
    {
        fprintf(stderr, " inet_pton() HAS FAILED\n");
        exit(2);
    }

    // BELOW LOGIC CONNECTS TO THE SERVER USING ADDRESS
    if (connect(server, (struct sockaddr *)&servAdd, sizeof(servAdd)) < 0)
    {
        fprintf(stderr, "CONNECTED TO SERVER/MIRROR HAS FAILED, EXISTING...\n");
        exit(3);
    }

    return server;
}

// BELOW IS THE MAIN METHOD AND ALL THE EXECUTION STARTS FROM HERE
    int main(int argc, char *argv[])
    {
        char message[255];

        // VALIDATE IF PARAMETERS ARE PASSED BY CLIENT ARE IN CORRECT FORMAT
        if (argc != 3)
        {
            fprintf(stderr, "INVALID CLIENT COMMAND...\nFORMAT-> %s <IP> <Port>\n", argv[0]);
            exit(0);
        }

        // IT CONNECTS TO THE SERVER BASED ON THE host IP AND port number
        int server;
        server = invokeConnectClientToServer(argv[1], argv[2]);
        long int n1;
        
        if ((n1 = read(server, message, 255)) < 0)
        {
            fprintf(stderr, "RECEIVED ERROR MESSAGE FROM SERVER\n");
            exit(3);
        }
        message[n1] = '\0';

        if (strstr(message, "CONNECTION HAS BEEN TERMINATED/REJECTED BY THE MAIN SERVER") != NULL)
        {
            printf("CONNECTION REJECTED BY SERVER.\nCONNECT TO MIRROR IP AND PORT. EXITING...\n");
            close(server);
            exit(0);
        }

        // BELOW LOGIC VERIFY IF CONNECTION IS SUCCESSFUL OR NOT
        // INCASE OF UNSUCCESSFUL CONNECTION, IT WILL RETRY TO CONNECT THE SERVER
        if (strcmp(message, "success") != 0)
        {
            close(server);
            server = invokeConnectClientToServer(message, argv[2]);
        }

        printf("SUCCESSFULLY CONNECTED TO THE SERVER/MIRROR\n");
        while (1)
        {
            int n;
            isUnzip = 0;
            printf("<<-------------------------------------------------------------->>\n");
            printf("C$ ");
            char roabRawClientCommand[1000];
            fgets(roabRawClientCommand, MAX_COMMAND_LENGTH, stdin);
            char *copy = malloc(strlen(roabRawClientCommand) + 1);
            strcpy(copy, roabRawClientCommand);

            // BELOW LOGIC TAKE ALL THE RAW COMMAND PROVIDED BY CLIENT APART FROM CLIENT COMMAND
            roabRawClientCommand[strcspn(roabRawClientCommand, "\n")] = '\0';

            // VALIDATE INPUT COMMAND PROVIDED BY THE USER/CLIENT
            int type = invokeValidateclientCommand(copy);
            if (type == -1)
            {
                continue;
            }

            // BELOW LOGIC SENDS COMMAND FROM CLIENT TO SERVER
            write(server, roabRawClientCommand, strlen(roabRawClientCommand) + 1);

            if (!strcasecmp(roabRawClientCommand, "Bye\n"))
            {
                kill(getppid(), SIGTERM);
                exit(0);
            }

            printf("REQUEST HAS BEEN SENT TO THE SERVER/MIRROR, NOW WAITING FOR RESPONSE........\n");

            // BELOW LOGIC READS THE RESPONSE FROM SERVER
            memset(message, 0, sizeof(message));  //to flush the message memory
            sleep(5);
            if ((n = read(server, message, 255)) < 0)
            {
                fprintf(stderr, "INVALID REQUEST RECEIVED FROM SERVER/MIRROR...\n");
                exit(3);
            }

            message[n] = '\0';
            printf("MESSAGE RECEIVED FROM SERVER/MIRROR--> %s\n", message);

            if (strcasecmp(message, "TAR FILE SENT") == 0)
            {
                invokeUnzipFileFromServer(isUnzip); // receive files from server
            }

            if (strcmp(message, "Exit") == 0)
            {
                fprintf(stderr, "CLIENT TERMINATED!!\n");
                close(server);
                exit(0);
            }   
        }
    }