#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "object.h"


// if each static function in this program could be converted to 1kg of meat, then it'd solve world hunger
static void consume_token(TokenType type, const char *msg);
static void parse_token();
static void parse_precedence(Precedence precedence);

static void emit_byte(uint8_t byte);
static void emit_bytes(uint8_t byte1, uint8_t byte2);
static void emit_return();
static uint8_t make_constant(Value value);
static void emit_constant(Value value);

static Chunk *current_chunk();

static void parse_constant(bool can_assign);
static void parse_string(bool can_assign);
static void parse_literal(bool can_assign);
static void parse_unary(bool can_assign);
static void parse_binary_op(bool can_assign);
static void paren_grouping(bool can_assign);

static void error(Token *token, const char *msg);
static void error_at(const char*, bool);

static bool check_type(TokenType type);
static bool match(TokenType type);
static void expression();
static void declaration();

static void statement();
static void print_statement();
static void expression_statement();

static uint8_t parse_var_token(const char *err_msg);
static void var_declaration();
static uint8_t identifier_constant(Token *name);
static void define_var(uint8_t global);
static void parse_var_identifier(bool can_assign);
static void named_var(Token name, bool can_assign);
static void end_compile();

Parser parser;
Chunk *compile_chunk;

static void error(Token *token, const char *msg) {
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

static void error_at(const char *msg, bool is_current) {
    /* if @param is_current is true, the current token is error,
     * if it's false then it's the previous token */
    if(parser.panic_mode) return;
    parser.panic_mode = true;
    is_current ? error(&parser.current, msg) : error(&parser.previous, msg);
}


static void parse_token() { // NOTE: advance(); delete later
    parser.previous = parser.current;

    while (1) {
        parser.current = scan_token();
        if (parser.current.type != TOKEN_ERROR) break;

        error_at(parser.current.start, true);
    }
}

static void parse_precedence(Precedence precedence) {
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
        error_at("[ERROR]: Expected expression", false);
        return;
    }
    bool can_assign = precedence <= PREC_ASSIGNMENT; // so if it doesn't have precedence such as identifier and such such it is assign-able

    prefix_rule(can_assign);
    while (precedence <= get_rule(parser.current.type)->precedence) {
        parse_token();
        ParseFn infix_rule = get_rule(parser.previous.type)->infix;
        infix_rule(can_assign);
    }
}

static Chunk *current_chunk() {
    return compile_chunk;
}

