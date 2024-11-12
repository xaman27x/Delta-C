#include "vcs.h"

Tree* findCommitTree(int x, commitList* commitList) {
    Commit* curr_commit = *commitList;
    while(curr_commit && x != 0) {
        x--;
        curr_commit = curr_commit->parent;
    }
    return curr_commit->tree;
}

void deleteDirectoryContents(const char* dirPath) {
    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Failed to open directory");
        return;
    }
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        if (shouldExclude(entry->d_name)) continue;

        char path[256];
        snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);

        if (entry->d_type == DT_DIR) {
            deleteDirectoryContents(path);
            if (rmdir(path) != 0) {
                perror("Failed to remove directory");
            }
        }
        else if (entry->d_type == DT_REG) {
            if (remove(path) != 0) {
                perror("Failed to remove file");
            }
        }
    }
    closedir(dir);  
}



void traverseCommitTree(const char* dirPath, Tree* tree) {

    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Failed to open directory");
        return NULL;
    }

    const Blob* curr_blob = tree->blobs;
    while (curr_blob) {

        char path[256];
        snprintf(path, sizeof(path), "%s/%s", dirPath, curr_blob->filename);
        FILE* file = fopen(path, "w");
        if (!file) {
            perror("Failed to open file");
            return;
        }

        char filepath[256];
        snprintf(filepath, sizeof(filepath), ".delta/objects/blobs/%s", curr_blob->hash);
        FILE* blobFile = fopen(filepath, "w");
        if (!blobFile) {
            perror("Failed to open file");
            return;
        }

        fseek(blobFile, 0, SEEK_END);
        long fileSize = ftell(blobFile);
        fseek(blobFile, 0, SEEK_SET);

        char* buffer = (char *)malloc(fileSize);
        if (buffer == NULL) {
            printf("Error: Memory allocation failed.\n");
            fclose(file);
            fclose(blobFile);
            return;
        }

        fread(buffer, 1, fileSize, blobFile);
        fwrite(buffer, 1, fileSize, file);
        free(buffer); 

        curr_blob = curr_blob->parent;
    }

    const Tree* curr_subtree = tree->subtrees;
    while (curr_subtree) {
        traverseCommitTree(curr_subtree->path, curr_subtree);
        curr_subtree = curr_subtree->next;
    }

    closedir(dir);
}

void reset(int x, commitList* commitList) {
    deleteDirectoryContents(".");
    Tree* tree = findCommitTree(x, commitList);
    traverseCommitTree(".", tree);
}