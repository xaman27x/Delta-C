#include "vcs.h"


void calculateSHA(const char* input, char output[41]) {
    uint64_t hash1 = 0x123456789ABCDEF;
    uint64_t hash2 = 0xFEDCBA987654321; 
    for (size_t i = 0; i < strlen(input); i++) {
        hash1 ^= (hash1 << 5) + (hash1 >> 2) + (input[i]);
        hash2 ^= (hash2 << 5) + (hash2 >> 2) + (input[i]);
    }

    snprintf(output, 41, "%010lx%010lx", hash1 & 0xFFFFFFFFFF, hash2 & 0xFFFFFFFFFF); 

    output[40] = '\0';
}

