#ifndef VCS_H
#define VCS_H

#include <stdint.h>
#include <time.h>

#define COMMIT_MSG_SIZE 256
#define INDEX_FILE ".delta/index"
#define BLOB_DIR ".delta/objects/blobs"
#define COMMIT_DIR ".delta/objects/commits"
#define DELTA_DIR ".delta"
#define VS_EXT ".vscode"

#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

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
    Hash hash;                    // Hash of the commit
    char message[256];            // Commit message
    time_t timestamp;             // Commit timestamp as time_t
    Tree* tree;                   // Pointer to the root tree snapshot
    struct Commit* parent;        // Pointer to the parent commit
    struct Commit* secondParent;  // Pointer to second parent (for merges)
} Commit;

// Init Function
void initRepository();                            // Initialize the repository

// Add Functions
void calculateSHA(const char* input, char output[41]); // Hash calculation
void add(const char* path);                      // Add file to staging area
void storeBlob(Blob* blob);                      // Store blob in .delta/objects
int hashFile(const char* filename, char hash[41]); // Get hash of file
int isFileInStagingArea(const char* filename, const char hash[41]); // Check if the file exists already
void addToStagingArea(const char* filename, const char hash[41]);  // Add file to index
Blob* createBlob(const char* filename);  // Create a blob from file
void freeBlob(Blob* blob);  // Free memory used by a blob

// Functions for commit and tree management
void initTree(Tree** tree);  // Initialize tree
Tree* createCommitTree(const char* dirpath);  // Create commit tree from directory
Commit* createCommit(char* commitMessage, Tree* tree, time_t timestamp);  // Create a commit
typedef Commit* commitList;  // Commit list type
int shouldExclude(const char *filename);
int loadStagedFiles(char staged_files[][100], char hashes[][41], int *count);
Tree* createCommitTree(const char* dirPath, char staged_files[][100], int staged_count)
void initCommitList(commitList* commits);  // Initialize commit list
void appendCommitList(commitList* commits, Commit* commit);  // Append commit to list
void storeCommit(const Commit* commit);  // Store commit in .delta/objects/commits
void storeCommitTreeFile(const Tree* tree);  // Store tree in .delta/objects/trees
void commit(char commitMessage[COMMIT_MSG_SIZE], Commit* commitList);  // Commit function to create and store commit

//Status Functions
void calculateFileHash(const char *filename, char hash[41]);
void status();

#endif
