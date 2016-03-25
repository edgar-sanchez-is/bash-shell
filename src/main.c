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
#include <ctype.h>
#include <stdbool.h>

#define MAX_LENGTH 512

// Global variables
char* PATH = "/bin/bash";			// Default PATH directory
char historyList[MAX_LENGTH][MAX_LENGTH] = {0}; // Max number of commands the user can enter
int historyIterator = 0;			// Number of commands the user has enetered



// Function prototypes
bool runCommand(char*, bool);
void trimSpaces(char*);
void history(char*);
void customPrompt();
void colorSelectionPrompt();
void defaultColor();




int main(int argc, char* argv[]) {
	
	
	bool shellStatus;			// Controls the Shell loop

	// ===============
	// Main Shell Loop
	// ===============
	do {
		if (argc == 2) {
			// ====================
			// Processes batch file
			// ====================
			char* batchDir = argv[1]; 		// Stores shell argument (file directory)
			FILE* batchFile = fopen(batchDir, "r");	// Opens batchDir for reading and stores stream in batchFile
			char batchInput[MAX_LENGTH];		// Stores string within batch file

			// Seeks to end of batchFile and stores file size
			fseek(batchFile, 0, SEEK_END);
			long fileSize = ftell(batchFile);

			// Exits with failure if batchFile could not be opened
			if (batchFile == NULL) {
				fprintf(stderr, "Error: Batch file does not exist or cannot be opened.\n");
				return EXIT_FAILURE;
			}
			else if (fileSize == 0) {
				fprintf(stderr, "Error: Batch file is empty\n");
				return EXIT_FAILURE;
			}
			else if (fileSize > 516) {
				// Exits with failure if batchFile is larger than 512 chars (516 including \n\0)
				fprintf(stderr, "Error: Batch file must be 512 characters or less. %lu\n", fileSize);
				return EXIT_FAILURE;
			}

			// Sets file pointer back to beginning of file
			fseek(batchFile, 0, SEEK_SET);

			// Reads batchFile stream and stores it in batchInput
			fgets(batchInput, MAX_LENGTH, batchFile);

			// Prints first part of batch commands
			printf("Batch commands: ");

			// Parses/executes batchInput and stores returned bool value in shellStatus
			shellStatus = runCommand(batchInput, true);
			
			
			
			
			// Closes batchFile
			fclose(batchFile);
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
			char userInput[MAX_LENGTH]; 			// Stores string input by user

			// Displays prompt within interactive shell
			static int counter = 1;
			
			if(counter++ > 1)
				printf("prompt> ");
			else{
				printf("\e[91mTo customize prompt enter the 'customize' command\n");
				defaultColor();
				printf("prompt> ");
			}

			// Reads user input and handles Ctrl-D
			if (fgets(userInput, MAX_LENGTH, stdin) == NULL) {
				putchar('\n');
				fflush(stdout);
				return EXIT_SUCCESS;
			}

			// Parses and executes userInput
			shellStatus = runCommand(userInput, false);
		}
	} while(shellStatus);
	
	defaultColor();//sets to default color when quit or exit is called in bash file

	return EXIT_SUCCESS;
}

// Parses/executes inputString as userInput if batchMode is false or as batchInput if batchMode is true
bool runCommand(char* strInput, bool batchMode) {
	int totalChildren = 0;							// Counter tlo keep track of total child processes
	char* command = strtok(strInput, ";");			// Stores each command separated by ";"
	pid_t pid; 										// Initializes Process ID
	bool exitStatus = true;							// Controls runCommand()'s return value


	// Processes each command until strtok() returns NULL
	while (command) {
		// Trims leading and trailing spaces around current command
		trimSpaces(command);
		
		// Adds current command to history
		history(command);

		if (strcmp(command, "") == 0) {
			// Strips empty commands
			command = strtok(NULL, ";");
			continue;
		}
		else if (strncmp(command, "PATH=", 5) == 0 && batchMode == false) {
			// Allows change of PATH variable
			PATH = (command + 5);
			printf("NEW PATH: %s\n", PATH);
			break;
		}
		else if (batchMode == true) {
			// Prints current commands if batchMode is true
			printf("[ %s ] ", command);
			fflush(stdout);					// Forces printing to screen
		}

		if ( (strcmp(command, "quit") == 0) || (strcmp(command, "exit") == 0)) {
			exitStatus = false;
			
			break;
		}
		else if(strstr(command, "cd") != NULL) { 
			// Searches for 'cd' on the commands
			chdir(command+3);
			break;
		}
		else if(strstr(command, "customize") != NULL){
			customPrompt(); // Enters the prompt customization interface
			break;
		}
		else if ((pid = fork()) == 0) {		// Creates child by calling fork()
			// ================
			// Child Process
			// ================

			// Executes command and terminates child if successful or command not found
			execl(PATH, "Shell", "-c", command, NULL);
			fprintf(stderr, "Error: %s: Failed to execute\n", command);
			_exit(EXIT_FAILURE); 			// Executes and terminates child if execl() fails
		}
		else if (pid < 0) {
			// ================
			// Forking failed
			// ================

			fprintf(stderr, "Error: Failed to fork child process\n");
			_exit(EXIT_FAILURE); 			// Terminates child
		}
		else {								// Can only be run by parent because pid > 0
			// ================
			// Parent Process
			// ================
			
			// Continues searching for commands starting from last ";"
			command = strtok(NULL, ";");		
			totalChildren++;				// Increases total number of child processes
		}
	}

	// Prints newline after current batch commands and sets exitStatus to false if batchMode is true
	if (batchMode == true) {
		printf("\n");
		exitStatus = false;
	}

	// Waits for each child to terminate
	for(int i=0; i < totalChildren; ++i) {
		wait(NULL); 						// Proceeds if a single child is terminated
	}

	return exitStatus;						// Returns false if shell should exit successfully
}

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
// Adds commands to history or prints it out
void history(char* command) {
	
	// Copies command into the array of strings
	strcpy(historyList[historyIterator], command);
	//add one to itterator
	historyIterator++;
	
	// User has typed in history so we print the history list
	if(strcmp(command, "history")== 0) {
		for(int i = 0; i <= historyIterator-1; i++) {
			printf("Command[%i] = %s\n", i, historyList[i]);
		}	
	}
	
}

