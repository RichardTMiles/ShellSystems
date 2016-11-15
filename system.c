/*+=================================================================+
  |FILE: SYSTEM.C 						                            |
  |DESC: System takes input(SendToSystem), sends to (runCMD) &      |
  | proceds to run command allowing for multiple proceesses at once.|
  |								                                    |
  |VERSION: 3							                            |
  +=================================================================+
*/
extern struct histnode *head;

void SendToSystem(char *input);

void *runCommand(void *idp);

void SendToSystem(char *input) {
    pthread_t array[6];                                  // no more that 6 cmds per line
    pthread_mutex_init(&threading, NULL);                // were going to thread

    int i = 0, output = 1;
    char *command = strtok(input, ";");                  // Stores each command separated by ";"

    while (command && !exitStatus) {
        i++;
        pthread_t array[i];
        pthread_create(&array[i], NULL, runCommand, (void *) command);
        command = strtok(NULL, ";");
    }

    printf("JOIN - ");
    while (i != 0) {
        pthread_cond_wait(&conditions, &threading);
        printf("DONE WAIT");
        pthread_join(array[i], NULL);
        printf(" JOIN \n");
        i--;
    }

    return;

}

void *runCommand(void *idp) {
    char *command = ((char *) idp);
    int option = 0;

    // Trims leading spaces
    while (command[option] != '\0' && isspace((unsigned char) command[option])) option++;

    // Trims trailing spaces
    memmove(command, command + option, strlen(command) - option + 1);

    printf("%s\n", command);

    option = 0;
    if (strcmp(command, "") == 0) return idp;
    else if (strcmp(command, " ") == 0) return idp;
    else if (strcmp(command, "help\n") == 0) option = 1;
    else if (strcmp(command, "quit\n") == 0) option = 2;
    else if (strcmp(command, "cd\n") == 0) option = 3;
    else if (strcmp(command, "path\n") == 0) option = 4;
    else if (strcmp(command, "prompt\n") == 0) option = 5;
    else if (strcmp(command, "history\n") == 0) option = 6;

    pthread_mutex_lock(&threading);
    if (exitStatus == 0) {
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

            case 2: // Exits            -- need to test
                exitStatus = 1;
                exit(0);

            case 3: // CD
                if (chdir(command + 3) != 0) {
                    fprintf(stderr, "Error: cd: Invalid directory\n");
                } else { execl(PATH, "Shell", "-c", command, NULL); }
                pthread_mutex_unlock(&threading);
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
                pthread_mutex_unlock(&threading);

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
                pthread_mutex_unlock(&threading);

            case 6: //print from head
                print(head);
                break;

            default:
                printf("Executing: %s\n", command);
                char *Buffer;
                int data[2];
                pipe(data);

                switch (fork()) {
                    case -1:
                        perror("Fork");
                        exit(2);
                    case 0: /* in child */
                        dup2(data[WRITE], 1);
                        close(data[WRITE]);
                        close(data[READ]);

                        if (execl(PATH, "Shell", "-c", command, NULL) == -1) {
                            fprintf(stderr, "Error: %s: Failed to execute\n", command);
                            _exit(EXIT_FAILURE);
                        }

                    default: /* in parent */
                        close(data[WRITE]);
                        int bytesRead = 0;
                        char message[MAX_LENGTH];
                        bytesRead = read(data[READ], message, MAX_LENGTH);
                        printf("%s\n", message);
                        close(data[READ]);
                        pthread_cond_signal(&conditions);
                        break;
                }
                pthread_mutex_unlock(&threading);
        }
    }
    return idp; // We should never reach this, but tit wil
}
