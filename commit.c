#include "vcs.h"

typedef struct DirStack {
    char path[512];  // Directory path
    struct DirStack* next;
} DirStack;

// Push a new directory path onto the stack
void pushDirStack(DirStack** stack, const char* path) {
    DirStack* newNode = (DirStack*)malloc(sizeof(DirStack));
    if (!newNode) {
        perror("Failed to allocate memory for DirStack");
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->path, path, sizeof(newNode->path) - 1);
    newNode->next = *stack;
    *stack = newNode;
}

// Pop a directory path from the stack
int popDirStack(DirStack** stack, char* path) {
    if (*stack == NULL) return 0;
    DirStack* temp = *stack;
    strncpy(path, temp->path, sizeof(temp->path) - 1);
    *stack = temp->next;
    free(temp);
    return 1;
}

// Initialize a Tree structure
void initTree(Tree** tree) {
    *tree = (Tree*)calloc(1, sizeof(Tree));  // Use calloc for zeroed memory
    if (*tree == NULL) {
        perror("Failed to allocate memory for Tree");
        exit(EXIT_FAILURE);
    }
}

void hashTree(Tree* tree, char hash[41]) {
    size_t combinedDataSize = 1024; 
    char* combinedData = (char*)malloc(combinedDataSize);
    if (combinedData == NULL) {
        perror("Failed to allocate memory for combined data");
        exit(EXIT_FAILURE);
    }
    combinedData[0] = '\0';

    Blob* blob = tree->blobs;
    Tree* subtree = tree->subtrees;

    while (blob) {
        strncat(combinedData, blob->filename, combinedDataSize - strlen(combinedData) - 1);
        strncat(combinedData, blob->hash, combinedDataSize - strlen(combinedData) - 1);
        blob = blob->next;
    }

    while (subtree) {
        strncat(combinedData, subtree->path, combinedDataSize - strlen(combinedData) - 1);
        strncat(combinedData, subtree->hash, combinedDataSize - strlen(combinedData) - 1);
        subtree = subtree->next;
    }

    calculateSHA(combinedData, hash);
    free(combinedData); 
}

// Helper function to check if a file or directory should be excluded
int shouldExclude(const char *filename) {
    // Exclude particular files
    if (strstr(filename, ".git") != NULL || strstr(filename, ".sample") != NULL || strstr(filename, DELTA_DIR) != NULL || strstr(filename, ".dist") || strstr(filename, VS_EXT)) {
        return 1;
    }
    return 0;
}

// Helper function to load staged files from index file
int loadStagedFiles(char staged_files[][100], char hashes[][41], int *count) {
    FILE* indexFile = fopen(".delta/index", "r");
    if (!indexFile) {
        perror("Failed to open index file");
        return -1;
    }

    *count = 0;
    while (fscanf(indexFile, "%40s %99s", hashes[*count], staged_files[*count]) != EOF) {
        (*count)++;
    }
    fclose(indexFile);
    return 0;
}

