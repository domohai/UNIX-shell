#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 80 /* The maximum length command */
#define DELIMITER " \t\n"

char* read_input();
char** tokenizer(char* string, int* argc);

int main(void) {
    bool should_exit = false;
    char** argv = NULL;
    int argc = 0; // argument counter

    argv = tokenizer(read_input(), &argc);
    for (int i = 0; i < argc; i++) printf("%s\n", argv[i]);
    
    free(argv);
    // while (should_run) {
    //     /**
    //      * After reading user input, the steps are:
    //      * (1) fork a child process using fork()
    //      * (2) the child process will invoke execvp()
    //      * (3) if command included &, parent will invoke wait()
    //      */
    // }
    return 0;
}

char* read_input() {
    char* input = NULL;
    size_t size = 0;
    printf("> ");
    fflush(stdout); // ensure the buffer of stdout is empty, make printf() execute immediately
    size = getline(&input, &size, stdin);
    if (size == -1 && size < MAX_LINE) {
        printf("Command is too long or Error when reading input.");
        return NULL;
    }
    return input;
}

char** tokenizer(char* string, int* argc) {
    char** res = (char**)malloc(sizeof(char*) * 1000); // 1000 arguments max 
    char* token = strtok(string, DELIMITER);
    while (token) {
        res[(*argc)++] = token;
        token = strtok(NULL, DELIMITER);
    }
    return res;
}