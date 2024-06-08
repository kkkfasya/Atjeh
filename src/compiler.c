#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "common.h"
#include "compiler.h"


Parser parser;
Chunk *compile_chunk;

ParseRule rules[] = {   /* prefix   |   infix     |   precedence */
    [TOKEN_LEFT_PAREN]    = {paren_grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {parse_unary,    parse_binary_op, PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     parse_binary_op, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_SLASH]         = {NULL,     parse_binary_op, PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,     parse_binary_op, PREC_FACTOR},
    [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
    [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {parse_constant,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FUNC]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

Chunk *current_chunk() {
    return compile_chunk;
}

void emit_byte(uint8_t byte) {
    append_chunk(current_chunk(), byte, parser.previous.line);
}

void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

ParseRule *get_rule(TokenType type) {
    return &rules[type];
}

void parse_precedence(Precedence precedence) {
    /* We read the next token and look up the corresponding ParseRule. If there
     * is no prefix parser, then the token must be a syntax error, report that
     * and return to the caller.
     * The prefix expression is done. now we look for an infix parser for the
     * next token. If we find one, it means the prefix expression we already
     * compiled might be an operand for it. But only if the call to
     * parse_precedence() has a precedence that is low enough to permit that infix operator.
     */
    parse_token();
    ParseFn prefix_rule = get_rule(parser.previous.type)->prefix;
    if (prefix_rule == NULL) {
        error_at("Expected expression", false);
        return;
    }
    prefix_rule();
    while (precedence <= get_rule(parser.current.type)->precedence) {
        parse_token();
        ParseFn infix_rule = get_rule(parser.previous.type)->infix;
        infix_rule();
    }
}

void expression() {
    parse_precedence(PREC_ASSIGNMENT);
}

void emit_return() {
    emit_byte(OP_RETURN);
}

void end_compile() {
    emit_return();
#ifdef DEBUG_PRINT_CODE
#include "debug.h"
    //if (!parser.is_error) {
    if (1) {
        disassemble_chunk(current_chunk(), "CODE");
    }
#endif
}

void parse_token() { // NOTE: advance(); delete later
    parser.previous = parser.current;

    while (1) {
        parser.current = scan_token();
        if (parser.current.type != TOKEN_ERROR) break;

        error_at(parser.current.start, true);
    }
}

void error(Token *token, const char *msg) {
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        //
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.is_error = true;
}

void error_at(const char *msg, bool is_current) {
    if(parser.panic_mode) return;
    parser.panic_mode = true;
    is_current ? error(&parser.current, msg) : error(&parser.previous, msg);
}

void consume_token(TokenType type, const char *msg) {
    if (parser.current.type == type) {
        parse_token();
        return;
    }

    error_at(msg, true);
}

uint8_t make_constant(Value value) {
    int constant = add_constant(current_chunk(), value);
    if (constant > UINT8_MAX) {
        error_at("Too many constants in one chunk", false);
        return 0;
    }

    return (uint8_t) constant;
}

// void emit_constant(Value constant) {
//     emit_bytes(OP_CONSTANT, make_constant(constant));
// }

void parse_constant() {
    Value value = strtod(parser.previous.start, NULL);
    // emit_constant(value)
    emit_bytes(OP_CONSTANT, make_constant(value));
}

void paren_grouping() {
    /* We assume the initial ( has already been consumed.
     * We recursively call back into expression() to compile the expression
     * between the parentheses, then parse the closing ) at the end.*/
    expression();
    // consume_token(TOKEN_RIGHT_PAREN, "Expected ')' after expression"); WARN: fuck this for now
}

void parse_unary() {
    TokenType op_type = parser.previous.type;

    // Compile the operand.
    parse_precedence(PREC_UNARY);

    // Emit the operator instruction.
    switch (op_type) {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        default: return; // Unreachable.
    }
}

void parse_binary_op() {
    TokenType op_type = parser.previous.type;
    ParseRule *rule = get_rule(op_type);
    parse_precedence((Precedence)(rule->precedence + 1));

    switch (op_type) {
        case TOKEN_PLUS:    emit_byte(OP_ADD);       break;
        case TOKEN_MINUS:   emit_byte(OP_SUBTRACT);  break;
        case TOKEN_STAR:    emit_byte(OP_MULTIPLY);  break;
        case TOKEN_SLASH:   emit_byte(OP_DIVIDE);    break;

        default:
            return; // Cant handle what we dont have
    }
}

bool compile(const char *src, Chunk *chunk) {
    parser.is_error = false;
    parser.panic_mode = false;
    compile_chunk = chunk;

    init_scanner(src);
    expression();
    consume_token(TOKEN_EOF, "Expected end of expression");
    end_compile();
    return !parser.is_error;

#if 0
    uint32_t line = 0;
    for (;;) {
        Token token = scan_token();
        if (token.line != line) {
            line = token.line;
            printf("%d", line);
        } else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);
        if (token.type == TOKEN_EOF) break;
    }
#endif

    return true;
}


