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

#define TEST "/Users/Edgar/Documents/GitHub/major-assignment-1/src/test/batch"

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
		//printf("Batch command: %s\n", batchFile);

		fscanf(batchFile, );

		fclose(batchFile);
	}
	else if (argc > 2) {
		fprintf(stderr, "Too many arguments.\n");
		return EXIT_FAILURE;
	}

	return 0;
}