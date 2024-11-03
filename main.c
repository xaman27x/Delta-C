#include <stdio.h>
#include "init.h"
#include "blob.h"
#include "vcs.h"

//Testing init command
int main() {
    // Initialize the repository
    initRepository();

    const char *filename = "testfile.txt";
    Blob *blob = stageFile(filename);
    if (blob) {
        storeBlob(blob);
        printf("Blob stored in .delta/objects/%s\n", blob->hash);
        free(blob);
    } else {
        printf("Failed to create blob for %s\n", filename);
    }
    return 0;
}
