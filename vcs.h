#ifndef VCS_H
#define VCS_H

#include <time.h>

// Blob represents file contents in the VCS
typedef struct Blob {
    char hash[41];            // SHA-1 hash of the file content
    char filename[100];        // Filename
    struct Blob* next;         // Pointer to the next blob
} Blob;

// Tree represents a directory structure in the VCS
typedef struct Tree {
    char hash[41];             // SHA-1 hash of the tree
    Blob* blobs;               // List of blobs (files) in this directory
    struct Tree* subtrees;     // List of subtrees (directories)
} Tree;

// Commit represents a snapshot of the file system in the VCS
typedef struct Commit {
    char hash[41];             // SHA-1 hash of the commit
    char message[256];         // Commit message
    struct tm timestamp;       // Commit timestamp
    Tree* tree;                // Pointer to the tree snapshot
    struct Commit* parent;     // Pointer to the parent commit
    struct Commit* secondParent;  // Pointer to second parent (for merges)
} Commit;

// Function declarations for SHA-1 hashing and object management
Tree* createTree(Blob* blobs);
Commit* createCommit(const char* message, Tree* tree, Commit* parent);
void storeBlob(Blob* blob);
void storeTree(Tree* tree);
void storeCommit(Commit* commit);
Blob* createBlob(const char* filename);


#endif
