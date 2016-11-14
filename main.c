// ========================================================================
// Name:        Richard Miles, Bryan Adams, Steven Harris, Brashad Hasley
// Course:		CSCE 3600
// Date:		November 1, 2016
// Title:		Major Assignment 1
// Version:		17
// Description:
//          Implement a command line interpreter or shell.
// Format:		shell [batchFile]
// ========================================================================

#include <stdio.h>            // everything
#include <stdlib.h>           // EXIT_FAILURE
#include <string.h>           // strncmp
#include <unistd.h>           // execl
#include <ctype.h>            // processes

// Global Constants
#define MAX_LENGTH 513        // 512 + 1 for null

// Global variables
char prompt[32] = "prompt>";                                // Default prompt
char PATH[32] = "/bin/bash";                                // Default PATH directory
struct histnode *head = NULL;
struct histnode *tail = NULL;

#include "history.h"	      // history.h  for history bonus.
#include "system.c"           // for use with system files
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
            printf("fgets returned NULL\n");
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

        //adding command to history
        if(head == NULL){
            head = (struct histnode *) malloc(sizeof(struct histnode));
            strcpy(head->command, userInput);
            tail = head;
            tail->next = NULL;
        }
        else {
            tail->next = (struct histnode *) malloc(sizeof(struct histnode));
            tail = tail->next;
            strcpy(tail->command, userInput);
            tail->next = NULL;
        }

        SendToSystem(userInput);
    } while (1);

}

