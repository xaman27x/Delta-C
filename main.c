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


void rebuildTreeFromFile(const char* treeHash, Tree** tree) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/trees/%s", treeHash);

    FILE* treeFile = fopen(path, "r");
    if (!treeFile) {
        perror("Failed to open tree file");
        return;
    }

    *tree = (Tree*)calloc(1, sizeof(Tree));
    if (!*tree) {
        perror("Failed to allocate memory for Tree");
        fclose(treeFile);
        return;
    }

    strncpy((*tree)->hash, treeHash, sizeof((*tree)->hash) - 1);

    char line[512];
    while (fgets(line, sizeof(line), treeFile)) {
        if (strncmp(line, "<blob>:", 7) == 0) {
            Blob* newBlob = (Blob*)calloc(1, sizeof(Blob));
            if (!newBlob) {
                perror("Failed to allocate memory for Blob");
                fclose(treeFile);
                return;
            }
            sscanf(line + 7, "%s %s", newBlob->filename, newBlob->hash);
            newBlob->next = (*tree)->blobs;
            (*tree)->blobs = newBlob;
        } 
        else if (strncmp(line, "<tree>:", 7) == 0) {
            char subtreePath[100], subtreeHash[41];
            sscanf(line + 7, "%s %s", subtreePath, subtreeHash);

            Tree* subtree = NULL;
            rebuildTreeFromFile(subtreeHash, &subtree);

            if (subtree) {
                strncpy(subtree->path, subtreePath, sizeof(subtree->path) - 1);
                subtree->next = (*tree)->subtrees;
                (*tree)->subtrees = subtree;
            }
        }
    }

    fclose(treeFile);
}

void rebuildCommitFromFile(const char* commitHash, Commit** commit) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/commits/%s", commitHash);

    FILE* commitFile = fopen(path, "r");
    if (!commitFile) {
        perror("Failed to open commit file");
        return;
    }

    *commit = (Commit*)calloc(1, sizeof(Commit));
    if (!*commit) {
        perror("Failed to allocate memory for Commit");
        fclose(commitFile);
        return;
    }

    char treeHash[41];
    long timestamp;

    fscanf(commitFile, "%s\n", (*commit)->hash);
    fscanf(commitFile, "%s\n", (*commit)->message);
    fscanf(commitFile, "%s\n%ld\n", treeHash, &timestamp);
    (*commit)->timestamp = (time_t)timestamp;
    (*commit)->parent = NULL;  
    rebuildTreeFromFile(treeHash, &(*commit)->tree);

    fclose(commitFile);
}

void rebuildCommitList(commitList* commits) {
    DIR* commitDir = opendir(".delta/objects/commits");
    if (!commitDir) {
        perror("Failed to open commit directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(commitDir)) != NULL) {
        if (entry->d_name[0] == '.') continue; 

        Commit* newCommit = NULL;
        rebuildCommitFromFile(entry->d_name, &newCommit);

        if (newCommit) {
            appendCommitList(commits, newCommit);
        }
    }

    closedir(commitDir);
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
