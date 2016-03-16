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

#define MAX_LENGTH 512

// Function prototypes
void parseCommand(char *);


int main(int argc, char* argv[]) {

	// ===============
	// Main Shell Loop
	// ===============
	do {
		if (argc == 2) {
			char* batchDirectory = argv[1]; // Stores shell argument (file directory)
			FILE* batchFile = fopen(batchDirectory, "r"); // Opens file for reading and stores in batchFile
			char batchInput[MAX_LENGTH]; // Stores string within batch file

			// ====================
			// Processes batch file
			// ====================
			// Exits with failure if batchFile could not be opened
			if (batchFile == NULL) {
				fprintf(stderr, "Batch file does not exist or cannot be opened.\n");
				return EXIT_FAILURE;
			}

			fgets (batchInput, MAX_LENGTH, batchFile);
			// Reads batchFile stream and stores it in batchInput

			parseCommand(batchInput);
			// Prints first part of batch commands

			// Parses/executes batchInput and stores returned bool value in shellStatus

			// Closes batchFile
			fclose(batchFile);
			return EXIT_SUCCESS;
		}
		else if (argc > 2) {
			// ====================
			// Invalid program call
			// ====================
			fprintf(stderr, "Too many arguments.\n");
			return EXIT_FAILURE;
		}
		else {
			char userInput[MAX_LENGTH]; // Stores string input by user
			// ====================
			// Processes user input
			// ====================

			// Displays prompt within interactive shell
			printf("prompt> ");

			// Reads string input by user and stores it in userInput
			fgets(userInput, MAX_LENGTH, stdin);

			// Parses and executes userInput
			parseCommand(userInput);
		}
	} while (1);
}

void parseCommand(char *inputString) {
	int totalChildren = 0; // Counter to keep track of total child processes
	char* parsedInput = strtok(inputString, ";"); // Stores each command in parsedInput, separated by ";"
	pid_t pid; // Process ID initialization

	while (parsedInput) {
		if ((pid = fork()) == 0) {

			execl("/bin/bash", "Error", "-c", parsedInput, NULL);
			_exit(EXIT_FAILURE); // Only executes if execl fails
// Parses/executes inputString as userInput if batchMode is false or as batchInput if batchMode is true

	// Processes each command until strtok() returns NULL
		// Trims leading and trailing spaces around current command
		trimSpaces(command);
		// Prints current commands if batchMode is true
			// ================
			// Child Process
			// ================

			// Executes command and terminates child if successful or command not found
		}
		else if (pid < 0) {
			fprintf(stderr, "Fork failed.\n");
			// ================
			// Forking failed
			// ================

		}
		else {
			totalChildren++;
			// ================
			// Parent Process
			// ================

			// Continues searching for commands starting from last ";"
			parsedInput = strtok(NULL, ";");
		}
	}

	// Prints newline after current batch commands and sets exitStatus to false if batchMode is true
	// Waits for each child to terminate
	for(int i=0; i < totalChildren; ++i){
		wait(NULL); // Proceeds if a single child is terminated
// Trims leading and trailing spaces from a given string
void trimSpaces(char* parsedInput)
{
	// Trims trailing spaces
	char *s = parsedInput + strlen(parsedInput);
	while (--s >= parsedInput) {
		if (!isspace(*s)) {
			break;
		}
		*s = 0;
	}

	// Trims leading spaces
	size_t n = 0;
	while (parsedInput[n] != '\0' && isspace((unsigned char)parsedInput[n])) {
		n++;
	}
	memmove(parsedInput, parsedInput + n, strlen(parsedInput) - n + 1);
}