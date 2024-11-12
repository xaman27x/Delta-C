#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "vcs.h"


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

int isFileInStagingArea(const char* filename, const char hash[41]) {
    FILE* indexFile = fopen(INDEX_FILE, "r");
    if (!indexFile) {
        return 0; // No staging area exists yet
    }

    char fileHash[41];
    char fileName[100];

    while (fscanf(indexFile, "%40s %99s", fileHash, fileName) != EOF) {
        if (strcmp(fileName, filename) == 0) {
            // File is already in staging area, check if hash matches
            if (strcmp(fileHash, hash) == 0) {
                fclose(indexFile);
                return 1; // File with the same hash is already staged
            }
        }
    }

    fclose(indexFile);
    return 0; // File not found in staging area
}

void updateStagingArea(const char* filename, const char hash[41]) {

    FILE* indexFile = fopen(INDEX_FILE, "r");
    FILE* tempFile = fopen(".delta/index_temp", "w");

    if (!indexFile || !tempFile) {
        perror("Failed to open index or temporary file");
        if (indexFile) fclose(indexFile);
        if (tempFile) fclose(tempFile);
        return;
    }

    char fileHash[41];
    char fileName[100];
    int updated = 0;

    while (fscanf(indexFile, "%40s %99s", fileHash, fileName) != EOF) {
        if (strcmp(fileName, filename) == 0) {
            fprintf(tempFile, "%s %s\n", hash, filename);
            updated = 1; 
        } else {
            fprintf(tempFile, "%s %s\n", fileHash, fileName);
        }
    }

    if (!updated) {
        fprintf(tempFile, "%s %s\n", hash, filename);
    }

    fclose(indexFile);
    fclose(tempFile);

    remove(INDEX_FILE);
    rename(".delta/index_temp", INDEX_FILE);
}

// Store the file content as a blob in .delta/objects
void storeBlob(Blob* blob) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/blobs/%s", blob->hash);

    if (access(path, F_OK) == 0) {
        return; // Blob exists
    }

    FILE* file = fopen(blob->filename, "rb");
    if (!file) {
        perror("Failed to open source file");
        return;
    }

    FILE* blobFile = fopen(path, "wb");
    if (!blobFile) {
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
            if (entry->d_type == DT_REG && entry->d_name[0] != '.') {
                add(entry->d_name);  // Recursive call for files in subdirectories
            }
        }
        closedir(dir);
        return;
    }

    char hash[41];
    if (hashFile(path, hash) != 0) {
        return;
    }

    if (isFileInStagingArea(path, hash)) {
        printf("File '%s' is already staged.\n", path);
        return;
    }

    Blob* blob = createBlob(path);
    strcpy(blob->hash, hash);

    // Store the blob in .delta/objects
    storeBlob(blob);

    // Update the staging area to include or update the file
    updateStagingArea(path, hash);

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
}
