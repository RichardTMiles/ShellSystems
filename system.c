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
    char *split = strtok(input, ";");                  // Stores each command separated by ";"
    int i = 0, option;

    while (split && !exitStatus) {
        char *command = split;
        option = 0;
        char *s = command + strlen(command);
        while (--s >= command) {
            if (!isspace(*s)) {
                break;
            }
            *s = 0;
        }

        while (command[option] != '\0' && isspace((unsigned char) command[option])) option++;
        memmove(command, command + option, strlen(command) - option + 1);

        if (strcmp(command, "") != 0) {
            i++;
            pthread_t array[i];
            pthread_create(&array[i], NULL, runCommand, (void *) command);
        }
        split = strtok(NULL, ";");
    }

    printf("JOIN");
    while (i != 0) {
        pthread_cond_wait(&conditions, &threading);
        pthread_join(array[i], NULL);
        i--;
    }
    printf("FOR");
    for(;;) {
        //pthread_cond_wait(&nextInput, &threading);
        break;
    }
    printf("MAIN");
    return;

}

void *runCommand(void *idp) {
    int data[2];
    char *command = ((char *) idp);

    int option = 0;
    if (strcmp(command, "") == 0) return idp;
    else if (strcmp(command, " ") == 0) return idp;
    else if (strcmp(command, "help") == 0) option = 1;
    else if (strcmp(command, "quit") == 0) option = 2;
    else if (strcmp(command, "cd") == 0) option = 3;
    else if (strcmp(command, "path") == 0) option = 4;
    else if (strcmp(command, "prompt") == 0) option = 5;
    else if (strcmp(command, "history") == 0) option = 6;

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
                pipe(data);

                switch (fork()) {
                    case -1:
                        perror("Fork Error");
                        exit(2);
                    case 0: /* in child */
                        dup2(data[WRITE], 1);
                        close(data[WRITE]);
                        close(data[READ]);

                        if (execl(PATH, "Shell", "-c", command, NULL) == -1) {
                            fprintf(stderr, "Error: %s: Failed to execute\n", command);
                            _exit(EXIT_FAILURE);
                        }
                        _exit(EXIT_FAILURE);

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
}