static void emit_byte(uint8_t byte) {
    append_chunk(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2) {
    emit_byte(byte1);
    emit_byte(byte2);
}

static void emit_return() {
    emit_byte(OP_RETURN);
}

static bool check_type(TokenType type) {
    return parser.current.type == type;
}

static bool match(TokenType type) {
    if (!check_type(type)) return false;
    parse_token();
    return true;
}

static void var_declaration() {
    // basically we shove OP_DEFINE_GLOBAL TOKEN
    uint8_t global = parse_var_token("Expect a variable name");

    if (match(TOKEN_EQUAL)) {
        expression();
    } else {
        emit_byte(OP_NIL);
    }
    consume_token(TOKEN_SEMICOLON, "Expect ; after variable declaration");

    define_var(global);
}

static void expression() {
    parse_precedence(PREC_ASSIGNMENT);
}

static void expression_statement() {
    expression();
    consume_token(TOKEN_SEMICOLON, "Expect ';' after expression.");
    emit_byte(OP_POP);
}

static void print_statement() {
    expression();
    consume_token(TOKEN_SEMICOLON, "Expect ';' after value.");
    emit_byte(OP_PRINT);
}

static void statement() {
    if (match(TOKEN_PRINT)) {
        print_statement();
    } else {
        expression_statement();
    }
}

static void synchronize_error() {
    parser.panic_mode = false;

    while (parser.current.type != TOKEN_EOF) {
        if (parser.previous.type == TOKEN_SEMICOLON) return;
        switch (parser.current.type) {
            case TOKEN_FUNC:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;

            default:
                // yeah

        }
        parse_token();
    }
}

static void declaration() {
    if (match(TOKEN_VAR)) {
        var_declaration();
    } else {
        statement();
    }
    if (parser.panic_mode) synchronize_error();
}

static void parse_unary(bool can_assign) {
    TokenType op_type = parser.previous.type;

    // Compile the operand.
    parse_precedence(PREC_UNARY);
    // Emit the operator instruction.
    switch (op_type) {
        case TOKEN_MINUS: emit_byte(OP_NEGATE); break;
        case TOKEN_BANG: emit_byte(OP_NOT); break;
        default: return;
    }
}

static void parse_binary_op(bool can_assign) {
    TokenType op_type = parser.previous.type;
    ParseRule *rule = get_rule(op_type);
    parse_precedence((Precedence)(rule->precedence + 1));

    switch (op_type) {
        case TOKEN_PLUS:    emit_byte(OP_ADD);       break;
        case TOKEN_MINUS:   emit_byte(OP_SUBTRACT);  break;
        case TOKEN_STAR:    emit_byte(OP_MULTIPLY);  break;
        case TOKEN_SLASH:   emit_byte(OP_DIVIDE);    break;

        case TOKEN_BANG_EQUAL:    emit_bytes(OP_EQUAL, OP_NOT);     break;
        case TOKEN_EQUAL_EQUAL:   emit_byte(OP_EQUAL);              break;

        case TOKEN_GREATER_EQUAL: emit_bytes(OP_LESS, OP_NOT);      break;
        case TOKEN_GREATER:       emit_byte(OP_GREATER);            break;

        case TOKEN_LESS:          emit_byte(OP_LESS);               break;
        case TOKEN_LESS_EQUAL:    emit_bytes(OP_GREATER, OP_NOT);   break;

        default:
                                  return; // Cant handle what we dont have
    }
}

static void consume_token(TokenType type, const char *msg) {
    // TODO: rename to check_token_then_parse
    if (parser.current.type == type) {
        parse_token();
        return;
    }

    error_at(msg, true);
}

static void paren_grouping(bool can_assign) {
    /* We assume the initial ( has already been consumed.
     * We recursively call back into expression() to compile the expression
     * between the parentheses, then parse the closing ) at the end.*/
    expression();
    consume_token(TOKEN_RIGHT_PAREN, "Expected ')' after expression");
}

static uint8_t make_constant(Value value) {
    int constant = add_constant(current_chunk(), value);
    if (constant > UINT8_MAX) {
        error_at("[ERROR]: Too many constants in one chunk", false);
        return 0;
    }

    return (uint8_t) constant;
}

static void emit_constant(Value constant) {
    emit_bytes(OP_CONSTANT, make_constant(constant));
}

static void parse_constant(bool can_assign) {
    double value = strtod(parser.previous.start, NULL);
    emit_constant(NUMBER_VAL(value)); // XXX: do we really need this?
                                      // emit_bytes(OP_CONSTANT, make_constant(value));
}

static void parse_literal(bool can_assign) {
    switch (parser.previous.type) {
        case TOKEN_FALSE: emit_byte(OP_FALSE); break;
        case TOKEN_NIL: emit_byte(OP_NIL); break;
        case TOKEN_TRUE: emit_byte(OP_TRUE); break;
        default: return;
    }
}

static void define_var(uint8_t global) {
    emit_bytes(OP_DEFINE_GLOBAL, global);
}

static uint8_t identifier_constant(Token *name) {
    /* This function takes the given token and adds its lexeme to the chunk’s constant table as a string. It then returns the index of that constant in the constant table.

       Global variables are looked up by name at runtime. That means the VM—the bytecode interpreter loop—needs access to the name. A whole string is too big to stuff into the bytecode stream as an operand. Instead, we store the string in the constant table and the instruction then refers to the name by its index in the table.*/
    
    // basically put the var name in the constant table and return its index so we can refer to that later
    return make_constant(OBJ_VAL(copy_string(name->start,
                    name->length)));
}

static uint8_t parse_var_token(const char *err_msg) {
    consume_token(TOKEN_IDENTIFIER, err_msg);
    return identifier_constant(&parser.previous); // why previous? because the current is semicolon
}

static void parse_string(bool can_assign) {
    /* The + 1 and - 2 parts trim the leading and trailing quotation marks.
     * It then creates a string object,
     * wraps it in a Value, and stuffs it into the constant table. */
    emit_constant(OBJ_VAL(copy_string(parser.previous.start + 1,
                    parser.previous.length - 2)));
}

static void named_var(Token name, bool can_assign) {
  uint8_t arg = identifier_constant(&name);
  /* In the parse function for identifier expressions, 
   * we look for an equals sign after the identifier. 
   * If we find one, instead of emitting code for a variable access, 
   * we compile the assigned value and then emit an assignment instruction. */

  if (can_assign && match(TOKEN_EQUAL)) {
      expression();
      emit_bytes(OP_SET_GLOBAL, arg);
  } else {
      emit_bytes(OP_GET_GLOBAL, arg);
  }

}

static void parse_var_identifier(bool can_assign) { // NOTE: variable, delete later
    named_var(parser.previous, can_assign);
}

ParseRule rules[] = {   /*  prefix           |   infix      |   precedence */
    [TOKEN_LEFT_PAREN]    = {paren_grouping, NULL,              PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,           NULL,              PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,           NULL,              PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,           NULL,              PREC_NONE},
    [TOKEN_COMMA]         = {NULL,           NULL,              PREC_NONE},
    [TOKEN_DOT]           = {NULL,           NULL,              PREC_NONE},
    [TOKEN_MINUS]         = {parse_unary,    parse_binary_op,   PREC_TERM},
    [TOKEN_PLUS]          = {NULL,           parse_binary_op,   PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,           NULL,              PREC_NONE},
    [TOKEN_SLASH]         = {NULL,           parse_binary_op,   PREC_FACTOR},
    [TOKEN_STAR]          = {NULL,           parse_binary_op,   PREC_FACTOR},
    [TOKEN_BANG]          = {parse_unary,    NULL,              PREC_NONE},
    [TOKEN_EQUAL]         = {NULL,           NULL,              PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,           parse_binary_op,   PREC_EQUALITY},
    [TOKEN_EQUAL_EQUAL]   = {NULL,           parse_binary_op,   PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,           parse_binary_op,   PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,           parse_binary_op,   PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,           parse_binary_op,   PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,           parse_binary_op,   PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {parse_var_identifier,           NULL,              PREC_NONE},
    [TOKEN_STRING]        = {parse_string,           NULL,              PREC_NONE},
    [TOKEN_NUMBER]        = {parse_constant, NULL,              PREC_NONE},
    [TOKEN_AND]           = {NULL,           NULL,              PREC_NONE},
    [TOKEN_ELSE]          = {NULL,           NULL,              PREC_NONE},
    [TOKEN_FALSE]         = {parse_literal,  NULL,              PREC_NONE},
    [TOKEN_FOR]           = {NULL,           NULL,              PREC_NONE},
    [TOKEN_FUNC]          = {NULL,           NULL,              PREC_NONE},
    [TOKEN_IF]            = {NULL,           NULL,              PREC_NONE},
    [TOKEN_NIL]           = {parse_literal,  NULL,              PREC_NONE},
    [TOKEN_OR]            = {NULL,           NULL,              PREC_NONE},
    [TOKEN_PRINT]         = {NULL,           NULL,              PREC_NONE},
    [TOKEN_RETURN]        = {NULL,           NULL,              PREC_NONE},
    [TOKEN_TRUE]          = {parse_literal,  NULL,              PREC_NONE},
    [TOKEN_VAR]           = {NULL,           NULL,              PREC_NONE},
    [TOKEN_WHILE]         = {NULL,           NULL,              PREC_NONE},
    [TOKEN_ERROR]         = {NULL,           NULL,              PREC_NONE},
    [TOKEN_EOF]           = {NULL,           NULL,              PREC_NONE},
};

ParseRule *get_rule(TokenType type) {
    return &rules[type];
}

static void end_compile() {
    emit_return();
#ifdef DEBUG_PRINT_CODE
#include "debug.h"
    if (!parser.is_error) {
        disassemble_chunk(current_chunk(), "DEBUG CODE");
    }
#endif
}

bool compile(const char *src, Chunk *chunk) {
    init_scanner(src);
    compile_chunk = chunk;
    parser.is_error = false;
    parser.panic_mode = false;
    parse_token();

    while (!match(TOKEN_EOF)) {
        declaration();
    }
    end_compile();
    return !parser.is_error;
}
