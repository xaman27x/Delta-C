#ifndef VCS_H
#define VCS_H

#include <time.h>

typedef char Hash[41];  

typedef struct Blob {
    Hash hash;  // SHA-1 hash of the file content
    char filename[100]; // Filename
    char* fileContents; //File Contents
    struct Blob* next; // Pointer to the next blob (for linked list)
} Blob;

typedef struct Tree {
    Hash hash;   // SHA-1 hash of the directory structure
    Blob* blobs; // List of blobs (files) in this directory
    struct Tree* subtrees; // List of subtrees (subdirectories)
    struct Tree* next; // Pointer to next directory in the list
} Tree;

typedef struct Commit {
    Hash hash; // Hash of the commit
    char message[256]; // Commit message
    struct tm timestamp; // Commit timestamp
    Tree* tree; // Pointer to the root tree snapshot
    struct Commit* parent; // Pointer to the parent commit
    struct Commit* secondParent; // Pointer to second parent (for merges)
} Commit;

Tree* createTree(Blob* blobs);                     // Initialize a new tree 
Commit* createCommit(const char* message, Tree* tree, Commit* parent);  // Create a new commit
Blob* createBlob(const char* filename);            // Create a blob 

// Function declarations for storing and hashing
void storeBlob(Blob* blob);                        // Store blob in .git
void storeTree(Tree* tree);                        // Store tree in .git
void storeCommit(Commit* commit);                  // Store commit in .git

void freeBlob(Blob* blob);
void freeTree(Tree* tree);
void freeCommit(Commit* commit); 
#endif
