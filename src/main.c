// =====================================================================
// Name:		Alex, Edgar, Luis, Russell
// Course:		CSCE 3600
// Date:		March 30, 2016
// Title:		Major Assignment 1
// Version:		1.0
// Description:		Implement a command line interpreter or shell.
// 			When a command is entered, the shell creates a child
//			process that executes the command and then prompts for
//			more input. Multiple commands can be separated by ';'
// 			or by using a batch file.
// Format:		shell [batchFile]
// =====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Linux Test Dir	"/Users/Edgar/Documents/GitHub/major-assignment-1/src/test/batch"
// Windows Test Dir	"/cygdrive/c/Edgar/Documents/GitHub/major-assignment-1/src/test/batch"
#define MAX_LENGTH 512

// Function prototypes
void parseCommand(char *);

int main(int argc, char* argv[]) {
	do {
		// Processes batch file
		if (argc == 2) {
			char* batchDirectory = argv[1]; // Stores shell argument (file directory)
			FILE* batchFile = fopen(batchDirectory, "r"); // Opens file for reading and stores in batchFile
			char batchInput[MAX_LENGTH]; // Stores string within batch file

			if (batchFile == NULL) {
				fprintf(stderr, "Could not open batch file.\n");
				return EXIT_FAILURE;
			}

			// Reads text within batchFile and stores it in batchInput
			fgets (batchInput, MAX_LENGTH, batchFile);

			// Parses and executes batchInput
			parseCommand(batchInput);

			// Closes batchFile
			fclose(batchFile);
			return EXIT_SUCCESS;
		}
		else if (argc > 2) {
			fprintf(stderr, "Too many arguments.\n");
			return EXIT_FAILURE;
		}
		// Processes interactive input
		else {
			char userInput[MAX_LENGTH]; // Stores string input by user

			// Displays prompt within interactive shell
			printf("prompt> ");

			// Reads string input by user and stores it in userInput
			fgets(userInput, MAX_LENGTH, stdin);

			// Parses and executes userInput
			parseCommand(userInput);
		}
	} while (1);
}

// Parses the string of commands provided by the user or within batch file
void parseCommand(char *inputString) {
	int totalChildren = 0; // Counter to keep track of total child processes
	char* parsedInput = strtok(inputString, ";"); // Stores each command in parsedInput, separated by ";"
	pid_t pid; // Process ID initialization

	// Loops through each single command within the string of commands, parsedInput returns NULL if empty
	while (parsedInput) {
		// Creates a child process by calling fork().
		// The child process is a mirror of the parent process.
		// In the child process, pid == 0 and it executes the if statement.
		// In the parent process, pid is > 0 and it executes the else statement.
		// If pid is < 0, then there was an error creating the fork.
		if ((pid = fork()) == 0) {
			// printf("COMMAND: %s", parsedInput); // TODO: Troubleshoot printing each command executed

			// Executes parsedInput using bash
			// If successful, child process terminates
			execl("/bin/bash", "/bin/bash", "-c", parsedInput, NULL);
			_exit(EXIT_FAILURE); // Only executes if execl fails
		}
		else if (pid < 0) {
			fprintf(stderr, "Fork failed.\n");
		}
		else {
			totalChildren++;
			// Continues searching for commands starting from last ";"
			parsedInput = strtok(NULL, ";");
		}
	}

	// Waits for each child to terminate
	for(int i=0; i < totalChildren; ++i){
		wait(NULL); // Proceeds if a single child is terminated
	}
	printf("ALL CHILDREN ARE DEAD!\n");
}