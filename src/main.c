// TODO: change int to int32_t, and such when it all works
// TODO: add exit keyword exclusive in repl
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "vm.h"
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "compiler.h"



static char* read_file(const char *path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "[ERROR]: Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = (char *) malloc(file_size + 1);
    if (buffer == NULL) {
        fprintf(stderr, "[ERROR]: Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "[ERROR]: Could not read file \"%s\".\n", path);
        exit(74);
  }
    buffer[bytes_read] = '\0';

    fclose(file);
    return buffer;
}

static void run_file(const char *path) {
    char *src = read_file(path);
    InterpretResult result = interpret_VM(src);
    free(src); 

    if (result == INTERPRET_COMPILE_ERROR) exit(69);
    if (result == INTERPRET_RUNTIME_ERROR) exit(69);
}

static void repl() {
    char line[1024];
    for (;;) {
        printf("ATJ >> ");
        char *input = fgets(line, sizeof(line), stdin);
        if (strncmp(input, "exit", 4) == 0) {
            printf("\e[1;1H\e[2J");
            printf("[INFO]: Exiting repl.");
            exit(0);
        } if (!input) {
            printf("MEMEK\n");
            break;

        }
        interpret_VM(line);
    }
}


int main(int argc, char **argv) {
    init_VM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr, "Usage: atjeh [path]\n");
        exit(64);
    }

    free_VM();
    return 0;
}
