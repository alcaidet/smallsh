#include "header.h"


/*
* io_redirection() redirects standard input / output to either the desired files
* or to /dev/null if the command given is meant to be a background process.
* 
* REFERENCE CITED: This function is modified from the last repl.it example
* for redirecting both stdin and stdin from the Proceses and I/O Module
*/
void io_redirection(struct command* cmd_line) {

    /* IF THE COMMAND IS A BACKGRUOND PROCESS AND INPUT FILE IS NOT SPECIFIED, REDIRECT STDIN TO /dev/null */
    if (cmd_line->background_true == 1 && cmd_line->input_true == 0) {

        // Open source file
        int sourceFD = open("/dev/null", O_RDONLY);
        if (sourceFD == -1) {
            perror("source open()");
            exit(1);
        }

        // Redirect stdin to source file
        int result = dup2(sourceFD, 0);
        if (result == -1) {
            perror("source dup2()");
            exit(2);
        }
        close(sourceFD);
    }


    /* IF THE COMMAND IS A BACKGRUOND PROCESS AND OUTPUT FILE IS NOT SPECIFIED, REDIRECT STDOUT TO /dev/null */
    if (cmd_line->background_true == 1 && cmd_line->output_true == 0) {

         // Open target file
        int targetFD = open("/dev/null", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1) {
            perror("target open()");
            exit(1);
        }

         // Redirect stdout to target file
        int result = dup2(targetFD, 1);
        if (result == -1) {
            perror("target dup2()");
            exit(2);
        }
        close(targetFD);
    }


    /* IF THE USER SPECIFIED AN INPUT FILE FOR REDIRECTION, OPEN THAT FILE */
    if (cmd_line->input_true == 1) {

        // Open source file
        int sourceFD = open(cmd_line->stdin_redirect, O_RDONLY);
        if (sourceFD == -1) {
            printf("cannot open %s for input\n", cmd_line->stdin_redirect);
            fflush(stdout);
            exit(1);
        }

        // Redirect stdin to source file
        int result = dup2(sourceFD, 0);
        if (result == -1) {
            perror("source dup2()");
            exit(2);
        }

        close(sourceFD);
    }

    /* IF THE USER SPECIFIED AN OUTPUT FILE FOR REDIRECTION, OPEN THAT FILE */
    if (cmd_line->output_true == 1) {
        // Open target file
        int targetFD = open(cmd_line->stdout_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (targetFD == -1) {
            printf("cannot open %s for output\n", cmd_line->stdout_redirect);
            fflush(stdout);
            exit(1);
        }

         // Redirect stdout to target file
        int result = dup2(targetFD, 1);
        if (result == -1) {
            perror("target dup2()");
            exit(2);
        }

        close(targetFD);
    }

    if (cmd_line->input_true == 1 || cmd_line->output_true == 1) {
        cmd_line->argv[1] = NULL; // execvp arguments MUST be terminated by NULL, so set the second arg in array to NULL
    }

}