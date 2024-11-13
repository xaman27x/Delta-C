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

// Load the commit list from stored commit files
void loadCommitHistory(commitList* commits) {
    DIR* dir = opendir(".delta/objects/commits");
    if (!dir) {
        perror("Failed to open commits directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Read commit file
        char commitPath[256];
        snprintf(commitPath, sizeof(commitPath), ".delta/objects/commits/%s", entry->d_name);
        FILE* commitFile = fopen(commitPath, "r");
        if (!commitFile) {
            perror("Failed to open commit file");
            continue;
        }

        Commit* commit = (Commit*)calloc(1, sizeof(Commit));
        if (!commit) {
            perror("Failed to allocate memory for Commit");
            fclose(commitFile);
            continue;
        }

        fscanf(commitFile, "%40s\n", commit->hash);  // Commit hash
        fgets(commit->message, sizeof(commit->message), commitFile);  // Commit message
        commit->message[strcspn(commit->message, "\n")] = '\0';  // Remove newline
        fscanf(commitFile, "%40s\n", commit->tree->hash);  // Root tree hash
        fscanf(commitFile, "%ld\n", &commit->timestamp);  // Timestamp

        // Load parent commit hash (if exists)
        char parentHash[41];
        if (fscanf(commitFile, "%40s\n", parentHash) > 0) {
            Commit* parentCommit = findCommitByHash(parentHash, *commits);  // Find parent commit by hash
            commit->parent = parentCommit;
        }

        fclose(commitFile);

        // Append to commit list
        appendCommitList(commits, commit);
    }

    closedir(dir);
}
Commit* findCommitByHash(const char* hash, commitList commits) {
    Commit* current = commits;
    while (current) {
        if (strcmp(current->hash, hash) == 0) {
            return current;
        }
        current = current->parent;
    }
    return NULL;
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
            loadCommitHistory(&commitList);
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
