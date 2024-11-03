#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcs.h"

// Stages a file by creating a blob and hashing its content
Blob* stageFile(const char* filename) {
    Blob* newBlob = (Blob*)malloc(sizeof(Blob));
    strcpy(newBlob->filename, filename);

    // Read the file content and calculate its hash
    char content[1024];
    FILE* file = fopen(filename, "r");
    size_t bytesRead = fread(content, 1, sizeof(content), file);
    content[bytesRead] = '\0';
    calculateSHA1(content, newBlob->hash);

    fclose(file);
    return newBlob;
}

// Stores the blob
void storeBlob(Blob* blob) {
    char path[256];
    snprintf(path, sizeof(path), ".delta/objects/%s", blob->hash);
    FILE* blobFile = fopen(path, "w");

    if (!blobFile) {
        printf("Error: Unable to store blob %s\n", blob->filename);
        return;
    }

    fprintf(blobFile, "%s\n", blob->filename); 
    fclose(blobFile);
}

// Function to create a blob
Blob* createBlob(const char* filename) {
    Blob* newBlob = (Blob*)malloc(sizeof(Blob));
    strcpy(newBlob->filename, filename);

    // Read the file content and calculate its hash
    char content[1024];
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Unable to open file %s\n", filename);
        free(newBlob);
        return NULL;
    }

    size_t bytesRead = fread(content, 1, sizeof(content), file);
    content[bytesRead] = '\0';
    calculateSHA1(content, newBlob->hash); 

    fclose(file);
    return newBlob;
}







