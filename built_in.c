#include "header.h"



// -------------------------------- CHANGE DIRECTORY --------------------------------------
/*
* change_cd() changes the current working directory indicated by the user. Takes in the 
* comamnd struct, cmd_line, and changes either to the HOME directory or the directory 
* specified by the user
*/
void change_cd(struct command* cmd_line) {

	// if the first command given was "cd" and is the only argument given,
	// use getenv to get the HOME environment variable and change to that directory
	if (strcmp(cmd_line->argv[0], CD_HOME) == 0 && cmd_line->argc == 1) {
		char* home_path = getenv("HOME");
		int changed = chdir(home_path);
		if (changed == -1) {
			printf("Unable to change directory to HOME");
			fflush(stdout);
		}

	}

	// else, the second argument given at the command line is the desired directory
	// directly call chdir() to change to the desired directory
	else if (strcmp(cmd_line->argv[0], CD_HOME) == 0 && cmd_line->argc == 2) {

		int changed = chdir(cmd_line->argv[1]);
		if (changed == -1) {
			printf("Unable to change directory to %s", cmd_line->argv[1]);
			fflush(stdout);
		}

	}
}



// --------------------------- SHOW STATUS ------------------------------------

/*
* show_status() calls WIEXITED to see if the child was terminated normally.
* Prints the exit value (which should be 0) if child termination was normal.
* If the child was terminated abnormally (checked by WIFSIGNALED), then
* the function prints the signal number that caused the child's termination
* 
* REFERENCE CITED: This function is based on the last repl.it in the Monitoring
* Child Processes Module
*/
void show_status(int childStatus) {

	/* If WIFEXITED is true then the child exited normally */
	if (WIFEXITED(childStatus)) {
		printf("exit value %d\n", WEXITSTATUS(childStatus));
		fflush(stdout);
	}
	/* Otherwise if WIFSIGNALED is true then the child exited abnormally */
	else if (WIFSIGNALED(childStatus)) {
		printf("terminated by signal %d\n", WTERMSIG(childStatus));
		fflush(stdout);
	}
}
