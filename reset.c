#include "vcs.h"

Tree* findCommitTree(int x, commitList* commitList) {
    Commit* curr_commit = *commitList;
    while (curr_commit && x != 0) {
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
        if (shouldExclude(entry->d_name)) continue;  // Assumes shouldExclude is implemented elsewhere

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


void traverseCommitTree(const char* dirPath, const Tree* tree) {
    if (!tree) return;  // Ensure the tree is valid before traversing

     if (mkdir(dirPath) == -1) {
        perror("Failed to create directory");
        return;
    }

    DIR* dir = opendir(dirPath);

    if (!dir) {
        perror("Failed to open directory");
        return;
    }

    const Blob* curr_blob = tree->blobs;
    while (curr_blob) {
        char path[256];
        snprintf(path, sizeof(path), "%s",  curr_blob->filename);

        FILE* file = fopen(path, "w");
        if (!file) {
            perror("Failed to open file for writing");
            closedir(dir);
            return;
        }

        char blobPath[256];
        snprintf(blobPath, sizeof(blobPath), ".delta/objects/blobs/%s", curr_blob->hash);

        FILE* blobFile = fopen(blobPath, "r");
        if (!blobFile) {
            perror("Failed to open blob file");
            fclose(file);
            closedir(dir);
            return;
        }

        fseek(blobFile, 0, SEEK_END);
        long fileSize = ftell(blobFile);
        fseek(blobFile, 0, SEEK_SET);

        char* buffer = (char*)malloc(fileSize);
        if (buffer == NULL) {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            fclose(file);
            fclose(blobFile);
            closedir(dir);
            return;
        }

        fread(buffer, 1, fileSize, blobFile);
        fwrite(buffer, 1, fileSize, file);
        free(buffer);
        fclose(file);
        fclose(blobFile);

        curr_blob = curr_blob->next;
    }

    const Tree* curr_subtree = tree->subtrees;
    while (curr_subtree) {
        traverseCommitTree(curr_subtree->path, curr_subtree);
        curr_subtree = curr_subtree->next;
    }

    closedir(dir);
}


void reset(int x, commitList* commitList) {
    if(!commitList) {
        return;
    }
    deleteDirectoryContents(".");
    Tree* tree = findCommitTree(x, commitList);
    Tree* p = tree->subtrees;
    while (p) {
        traverseCommitTree( p->subtrees->path, tree);
    }
  
}
