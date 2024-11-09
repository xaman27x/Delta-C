#ifndef BLOB_H
#define BLOB_H

#include "vcs.h"  // Ensure we have access to Blob definitions

// Function to stage a file for commit
Blob* stageFile(const char* filename);
void storeBlobObject(Blob* blob);

#endif
