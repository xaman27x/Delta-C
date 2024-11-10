#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include "vcs.h"

void initTree(Tree* tree) {
    tree = malloc(sizeof(Tree));
    tree->blobs = NULL;
    tree->subtrees = NULL;
    tree->next = NULL;
}

Tree* createCommitTree(char* dirpath) {     //creates tree representing current directory structure using blobs identified by hashes
    DIR* dir = opendir(dirpath);
    if (!dir) {
        perror("Failed to open directory");
        return NULL;
    }

    Tree* tree;
    initTree(&tree);

    struct dirent* entry;
    struct stat file_stat;
    char curr_path[512];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        snprintf(curr_path, sizeof(curr_path), "%s/%s", dirpath, entry->d_name);
        stat(curr_path, &file_stat);


        if(S_ISDIR(file_stat.st_mode)) {
            Tree* subtree = createCommitTree(curr_path);
            subtree->next = tree->subtrees;
            tree->subtrees = subtree;
        }
        else if (S_ISREG(file_stat.st_mode)) {
            Blob* newBlob = createBlob(filename);
            hashFile(filename, newBlob->hash);
            new_blob->next = tree->blobs;
            tree->blobs = new_blob;
        }
    }

    hashTree(&tree, tree->hash);
    close(dir);
    return tree;

}

void createCommit(Hash commitHash, char commitMessage[], Tree* tree, time_t timestamp) { //creates and initialises commit object
    Commit* commit = (*commit)malloc(sizeof(Commit));
    strcpy(commit->hash, commitHash);
    strcpy(commit->message, commitMessage);
    commit->tree = &tree;
    commit->timestamp = timestamp;
    commit->parent = NULL;
    commit->secondParent = NULL;
}


typedef Commit* commitList;

void initCommitList(commitList* commits) {
    *commits = NULL;
}

void appendCommitList(commitList* commits, Commit* commit) { //commit list points to newest commit
    if(*commits == NULL) {
        *commits = commit;
        commit->parent = NULL;
        commit->secondParent = NULL;
        return;
    }
    commit->parent = *commits;
    *commits = commit;
}


void storeCommit(Commit* commit) {      //to store commit in objects/commits folder
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/commits/%s", commit->hash);

    if (access(path, F_OK) == 0) {
        return; 
    }

    FILE* commitFile = fopen(path, "wb");
    if (!commitFile) {
        fclose(file);
        return;
    }

    fwrite(commit->hash, 1,strlen(commit->hash) , commitFile);
    fwrite(commit->message, 1, strlen(commit->message), commitFile);
    fwrite(commit->tree->hash, 1, strlen(commit->tree->hash), commitFile);
    fwrite(commit->timestamp, 1, strlen(commit->timestamp), commitFile);

    if(commit->parent) {
        fwrite(commit->parent->hash, 1, strlen(commit->parent->hash), commitFile);
    }
    if(commit->secondParent) {
        fwrite(commit->secondParent->hash, 1, strlen(commit->secondParent->hash), commitFile);
    }

    fclose(commitFile);

}

storeCommitTreeFile(Tree* tree) {       //to store tree in objects/tree folder
   
    char path[60];
    snprintf(path, sizeof(path), ".delta/objects/trees/%s", tree->hash);

    if (access(path, F_OK) == 0) {
        return; 
    }

    FILE* treeFile = fopen(path, "wb");
    if (!treeFile) {
        perror("Failed to open file to store tree");
        return;
    }

    Blob* curr_blob = tree->blobs;
    while(curr_blob) {
        fwrite(curr_blob->filename, 1,strlen(curr_blob->filename) , treeFile);
        fwrite(curr_blob->hash, 1,strlen(curr_blob->hash) , treeFile);
    }

    Tree* curr_subtree = tree->subtrees;
    while(curr_subtree) {
        storeCommitTreeFile(curr_subtree);
        fwrite(curr_subtree->hash, 1,strlen(curr_subtree->hash) , treeFile);
    }

    fclose(treeFile);
}


void commit() {
    Hash commitHash;
    char commitMessage[];
    time_t timestamp = time(NULL);

    Tree* tree;
    initTree(&tree)
    tree = createCommitTree(".");

    Commit* commit = createCommit(Hash commitHash, char commitMessage[], &tree, timestamp);

    commitList* commitList;
    initCommitList(&commitList);
    appendCommitList(commit);

    storeCommit(&commit);
    
    storeCommitTreeFile(&tree); 
}