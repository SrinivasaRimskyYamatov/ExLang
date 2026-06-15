#pragma once
#include <string>
#include <vector>
#include "token.h"

// TokenType and Token defined in token.h

// Token struct defined in token.h

class Lexer {
public:
    Lexer(const std::string& source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;
    int current = 0;
    int line = 1;

    void scanToken();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    bool isAtEnd() const;

    void string();
    void number();
    void identifier();

    void addToken(TokenType type);
    void addToken(TokenType type, const std::string& lexeme);
};
