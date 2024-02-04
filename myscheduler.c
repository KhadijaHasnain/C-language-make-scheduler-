#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants for maximum sizes and time measurements
#define MAX_DEVICES                 4
#define MAX_DEVICE_NAME             20
#define MAX_COMMANDS                10
#define MAX_COMMAND_NAME            20

#define DEFAULT_TIME_QUANTUM        120
#define TIME_CONTEXT_SWITCH         5
#define TIME_CORE_STATE_TRANSITIONS 10
#define TIME_ACQUIRE_BUS            20
#define MAX_LINE_LENGTH             1000

// Structure to represent a command
typedef struct 
{
    char name[MAX_COMMAND_NAME];
    int duration;
} 
Command;

// Function to read system configuration
void read_sysconfig(char argv0[], char filename[], int *numDevices, int *timeQuantum) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        fprintf(stderr, "Error opening sysconfig file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    char word_to_count[] = "device";
    char word[100];
    int count = 0;
    // Read and count word
    while (fscanf(file , "%s" , word) != EOF)
    {
        if (strcmp(word , word_to_count)==0)
        {
            count++;
        }
    }
    printf("Found %d Devices \n", count);

    char buff[1024];
    char lastNonEmptyLine[1024];
    memset(lastNonEmptyLine, 0x00, sizeof(lastNonEmptyLine));

    fseek(file, 0, SEEK_SET); // make sure to start from 0

    while (fgets(buff, sizeof(buff), file) != NULL)
    {
        if (strlen(buff) > 1) // Check if the line is not empty
        {
            strcpy(lastNonEmptyLine, buff); // Copy the non-empty line to lastNonEmptyLine
        }
    }

    if (strlen(lastNonEmptyLine) > 0)
    {
        printf("%s\n", lastNonEmptyLine);
    }
    else
    {
        printf("No non-empty lines found.\n");
    }
    fclose(file);
}

// Function to read command file
void read_commands(char argv0[], char filename[], Command commands[], int *numCommands) 
{
    FILE *file = fopen(filename, "r");
    if (file == NULL) 
    {
        fprintf(stderr, "Error opening commands file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    int commandCount = 0 ;
    int commandFound [6] = {0} ; // store 6 unique commands

    const char *validCommands[]={"read" , "write" , "wait" , "sleep" , "exit" , "spawn"};
    *numCommands = sizeof(validCommands)/sizeof(validCommands[0]);

    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) 
    {
        // Tokenize the line to extract words (commands)
        char *token = strtok(line, " \t\n");
        while (token != NULL) 
        {
            for (int i = 0; i < *numCommands; i++)
            {
                if (strcmp(token, validCommands[i]) == 0 && !commandFound[i]) 
                {
                    commandFound[i] = 1;
                    commandCount++;
                    break; // Stop checking once a command is found
                }
            }
            token = strtok(NULL, " \t\n");
        }
    }

    // Close the file
    fclose(file);

    // Print the total count of specific commands
    printf("Found %d Commands\n", commandCount);
}   

// Function to execute commands
void execute_commands(Command commands[], int numCommands, int timeQuantum) 
{
    int totalTime = 0 ;
    int cpuUtilization = 0 ;

    for (int i = 0; i < numCommands; i++) 
    {
        int executionTime = commands[i].duration;

        // Simulate context switch and core state transitions
        totalTime += TIME_CONTEXT_SWITCH + TIME_CORE_STATE_TRANSITIONS;

        // Calculate CPU utilization
        cpuUtilization += executionTime;
    }

    // Add time for acquiring bus
    totalTime += TIME_ACQUIRE_BUS;

    // Calculate the total time taken for all commands
    int totalCommandTime = totalTime - TIME_ACQUIRE_BUS;

    // Print the measurements
    printf("Measurements %d %d\n", (cpuUtilization * 100) / totalCommandTime ,totalCommandTime );
}

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
        printf("Usage: %s sysconfig-file command-file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int numDevices, timeQuantum, numCommands;
    Command commands[MAX_COMMANDS];

    read_sysconfig(argv[0], argv[1], &numDevices, &timeQuantum);
    read_commands(argv[0], argv[2], commands, &numCommands);

    // Extra line
    printf("What a fun project! \n") ;

    // Execute commands
    execute_commands(commands, numCommands, timeQuantum);

    exit(EXIT_SUCCESS);
}
