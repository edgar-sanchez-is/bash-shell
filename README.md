The Shell and System Calls
===============================

Major Assignment 1

##### Created By

Alex, Edgar, Luis, and Russell

##### Design Overview

A simple shell that can be run in interactive or batch mode.

##### Complete Specification

To compile the program, execute:

    $ make

To run the program, execute:

    $ ./shell [batchFile]

- **batchFile**: an optional argument (indicated by square brackets as above). If present, the shell will read each line of the batchFile for commands to be executed. If not present, the shell will run in interactive mode by printing a prompt to the user at stdout and reading the command stdin.

For example, if you run the program as:

    $ ./shell /home/mat0299/csce3600/batchFile

then the shell will read commands from `/home/mat0299/csce3600/batchFile` until it sees the `quit` command, it reaches the end of file, or you tyep '`Ctrl-D`'/'`Ctrl-C`'.

Alternatively, if you run the program as:

    $ ./shell

then the shell will display an interactive prompt:

    prompt>

This interactive shell will accept commands and display output until you quit the program.

##### Directory Structure

Directory   |   Description
-------     |   --------
/docs  	      |   Program documentation.
/src 	      |   Source(.c), header(.h), and make files.
/src/test     |   Testing/debugging files.
README.md         |   Basic documentation and information.


##### Known Bugs or Problems

[View current issues.](https://github.com/unt-sysprog-team/major-assignment-1/issues)
