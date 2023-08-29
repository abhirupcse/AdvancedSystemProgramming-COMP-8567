//@Author: Abhirup Ranjan
// Section: 3
// COMP8567

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//GLOBAL VARIBALE DECLARATION

#define MAX_PIPES 8 // USED IN checkArgumentCount METHOD
#define MAX_ARGS 5  // USED UNDER checkArgument METHOD
#define OUTPUT_APPEND 2 // USED UNDER runStandaloneArgument METHOD
#define OUTPUT_TRUNC 1  // USED UNDER runStandaloneArgument METHOD

// DETERMINE IF A CHARACTER AT A SPECIFIC INDEX IN A GIVEN STRING IS ESCAPED
//----------------------------------------------------------------
int checkEscaped(const char* abhiArgument, int a) {

    if (a > 0 && abhiArgument[a-1] == '\\') {
        
        int count = 0;
        while (a > 0 && abhiArgument[a-1] == '\\') {
            count++;
            a--;
        }
        return count % 2 != 0;
    }
    return 0;
}

// FUNCTION USED TO TRIM LEADING AND TRAILING WHITESPACE CHARACTERS FROM STRING
//----------------------------------------------------------------
char* getTrimFun(char *abhiArgument) {

    int i;
    while (isspace (*abhiArgument)) {
       
        abhiArgument++;  
    }
    for (i = strlen (abhiArgument) - 1; (isspace (abhiArgument[i])); i--) ;   
    abhiArgument[i + 1] = '\0';
    
    return abhiArgument;
}

// INSERT SPACES AROUND SPECIAL CHARACTERS IN A COMMAND STRING FORMAT
// READS IT CHARACTER BY CHARACTER, FORMATTING IT BY ENCLOSING SPECIAL CHARACTERS WITH SPACES
// THE FORMATTED COMMAND IS THEN GIVEN BACK AS A FRESH STRING THAT WAS ALLOCATED DYNAMICALLY
//----------------------------------------------------------------
char* argFormat(const char* AbhiInputVar) {
    const char* abhiSpecialChar = "<>|&;";
    const char* repSpecialChar = ">|&";
    size_t l1 = strlen(AbhiInputVar);
    char* formattedArg = malloc(l1*2 + 1); 
    size_t i = 0, j = 0;

    // RUNNING LOOP OVER PASSED COMMAND
    while (i < l1) {
        if (strchr(abhiSpecialChar, AbhiInputVar[i]) != NULL && !checkEscaped(AbhiInputVar, i)) {
            
            if (strchr(repSpecialChar, AbhiInputVar[i]) != NULL &&
                i + 1 < l1 &&
                AbhiInputVar[i] == AbhiInputVar[i+1]) {
                formattedArg[j++] = ' ';
                formattedArg[j++] = AbhiInputVar[i++];
                formattedArg[j++] = AbhiInputVar[i];
                formattedArg[j++] = ' ';
                formattedArg[j++] = ' ';
            } else {
                formattedArg[j++] = ' ';
                formattedArg[j++] = AbhiInputVar[i];
                formattedArg[j++] = ' ';
            }
        } else {
            
            formattedArg[j++] = AbhiInputVar[i];
        }
        i++;
    }

    // APPENDING THE NULL CHARACTER IN THE END TO MARK END OF ARRAY
    formattedArg[j] = '\0';
    return formattedArg;
}

// FUNCTION CHECK IF A COMMAND ENTERED HAS VALID NUMBER OF ARGUMENTS
// IT RETURNS 1 IF NUMBER OF ARGUMENTS IS WITHIN THE ALLOWED RANGE AND RETURN 0 IF GREATER
//----------------------------------------------------------------
int checkArgument(char* abhiArgument) {

    char* abhiCommand = strdup(abhiArgument);
    abhiCommand = getTrimFun(abhiCommand);
    char* abhiStr = strtok (abhiCommand," ");
    int a = 0;

    // CHECK FOR NULL
    while (abhiStr != NULL) {
        abhiStr = strtok (NULL, " ");
        a++;
    }
    return a <= MAX_ARGS && a > 0;
}

