#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "vcs.h"

void initRepository() {
    if (mkdir(".delta") == 0) {
        mkdir(".delta/objects");      // Store blobs and trees
        mkdir(".delta/trees");        // Store tree objects
        mkdir(".delta/commits");      // Store commit objects
        mkdir(".delta/refs");         // Store branch references
        mkdir(".delta/refs/heads");   // Store individual branches
        
        FILE* headFile = fopen(".delta/HEAD", "w");
        if (headFile) {
            fprintf(headFile, "ref: refs/heads/master\n");
            fclose(headFile);
        } else {
            perror("Failed to initialize HEAD file");
        }
        FILE* indexFile = fopen(".delta/index", "w");
        if (indexFile) {
            fclose(indexFile);
        };
        printf("Repository initialized.\n");
    } else if (access(".delta", F_OK) == 0) {
        printf("Repository already exists.\n");
    } else {
        perror("Failed to initialize repository");
    }
}
