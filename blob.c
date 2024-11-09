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
void storeBlobObject(Blob* blob) {

    char object_subdir[128];
    char object_filepath[256];
  
    snprintf(object_subdir, sizeof(object_subdir), ".git/objects/%.2s", blob->hash);
    snprintf(object_filepath, sizeof(object_filepath), ".git/objects/%.2s/%s", blob->hash, blob->hash + 2);
    mkdir("object_subdir");
    FILE *blob_file = fopen(filepath, "w");
    if (blob_file) {
        fwrite(blob->fileContents, 1, sizeof(blob->fileContents), blob_file);
        fclose(blob_file);
    }
}

// Function to create a blob
Blob* createBlob(const char* filename) {
    Blob* newBlob = (Blob*)malloc(sizeof(Blob));
    strcpy(newBlob->filename, filename);
    newBlob->next = NULL;

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
    newBlob->fileContents = content;
    calculateSHA1(content, newBlob->hash); 

    fclose(file);
    return newBlob;
}







