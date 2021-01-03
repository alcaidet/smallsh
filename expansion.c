#include "header.h"

/*
* make_expansion() takes in the passed token and substitutes all instances of "$$" in
* the token for the PID of smallsh. 
* Returns the newly subtituted (or not) token as "newptr"
*/
char* make_expansion(char* token) {

    int pid = getpid(); // get PID of smallsh

    // Create a buffer, buff, and use sprintf() to convert PID to a string
    // buff is now the PID as a string
    char* buff = (char*)calloc(MAX_CHAR, sizeof(char*));
    sprintf(buff, "%d", pid);

    int count = 0; // count keeps track of the number of characters between instances of "$$"
    int i; // i keeps track of the current index of the token string
    char* newptr = (char*)calloc(MAX_CHAR, sizeof(char*)); // newptr will hold the newly substituted token

    // now iterate through the string
    for (i = 0; i < strlen(token); i++) {

        /* If the current character AND the next character in the token is '$', 
        *  then we use strncat to concatenate all characters in the token UP TO the current character in newptr.
        *  Then we use strcat to concatenate the new PID to newptr */
        if (token[i] == EXPAND_CHAR && token[i + 1] == EXPAND_CHAR) {
            strncat(newptr, &token[i - count], count);
            strcat(newptr, buff);

            /* If the next 2 characters after the current char are also '$', then we add 1 to i and 
             * reset count to 0 */
            if (token[i + 2] == EXPAND_CHAR && token[i + 3] == EXPAND_CHAR) {
                i = i + 1;
                count = 0;
            }

            /* Otherwise reset count to 1 and add 2 to i to get the for loop to pass over the "$$"
            * iterations */
            else {
                count = 1;
                i = i + 2;
            }

            continue;
        }
        count++; // increment count with every for loop iteration
    }

    // If there's still a character left at the end of the string, then we strcat to newptr
    if (count >= 1) {
        strcat(newptr, &token[i - count]);
    }
 
    free(buff);
    return newptr;
}