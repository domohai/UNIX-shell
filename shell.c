#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */
#define DELIMITER " \t\n"
#define MAX_HISTORY 10

bool should_exit = false;
bool run_concurrently = false;
int history_pointer = 0;
int history_argc[MAX_HISTORY];

char* read_input();
char** tokenizer_for_argv(char* string, int* argc);
void execute(int* argc, char** argv);
bool check_for_history_call(int* argc, char*** argv, char*** history);
void save_command(int* argc, char** argv, char*** history);
char** copyCharDoublePointer(char** source, int size);

int main(void) {
    char** argv = NULL;
    char* input = NULL;
    char*** history_argv = (char***)malloc(sizeof(char**) * MAX_HISTORY);
    int argc = 0; // argument counter

    while (!should_exit) {
        input = read_input();
        if (input != NULL) {
            argv = tokenizer_for_argv(input, &argc);
            // save_command(&argc, argv, history_argv);
            if (check_for_history_call(&argc, &argv, history_argv)) {
                execute(&argc, argv);
            }
            // reset command
            // free(argv);
            free(input);
            argc = 0;
        }
    }
    return 0;
}

void execute(int* argc, char** argv) {
    if (!strcmp(argv[0], "exit")) { // exit shell
        should_exit = true;
        return;
    }
    pid_t pid = fork();
    if (pid < 0) printf("Failed to fork.\n");
    else if (pid == 0) {
        if (execvp(argv[0], argv) == -1) {
            printf("Invalid command.\n");
            fflush(stdout);
            _Exit(3);
        }
    } else {
        if (run_concurrently) {
            wait(NULL);
            run_concurrently = false;
        }
    }
}

bool check_for_history_call(int* argc, char*** argv, char*** history) {
    if (!strcmp(*argv[0], "history")) {
        if (history_pointer == 0) printf("No command in history.\n");
        else {
            for (int i = 0; i < history_pointer; i++) {
                // int length = strlen(*history[i]);
                printf("%d ", i+1);
                for (int j = 0; history[i][j] != NULL; j++) 
                    printf("%s ", history[i][j]);
                // printf("%d\n", history_argc[i]);
                printf("\n");
            }
        }
    } else if (!strcmp((*argv)[0], "!!")) { // most recent command
        if (history_pointer == 0) printf("No command in history.\n");
        else {
            *argv = copyCharDoublePointer(history[history_pointer-1], history_argc[history_pointer-1]);
            return true;
        }   
    } else if ((*argv)[0][0] == '!') {
        if (history_pointer == 0) printf("No command in history.\n");
        else {
            char c = (*argv)[0][1];
            *argv = copyCharDoublePointer(history[(c - '0') - 1], history_argc[(c - '0') - 1]);
            history_argc[(c - '0') - 1] = *argc;
            return true;
        }
    } else {
        save_command(argc, *argv, history);
        return true;
    }
    return false;
}

void save_command(int* argc, char** argv, char*** history) {
    if (history_pointer >= MAX_HISTORY) { // history is full
        for (int i = 1; i < history_pointer; i++) { // shifting
            history[i-1] = history[i];
            history_argc[i-1] = history_argc[i];
        }
        history_pointer--;
    }
    history[history_pointer] = copyCharDoublePointer(argv, *argc);
    history_argc[history_pointer++] = *argc;
}

char* read_input() {
    char* input = NULL;
    size_t size = 0;
    printf("> ");
    fflush(stdout); // ensure the buffer of stdout is empty, make printf() execute immediately
    size = getline(&input, &size, stdin);
    if (size == -1 || size > MAX_LINE || !strcmp(input, "\n")) {
        if (size == -1) printf("Error when reading input.\n");
        else if (size > MAX_LINE) printf("Command is too long.\n");
        else printf("You must enter a command to proceed.\n");
        input = NULL;
    }
    return input;
}

char** tokenizer_for_argv(char* string, int* argc) {
    char** argv = (char**)malloc(sizeof(char*) * 100); // 100 arguments max 
    char* token = strtok(string, DELIMITER);
    while (token) {
        argv[(*argc)++] = token;
        token = strtok(NULL, DELIMITER);
    }
    if (!strcmp(argv[(*argc) - 1], "&")) {
        run_concurrently = true;
        argv[(*argc) - 1] = NULL; // replace "&" with NULL
        printf("Running concurrently.\n");
    } else {
        argv[(*argc)] = NULL; // the last element of argv must be NULL
    }
    // free(token);
    return argv;
}

char** copyCharDoublePointer(char** source, int size) {
    char** destination = (char**)malloc(size * sizeof(char*)); 
    if (destination == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    // Copy each string element
    for (int i = 0; i < size; i++) {
        int length = strlen(source[i]) + 1;  // Length of the current string, including the null terminator
        destination[i] = (char*)malloc(length * sizeof(char));
        if (destination[i] == NULL) {
            printf("Memory allocation failed.\n");
            for (int j = 0; j < i; j++) {
                free(destination[j]);
            }
            free(destination);
            return NULL;
        }
        strcpy(destination[i], source[i]);
    }
    return destination;
}