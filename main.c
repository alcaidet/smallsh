#include "header.h"
#include <signal.h>


// https://wiki.sei.cmu.edu/confluence/display/c/SIG31-C.+Do+not+access+shared+objects+in+signal+handlers
// the volatile sig_atmoic_t flag will be used to keep track of foregorund-only mode
volatile sig_atomic_t fg_flag = 0; 



/*
* handle_SIGTSTP is the custom handler for SIGTSTP. If the
* foreground flag, fg_flag is 0, the flag is changed to 1
* when the user sends a Ctrl-Z signal. This makes smallsh enter
* foreground only mode. If fg_flag is 1 when the user sends Ctrl-Z signal,
* then the flag is changed to 0 and smallsh exits foreground mode.
*/
void handle_SIGTSTP(int signo) {
	char* new_menu = ": ";
	if (fg_flag == 0) {
		char* newline = "\n";
		write(STDOUT_FILENO, newline, 2);
		char* message = "Entering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, message, 49);
		write(STDOUT_FILENO, new_menu, 2); // immediately show the : prompt
		fg_flag = 1;
	}
	else if (fg_flag == 1) {
		char* newline = "\n";
		write(STDOUT_FILENO, newline, 2);
		char* message = "Exiting foreground-only mode\n";
		write(STDOUT_FILENO, message, 29);
		write(STDOUT_FILENO, new_menu, 2); // immediately show the : prompt
		fg_flag = 0;

	}
}



/* ------------------------ MAIN FUNCTION ----------------------------- */
int main(void) {

	// initialize an array of size 100 for holding PIDs of background processes
	int bg_pid_list[MAX_BG] = { 0 };
	int bg_pid_index = 0;
	int menu = 0; // if menu is anything other than 0, exit the menu

	/* ------------------ PARENT SIGNAL HANDLERS --------------------------- */
	/* SIGINT HANDLER - Parent must IGNORE SIGINT  */
	struct sigaction SIGINT_action = {{0}};
	SIGINT_action.sa_handler = SIG_IGN; // set flag to SIG_IGN; 
	sigaction(SIGINT, &SIGINT_action, NULL);

	/* SIGTSTP HANDLER - smallsh must enter/exit foreground mode - REFERENCE CITED: code is modified from the custom signal handler repl.it from the Signal Handline Module*/
	struct sigaction SIGTSTP_action = {{ 0 }};
	SIGTSTP_action.sa_handler = handle_SIGTSTP; // set the sa_handler to custom SIGTSTP handler
	SIGTSTP_action.sa_flags = SA_RESTART; // for use with getline() - change: I switched to fgets() but I'll keep this here
	sigfillset(&SIGTSTP_action.sa_mask); // Block all catchable signals while handle_SIGTSTP is running
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	char** arg_array; // arg_array is the array from the space-delimited user input
	int array_count; // count of all space-delimited tokens in arg_array


	/* ---------------------- SMALLSH LOGIC --------------------------------*/
	do {
		char* cmd_line = (char*)malloc(sizeof(char) * MAX_CHAR);
		int childStatus;
		pid_t childPid;

		// show the command prompt and get the commands from the user
		printf(": ");
		fgets(cmd_line, MAX_CHAR, stdin);

		// parse the command line and get the array of cmd line arguments
		arg_array = parse_cmd_line(cmd_line);
		array_count = arg_count(arg_array);

		//check for blank lines or comments
		int comment = check_comment(arg_array);
		int blank = check_blank(arg_array);

		// this command struct holds all information taken from the user input
		struct command* commands = parse_array(arg_array, array_count);

		/* ------------ CHECK FOR COMMENTS OR BLANK LINES ------------- */
		if (comment == 0 || blank == 0) {
			menu = 0;
		}

		/* -------------- CHECK FOR BUILT-IN COMMANDS ------------------- */
		// -------------- CD COMMAND ---------------------------
		else if (strcmp(commands->argv[0], CD_HOME) == 0) {
			change_cd(commands);

		}

		// ------------- STATUS COMMAND ---------------------------------
		else if (strcmp(commands->argv[0], USER_STATUS) == 0) {
			show_status(childStatus);
		}

		// ------------- EXIT COMMAND ------------------------------------
		else if (strcmp(commands->argv[0], USER_EXIT) == 0) {
			menu = 1;
		}
		
		
		/*
		* -------------- OTHER COMMANDS ----------------------------------
		* In this condition, we fork a child and use exec() functions to run the command
		* REFERENCE CITED: fork examples from Executing a New Program Module
		*/
		else {
			childPid = fork(); // Fork a new process

			switch (childPid) {

			/* ---------- ERROR IN FORKING ------------------------- */
			case -1:
				perror("fork()\n");
				break;

			/* ----------------- CHILD PROCESS ------------------------- */
			case 0:

				/*---------------- RESET THE SIGNAL HANDLERS IN CHILD------------------ */
				/* SIGINT ACTION: TERMINATE ONLY FOREGROUND CHILD BY SETTING SIG_DFL */
				if (commands->background_true == 0) {
					SIGINT_action.sa_handler = SIG_DFL;
					sigaction(SIGINT, &SIGINT_action, NULL);
				}

				/* ALL CHILDREN IGNORE SIGTSTP */
				SIGTSTP_action.sa_handler = SIG_IGN;
				sigaction(SIGTSTP, &SIGTSTP_action, NULL);

				/* ------- CHECK FOR IO REDIRECTION -------------------  */
				if (commands->input_true == 1 || commands->output_true == 1) {
					io_redirection(commands);
				}

				/* --- BACKGROUND PROCESSES MUST BE CHECKED FOR IO REDIRECTION --- */
				if (commands->background_true == 1) { 
					io_redirection(commands);
				}

				execvp(commands->argv[0], commands->argv);
				perror(commands->argv[0]); // exec function will return only on errors
				exit(1);
				break;

			/* -------------------- PARENT PROCESS ------------------------*/
			default:

				// go into foreground-only mode if fg_flag = 1
				if (fg_flag == 1) {
					commands->background_true = 0;
				}

				// if the child is a background process, then we show the pid and DON'T WAIT for termination
				if (commands->background_true == 1 && fg_flag == 0) {
					printf("background pid is %d\n", childPid);
					fflush(stdout);
					bg_pid_list[bg_pid_index] = childPid; // add the pid to bg_pid_list
					bg_pid_index++; // move to the next available index in bg_pid_list
				}

				// if the child is a foreground process, then we WAIT for termination
				else  {
					childPid = waitpid(childPid, &childStatus, 0);
				}
				break;
			}
			
		}

		/* ------------- FREE MEMORY BEFORE SHOWING NEXT COMMAND PROMPT --------------- */
		free_cmd_array(arg_array, array_count);
		free_struct(commands);
		free(cmd_line);
		array_count = 0;
		check_bg_child(childPid, childStatus, bg_pid_list);
		
	} while (menu == 0);
	

	kill_children(bg_pid_list); // at smallsh exit, kill remaining background children
	return EXIT_SUCCESS;
}