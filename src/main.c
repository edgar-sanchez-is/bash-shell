// ========================================================================
// Name:		Alex McCulloch, Edgar Sanchez, Luis Chaparro, Russell Price
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
// ========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_LENGTH 512

// Global variables
char prompt[32] = "prompt>";                                // Default prompt
char PATH[32] = "/bin/bash";                                // Default PATH directory
char historyList[MAX_LENGTH][MAX_LENGTH];                   // Max number of commands the user can enter
char oldComm[MAX_LENGTH][20] = {{0}};                       // Number of old commands [NUMBER_OF_WORDS][MAX_SIZE_OF_WORD]
char newComm[MAX_LENGTH][20] = {{0}};                       // Number of renamed commands [NUMBER_OF_WORDS][MAX_SIZE_OF_WORD]
int historyIterator = 1;                                    // Number of commands the user has entered
int indexComm = 0;

// Function prototypes
bool runCommand(char*, bool);
void trimSpaces(char*);
void history(char*);
void customPrompt();
void colorPrompt();
void colorSelectionPrompt(char[], int*);
void defaultColor();
void changeCommand();
char* altNameComm(char*);

int main(int argc, char* argv[]) {
	bool shellStatus = true;                                // Controls the Shell loop

	do {
		// ===============
		// Main Shell Loop
		// ===============
		if (argc == 2) {
			// ====================
			// Processes batch file
			// ====================

			// Opens file for reading and stores stream in batchFile
			FILE* batchFile = fopen(argv[1], "r");
			if (batchFile == NULL) {
				fprintf(stderr, "Error: Batch file does not exist or cannot be opened.\n");
				return EXIT_FAILURE;
			}

			// Checks if batchFile is empty
			fseek(batchFile, 0, SEEK_END);
			if (ftell(batchFile) == 0) {
				fprintf(stderr, "Error: Batch file is empty.\n");
				return EXIT_FAILURE;
			}
			fseek(batchFile, 0, SEEK_SET);

			// Prints batch mode banner and active file
			printf("-----[ BATCH MODE ]-----\n");
			printf("--> File: %s\n", argv[1]);

			char buf[MAX_LENGTH + 1];
			while (fgets(buf, MAX_LENGTH, batchFile)) {
				if (buf[strlen(buf) - 1] != '\n') {
					fprintf(stderr, "Error: Line must be less than 512 characters\n");
					fflush(stdout);
					int ch;
					while ((ch = getc(batchFile))) {
						if (ch == '\n') {
							break;
						}
					}
				}
				else {
					// Execute each line of commands read in from batchFile
					printf("\nBatch line> %s", buf);
					runCommand(buf, true);
				}
			}

			// Closes batchFile
			fclose(batchFile);
			shellStatus = false;
		}
		else if (argc > 2) {
			// ====================
			// Invalid program call
			// ====================
			fprintf(stderr, "Error: Too many arguments.\n");
			return EXIT_FAILURE;
		}
		else {
			// ====================
			// Processes user input
			// ====================
			char userInput[MAX_LENGTH + 2];                 // Stores string input by user
			static bool firstLaunch = true;                 // Displays welcome prompt on shell launch

			if (firstLaunch == true) {
				printf("-----[ INTERACTIVE MODE ]-----\n");
				printf("--> Type \"help\" to view commands\n");
				defaultColor();
				printf("\n%s ", prompt);
				firstLaunch = false;
			}
			else {
				printf("\n%s ", prompt);
			}

			// Reads user input and handles Ctrl-D
			if (fgets(userInput, (MAX_LENGTH + 3), stdin) == NULL) {
				putchar('\n');
				fflush(stdout);
				return EXIT_SUCCESS;
			}
			else if (strlen(userInput) > 512 ) {
				fprintf(stderr, "Error: %u: command must be less than 512 characters long\n", (unsigned)strlen(userInput));
				int ch;
				while ((ch = getc(stdin)) != '\n' || ch != EOF);
				continue;
			}

			// Parses and executes userInput
			shellStatus = runCommand(userInput, false);
		}
	} while(shellStatus);

	return EXIT_SUCCESS;
}