// DIVIDE THE PROVIDED RAW ARGUMENT STRINGS INTO SPERATE TOKENS
// RETURN THE TOTAL NUMBER OF TOKENS THAT WERE LOCATED IN THE STRING(INCLUDE SPECIAL CHAR AS WELL)
//----------------------------------------------------------------
int splitArguments(char *formatInput, char **abhiToken)
{
    int tokenCount = 0;  
    int l1 = strlen(formatInput);
    int i;
    int indexPlace = 0;

    for (i = 0; i < l1; i++)
    {
        // CHECK FOR OCCURENCE FOR && IN ARGUMENT
        if (strncmp(formatInput + i, "&&", 2) == 0)
        {
            abhiToken[tokenCount] = malloc((i - indexPlace + 1) * sizeof(char)); 
            strncpy(abhiToken[tokenCount++], formatInput + indexPlace, i - indexPlace);
            abhiToken[tokenCount++] = "&&";
            i++;
            indexPlace = i + 2;
        }

        // CHECK FOR OCCURENCE FOR || IN ARGUMENT
        else if (strncmp(formatInput + i, "||", 2) == 0)
        {
            abhiToken[tokenCount] = malloc((i - indexPlace + 1) * sizeof(char));
            strncpy(abhiToken[tokenCount++], formatInput + indexPlace, i - indexPlace);
            abhiToken[tokenCount++] = "||";
            i++;
            indexPlace = i + 2;
        }

        // CHECK FOR OCCURENCE FOR | IN ARGUMENT
        else if (formatInput[i] == '|')
        {
            abhiToken[tokenCount] = malloc((i - indexPlace + 1) * sizeof(char));
            strncpy(abhiToken[tokenCount++], formatInput + indexPlace, i - indexPlace);
            abhiToken[tokenCount++] = "|";
            indexPlace = i + 1;
        }

        // CHECK FOR OCCURENCE FOR ; IN ARGUMENT
        else if (formatInput[i] == ';')
        {
            abhiToken[tokenCount] = malloc((i - indexPlace + 1) * sizeof(char));
            strncpy(abhiToken[tokenCount++], formatInput + indexPlace, i - indexPlace);
            abhiToken[tokenCount++] = ";";
            indexPlace = i + 1;
        }
    }
    // LAST TOKEN IS HANDLED SEPARATELY
    abhiToken[tokenCount] = malloc((l1 - indexPlace + 1) * sizeof(char));
    strncpy(abhiToken[tokenCount++], formatInput + indexPlace, l1 - indexPlace);
    return tokenCount;
}

