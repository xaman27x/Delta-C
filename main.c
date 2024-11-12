#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcs.h"

#define ENTRY_SIZE 20
#define MAX_FILES 50
#define MAX_FILENAME_LENGTH 100
#define MAX_MESSAGE_LENGTH 256

void addFile(const char *filename) {
    add(filename);
    
    FILE *indexFile = fopen(".delta/index", "r");
    if (indexFile) {
        char line[256];
        while (fgets(line, sizeof(line), indexFile)) {
            line[strcspn(line, "\n")] = '\0';
            printf("Staged: %s\n", line);
        }
        fclose(indexFile);
    } else {
        printf("Failed to open index file\n");
    }
}

int main() {
    Commit* commitList = NULL;
    char inputLine[256];
    char entry[ENTRY_SIZE];

    while (fgets(inputLine, sizeof(inputLine), stdin) != NULL) {
        inputLine[strcspn(inputLine, "\n")] = '\0';

        char *command = strtok(inputLine, " ");
        if (command == NULL) continue;

        if (strcmp(command, "init") == 0) {
            initRepository();
        
        } else if (strcmp(command, "add") == 0) {
            char **files = (char **)malloc(MAX_FILES * sizeof(char *));
            int len = 0;

            if (!files) continue;

            char *filename = strtok(NULL, " ");
            while (filename && len < MAX_FILES) {
                files[len] = (char *)malloc(MAX_FILENAME_LENGTH * sizeof(char));
                if (!files[len]) break;
                strncpy(files[len++], filename, MAX_FILENAME_LENGTH - 1);
                filename = strtok(NULL, " ");
            }

            if (len == 0) {
                printf("Specify a file\n");
            } else {
                for (int i = 0; i < len; i++) {
                    addFile(files[i]);
                    free(files[i]);
                }
            }
            free(files);

        } else if (strcmp(command, "commit") == 0) {
            char *commitMessage = (char *)malloc(MAX_MESSAGE_LENGTH * sizeof(char));
            if (!commitMessage) continue;

            printf("Commit Message: ");
            fgets(commitMessage, MAX_MESSAGE_LENGTH, stdin);
            commitMessage[strcspn(commitMessage, "\n")] = '\0';

            commit(commitMessage, commitList);
            free(commitMessage);

        } else if (strcmp(command, "status") == 0) {
            status();

        } else if (strcmp(command, "log") == 0) {
            commitLog(&commitList);

        } else {
            printf("Invalid Keyword!\n");
        }
    }
    return 0;
}