// Parses/executes inputString as userInput if batchMode is false or as batchInput if batchMode is true
bool runCommand(char strInput[], bool batchMode) {
	history(strInput);                                      // Adds current command to history
	int totalChildren = 0;                                  // Counter tlo keep track of total child processes
	char* command = strtok(strInput, ";");                  // Stores each command separated by ";"
	pid_t pid;                                              // Initializes Process ID
	bool exitStatus = true;                                 // Controls runCommand()'s return value

	// Processes each command until strtok() returns NULL
	while (command) {
		// Trims leading and trailing spaces around current command
		trimSpaces(command);

		// Takes in changed command and returns original command while also adding it to history
		command = altNameComm(command);

		if (strcmp(command, "") == 0) {
			// Strips empty commands
			command = strtok(NULL, ";");
			continue;
		}
		else if (strncmp(command, "cd ", 3) == 0 && batchMode == false) {
			// Allows change of current directory
			if (chdir(command + 3) != 0) {
				fprintf(stderr, "Error: cd: Invalid directory\n");
			}
			break;
		}

		if ( (strcmp(command, "quit") == 0) || (strcmp(command, "exit") == 0)) {
			// Exits parent shell
			exitStatus = false;
			command = strtok(NULL, ";");
			continue;
		}
		else if ((strcmp(command, "help") == 0)) {
			printf("/--------[ HELP: LIST OF INTERNAL COMMANDS ]-------\\\n"
			       "| history       - prints list of commands entered  |\n"
			       "| prompt        - sets custom prompt string        |\n"
				   "| customize     - sets customized shell options    |\n"
				   "| path          - sets PATH directory              |\n"
				   "| cd            - change current directory         |\n"
				   "| quit OR exit  - exits shell program              |\n"
				   "\\--------------------------------------------------/\n");
			break;
		}
		else if (strcmp(command, "path") == 0 && batchMode == false) {
			// Sets custom prompt for current shell session
			do {
				printf("Enter custom path: ");
				fgets(PATH, 32, stdin);
				if (PATH[strlen(PATH) - 1] != '\n') {
					fprintf(stderr, "Error: Maximum of 30 characters allowed\n");
					int ch;
					while ((ch = getc(stdin)) != '\n' && ch != EOF);
				}
			} while (PATH[strlen(PATH) - 1] != '\n');
			PATH[strcspn (PATH, "\n")] = '\0';              // Removes trailing '\n' from string
			printf("PATH successfully updated\n");
			break;
		}
		else if (strcmp(command, "prompt") == 0 && batchMode == false) {
			// Sets custom prompt for current shell session
			do {
				printf("Enter custom prompt: ");
				fgets(prompt, 32, stdin);
				if (prompt[strlen(prompt) - 1] != '\n') {
					fprintf(stderr, "Error: Maximum of 30 characters allowed\n");
					int ch;
					while ((ch = getc(stdin)) != '\n' && ch != EOF);
				}
			} while (prompt[strlen(prompt) - 1] != '\n');
			prompt[strcspn (prompt, "\n")] = '\0';          // Removes trailing '\n' from string
			printf("Prompt successfully updated\n");
			break;
		}
		else if(strstr(command, "customize") != NULL) {
			customPrompt(); // Enters the prompt customization interface
			break;
		}
		else if ((pid = fork()) == 0) {                     // Creates child by calling fork()
			// ================
			// Child Process
			// ================

			// Executes command and terminates child if successful or command not found
			execl(PATH, "Shell", "-c", command, NULL);
			fprintf(stderr, "Error: %s: Failed to execute\n", command);
			_exit(EXIT_FAILURE);                            // Executes and terminates child if execl() fails
		}
		else if (pid < 0) {
			// ================
			// Forking failed
			// ================

			fprintf(stderr, "Error: Failed to fork child process\n");
			_exit(EXIT_FAILURE);                            // Terminates child
		}
		else {                                              // Can only be run by parent because pid > 0
			// ================
			// Parent Process
			// ================
			
			// Continues searching for commands starting from last ";"
			command = strtok(NULL, ";");		
			totalChildren++;                                // Increases total number of child processes
		}
	}

	// Waits for each child to terminate
	for(int i = 0; i < totalChildren; ++i) {
		wait(NULL);                                         // Proceeds if a single child is terminated
	}

	return exitStatus;                                      // Returns false if shell should exit successfully
}

