#include <stdio.h>
#include <stdlib.h>
#include "vcs.h"

void addFile(const char *filename) {
    add(filename);
    
    FILE *indexFile = fopen(".delta/index", "r");
    if (indexFile) {
        char line[256];
        while (fgets(line, sizeof(line), indexFile)) {
            printf("Staged: %s", line);
        }
        fclose(indexFile);
    } else {
        printf("Failed\n");
    }
}

int main() {
    initRepository();
    addFile("testfile.txt");
    return 0;
}
