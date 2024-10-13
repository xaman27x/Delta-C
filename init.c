#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "init.h"
#include "vcs.h"

void initRepository() {
    if (mkdir(".delta") == 0) {
        mkdir(".delta/objects");  // Store blobs and trees
        mkdir(".delta/refs");     // Store branch references
        FILE* headFile = fopen(".delta/HEAD", "w");
        fprintf(headFile, "ref: refs/heads/master\n");  // Initialize HEAD
        fclose(headFile);
        printf("Repository initialized.\n");
    } else if (access(".delta", F_OK) == 0) {
        printf("Repository already exists.\n");
        return;
    } else {
        printf("Failed to initialize repository.\n");
        return;
    }
    return;
}