// Trims leading and trailing spaces from a given string
void trimSpaces(char* parsedInput) {
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

// Adds commands to history or prints it out
void history(char* cmdString) {
	// Copies command into the array of strings
	strcpy(historyList[historyIterator], cmdString);
	historyIterator++;

	// User has typed in history so we print the history list
	if(strncmp(cmdString, "history", 7) == 0) {
		for(int i = 1; i < (historyIterator - 1); i++) {
			printf("   %i  %s", i, historyList[i]);
		}	
	}
}

// Pass the string "foreGround" or "Background"
void colorSelectionPrompt(char foregroundOrHighlight[],int *menuValue) {
	char catchNewLineFromBuffer;                            // Catches new line from the scanf() buffer

	while(1){
		*menuValue = 0;
		printf("Enter the corresponding number that you want the %s to be for the font.\n", foregroundOrHighlight);
		printf("1-Black\n2-Red\n3-Green\n4-Yellow\n5-Blue\n6-Magenta\n7-Cyan\n8-White\n");

		scanf("%i", menuValue);                             // Catches the first character as an integer
		scanf("%c", &catchNewLineFromBuffer);               // Catches the \n characters so it no longer exists in the buffer

		if(*menuValue > 0 && *menuValue < 9) {              // If the value is correct break from the infinite loop
			break;
		}
		else{
			printf("You did not type the right value! Try again.\n"); // No appropriate value selected keeps on looping until the user types the correct value
		}
	}
}

void defaultColor(){
	printf("\e[%dm", 39);                                                           // Default foreground color
	printf("\e[%dm", 49);                                                           // Default background color
}

void colorPrompt(){
	int foregroundAsciiValue[8] = {30,31,32,33,34,35,36,37};                        // Black,Red,Green,Yellow,Blue,Magenta,Cyan,White
	int highlightAsciiValue[8]  = {40,41,42,43,44,45,46,47};                        // Same colors as above
	int indexForeground;                                                            // The default color for foreground is white
	int indexHighlight;                                                             // Default background or highlight of black
	char catchNewLineFromBuffer;                                                    // Catches new line from the scanf buffer
	 
	int menuValue;
	bool loopAgain = true;
	
	while(loopAgain){
		char foreOrBack[11] = "foreground";                                         // Value for foreground or background
		// indexForeground = 7;
		// indexHighlight = 0;

		// Calls the prompt or menu selection for selecting colors
		colorSelectionPrompt(foreOrBack,&menuValue);
	
		indexForeground = menuValue - 1;                                            // ForegroundAsciiValue[indexForeground] selects the requested color

		// Resets the string to null
		memset(foreOrBack,0,strlen(foreOrBack));

		// Fills the c string with highlight
		strcpy(foreOrBack,"highlight");

		// Calls the prompt or menu selection for selecting colors
		colorSelectionPrompt(foreOrBack, &menuValue);
	
		indexHighlight = menuValue - 1;                                             // HighlightAsciiValue[indexHighlight] selects the requested color
		
		printf("\e[%dm", foregroundAsciiValue[indexForeground]);                    // ASCII escape sequence for foreground
		printf("\e[%dm", highlightAsciiValue[indexHighlight]);                      // ASCII escape sequence for background
	
		printf("Example Text!\n");                                                  // Example color pallete the user selected
	
		defaultColor();
		
		while(1){
			printf("\nWould you like to keep these settings\n1-Yes\n2-No\n");       // Prompts user
			scanf("%i",&menuValue);                                                 // Reads menu selection
			scanf("%c", &catchNewLineFromBuffer);                                   // Catches new line from scanf() buffer

			if(menuValue == 1){
				// Infinite loops if the user types an incorrect menu value
				printf("\e[%dm", foregroundAsciiValue[indexForeground]);            // Sets the color the user selected for foreground
				printf("\e[%dm", highlightAsciiValue[indexHighlight]);              // Sets the color the user selected for background or highlight
				loopAgain = false;                                                  // The outer for loop no longer needs to run
				break;                                                              // Breaks from infinite loop
			}
			else if(menuValue == 2){
				// If the user cant see or hates the colors selected this is the opportunity to change it
				loopAgain = true;                                                   // Puter while loop runs again
				break;                                                              // Breaks from infinite loop
			}
			else{
				printf("Incorrect menu value try again.");
			}
		}
    }
}

void customPrompt(){
	printf("Would you like to customize the prompt's colors (0) or change a commands name (1)?\ncustomize> ");
	char userResponse[2];                                                           // Stores user response
	scanf ("%[^\n]%*c", userResponse);

	// If response = 0 stay in color customization
	if(*userResponse == '0'){
		printf("Entering prompt customization interface...\n");
		sleep(2);
		colorPrompt();                                                              // Enters color customization prompt
	}
	// If response = 1 go to changeCommand()
	else if(*userResponse == '1'){
		printf("Entering command change interface...\n");
		sleep(2);
		changeCommand();                                                            // Enters command change prompt
	}
	else {
		printf("Invalid command, returning to prompt...\n");                        // If invalid command return to prompt
	}
}
void changeCommand(){
	bool status = true;
	while(status){
		indexComm++;
		char chngInput[100];
		
		printf("\e[1;1H\e[2J");                                                     // Clear screen
		printf("\e[91mEnter a command, without flags, that you would like to change followed by its new name.\n");
		printf("Ex. 'ls [newName]'\n");
		defaultColor();
		printf("customize> ");
		
		if(fgets(chngInput,sizeof(chngInput),stdin) != NULL){							// Read input from user
			int wordCount = 0;
			for (char *p = strtok(chngInput," "); p != NULL; p = strtok(NULL, " ")){	// Store commands in array
				if(++wordCount == 1) 
					strcpy(oldComm[indexComm], p);										// Old Command 
				else if(wordCount == 2)
					strncpy(newComm[indexComm], p, strlen(p)-1);						// New command
				else{
					printf("Too many commands, command not added..\n");
					break;																// Too many words
				}
			}
		}
		fflush(stdout);
		
		// Stores User Response 
		char userResponse[2];
		printf("Would you like to change another command (1) or exit (0)?\ncustomize> ");
		scanf ("%[^\n]%*c", userResponse);

		// If response = 0 return to prompt interface
		if(*userResponse == '0') status = false;
		// If response = 1 change another command
		else if(*userResponse == '1') status = true;
		else {			
			printf("Invalid response, exiting customization interface...\n");
			status = false;
			sleep(2);
		}		
	}
	printf("Returning to prompt...\n");
	sleep(2);	
	printf("\e[1;1H\e[2J"); // Clear screen
}
char* altNameComm(char* command){
	int i;
	// Checks if the command entered has a different name, if so it switches it with its original name
	for(i = 1; i <= indexComm; i++){
		if (strcmp(newComm[i], command) == 0)
			return oldComm[i];
	}
	return command;
}