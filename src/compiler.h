#ifndef ATJEH_COMPILER_H
#define ATJEH_COMPILER_H

#include <stdint.h>
#include "chunk.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool is_error;
    bool panic_mode; // when set to true, it stop code compilation completely
} Parser;

typedef struct {
    Token name;
    int depth; // number variables with the level of nesting where they appear. Zero is the global scope, one is the first top-level block, two is inside that, and so on
} Local;

// TODO: change to LocalTracker
typedef struct {
    Local locals[UINT8_MAX]; // TODO: Do we need UINT8_MAX + 1? wtf is this book
    int local_count;
    int scope_depth; // scope depth currently in
} Compiler; // keep track of local var and its scope 


typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} Precedence; // HACK: never change order of this enum, its order determine its precedence

typedef void (*ParseFn)(bool can_assign); // ParseFn type is a simple typedef for a function type that takes no arguments and returns nothing.

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;

#define MAX_LOCAL_VAR 256

/* One-pass compiler*/
bool compile(const char *src, Chunk *chunk);
ParseRule *get_rule(TokenType type);

#endif // ATJEH_COMPILER_H
