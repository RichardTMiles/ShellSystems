// ========================================================================
// Name:        Richard Miles,
// Course:		CSCE 3600
// Date:		November 1, 2016
// Title:		Major Assignment 1
// Version:		16
// Description:
//          Implement a command line interpreter or shell.
// Format:		shell [batchFile]
// ========================================================================

#include <stdio.h>            // everything
#include <stdlib.h>           // EXIT_FAILURE
#include <string.h>           // strncmp
#include <unistd.h>           // execl
#include <ctype.h>            // processes

#define MAX_LENGTH 513        // 512 + 1 for null

// Global variables
char prompt[32] = "prompt>";                                // Default prompt
char PATH[32] = "/bin/bash";                                // Default PATH directory

#include "system.c"
#include "batchMode.c"


int main(int argc, char *argv[]) {

    if (argc > 2) { // Too many Arguments
        fprintf(stderr, "Error: Too many arguments.\n");
        return EXIT_FAILURE;
    }

    if (argc == 2) { // Batch Mode Common Case First
        batchMode(argv[1]);
    }

    // *** interactive mode ***
    char userInput[MAX_LENGTH];                 // Stores string input by user

    printf("*** interactive mode ***\n");
    do {
        printf("\n%s ", prompt);

        // Reads user input and handles Ctrl-D
        if (fgets(userInput, (MAX_LENGTH + 3), stdin) == NULL) {
            putchar('\n');
            fflush(stdout);
            return EXIT_SUCCESS;
        }
        else if (strlen(userInput) > 512) {
            fprintf(stderr, "Error: %u: command must be less than 512 characters long\n", (unsigned) strlen(userInput));
            int ch;
            while ((ch = getc(stdin)) != '\n' || ch != EOF);
            continue;
        }

        SendToSystem(userInput);

    } while (1);

    return EXIT_SUCCESS;
}
