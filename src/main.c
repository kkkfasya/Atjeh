#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "vm.h"


static void repl() {
    char line[1024];
    while (1) {
        printf("ATJ >> ");

        // if no input then exit
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\nEXIT\n");
            break;
        }

        interpret(line);
    }
}

static char *read_file(const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        fprintf(stderr, "[ERROR]: Could not open file \"%s\"\n", file_path);
        exit(69);
    }
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *buffer = (char *) malloc(sizeof(file_size + 1)); // +1 for null byte
    if (buffer == NULL) {
        fprintf(stderr, "[ERROR]: Could not allocate memory\n");
        exit(69);
    }
    size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
    if (bytes_read < file_size) {
        fprintf(stderr, "[ERROR]: Could not read file \"%s\"\n", file_path);
        exit(69);
    }
    buffer[bytes_read] = '\0'; // my belove null-byte
   
    fclose(file);
    return buffer; 
}

static void run_file(const char *file_path) {
   char *source = read_file(file_path);
   InterpretResult result = interpret(source);
   free(source);

   if (result == INTERPRET_COMPILE_ERROR) exit(69);
   if (result == INTERPRET_RUNTIME_ERROR) exit(69);
}


int main(int argc, char *argv[]) {
    Chunk *chunk = (Chunk *)  malloc(sizeof(Chunk));
    uint8_t constant;
    init_chunk(chunk);
    init_VM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        run_file(argv[1]);
    } else {
        fprintf(stderr,  "[ERROR]: Usage: atjeh [file.atj]\n");
        exit(69);
   }

    disassemble_chunk(chunk, "TEST CHUNk");
    interpret_VM(chunk);


    return 0;
}

