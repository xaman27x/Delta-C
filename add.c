#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "vcs.h"

#define INDEX_PATH ".delta/index"

int hashFile(const char* filename, char hash[41]) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for hashing");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *fileContent = malloc(fileSize + 1);
    if (!fileContent) {
        perror("Failed to allocate memory for file content");
        fclose(file);
        return -1;
    }

    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';
    fclose(file);

    calculateSHA(fileContent, hash);
    free(fileContent);

    return 0;
}

// Append filename and hash to `.delta/index`
void addToStagingArea(const char* filename, const char hash[41]) {
    FILE* indexFile = fopen(INDEX_PATH, "a");
    if (!indexFile) {
        perror("Failed to open index file");
        return;
    }

    fprintf(indexFile, "%s %s\n", hash, filename);  // Append hash and filename
    fclose(indexFile);
}

// Store the file content as a blob in .delta/objects using the hash as the filename
void storeBlob(Blob* blob) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/%s", blob->hash);

    if (access(path, F_OK) == 0) {
        return; // Blob already exists
    }

    FILE* file = fopen(blob->filename, "rb");
    if (!file) {
        perror("Failed to open source file for blob storage");
        return;
    }

    FILE* blobFile = fopen(path, "wb");
    if (!blobFile) {
        perror("Failed to open blob file for writing");
        fclose(file);
        return;
    }

    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytesRead, blobFile);
    }

    fclose(file);
    fclose(blobFile);
}

// Add a file or all files in the directory (if path == ".") to the staging area
void add(const char* path) {
    if (strcmp(path, ".") == 0) {
        // Add all regular files in the current directory
        DIR* dir = opendir(".");
        if (!dir) {
            perror("Failed to open directory");
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // Skip directories and hidden files
            if (entry->d_type == DT_REG && entry->d_name[0] != '.') {
                add(entry->d_name);  // Recursive call to add each individual file
            }
        }
        closedir(dir);
        return;
    }

    char hash[41];
    if (hashFile(path, hash) != 0) {
        return;
    }

    Blob* blob = createBlob(path);
    strcpy(blob->hash, hash);

    // Store the blob in .delta/objects
    storeBlob(blob);

    // Add the file to the staging area (.delta/index)
    addToStagingArea(path, hash);

    freeBlob(blob);
}

Blob* createBlob(const char* filename) {
    Blob* newBlob = malloc(sizeof(Blob));
    if (!newBlob) {
        perror("Failed to allocate memory");
        return NULL;
    }
    
    strncpy(newBlob->filename, filename, sizeof(newBlob->filename) - 1);
    newBlob->next = NULL; 

    return newBlob;
}

void freeBlob(Blob* blob) {
    if (blob) free(blob);
    return;
}
