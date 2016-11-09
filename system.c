//
// Created by Richard Miles on 10/31/16.
//

/*
 * This is the system file, there are three steps
 *
 *
 *
 */

void SendToSystem(char *input);
int runCommand(char command[]);

void SendToSystem(char *input) {
    int i = 0, output = 1;
    char* command = strtok(input, ";");                  // Stores each command separated by ";"

    while(command && output) {

        output = runCommand(command);
        command = strtok(NULL, ";");
        i++;
    }

    while(i != 0){



        i--;
    }
    return;

}

int runCommand(char command[]) {

    int option = 0;

    // Trims leading spaces
    while (command[option] != '\0' && isspace((unsigned char) command[option]))  option++;

    // Trims trailing spaces
    memmove(command, command + option, strlen(command) - option + 1);

    option = 0;

    if (strcmp(command, "") == 0) return 1;
    else if (strcmp(command, "help") == 0) option = 1;
    else if (strcmp(command, "quit") == 0) option = 2;
    else if (strcmp(command, "cd") == 0)   option = 3;
    else if (strcmp(command, "path") == 0) option = 4;
    else if (strcmp(command, "prompt") == 0) option = 5;


    switch (option) {

        case 1: // help
            printf("/------------------  Help Menu   -------------------\\\n"
                   "| help          - Display the Help Menu            |\n"
                   "| cd            - change current directory         |\n"
                   "| history       - prints list of commands entered  |\n"
                   "| prompt        - sets custom prompt string        |\n"
                   "| customize     - sets customized shell options    |\n"
                   "| path          - sets PATH directory              |\n"
                   "| cd            - change current directory         |\n"
                   "| quit OR exit  - exits shell program              |\n"
                   "\\--------------------------------------------------/\n");
            return 1;

        case 2: // Exits            -- need to test
            return 0;

        case 3: // CD
            if (chdir(command + 3) != 0) {
                fprintf(stderr, "Error: cd: Invalid directory\n");
            } else { execl(PATH, "Shell", "-c", command, NULL); }
            return 1;

        case 4: // Path
            do {
                printf("Enter custom path: ");
                fgets(PATH, 32, stdin);
                if (PATH[strlen(PATH) - 1] != '\n') {
                    fprintf(stderr, "Error: Maximum of 30 characters allowed\n");
                    int ch;
                    while ((ch = getc(stdin)) != '\n' && ch != EOF);
                }
            } while (PATH[strlen(PATH) - 1] != '\n');
            PATH[strcspn(PATH, "\n")] = '\0';              // Removes trailing '\n' from string
            printf("PATH successfully updated\n");
            return 1;

        case 5: // prompt
            do {
                printf("Enter custom prompt: ");
                fgets(prompt, 32, stdin);
                if (prompt[strlen(prompt) - 1] != '\n') {
                    fprintf(stderr, "Error: Maximum of 30 characters allowed\n");
                    int ch;
                    while ((ch = getc(stdin)) != '\n' && ch != EOF);
                }
            } while (prompt[strlen(prompt) - 1] != '\n');
            prompt[strcspn(prompt, "\n")] = '\0';          // Removes trailing '\n' from string
            printf("Prompt successfully updated\n");
            return 1;

        default:
            execl(PATH, "Shell", "-c", command, NULL);
            fprintf(stderr, "Error: %s: Failed to execute\n", command);
            _exit(EXIT_FAILURE);
            return 1;
    }     // Returns false if shell should exit successfully

}