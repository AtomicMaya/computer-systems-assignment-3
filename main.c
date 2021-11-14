#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include "lineReader.h"
#include "utils.h"

/** @brief Nope */
int main(int argc, char **argv) {
	int programShouldRun = 1;

	if (argc != 2) // Standard check for clean use.
		FAIL_FSUCCESFULLY("Usage: %s file\n", argv[0]);

	char *fileAddr = argv[1];

	// Check for does this file exist / does the user have sufficient rights ?
	struct stat *statBuffer = malloc(sizeof(struct stat*));
	if (lstat(fileAddr, statBuffer) != 0)
		FAIL_FSUCCESFULLY("Error while reading '%s' : %s\n", fileAddr, strerror(errno)); // Macros galore !

	free(statBuffer);

	printf("Press ? for help.\n");
	int fd = open(fileAddr, O_RDWR, 0600);	// Open it with sufficient rights.

	// Program loop
	do {
		printf("\n[PID=%d]> ", getpid()); // UX !

		Options *options = malloc(sizeof(Options));
		setToDefault(options);	// Default the options, just in case.
		getLockingOptions(options);
		
		struct flock fl;
		memset (&fl, 0, sizeof(fl));

		if (options->quit == 1) {	// Clean up and exit gracefully.
			if (close(fd) == -1)
				printf("Closing %s produced an error %d : %s\n", fileAddr, errno, strerror(errno));
			
			programShouldRun = 0;
		} else if (options->help == 1)	// Plop a "help" message.
			printf("%s", getHelpMessage());
		else if (errno != 0)  // Skip handling if invalid arguments
			errno = 0;
		else {	// Generic use case.
			// Set flock params
			fl.l_type = options->l_type;
			fl.l_start = options->start;
			fl.l_len = options->length;
			fl.l_whence = options->whence;
			fl.l_pid = getpid();
			
			errno = 0;
			int lockstatus = fcntl(fd, options->cmd, &fl);	// Do the thing

			if (options->cmd == F_GETLK && fl.l_type != F_UNLCK) { // The thing failed
				char *info = malloc(1024);	// Print info for
				fcntl(fd, F_GETLK, &fl);	// related lock
				lockToString(&fl, info);
				printf("%s", info);
				free(info);
			} else if (lockstatus == 0)
				printf((fl.l_type == F_UNLCK) ? "%s Unlocked\n" : "%s Locked\n", fileAddr);	// Causal Casual prints.
			else {
				// Handle errors gracefully.
				printf("An error occured while applying lock : %d which means %s\n", errno, strerror(errno));
				if (errno == EAGAIN) {
					lockstatus = fcntl(fd, F_GETLK, &fl);
					printf("%d currently has lock privileges over %s\n", fl.l_pid, fileAddr);
				}
			}
		}
		free(options);
	} while (programShouldRun == 1);	// Self explanatory, really !

	exit(EXIT_SUCCESS);	// exit(), Exit, eeeexit, tixe, ʇᴉxǝ, eᴉxt
}