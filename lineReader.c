#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "lineReader.h"
#include "utils.h"

/** @brief Initializes an Options struct to default values (ie. Whence should be set to SEEK_SET)
 * @param o The concerned Options struct.
 */
void setToDefault(Options *o) {
	o->cmd = -1;        o->l_type = -1;
	o->start = -1;      o->length = -1;
	o->quit = -1;       o->help = -1;
	o->whence = SEEK_SET;
}

#define LINE_BUFFER_SIZE 1024
#define LINE_ELEMENTS_SIZE 32
#define LINE_SPLIT_PLACES " \t\r\n\a"

/** @brief Acquires a line from the stdin and returns it.
 * @return a string containing the contents of the input.
 */
char* getLine() {
	int bufferSize = LINE_BUFFER_SIZE;
	int pos = 0;
	char *line = malloc(bufferSize);
	int c;

	if (!line) FAIL_SUCCESFULLY("mAllocation Error");

	while (1) {
		c = getchar();
		if (c == EOF || c == '\n') {		// Acquire end of line (Enter key)
			line[pos] = '\0';
			return line;
		} else
			line[pos] = c;	// Append to buffer
		
		if (pos >= bufferSize) {	// Reallocate if necessary
			bufferSize += LINE_BUFFER_SIZE;
			line = realloc(line, bufferSize);
			if (!line) FAIL_SUCCESFULLY("mAllocation Error");
		}
		pos++;
	}
}

/** @brief Tokenizes the user's input
 * @param line The users input.
 * @param count The amount of params, useful externally.
 * @return The different tokens.
 */
char **splitLine(char *line, int *count) {
	int bufferSize = LINE_ELEMENTS_SIZE;
	int pos = 0;
	char **elements = malloc(bufferSize * sizeof(char*));		// Token holder
	char *element;

	if (!elements) FAIL_SUCCESFULLY("mAllocation Error");	// Handle errors

	element = strtok(line, LINE_SPLIT_PLACES);	// Tokenize on flags
	while (element != NULL) {
		elements[pos] = element;
		pos++;

		if (pos >= bufferSize) {	// Reallocate if excess
			bufferSize += LINE_ELEMENTS_SIZE;
			elements = realloc(elements, bufferSize * sizeof(char*));
			if (!elements) FAIL_SUCCESFULLY("mAllocation Error");
		}
		element = strtok(NULL, LINE_SPLIT_PLACES);
	}
	*count = pos;		// External use
	elements[pos] = NULL;
	return elements;
}

/** @brief Recuperate the options (get input, tokenize and setup Options struct accordingly)
 * @param options The Options struct to be affected.
 */
void getLockingOptions(Options *options) {
	char *line = getLine();
	int *count = malloc(sizeof(int));
	char **lineValues = splitLine(line, count);		// Get input & params

	if (*count != 1 && *count != 4 && *count != 5) {	// Count the number of args (1 = q | ?, 4 = standard, 5 = standard + whence)
		errno = 0;
		fprintf(stderr, "%s : %d\n", "Incompatible amount of arguments", *count);
		return;
	}

	int pos = 0;
	while(lineValues[pos]) {		// Parse tokens
		if (pos == 0) {				// CMD
			char v = lineValues[pos][0];
			switch(v) {
				case 'g': options->cmd = F_GETLK; break;	// FLOCK behavior
				case 's': options->cmd = F_SETLK; break;
				case 'w': options->cmd = F_SETLKW; break;
				case 'q': options->quit = 1; return; break;	// Or UX
				case '?': options->help = 1; return; break;
				default: fprintf(stderr, "Invalid argument provided at position %d : %c\n", pos, v); break;
			}
		} else if (pos == 1) {	// FLOCK LTYPE
			char v = lineValues[pos][0];
			switch(v) {
				case 'r': options->l_type = F_RDLCK; break;
				case 'w': options->l_type = F_WRLCK; break;
				case 'u': options->l_type = F_UNLCK; break;
				default: fprintf(stderr, "Invalid argument provided at position %d : %c\n", pos, v); return;
			}
		} else if (pos == 2) {	// START INDEX
			options->start = isnumber(lineValues[pos]);
			if (errno != 0) {
				printf("Invalid argument at position : %d -> %s is not a number !\n", pos, lineValues[pos]);
				return;
			}
		}
		else if (pos == 3) {	// LENGTH
			options->length = isnumber(lineValues[pos]);
			if (errno != 0) {
				printf("Invalid argument at position : %d -> %s is not a number !\n", pos, lineValues[pos]);
				return;
			}
		} else if (pos == 4) {	// Optionally : WHENCE (Skipped if not exists, defaulted to SEEK_SET)
			char v = lineValues[pos][0];
			switch(v) {
				case 's': options->whence = SEEK_SET; break;
				case 'c': options->whence = SEEK_CUR; break;
				case 'e': options->whence = SEEK_END; break;
				default: fprintf(stderr, "Invalid argument provided at position %d : %c\n", pos, v); return;
			}
		}
	
		pos-=-1; // https://www.reddit.com/r/ProgrammerHumor/comments/dtuhdu/symmetric/
	}

	// Memory management
	free(line);
	free(lineValues);
}