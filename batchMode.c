/*
 *  Function: batchMode
 *  Author:   Richard Miles
 *  Description:
 *      Manages the file input and sends it one line at a time to the system.
 *      We will end up placing the pThread commands here
 */
int batchMode(char* path) {
    printf("*** batch mode *** \n");
    // Opens file for reading and stores stream in batchFile
    FILE* input = fopen(path, "r");
    if (input == NULL) {
        fprintf(stderr, "Error: File does not exist or cannot be opened.\n");
        return EXIT_FAILURE;
    }
    // Move the input (file) pointer to the end of the file
    fseek(input, 0, SEEK_END);
    // If the current file position is 0 (start = end)
    if (ftell(input) == 0) {
        fprintf(stderr, "Error: File is empty.\n");
        return EXIT_FAILURE;
    }
    // SEEK_SET	= Beginning of file
    fseek(input, 0, SEEK_SET);

    // Lets make the array we'll return.
    char SHELL[MAX_LENGTH];

    // 'fgets' reads a whole line into the buffer stopping at '\n'

    while (fgets(SHELL, MAX_LENGTH, input)) { // This will iterate every line
        if (SHELL[strlen(SHELL) - 1] == '\n') {
            printf("\nBatch line> %s", SHELL);     // Print the line read
            SendToSystem(SHELL);               // Execute each line of commands read in from batchFile
        } else {
            fprintf(stderr, "Error: Line must be less than 512 characters\n");
            fflush(stdout);
            int ch;
            while ((ch = getc(input))) {
                if (ch == '\n') {
                    break;
                }
            }
        }
    }

    // Closes input file
    fclose(input);

    return 1;
}