// Helper function to check if a filename is staged
int isFileStaged(const char* filename, char staged_files[][100], int count) {
    for (int i = 0; i < count; i++) {
        if (strcmp(filename, staged_files[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

Tree* createCommitTreeRecursive(const char* dirPath, char staged_files[][100], int staged_count) {
    Tree* tree = NULL;
    initTree(&tree);
    strncpy(tree->path, dirPath, sizeof(tree->path) - 1);  // Set tree path for the directory

    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Failed to open directory");
        return NULL;
    }

    struct dirent* entry;
    struct stat fileStat;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        if (shouldExclude(entry->d_name)) continue;

        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);
        if (stat(fullPath, &fileStat) == -1) {
            perror("Failed to get file status");
            continue;
        }

        if (S_ISDIR(fileStat.st_mode)) {
            // If it's a directory, recursively create a subtree
            Tree* subtree = createCommitTreeRecursive(fullPath, staged_files, staged_count);
            if (subtree) {
                hashTree(subtree, subtree->hash);  // Hash each subtree
                subtree->next = tree->subtrees;
                tree->subtrees = subtree;  // Link subtree to parent tree
            }
        } else if (S_ISREG(fileStat.st_mode)) {
            // If it's a file, process it as a blob if it is staged
            if (isFileStaged(entry->d_name, staged_files, staged_count)) {
                Blob* newBlob = createBlob(entry->d_name);
                if (!newBlob) {
                    perror("Failed to create Blob");
                    continue;
                }
                hashFile(fullPath, newBlob->hash);
                newBlob->next = tree->blobs;
                tree->blobs = newBlob; 
            }
        }
    }
    closedir(dir);


    hashTree(tree, tree->hash);
    return tree;
}

Tree* createCommitTree(const char* rootDir) {
    char staged_files[100][100];
    char staged_hashes[100][41];
    int staged_count = 0;

    if (loadStagedFiles(staged_files, staged_hashes, &staged_count) != 0) {
        printf("No staged files to commit.\n");
        return NULL;
    }

    return createCommitTreeRecursive(rootDir, staged_files, staged_count);
}


void hashCommit(Commit* commit, char hash[41]) {
    char combinedData[1024];
    snprintf(combinedData, sizeof(combinedData), "%s%s%ld%s",
             commit->message,
             commit->tree->hash,
             (long)commit->timestamp,
             commit->parent ? commit->parent->hash : "");

    calculateSHA(combinedData, hash);
}

Commit* createCommit(char* commitMessage, Tree* tree, time_t timestamp) {
    Commit* commit = (Commit*)calloc(1, sizeof(Commit)); 
    if (!commit) {
        perror("Failed to allocate memory for Commit");
        exit(EXIT_FAILURE);
    }

    strncpy(commit->message, commitMessage, sizeof(commit->message) - 1);
    commit->tree = tree;
    commit->timestamp = timestamp;
    commit->parent = NULL;
    hashCommit(commit, commit->hash);
    return commit;
}

void storeCommit(const Commit* commit) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/commits/%s", commit->hash);

    if (access(path, F_OK) == 0) {
        return;  // Commit exists
    }

    FILE* commitFile = fopen(path, "wb");
    if (!commitFile) {
        perror("Failed to open commit file");
        return;
    }

    fprintf(commitFile, "%s\n%s\n%s\n%ld\n",
            commit->hash,
            commit->message,
            commit->tree->hash,
            (long)commit->timestamp);

    if (commit->parent) {
        fprintf(commitFile, "%s\n", commit->parent->hash);
    }

    fclose(commitFile);
}

// format: <blob>: filename hash
void storeCommitTreeFile(const Tree* tree) {
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/trees/%s", tree->hash);

    if (access(path, F_OK) == 0) {
        return;
    }

    FILE* treeFile = fopen(path, "wb");
    if (!treeFile) {
        perror("Failed to open tree file");
        return;
    }

    const Blob* curr_blob = tree->blobs;
    while (curr_blob) {
        fprintf(treeFile, "<blob>: %s %s\n", curr_blob->filename, curr_blob->hash);
        curr_blob = curr_blob->next;
    }

    const Tree* curr_subtree = tree->subtrees;
    while (curr_subtree) {
        storeCommitTreeFile(curr_subtree); 
        fprintf(treeFile, "<tree>: %s %s\n", curr_subtree->path, curr_subtree->hash);
        curr_subtree = curr_subtree->next;
    }

    fclose(treeFile);
}

void initCommitList(commitList* commits) {
    *commits = NULL;
}

void appendCommitList(commitList* commits, Commit* commit) {
    if (*commits == NULL) {
        *commits = commit;
    } else {
        Commit* current = *commits;
        while (current->parent) {
            current = current->parent;
        }
        current->parent = commit;
    }
}

// Main commit function 
void commit(char commitMessage[COMMIT_MSG_SIZE], Commit* commitList) {

    time_t currentTime = time(NULL);
    Tree* tree = createCommitTree(".");

    Commit* newCommit = createCommit(commitMessage, tree, currentTime);

    storeCommit(newCommit);
    storeCommitTreeFile(tree);

    initCommitList(&commitList);
    appendCommitList(&commitList, newCommit);
    return;
}