//  RESPONSIBLE FOR EXECUTING A STANDALONE COMMAND WHICH ARE NOT PART OF A PIPE SEQUENCE 
//----------------------------------------------------------------
int runStandaloneArgument(char* argument, int a){
    
    char *space = " "; // SET SPACE CHARACTER
    char *abhiFileOutput = NULL; // OUTPUT STORE FILENAME
    int bg = 0; // CHECK FOR BACKGROUND PROCESS
    char *abhiFileInput = NULL; // INPUT FILENAME
    char *abhiToken;
    char *arr[10];  
    char* abhiCommand = strdup(argument);
    abhiCommand = getTrimFun(abhiCommand); // REMOVE SPACES
    int outputMode = OUTPUT_TRUNC;

    // CHECK IF LAST CHARACTER OF abhiCommand is & HENCE BACKGROUND PROCESS
    if (abhiCommand[strlen(abhiCommand) - 1] == '&') {
        bg = 1;
        abhiCommand[strlen(abhiCommand) - 1] = '\0';
        abhiCommand = getTrimFun(abhiCommand);
    }

    // CHECK IF CHARACTER OF abhiCommand is EXIT
    if (strcmp(abhiCommand, "exit") == 0) {
        exit(0);
    }

    // CHECK IF CHARACTER OF abhiCommand is cd THEN CHANGE TO HOME DIRECTORY
    if (strcmp(abhiCommand, "cd") == 0) {
        chdir(getenv("HOME"));
        return 0;
    } 
    
    // CHANGE WORKING DIR TO GIVEN DIR
    else if (strstr(abhiCommand, "cd ") == abhiCommand) {
        char* dir = abhiCommand + 3; 
        dir = getTrimFun(dir);
        chdir(dir);
        return 0;
    }

    int i = 0;
    char* abhiNextToken;
    abhiToken = strtok_r(abhiCommand, space, &abhiNextToken);
    while (abhiToken != NULL) {

        // CHECK IF > < or >>
        if (strcmp(abhiToken, "<") == 0) 
            abhiFileInput = strtok_r(NULL, space, &abhiNextToken);
        else if (strcmp(abhiToken, ">") == 0) {
            abhiFileOutput = strtok_r(NULL, space, &abhiNextToken);
            outputMode = OUTPUT_TRUNC;
        } 
        else if (strcmp(abhiToken, ">>") == 0) {
            abhiFileOutput = strtok_r(NULL, space, &abhiNextToken);
            outputMode = OUTPUT_APPEND;
        } 
        else 
            arr[i++] = abhiToken;
        abhiToken = strtok_r(NULL, space, &abhiNextToken);
    }
    arr[i] = NULL;
    int forking = 0;
    if (a) {
        forking = fork();
    }

    if (forking < 0) {
        printf("ERROR WHILE FORKING!!\n");
        exit(1);

    } 
    else if (forking == 0) {
        int fd_in, fd_out;
        if (abhiFileInput) {
            fd_in = open(abhiFileInput, O_RDONLY);
            if (fd_in < 0) {
                printf("ERROR WHILE FORKING!!\n");
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        if (abhiFileOutput) {
            if (outputMode == OUTPUT_TRUNC) {
                fd_out = open(abhiFileOutput, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            } 
            else if (outputMode == OUTPUT_APPEND) {
                fd_out = open(abhiFileOutput, O_WRONLY | O_CREAT | O_APPEND, 0666);
            }
            if (fd_out < 0) {
                printf("UNABLE TO OPEN THE FILE!!\n");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        if (execvp(arr[0], arr) < 0) {
            printf("ERROR EHILE EXECUTING EXEC CALL!!\n");
            exit(1);
        }
    } 
    else {
        // PARENT
        if (!bg) {
            // WAIT FOR CHILD TO END
            int status;
            wait(&status);
            return status;
        } 
        else {
            printf("PROCESS IN BACKGROUND WITH PROCESS ID =  %d\n", forking);
            return 0;
        }
    }
    if (a == 0) {
        if (!bg) {
            // WAIT FOR CHILD TO END
            int status;
            wait(&status);
            return status;
        } 
        else {
            printf("PROCESS IN BACKGROUND WITH PROCESS ID = %d\n", forking);
            return 0;
        }
    }
    return 0;
    }

// RESPONSIBLE FOR EXECUTING A SEQUENCE OF COMMANDS CONNECTED BY PIPES
//----------------------------------------------------------------
int getPipeImplementation (char** argument, int numOfArg) {
    int forking = fork();
    int i=0;

    // CHILD PROCESS
    if (!forking) {
        for(  i=0; i<numOfArg-1; i+=2)
        {
            int pd[2];
            pipe(pd);
            if (!fork()) {
                dup2(pd[1], 1); 

                // CALLING STANDALONE METHOD
                runStandaloneArgument(argument[i], 0);
                printf("ERROR!!\n");
            }
            dup2(pd[0], 0);
            close(pd[1]); // CLOSE UNUSED FILE
        }

        // CALLING STANDALONE METHOD
        runStandaloneArgument(argument[i], 0);
    } 

    // PARENT PROCESS
    else if (forking > 0) {
        char* command = strdup(argument[numOfArg - 1]);
        command = getTrimFun(command);
        if (command[strlen(command) - 1] != '&') {
            waitpid(forking, NULL, 0);
        } else {
            printf("JOB/PROCESS RUNNING IN BACKGROUND WITH PID = %d\n", forking);
        }
    }
}

// THIS IS RESPONSIBLE FOR EXECUTING COMMANDS WHICH ARE STORED IN abhiToken ARRAY
// BASED ON CONDITIONS
//----------------------------------------------------------------
//void fetchCommand(char **commands, int len) 
void fetchCommand(char **abhiToken, int tokenCount) {

    int previousValue = 1;
    int i = 0; // WILL BE USED IN ITERATING LOOP
    int currentResult = 0;
    int fd[2];

    // RUNNING LOOP OVER ARGUMENT
    while (i<tokenCount) {
        
        // CHECK IF CURRENT COMMAND IS || IF YES CHECK PREVIOUS VALUE
        if (strcmp(abhiToken[i], "||") == 0) {
            if (previousValue == 0) {
                
                i++;
                while (i<tokenCount && strcmp(abhiToken[i], "&&") != 0 && 
                strcmp(abhiToken[i], "|") != 0 &&
                strcmp(abhiToken[i], ";") != 0) {
                    i++;
                }
                if (i>0 && i<tokenCount && strcmp(abhiToken[i], "|") == 0) i+=2;
            } else {
                i++;
            }
        } 
        
         // CHECK IF CURRENT COMMAND IS && IF YES CHECK PREVIOUS VALUE
        else if (strcmp(abhiToken[i], "&&") == 0) {
            if (previousValue != 0) {
                // checking for && if yes then skip next cmd
                i++;
                while (i<tokenCount && strcmp(abhiToken[i], "||") != 0 && 
                strcmp(abhiToken[i], "|") != 0 &&
                strcmp(abhiToken[i], ";") != 0) {
                    i++;
                }
                if (i>0 && i<tokenCount && strcmp(abhiToken[i], "|") == 0) i+=2;
            } else {
                i++;
            }
        } 
        
         // CHECK IF CURRENT COMMAND IS PIPE OR |
        else if (strcmp(abhiToken[i], "|") == 0) {
            i++;
        } 
        
        // CHECK IF CURRENT COMMAND IS ;
        else if (strcmp(abhiToken[i], ";") == 0) {
            i++;
        }

        // FUNCTION COMBINES ALL THE COMMANDS BEFORE THE PIPE AND EXECUTES THEM USING THE pipeImplementation FUNCTION
        else if (i<tokenCount-1 && strcmp(abhiToken[i+1], "|") == 0) {
            int start = i;
            while(i<tokenCount-1 && strcmp(abhiToken[i+1], "|") == 0) i+=2;
            getPipeImplementation(abhiToken+start, i-start+1);
            i++;
        }

        //  THE CURRENT COMMAND IS STANDALONE
        else {  
            currentResult = runStandaloneArgument(abhiToken[i], 1);
            previousValue = currentResult;
            i++;
        }
    }
}

// THIS METHOD CHECK 2 THINGS
// 1. THIS METHOD CHECK THAT EACH ARGUMENT SHOULD BE BETWEEN 1-5
// 2. THIS METHOD CHECK THAT SPECIAL ARGUMENT SHOULD BE BETWEEN 1-7
// RETURN 0 WHEN NOT VALID & 1 WHEN VALID COMMAND NUMBERS
//----------------------------------------------------------------
int checkArgumentCount(char** abhiToken, int tokenCount) {
    // CONDITION FOR SPECIAL CHAR SHOULD BE <= 7
    if (tokenCount > 2*MAX_PIPES - 1) {
        printf("ONLY UPTO 7 SPECIAL-CHARCTERS ALLOWED(; & | && or ||[or combination of last 2])!!\n\n"); 
        return 0;
    }

    // CHECK ALL COMMANDS ARGUMENT MORE THAN 5
    for (int i=0;i<tokenCount;i++){
        if (i%2) continue;
        int abhiIsValid = checkArgument(abhiToken[i]);
        if (!abhiIsValid) {
            printf("YOU ARE ALLOWED TO ENTER 5 COMMANDS ONLY..\nTRY AGAIN!!\n");
            return 0;
        }
    }
    return 1;
}

//MAIN METHOD OF THE PROGRAM
//----------------------------------------------------------------
int main()
{
    //MAKING AN INFINITE LOOP SO THAT THE SHEEL/WINDOW IS NOT KILLED
    while (1) {
        // CREATING A VARIABLE FOR 4096 BYTES
        char* AbhiInputVar = malloc(4096);
        fflush(stdout);
        printf("mshell$ ");

        // READS THE INPUT AND STORE IN A STRING ARRAY
        fgets(AbhiInputVar, 1024, stdin);

        // FUNCTION REMOVE LEADING AND TRAILING BLANK SPACES
        AbhiInputVar = getTrimFun(AbhiInputVar);

        // THIS CONDITION CHECKS IF LENGTH OF INPUT ARRAY IS 0 IF YES LOOP STARTS AGAIN 
        // AND ASK USER TO ENTER THE VALUE/COMMANDS
        if (strlen(AbhiInputVar) == 0) {
            continue;
        }

        // METHOD TO FORMAT THE ARGUMENT COMMAND BY ADDING SPACES
        // THE FORMATTED COMMAND IS THEN GIVEN BACK AS A FRESH STRING THAT WAS ALLOCATED DYNAMICALLY.
        char* formatInput = argFormat(AbhiInputVar);

        // THIS ARRAY HOLDS VALUES FOR TOKEN i.e. SINGLE WORDS/ARGUMENTS
        // METHOD TO SPLIT EACH FORMATTED INPUT COMMAND & RETURN TOTAL NO. OF FOUND TOKEN
        // TOKEN THIS INCLUDE SPECIAL CHARACTER COUNT AS WELL
        char *abhiToken[150];
        int tokenCount = splitArguments(formatInput, abhiToken);
        
        // THIS METHOD CHECK THAT EACH ARGUMENT SHOULD BE BETWEEN 1-5
        // ALSO CHECK IF NO SPECIAL CHAR > 7
        // RETURN 0 WHEN NOT VALID & 1 WHEN VALID COMMAND NUMBERS
        int abhiIsValid = checkArgumentCount(abhiToken, tokenCount);

        // WILL RUN IF COMMAND IS VALID OR 1
        if (abhiIsValid) 
        fetchCommand(abhiToken, tokenCount);
    }
    return 0;
}
