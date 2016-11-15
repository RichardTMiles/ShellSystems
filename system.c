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
    pthread_t array[6];                          // no more that 6 cmds per line
    pthread_mutex_init(&threading, NULL);        // were going to thread
    char *split = strtok(input, ";");            // Stores each command separated by ";"
    int i = 0, option;

    while (split && !exitStatus) {
        option = 0;
        char *cmd = split;
        char *s = cmd + strlen(cmd);
        while (--s >= cmd) {
            if (!isspace(*s)) {
                break;
            }
            *s = 0;
        }

        while (cmd[option] != '\0' && isspace((unsigned char) cmd[option])) option++;
        memmove(cmd, cmd + option, strlen(cmd) - option + 1);

        if (strcmp(cmd, "") != 0) {
            running++; i++;
            pthread_t array[i];
            pthread_create(&array[i], NULL, runCommand, (void *) cmd);
        }
        split = strtok(NULL, ";");
    }

    while (i != 0 && running != 0) {
        pthread_cond_wait(&conditions, &threading);
        pthread_join(array[i], NULL);
        i--;
    }

    for (;;) {
        if (running == 0) {
            pthread_mutex_destroy(&threading);
            if (exitStatus) { exit(1); }
            return;
        }
    }
}

void *runCommand(void *idp) {
    pthread_mutex_lock(&threading);
    int data[2];
    char *command = ((char *) idp);

    int option = 0;
    if (strcmp(command, "") == 0) { running--; return idp; }
    else if (strcmp(command, " ") == 0) { running--; return idp; }
    else if (strcmp(command, "help") == 0) option = 1;
    else if (strcmp(command, "quit") == 0) option = 2;
    else if (strcmp(command, "cd") == 0) option = 3;
    else if (strcmp(command, "path") == 0) option = 4;
    else if (strcmp(command, "prompt") == 0) option = 5;
    else if (strcmp(command, "history") == 0) option = 6;

    if (exitStatus == 0) {
        switch (option) {
            case 1: // help
                        printf("|===== Commands ====|\n"
                               "|        help       |\n"
                               "|       history     |\n"
                               "|       prompt      |\n"
                               "|        path       |\n"
                               "|         cd        |\n"
                               "|        quit       |\n"
                               "+++++++++++++++++++++\n");

                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
                break;

            case 2: // Exits            -- need to test
                exitStatus = 1;
                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
                break;

            case 3: // CD
                if (chdir(command + 3) != 0) {
                    fprintf(stderr, "Error: cd: Invalid directory\n");
                } else { execl(PATH, "Shell", "-c", command, NULL); }
                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
                break;

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
                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
                break;
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
                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
                break;

            case 6: //print from head
                print(head);
                pthread_cond_signal(&conditions);
                pthread_mutex_unlock(&threading);
                running--;
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
                        pthread_mutex_unlock(&threading);
                        running--;
                        break;
                }
        }
    }
}
