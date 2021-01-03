#include "header.h"
#include <signal.h>

/*
* check_bg_child() determines whether ANY background child has terminated (normally or abnormally).
* If a background child has terminated, then this function iterates through the list of background
* process pids, finds the pid of the terminated background child in the list, and then places a garbage value, -5,
* in that pid's space. We need this value to later check for unfinished background children.
*/
void check_bg_child(pid_t childPid, int childStatus, int bg_pid_list[]) {

    childPid = waitpid(-1, &childStatus, WNOHANG); // -1 tells waitpid() to wait for ANY child process

    // if waipid returns a positive int, then that int is a childPid
    if (childPid > 0) {

        /* WIFEXITED returns true of the child was terminated normally */
        if (WIFEXITED(childStatus)) {
            printf("background pid %d is done: exit value %d\n", childPid, WEXITSTATUS(childStatus));
            fflush(stdout);

            // find the child pid in the bg_pid_list[], and change the pid to -5
            for (int i = 0; i < MAX_BG; i++) {
                if (bg_pid_list[i] == childPid) {
                    bg_pid_list[i] = -5;
                }
            }
        }

        /* WIFSIGNALED returns true if the child was terminated abnormally. Indicate the signal that terminated the child */
        else if (WIFSIGNALED(childStatus)) {
            printf("background pid %d is done: terminated by signal %d\n", childPid, WTERMSIG(childStatus));
            fflush(stdout);

            // find the child pid in the bg_pid_list[], and change the pid to -5
            for (int i = 0; i < MAX_BG; i++) {
                if (bg_pid_list[i] == childPid) {
                    bg_pid_list[i] = -5;
                }
            }
        }
    }
}


/*
* kill_children() iterates through the bg_pid_list[] of background processes, finds their pids (by
* looking for positive integers in the list), and kills the process using SIGTERM.
*/
void kill_children(int bg_pid_list[]) {
    for (int i = 0; i < MAX_BG; i++) {
        if (bg_pid_list[i] > 0) {
            kill(bg_pid_list[i], SIGTERM);
        }
    }
}