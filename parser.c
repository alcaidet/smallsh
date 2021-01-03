#include "header.h"

/*
* parse_cmd_line() parses the command line arguments in the string, cmd_line,
* and puts each argument into the argument array, arg_array.
* 
* Returns arg_array[]
*/
char** parse_cmd_line(char* cmd_line) {
	char* save_ptr;
	char** arg_array = malloc(MAX_ARG * sizeof(char*));
	char* token = strtok_r(cmd_line, " ", &save_ptr);

	int i = 0; // keeps track of arg_array index
	int count = 0; // keeps track of how many arguments are placed into arg_array

	while (token != NULL) {
		token = make_expansion(token); // if the token has substring "$$", substitute for PID
		arg_array[i] = (char*)malloc(strlen(token) * sizeof(char*) + 1);
		strcpy(arg_array[i], token); // place the argument into arg_array
		free(token);
		token = strtok_r(NULL, " ", &save_ptr); //keep parsing the string
		count++; 
		i++;
	}

	// in last argument we must remove the trailing newline
	arg_array[count-1] = remove_newline_null(arg_array[count - 1]);
	arg_array[i] = NULL; // set last element to null

	free(token);

	return arg_array;

}



/*
* arg_count() gets the count of the number of arguments from the argument array.
* This helper function will be used for building the command struct and 
* for freeing arg_array[].
* 
* Returns the count.
*/
int arg_count(char* arg_array[]) {
	int count = 0;
	int i = 0;

	while (arg_array[i] != NULL) {
		count++; //keep track of how many commands there are
		i++;
	}

	return count;
}


/*
* parse_array() takes in the arg_array[] of commands given by the user and the count of 
* arg_array, arg_count, and parses it. This function places all valid arguments such as 
* "ls", "ps" etc. and places them into the array element, argv. All special characters 
* such as ">", "<", "&" change the appropriate element flags - input_true, output_true, background_true.
* If input / output files are indicated, then the filenames are places into elements, stdin_redirect
* or stdout_redirect.
* 
* Returns the command struct.
*
*/
struct command* parse_array(char* arg_array[], int arg_count) {

	/* -------------- INITIALIZE THE COMMAND STRUCT ----------------- */
	struct command* command_struct = malloc(sizeof(struct command));
	command_struct->background_true = 0;
	command_struct->input_true = 0;
	command_struct->output_true = 0;

	int index = 0; // keep track of struct array index 
	command_struct->argv = malloc(MAX_ARG * sizeof(char*)); //allocate space for the argv array

	// first argument in arg_array should be the path of the filename (the command) and we add this to the array in the struct
	command_struct->argv[0] = malloc(strlen(arg_array[0]) * sizeof(char*)+1);
	strcpy(command_struct->argv[0], arg_array[0]);

	// increment the argv index and count of arguments
	index++;
	int argv_count = 1;
	 
	/* --------------- PARSE THE REST OF THE ARG_ARRAY IF THERE'S MORE ------------------------*/
	if (arg_count > 1) { 
		for (int i = 1; i < arg_count; i++) {

			/* ------------------- CHECK FOR INPUT REDIRECTION BY '<' --------------------------*/
			if (strcmp(arg_array[i], INPUT) == 0) {
				command_struct->input_true = 1; // set the input flag
				command_struct->stdin_redirect = malloc(sizeof(char*) * strlen(arg_array[i + 1]) + 1); 
				strcpy(command_struct->stdin_redirect, arg_array[i + 1]); // the desired input file should be after the '<'
			}

			/* ------------------- CHECK FOR OUTPUT REDIRECTION BY '>' --------------------------*/
			else if (strcmp(arg_array[i], OUTPUT) == 0) {
				command_struct->output_true = 1; // set the output flag
				command_struct->stdout_redirect = malloc(sizeof(char*) * strlen(arg_array[i + 1]) + 1);
				strcpy(command_struct->stdout_redirect, arg_array[i + 1]); // the desired output file should be after the '>'
			}

			/* ------------------ CHECK FOR BACKGROUND PROCESSS BY '&\n' ------------------------ */
			else if (strcmp(arg_array[i], AMPERSAND) == 0) {
				command_struct->background_true = 1; // set background flag
			}

			/* ------------------- CHECK FOR VARIABLE  EXPANSION ---------------------------------*/
			else if (strcmp(arg_array[i], EXPANSION) == 0) {
				command_struct->expand_true = 1; // set expansion flag
			}


			/* -------------------  ANY OTHER ARGUMENTS GO TO ARGV ------------------------------ */
			else {
				command_struct->argv[index] = malloc(strlen(arg_array[i]) * sizeof(char*) + 1);
				strcpy(command_struct->argv[index], arg_array[i]);
				argv_count++;
				index++;
			}

		}
	}

	command_struct->argv[index] = NULL; // last argument in argv will be set to NULL
	command_struct->argc = argv_count; // assign the count of argments, argv_count, to argc

	return command_struct;
}