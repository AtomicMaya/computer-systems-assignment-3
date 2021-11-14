#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

/** @brief returns a help message, here to unclutter main.c */ 
char *getHelpMessage() {
    return "\nInput Format: cmd l_type start length [whence]\n\
    \r`cmd`    --- `g` (F_GETLK), `s` (F_SETLK) or `w` (F_SETLKW)\n\
    \r`l_type` --- `r` (F_RDLCK), `w` (F_WRLCK) or `u` (F_UNLCK)\n\
    \r`start`  --- lock starting offset\n\
    \r`length` --- number of bytes to lock\n\
    \r`whence` --- `s` (SEEK_SET, default), `c` (SEEK_CUR) or `e` (SEEK_END)\n\
    \r`q`      --- quit\n";
}

/** @brief Necessary addition to the utils as `atoi` or `strtol` do not set `errno` and default to 0, a possible value.
 * @param potential A string that can either be an int or something else
 * @return Either the integer value or -1 if an error occurs (errno is set accordingly).
 */
int isnumber(char* potential) {
    int conclusion = 1;
    for (int i = 0; potential[i] != '\0'; i++) 
        if (potential[i] == -49 || potential[i] < 48 || potential[i] > 57)
            conclusion = 0;
    
    if (conclusion == 1)
        return atoi(potential);
    else {
        errno = EINVAL;
        return -1;
    }
}

#define MAX_STR_SIZE 1024
/** @brief Prints out an flock to a human readable format.
 * @param fl    The concerned flock.
 * @param dest Where the human readable version should be piped to. (should be mAlloced beforehand)
 */
void lockToString(struct flock *fl, char *dest) {
    char *buffer = malloc(MAX_STR_SIZE);
    strncat(dest, "Denied by ", 20);
    switch (fl->l_type) {
        case F_RDLCK:
            snprintf(buffer, 12, "%s ", "READ lock");
            break;
        case F_WRLCK:
            snprintf(buffer, 12, "%s ", "WRITE lock");
            break;
        default:
            break;
    }

    strncat(dest, buffer, 20);
    strncat(dest, " on ", 20);

    snprintf(buffer, 100, "%ld:%ld ", fl->l_start, fl->l_start + fl->l_len);
    strncat(dest, buffer, 100);

    snprintf(buffer, 24, "(held by PID %d)\n", fl->l_pid);
    strncat(dest, buffer, 24);

    free(buffer);
}