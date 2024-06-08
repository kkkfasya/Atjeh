#include "common.h"
#include <stdint.h>

#ifndef ATJEH_SCANNER_H
#define ATJEH_SCANNER_H


typedef struct {
  const char *start; // point to the beginning of lexeme (fancy ahh name for word)
  const char *current; // point to current char that's scanned
  uint32_t line;
} Scanner;

typedef enum {
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUNC, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,

    TOKEN_ERROR, TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type; // type of token
  const char *start; //
  uint32_t length;
  uint32_t line;
} Token;

Token scan_token();
void init_scanner(const char *src);

/*  NOTE: Below should be static function, but i didn't make it static because
 *  i need to keep track of it */
Token parse_string_literal();
bool is_eof();
char next_char();
char peek();
char peek_next();
bool match(char expected);
TokenType check_keyword(int start, int length, const char *rest, TokenType type);
TokenType identifier_type();
Token make_token(TokenType type);
Token error_token(const char *msg);
void skip_whitespace();

#endif // ATJEH_SCANNER_H



