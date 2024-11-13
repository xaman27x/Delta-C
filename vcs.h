#ifndef VCS_H
#define VCS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <stdint.h> 
#include <windows.h>

#define COMMIT_MSG_SIZE 256
#define SHA_BLOCK_SIZE 20 
#define FILENAME_SIZE 100
#define PATH_SIZE 256

#define INDEX_FILE ".delta/index"
#define BLOB_DIR ".delta/objects/blobs"
#define COMMIT_DIR ".delta/objects/commits"
#define DELTA_DIR ".delta"
#define VS_EXT ".vscode"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

typedef char Hash[40];  

// Structure definitions
typedef struct Blob {
    Hash hash;                   // SHA-1 hash of the file content
    char filename[FILENAME_SIZE];          // Filename
    struct Blob* next;           // Pointer to the next blob (for linked list)
} Blob;

typedef struct Tree {
    Hash hash;                   // SHA-1 hash of the directory structure
    char path[PATH_SIZE];              // Path of the directory (used for subtree identification)
    Blob* blobs;                 // List of blobs (files) in this directory
    struct Tree* subtrees;       // List of subtrees (subdirectories)
    struct Tree* next;           // Pointer to next directory in the list
} Tree;

typedef struct Commit {
    Hash hash;                   // Hash of the commit
    char message[COMMIT_MSG_SIZE];           // Commit message
    time_t timestamp;            // Commit timestamp as time_t
    Tree* tree;                  // Pointer to the root tree snapshot
    struct Commit* parent;       // Pointer to the parent commit
} Commit;

// Function declarations
// Init function
void initRepository(); 

// Add functions
void calculateSHA(const char* input, char output[41]); // Hash calculation
void add(const char* path); 
int hashFile(const char* filename, char hash[41]); 
int isFileInStagingArea(const char* filename, const char hash[41]); 
void updateStagingArea(const char* filename, const char hash[41]); 
Blob* createBlob(const char* filename);  
void storeBlob(Blob* blob);  
void freeBlob(Blob* blob);  

// Commit and tree management functions
void initTree(Tree** tree);  
Tree* createCommitTree(const char* dirpath);  
Commit* createCommit(char* commitMessage, Tree* tree, time_t timestamp);  
typedef Commit* commitList;  
int shouldExclude(const char *filename);
int loadStagedFiles(char staged_files[][100], char hashes[][41], int *count);
Tree* createCommitTreeRecursive(const char* dirPath, char staged_files[][100], int staged_count);
void initCommitList(commitList* commits);  
void appendCommitList(commitList* commits, Commit* commit);  
void storeCommit(const Commit* commit);  
void storeCommitTreeFile(const Tree* tree);  
void commit(char commitMessage[COMMIT_MSG_SIZE], Commit* commitList);  

// Status functions
void calculateFileHash(const char *filename, char hash[41]);
void status();

// Log metrics
void commitLog(commitList* commitList);

// Reset Function
void reset(int x, commitList* commitList);

#endif
