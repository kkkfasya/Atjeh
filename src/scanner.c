#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "common.h"
#include "scanner.h"

Scanner scanner;

Token token;

bool is_eof() {
    return *scanner.current == '\0';
}

char next_char() { // advance; NOTE:: delete later
    scanner.current++;
    return scanner.current[-1]; // WARN: -1????
}

char peek() {
    return *scanner.current;
}

char peek_next() {
    if (is_eof()) return '\0';
    return scanner.current[1];
}

bool match(char expected) {
    /* Check for character
     * e.g when we encounter !, we are not sure if it's just ! or !=
     * so we check it */
    if (is_eof()) return false;
    if (*scanner.current == expected) {
        scanner.current++; // consume the next char
        return true;
    }
    return false;

}

TokenType check_keyword(int start, int length, const char *rest, TokenType type) {
    /* Once we’ve found a prefix that could only be one possible reserved word
     * we need to verify two things. The lexeme must be exactly as long as the keyword.
     * If the first letter is “s”, the lexeme could still be “sup” or “superb”.
     * And the remaining characters must match exactly—“supar” isn’t good enough. */
    if (scanner.current - scanner.start == start + length && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }

    return TOKEN_IDENTIFIER;
}

TokenType identifier_type() {
    // Trie also called digital tree or prefix tree
    // Tries are a special case of an even more fundamental data structure:
    //      a deterministic finite automaton (DFA).
    // we'll use that data structure

    switch (scanner.start[0]) {
        // TODO: use keyword from my language
        case 'a': return check_keyword(1, 2, "nd", TOKEN_AND);
        case 'e': return check_keyword(1, 3, "lse", TOKEN_ELSE);
        case 'f':
                  if (scanner.current - scanner.start > 1) {
                      switch (scanner.start[1]) {
                          case 'a': return check_keyword(2, 3, "lse", TOKEN_FALSE);
                          case 'o': return check_keyword(2, 1, "r", TOKEN_FOR);
                          case 'u': return check_keyword(2, 1, "n", TOKEN_FUNC);
                      }
                  }
                  break;
        case 'i': return check_keyword(1, 1, "f", TOKEN_IF);
        case 'n': return check_keyword(1, 2, "il", TOKEN_NIL);
        case 'o': return check_keyword(1, 1, "r", TOKEN_OR);
        case 'p': return check_keyword(1, 4, "rint", TOKEN_PRINT);
        case 'r': return check_keyword(1, 5, "eturn", TOKEN_RETURN);
        case 'v': return check_keyword(1, 2, "ar", TOKEN_VAR);
        case 'w': return check_keyword(1, 4, "hile", TOKEN_WHILE);
    }

    return TOKEN_IDENTIFIER;
}

/* Constructor-like function to make token*/
Token make_token(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (uint32_t) (scanner.current - scanner.start);
    token.line = scanner.line;

    return token;
}

Token error_token(const char *msg) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = msg;
    token.length = (uint32_t) strlen(msg);
    token.line = scanner.line;

    return token;
}

void skip_whitespace() {
    while (1) {
        char ch = peek();
        switch (ch) {
            case ' ':
            case '\r':
            case '\t':
                next_char();
                break;

            case '\n':
                scanner.line++;
                next_char();
                break;

            case '/': // Comments aren’t technically “whitespace”, but who cares
                if (peek_next() == '/') {
                    while (peek() != '\n' && !is_eof()) next_char(); // skip all
                } else {
                    break;
                }

            default:
                return;
        }
    }
}

Token parse_string_literal() {
    while (peek() != '"' && !is_eof()) {
        if (peek() != '\n') scanner.line++;
        next_char();
    }
    if (is_eof()) return error_token("[ERROR]: Unterminated string.");
    next_char(); // for the closing quote
    return make_token(TOKEN_STRING);
}

void init_scanner(const char *src) {
    scanner.start = scanner.current;
    scanner.current = src;
    scanner.line = 1;
}

Token scan_token() {
    skip_whitespace();
    scanner.start = scanner.current;
    if (is_eof()) return make_token(TOKEN_EOF);
    char ch = next_char();

    if (isalpha((int) ch) || ch == '_') {
        while (isalpha(peek()) || isdigit(peek())) next_char();
        return make_token(identifier_type());
    }

    if (isdigit(((int) ch))) {
        while (isdigit(peek())) next_char();

        // Look for a fractional part.
        if (peek() == '.' && isdigit(peek_next())) {
            // Consume the ".".
            next_char();
            while (isdigit(peek())) next_char();
        }

        return make_token(TOKEN_NUMBER);
    }
    // TODO: use :: for assignment, and keyword 'kon' for not operator
    switch (ch) {
        case '(': return make_token(TOKEN_LEFT_PAREN);
        case ')': return make_token(TOKEN_RIGHT_PAREN);
        case '{': return make_token(TOKEN_LEFT_BRACE);
        case '}': return make_token(TOKEN_RIGHT_BRACE);

        case ';': return make_token(TOKEN_SEMICOLON);
        case ',': return make_token(TOKEN_COMMA);
        case '.': return make_token(TOKEN_DOT);

        case '-': return make_token(TOKEN_MINUS);
        case '+': return make_token(TOKEN_PLUS);
        case '/': return make_token(TOKEN_SLASH);
        case '*': return make_token(TOKEN_STAR);

        case '!': return make_token(
                          match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG
                          );

        case '=': return make_token(
                          match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL
                          );
        case '<': return make_token(
                          match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS
                          );
        case '>': return make_token(
                          match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER
                          );

        case '"':
                  // consume characters until we reach the closing quote
                  while (peek() != '"' && !is_eof()) {
                      if (peek() == '\n') scanner.line++;
                      next_char();
                  }

                  if (is_eof()) return error_token("Unterminated string.");

                  // The closing quote.
                  next_char();
                  return make_token(TOKEN_STRING);
    }
    return error_token("Unexpected character");
}
