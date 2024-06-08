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
bool compile(const char *src, Chunk *chunk);
/*  NOTE: Below should be static function, but i didn't make it so because 
 *  i need to keep track of it */ 
void parse_token();
void error(Token *token, const char *msg);

/* if @param is_current is true, the current token is error,
 * if it's false then it's the previous token */
void error_at(const char *msg, bool is_current);

void consume_token(TokenType type, const char *msg);
void emit_byte(uint8_t byte);
void emit_bytes(uint8_t byte1, uint8_t byte2);

Chunk *current_chunk();
void expression();
void emit_return();
void end_compile();
uint8_t make_constant(Value value);
void emit_constant(Value constant);

void parse_constant();
void paren_grouping();
void parse_unary();
void parse_binary_op();
void expression();
ParseRule* get_rule(TokenType type);
void parse_precedence(Precedence precedence);

#endif // ATJEH_COMPILER_H
