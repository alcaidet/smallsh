#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h>
#include <fcntl.h>

#define USER_EXIT "exit"
#define CD_HOME "cd"
#define USER_STATUS "status"
#define AMPERSAND "&\n" // ampersand should only be at the end; fgets adds null terminator
#define COMMENT_TAG "#"
#define NEWLINE "\n"
#define COMMA ","
#define OUTPUT ">"
#define INPUT "<"
#define EXPANSION "$$" 
#define EXPAND_CHAR '$'
#define MAX_BG 100
#define MAX_CHAR 2048
#define MAX_ARG 512



struct command {
	int argc;
	char** argv;
	int input_true;
	char* stdin_redirect;
	int output_true;
	char* stdout_redirect;
	int background_true;
	int expand_true;
};



// -------------- PARSE COMMAND LINE FUNCTIONS ------------
char** parse_cmd_line(char* cmd_line);
int arg_count(char* arg_array[]);
struct command* parse_array(char* arg_array[], int arg_count);

// ---------------- HELPERS -------------------------------
int check_comment(char* arg_array[]);
int check_blank(char* arg_array[]);
char* remove_newline_null(char* last_arg);
void free_struct(struct command* cmd_line);
void free_cmd_array(char* cmd_array[], int cmd_array_count);

// ------------ BUILT-IN COMMANDS -----------------------
void change_cd(struct command* cmd_line);
void show_status(int childStatus);

// ------------- I/O REDIRECTION ---------------------------
void io_redirection(struct command* cmd_line);

// ------------- BACKGROUND PROCESSESS ---------------
void check_bg_child(pid_t childPid, int childStatus, int bg_pid_list[]);
void kill_children(int bg_pid_list[]);

// ------------- VARIABLE EXPANSION ----------------
char* make_expansion(char* token);

