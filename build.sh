#!/bin/sh
gcc -Wall -Wextra -std=c11 -g -O0 -o main src/main.c src/chunk.c src/memory.c src/debug.c src/value.c src/vm.c src/scanner.c src/compiler.c
#gcc -Wall -Wextra -std=c11 -c src/*.c
#gcc -Wall -Wextra -std=c11 -o main main.o chunk.o memory.o debug.o value.o vm.o scanner.o compiler.o
#rm *.o
#gcc -Wall -Wextra -std=c11 -o man full_main.c src/chunk.c src/memory.c src/debug.c src/value.c src/vm.c
