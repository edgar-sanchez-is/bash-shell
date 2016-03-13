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

//#define TEST "/Users/Edgar/Documents/GitHub/major-assignment-1/src/test/batch"
#define TEST "/cygdrive/c/Edgar/Documents/GitHub/major-assignment-1/src/test/batch"
#define MAXSIZE 512

void runCommand(char*);

int main (int argc, char* argv[]) {

	if (argc == 2) {
		//char* batchInput = argv[1];
		char* batchInput = TEST;
		FILE* batchFile = fopen(batchInput, "r");
		if (batchFile == NULL)
		{
			fprintf(stderr, "Could not open batch file.\n");
			return EXIT_FAILURE;
		}

		char promptCommand[MAXSIZE];
		fgets(promptCommand, MAXSIZE, batchFile);

		printf("COMMAND: %s", promptCommand);
		fclose(batchFile);
		return EXIT_SUCCESS;
	}
	else if (argc > 2) {
		fprintf(stderr, "Too many arguments.\n");
		return EXIT_FAILURE;
	}
	else {
		char userInput[MAXSIZE];
		printf("prompt> ");
		fgets(userInput, MAXSIZE, stdin);
		runCommand(userInput);
		return EXIT_SUCCESS;
	}
}

void runCommand(char* userInput) {
	printf("Here is your string again: %s", userInput);
}