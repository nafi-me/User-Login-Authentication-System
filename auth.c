#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>  // For SHA256 hashing
#include "auth.h"

// Simple password hashing using SHA-256
void hash_password(const char *password, char *outputBuffer) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)password, strlen(password), hash);
    
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[64] = 0;
}
