#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "vcs.h"

void status() {
    printf("Current status of files:\n\n");

    char tracked_files[100][100];
    char tracked_hashes[100][41];
    int tracked_count = 0;

    // Read `.delta/index` to populate tracked files and hashes
    FILE *indexFile = fopen(INDEX_FILE, "r");
    if (indexFile) {
        char hash[41];
        char filename[100];
        while (fscanf(indexFile, "%40s %99s", hash, filename) != EOF) {
            printf("DEBUG: Staged file in index: %s, Hash: %s\n", filename, hash);  // Debugging output
            strcpy(tracked_files[tracked_count], filename);
            strcpy(tracked_hashes[tracked_count], hash);
            tracked_count++;
        }
        fclose(indexFile);
    } else {
        printf("No staging index found.\n");
    }

    DIR *dir = opendir(".");
    struct dirent *entry;
    char runtime_hash[41];

    // Scan the working directory for files
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) {
                const char *filename = entry->d_name;

                if (strncmp(filename, ".", 1) == 0) continue;

                // Calculate the hash of the current file
                calculateFileHash(filename, runtime_hash);
                printf("DEBUG: Working directory file: %s, Runtime Hash: %s\n", filename, runtime_hash);  // Debugging output

                int is_tracked = 0;
                int is_modified = 0;

                // Check if the file is in `.delta/index`
                for (int i = 0; i < tracked_count; i++) {
                    if (strcmp(filename, tracked_files[i]) == 0) {
                        is_tracked = 1;
                        if (strcmp(runtime_hash, tracked_hashes[i]) != 0) {
                            is_modified = 1;
                        }
                        break;
                    }
                }

                if (is_tracked) {
                    if (is_modified) {
                        printf(RED "Modified: %s\n" RESET, filename);
                    } else {
                        printf(GREEN "Tracked: %s\n" RESET, filename);
                    }
                } else {
                    printf(RED "Untracked: %s\n" RESET, filename);
                }
            }
        }
        closedir(dir);
    }
}

void calculateFileHash(const char *filename, char hash[41]) {
    FILE *file = fopen(filename, "rb");
    if (file) {
        // Read file content and calculate SHA-1 hash
        calculateSHA(filename, hash);
        fclose(file);
    }
}
