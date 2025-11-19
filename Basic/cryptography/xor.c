#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *xor_encrypt(const char *input, const char *key) {
    size_t input_len = strlen(input);
    size_t key_len = strlen(key);
    char *output = (char *)malloc(input_len + 1);
    if (!output) {
        return NULL; // Memory allocation failed
    }

    for (size_t i = 0; i < input_len; i++) {
        output[i] = input[i] ^ key[i % key_len];
    }
    output[input_len] = '\0'; // Null-terminate the output string

    return output;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: %s <input> <key> \n", argv[0]);
        return 1;
    }
    
    const char *input = argv[1];
    const char *key = argv[2];

    char *encrypted = xor_encrypt(input, key);
    
    printf("Encrypted: ");
    for(size_t i = 0; i < strlen(input); i++){
        printf("%02x", (unsigned char)encrypted[i]);
    }

    return 0;
}