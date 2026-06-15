#pragma once
#include "lexer.h"
#include "ast.h"
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<StmtPtr> parse();

private:
    std::vector<Token> tokens;
    int current = 0;

    StmtPtr declaration();
    StmtPtr letDeclaration();
    StmtPtr functionDeclaration();
    StmtPtr statement();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr returnStatement();
    StmtPtr importStatement();
    StmtPtr blockStatement();
    StmtPtr expressionStatement();

    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr call();
    ExprPtr primary();

    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    Token advance();
    Token previous();
    bool isAtEnd() const;
    Token peek();

    void error(const Token& token, const std::string& message);
};
