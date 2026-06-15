#include "parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

StmtPtr Parser::declaration() {
    if (match({TokenType::LET})) return letDeclaration();
    if (match({TokenType::FN})) return functionDeclaration();
    if (match({TokenType::IMPORT})) return importStatement();
    return statement();
}

StmtPtr Parser::letDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    ExprPtr initializer = nullptr;
    if (match({TokenType::ASSIGN})) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_shared<LetStmt>(name.lexeme, initializer);
}

StmtPtr Parser::functionDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
    consume(TokenType::LPAREN, "Expect '(' after function name.");
    std::vector<std::string> parameters;
    if (!check(TokenType::RPAREN)) {
        do {
            Token param = consume(TokenType::IDENTIFIER, "Expect parameter name.");
            parameters.push_back(param.lexeme);
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RPAREN, "Expect ')' after parameters.");
    consume(TokenType::LBRACE, "Expect '{' before function body.");
    auto block = std::dynamic_pointer_cast<BlockStmt>(blockStatement());
    std::vector<StmtPtr> body = block ? block->statements : std::vector<StmtPtr>{};
    return std::make_shared<FunctionStmt>(name.lexeme, parameters, body);
}

StmtPtr Parser::importStatement() {
    Token module = consume(TokenType::STRING, "Expect module name string.");
    consume(TokenType::SEMICOLON, "Expect ';' after import.");
    return std::make_shared<ImportStmt>(module.lexeme);
}

StmtPtr Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::LBRACE})) return blockStatement();
    return expressionStatement();
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'if'.");
    ExprPtr condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after condition.");
    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }
    return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
}

StmtPtr Parser::whileStatement() {
    consume(TokenType::LPAREN, "Expect '(' after 'while'.");
    ExprPtr condition = expression();
    consume(TokenType::RPAREN, "Expect ')' after condition.");
    StmtPtr body = statement();
    return std::make_shared<WhileStmt>(condition, body);
}

StmtPtr Parser::returnStatement() {
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_shared<ReturnStmt>(value);
}

StmtPtr Parser::blockStatement() {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    consume(TokenType::RBRACE, "Expect '}' after block.");
    return std::make_shared<BlockStmt>(statements);
}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<ExpressionStmt>(expr);
}

ExprPtr Parser::expression() {
    return assignment();
}

ExprPtr Parser::assignment() {
    ExprPtr expr = equality();
    if (match({TokenType::ASSIGN})) {
        Token equals = previous();
        ExprPtr value = assignment();
        if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
            return std::make_shared<AssignExpr>(var->name, value);
        }
        error(equals, "Invalid assignment target.");
    }
    return expr;
}

ExprPtr Parser::equality() {
    ExprPtr expr = comparison();
    while (match({TokenType::EQ, TokenType::NEQ})) {
        Token op = previous();
        ExprPtr right = comparison();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::comparison() {
    ExprPtr expr = term();
    while (match({TokenType::LT, TokenType::GT, TokenType::LTE, TokenType::GTE})) {
        Token op = previous();
        ExprPtr right = term();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::term() {
    ExprPtr expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        ExprPtr right = factor();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::factor() {
    ExprPtr expr = unary();
    while (match({TokenType::STAR, TokenType::SLASH, TokenType::MODULO})) {
        Token op = previous();
        ExprPtr right = unary();
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

ExprPtr Parser::unary() {
    if (match({TokenType::MINUS})) {  // Only - for now
        Token op = previous();
        ExprPtr right = unary();
        return std::make_shared<UnaryExpr>(op, right);
    }
    return call();
}

ExprPtr Parser::call() {
    ExprPtr expr = primary();
    while (true) {
        if (match({TokenType::LPAREN})) {
            std::vector<ExprPtr> arguments;
            if (!check(TokenType::RPAREN)) {
                do {
                    arguments.push_back(expression());
                } while (match({TokenType::COMMA}));
            }
            consume(TokenType::RPAREN, "Expect ')' after arguments.");
            expr = std::make_shared<CallExpr>(expr, arguments);
        } else {
            break;
        }
    }
    return expr;
}

ExprPtr Parser::primary() {
    if (match({TokenType::NUMBER})) {
        if (previous().lexeme.find('.') != std::string::npos) {
            return std::make_shared<LiteralExpr>(std::stod(previous().lexeme));
        }
        return std::make_shared<LiteralExpr>(std::stoi(previous().lexeme));
    }
    if (match({TokenType::STRING})) {
        return std::make_shared<LiteralExpr>(previous().lexeme);
    }
    if (match({TokenType::TRUE})) return std::make_shared<LiteralExpr>(true);
    if (match({TokenType::FALSE})) return std::make_shared<LiteralExpr>(false);
    if (match({TokenType::NIL})) return std::make_shared<LiteralExpr>(std::monostate{});
    if (match({TokenType::IDENTIFIER})) {
        return std::make_shared<VariableExpr>(previous().lexeme);
    }
    if (match({TokenType::LPAREN})) {
        ExprPtr expr = expression();
        consume(TokenType::RPAREN, "Expect ')' after expression.");
        return std::make_shared<GroupingExpr>(expr);
    }
    error(peek(), "Expect expression.");
    return nullptr; // Should not reach
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    return Token{}; // Error recovery
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::previous() {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || tokens[current].type == TokenType::EOF_TOKEN;
}

Token Parser::peek() {
    return tokens[current];
}

void Parser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error: " << message << std::endl;
    // For now, throw to stop
    throw std::runtime_error(message);
}
