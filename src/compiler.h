#ifndef ATJEH_COMPILER_H
#define ATJEH_COMPILER_H

#include "chunk.h"
#include "scanner.h"

typedef struct {
    Token current;
    Token previous;
    bool is_error;
    bool panic_mode; // when set to true, it stop code compilation completely
} Parser;

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

typedef void (*ParseFn)(); // ParseFn type is a simple typedef for a function type that takes no arguments and returns nothing.

typedef struct {
    ParseFn prefix;
    ParseFn infix;
    Precedence precedence;
} ParseRule;


/* One-pass compiler*/
// bool compile(const char *src, Chunk *chunk);
bool compile(const char *src, Chunk *chunk);

ParseRule *get_rule(TokenType type);

#endif // ATJEH_COMPILER_H
