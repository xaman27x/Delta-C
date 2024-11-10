#ifndef VCS_H
#define VCS_H

#include <stdint.h>
#include <time.h>

typedef char Hash[41];  

typedef struct Blob {
    Hash hash;  // SHA-1 hash of the file content
    char filename[100]; // Filename
    struct Blob* next; // Pointer to the next blob (for linked list)
} Blob;

typedef struct Tree {
    Hash hash;   // SHA-1 hash of the directory structure
    char path[256]; // Path of the directory (used for subtree identification)
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

// Function declarations for storing and hashing
void initRepository();                            // Initialize the repository
void calculateSHA(const char* input, char output[41]); // Hash calculation
void add(const char* path);                      // Add file to staging area
void storeBlob(Blob* blob);                      // Store blob in .delta/objects
int hashFile(const char* filename, char hash[41]); // Get hash of file
void addToStagingArea(const char* filename, const char hash[41]);  // Add file to index
Blob* createBlob(const char* filename);  // Create a blob from file
void freeBlob(Blob* blob);  // Free memory used by a blob

// Commit functions and helpers
void saveToFile(const char* path, const char* content);
Tree* findOrCreateSubtree(Tree* root, const char* path);  // Find or create a subtree in the tree
void addBlobToTree(Tree* tree, const char* hash, const char* filename);  // Add blob to the tree
Tree* buildTreeFromIndex();  // Build tree based on the index file (staging area)
Commit* createCommit(Tree* rootTree, Commit* parentCommit, const char* message);  // Create commit object
void commit(const char* message, Commit* parentCommit);  // Commit changes

#endif
