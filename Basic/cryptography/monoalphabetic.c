#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *cypher(char *text, int mode){
    int len = strlen(text);
    char *result = malloc(len + 1);
    for (int i = 0; i < len; i++){
        char c = text[i];
        if (c >= 'a' && c <= 'z'){
            c = ((c - 'a' + mode) % 26) + 'a';
        } else if (c >= 'A' && c <= 'Z'){
            c = ((c - 'A' + mode) % 26) + 'A';
        }
        result[i] = c;
    }
    result[len] = '\0';
    return result;
}

int main (int argc, char *argv[]){
    if (argc != 3){
        printf("Usage: %s <text> <mode>\n", argv[0]);
        printf("Mode: positive for encryption, negative for decryption\n");
        return -1;
    }

    char *text = argv[1];
    int mode = atoi(argv[2]);

    if (mode < 0) {
        mode = 26 + (mode % 26);
    } else {
        mode = mode % 26;
    }

    char *result = cypher(text, mode);
    printf("%s\n", result);
    free(result);

    return 0;
}