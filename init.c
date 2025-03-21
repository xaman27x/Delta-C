#include "vcs.h"

void initRepository() {
    if (mkdir(".delta") == 0) {
        mkdir(".delta/objects");
        mkdir(".delta/objects/trees");      // Store blobs and trees
        mkdir(".delta/objects/blobs");        // Store tree objects
        mkdir(".delta/objects/commits");      // Store commit objects
        mkdir(".delta/refs");         // Store branch references
        mkdir(".delta/refs/heads");   // Store individual branches
        
        FILE* headFile = fopen(".delta/HEAD", "w");
        if (headFile) {
            fprintf(headFile, "NULL");
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
