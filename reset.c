#include "vcs.h"

// Find the commit tree at a given position in the commit list
Tree* findCommitTree(int x, commitList* commitList) {
    Commit* curr_commit = *commitList;
    while (curr_commit && x != 0) {
        x--;
        curr_commit = curr_commit->parent;
    }
    return curr_commit ? curr_commit->tree : NULL; // Handle case if commit is not found
}

// Delete files that are not part of the commit tree
void deleteUntrackedFiles(const char* dirPath, Tree* tree) {
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

        // Check if the file/directory is part of the commit tree
        int found = 0;
        const Blob* curr_blob = tree->blobs;
        while (curr_blob) {
            if (strcmp(curr_blob->filename, entry->d_name) == 0) {
                found = 1;
                break;
            }
            curr_blob = curr_blob->next;
        }

        if (!found) {
            // File is not in the commit tree, so delete it
            if (entry->d_type == DT_DIR) {
                deleteUntrackedFiles(path, tree);  // Recurse for directories
                if (rmdir(path) != 0) {
                    perror("Failed to remove directory");
                }
            } else if (entry->d_type == DT_REG) {
                if (remove(path) != 0) {
                    perror("Failed to remove file");
                }
            }
        }
    }

    closedir(dir);
}

// Traverse the commit tree and restore the files to the working directory
void restoreFilesFromCommit(const char* dirPath, Tree* tree) {
    // Create directories and restore blobs (files) from the commit
    const Blob* curr_blob = tree->blobs;
    while (curr_blob) {
        char path[256];
        snprintf(path, sizeof(path), "%s/%s", dirPath, curr_blob->filename);

        // Create the file if it does not exist
        FILE* file = fopen(path, "w");
        if (!file) {
            perror("Failed to create file");
            return;
        }

        // Read the file content from the blob storage and write it to the file
        char filepath[256];
        snprintf(filepath, sizeof(filepath), ".delta/objects/blobs/%s", curr_blob->hash);
        FILE* blobFile = fopen(filepath, "r");
        if (!blobFile) {
            perror("Failed to open blob file");
            return;
        }

        fseek(blobFile, 0, SEEK_END);
        long fileSize = ftell(blobFile);
        fseek(blobFile, 0, SEEK_SET);

        char* buffer = (char*)malloc(fileSize);
        if (buffer == NULL) {
            printf("Error: Memory allocation failed.\n");
            fclose(file);
            fclose(blobFile);
            return;
        }

        fread(buffer, 1, fileSize, blobFile);
        fwrite(buffer, 1, fileSize, file);
        free(buffer);

        fclose(blobFile);
        fclose(file);

        curr_blob = curr_blob->next; // Move to the next blob
    }

    // Traverse and restore subtrees (subdirectories)
    const Tree* curr_subtree = tree->subtrees;
    while (curr_subtree) {
        char subtreePath[256];
        snprintf(subtreePath, sizeof(subtreePath), "%s/%s", dirPath, curr_subtree->path);
        if (mkdir(subtreePath, 0755) != 0) {
            perror("Failed to create directory");
            return;
        }

        restoreFilesFromCommit(subtreePath, curr_subtree);
        curr_subtree = curr_subtree->next;
    }
}

// Reset function to restore the working directory to the state of a given commit
void reset(int x, commitList* commitList) {
    Tree* tree = findCommitTree(x, commitList);
    if (tree == NULL) {
        printf("Error: Commit not found.\n");
        return;
    }

    // Step 1: Remove untracked files
    deleteUntrackedFiles(".", tree);

    // Step 2: Restore files from the commit tree
    restoreFilesFromCommit(".", tree);
}
