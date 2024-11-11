#ifndef VCS_H
#define VCS_H

#include <stdint.h>
#include <time.h>
#define SHA1_BLOCK_SIZE 20 

typedef char Hash[41];  

typedef struct Blob {
    Hash hash;  // SHA-1 hash of the file content
    char filename[100]; // Filename
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

// Function declarations for storing and hashing
void initRepository();                            // Initialize the repository

void calculateSHA1(const char* input, char output[41]);


void add(const char* path);                      // Add file to staging area
void storeBlob(Blob* blob);                      // Store blob in .delta/objects
int hashFile(const char* filename, char hash[41]); // Get hash of file
void addToStagingArea(const char* filename, const char hash[41]);  // Add file to index
Blob* createBlob(const char* filename);  // Create a blob from file
void freeBlob(Blob* blob);

void initTree(Tree** tree);
Tree* createCommitTree(char* dirpath);
void createCommit(Hash commitHash, char commitMessage[], Tree* tree, time_t timestamp);
typedef Commit* commitList;
void initCommitList(commitList* commits);
void appendCommitList(commitList* commits, Commit* commit);
void storeCommit(Commit* commit);
void storeCommitTreeFile(Tree* tree);
void commit(char commitMessage[256]);


#endif
