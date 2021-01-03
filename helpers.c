#include "header.h"


/*
* remove_newline_null() removes the trailing '\n\0' character from the
* parameter last_arg, which should be the last argument in the argument array
* gotten from the command line
*/
char* remove_newline_null(char* last_arg) {

	// return if the last argument if the ampersand for bg processes
	if (strcmp(last_arg, AMPERSAND) == 0) {
		return last_arg;
	}

	// create a temprorary buffer and strcpy all characters expect the trailing '\n\0'
	char* temp = calloc((strlen(last_arg)),sizeof(char));
	strncpy(temp, last_arg, strlen(last_arg)-1);
	return temp;
}


/*
* check_comment() checks if the first character OR the first argument in the argument array 
* is '#'
* 
* Return 1 if either is True. Return 0 if False.
*/
int check_comment(char* arg_array[]) {
	// check if the first element OR first character is "#" character 
	if (strcmp(arg_array[0], COMMENT_TAG) == 0 || arg_array[0][0] == '#') {
		return 0;
	}
	return 1;
}


/*
* check_blank() checks if the first argument in the array is an empty string.
* I couldn't find the exact answer for this online, but through VS Debugger, I realized
* that fgets traps a "" empty string if the user enters a blank line.
* 
* Return 0 if true. Return 1 if false.
*/
int check_blank(char* arg_array[]) {
	if (strcmp(arg_array[0],"") == 0) {
		return 0;
	}
	return 1;
}

/*
* free_struct() frees all memory of the command struct, cmd_line
*/
void free_struct(struct command* cmd_line) {

	for (int i = 0; i < cmd_line->argc; i++) {
		free(cmd_line->argv[i]);
	}
	free(cmd_line->argv);
	if (cmd_line->input_true == 1) {
		free(cmd_line->stdin_redirect);
	}
	if (cmd_line->output_true == 1) {
		free(cmd_line->stdout_redirect);
	}
	
	free(cmd_line);
}


/*
* free_cmd_array() takes in the initial array of user input, cmd_array,
* and frees all allocated memory
*/
void free_cmd_array(char* cmd_array[], int cmd_array_count) {
	for (int i = 0; i < cmd_array_count; i++) {
		free(cmd_array[i]);
	}
	free(cmd_array);
}