void colorSelectionPrompt(char foregroundOrHighlight[],int *menuValue){//pass the string "foreGround" or "Background"
	
	char catchNewLineFromBuffer;//catches new line from the scanf buffer
	
	
	while(1){
		
		*menuValue = 0;
		
		printf("Enter the corresponding number that you want the %s to be for the font.\n", foregroundOrHighlight);
	
		printf("1-Black\n2-Red\n3-Green\n4-Yellow\n5-Blue\n6-Magenta\n7-Cyan\n8-White\n");
	
		
		
		
		scanf("%i", menuValue);//catches the first character as an integer
		scanf("%c", &catchNewLineFromBuffer);//catches the \n characters so it no longer exists in the buffer
			
			
			
		if(*menuValue > 0 && *menuValue < 9){//if the value is correct break from the infinite loop
			
			break;
		}
		else{
			
			printf("You did not type the right value! Try again.\n");//no appropriate value selected keeps on looping until the user types the correct value
		}
			
		
	}
	
}

void defaultColor(){
	printf("\e[%dm", 39);//default foreground color
	printf("\e[%dm", 49);//default background color
}

void customPrompt(){
	
	int foregroundAsciiValue[8] = {30,31,32,33,34,35,36,37};//Black,Red,Green,Yellow,Blue,Magenta,Cyan,White
	int highlightAsciiValue[8] = {40,41,42,43,44,45,46,47};//Same colors as above
	int indexForeground;//the default color for foreground is white
	int indexHighlight;//default background or highlight of black
	char catchNewLineFromBuffer;//catches new line from the scanf buffer
	 
	int menuValue;
	bool loopAgain = true;
	
	while(loopAgain){
		char foreOrBack[10] = "foreground";//value either foreGround or background
		indexForeground = 7;
		indexHighlight = 0;
		
		colorSelectionPrompt(foreOrBack,&menuValue);//calls the prompt or menu selection for selecting colors
	
		indexForeground = menuValue - 1;//foregroundAsciiValue[indexForeground] selects the requested color
	
		memset(foreOrBack,0,strlen(foreOrBack));//resets the string to null
	
		strcpy(foreOrBack,"highlight");//fills the c string with highlight
	
		colorSelectionPrompt(foreOrBack, &menuValue);//calls the prompt or menu selection for selecting colors
	
		indexHighlight = menuValue - 1;//highlightAsciiValue[indexHighlight] selects the requested color
		
		printf("\e[%dm", foregroundAsciiValue[indexForeground]);//acii escape sequence for foreground
		printf("\e[%dm", highlightAsciiValue[indexHighlight]);//ascii escape sequence for background
	
		printf("Example Text!\n");//example color pallete the user selected
	
		defaultColor();
		
		while(1){
			
			
			printf("\nWould you like to keep these settings\n1-Yes\n2-No\n");//prompts user
			
			scanf("%i",&menuValue);//reads menu selection
			scanf("%c", &catchNewLineFromBuffer); // catches new line from scanf buffer
			
	
			if(menuValue == 1){//infinitely loops if the user types an incorrect menu value
				printf("\e[%dm", foregroundAsciiValue[indexForeground]);//sets the color the user selected for foreground
				printf("\e[%dm", highlightAsciiValue[indexHighlight]);//sets the color the user selected for background or highlight
				loopAgain = false;//the outer for loop no longer needs to run
				break;	//breaks from infinite loop
			}
			else if(menuValue == 2){//if the user cant see or hates the colors selected this is the opportunity to change it
				loopAgain = true;//outer while loop runs again
				break;//breaks from infinite loop
			}
			else{
				printf("Incorrect menu value try again.");
			}
			
			//if the wrong value is typed inner while loop infinitely loops until the user selects an appropriate selection
		}	
    }//end of while(loopAgain){}
  
}//end of void customPrompt(){}