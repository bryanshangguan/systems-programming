name: Bryan Shangguan
netID: bys8

name: Christopher Chan
netID: cwc85

Test Plan: 
When testing the interactive mode, we tested the commands "cd", "cd ..", "echo", "ls", "pwd", "which" and "exit". The testing for the interactive mode worked as intended. 
When testing the batch mode, we ran two test files, testfile.sh and testfile2.sh. testfile.sh only used the echo command and displayed the word hello. testfile2.sh ran the 
commands cd .., cd, pwd, ls and exit. The commands in the testfiles worked as intended while in batch mode. 

Program Description: 
We developed a custom shell program, mysh, that operates in two modes: interactive and batch. When provided with a single argument, the shell runs in batch mode, reading 
commands from the specified file and executing them without any printed output. If no arguments are given, the shell operates in interactive mode, reading commands from 
standard input and providing user feedback, including a welcome message ("Welcome to my shell") and an exit message ("Exiting my shell"). In interactive mode, users input 
commands directly into the shell.

The shell supports key functionalities such as command execution, input/output redirection, and pipelining between processes. Input redirection is handled using the < 
operator, and output redirection is handled using the > operator. For pipelines, mysh enables chaining two commands with |, allowing the output of one process to serve as 
the input for another.

The built-in commands include:

cd: Changes the working directory using chdir(), with error handling for invalid paths.
pwd: Prints the current working directory using getcwd().
which: Locates the executable path for a given command, ensuring it is not a built-in.
exit: Terminates the shell, printing "Exiting my shell" in interactive mode.
We ensured that all commands execute normally, providing appropriate error messages for invalid inputs or operations. This shell is capable of executing external commands 
such as ls, cat, and echo, treating them as processes invoked through child process management. Overall, the shell is designed to handle standard operations with robust 
error checking and user feedback.

