#pragma once
#include <string>

enum class TokenType {
    // Keywords
    LET, FN, IF, ELSE, WHILE, RETURN, IMPORT,
    // Literals
    IDENTIFIER, NUMBER, STRING, TRUE, FALSE, NIL,
    // Operators
    PLUS, MINUS, STAR, SLASH, MODULO,
    EQ, NEQ, LT, GT, LTE, GTE, ASSIGN,
    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, COMMA,
    // End
    EOF_TOKEN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};
