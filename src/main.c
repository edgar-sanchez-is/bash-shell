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


int main () {
	
	
	/*
	test code for system to find out if it returns a number, will be using which
	if system() fucntions probably it will return a 0 resulting in the if statement 
	not being true
	-russell
	*/
	char* call = " at ! $$$ text.txt";
	if(system(call) != 0)
	printf("\n\n\n%i\n\n error", returned);

	return 0;
}