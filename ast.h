#pragma once
#include "value.h"
#include "token.h"
#include "visitor.h"
#include <memory>
#include <vector>

struct Expr;
struct Stmt;

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

// Forward declarations for visitor
class ExprVisitor;
class StmtVisitor;

struct Expr {
    virtual ~Expr() = default;
    virtual Value accept(ExprVisitor& visitor) = 0;
};

struct Stmt {
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

// Expressions
struct BinaryExpr : Expr {
    ExprPtr left;
    Token op;
    ExprPtr right;
    BinaryExpr(ExprPtr left, Token op, ExprPtr right);
    Value accept(ExprVisitor& visitor) override;
};

struct UnaryExpr : Expr {
    Token op;
    ExprPtr right;
    UnaryExpr(Token op, ExprPtr right);
    Value accept(ExprVisitor& visitor) override;
};

struct LiteralExpr : Expr {
    Value value;
    LiteralExpr(Value value);
    Value accept(ExprVisitor& visitor) override;
};

struct VariableExpr : Expr {
    std::string name;
    VariableExpr(const std::string& name);
    Value accept(ExprVisitor& visitor) override;
};

struct AssignExpr : Expr {
    std::string name;
    ExprPtr value;
    AssignExpr(const std::string& name, ExprPtr value);
    Value accept(ExprVisitor& visitor) override;
};

struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    CallExpr(ExprPtr callee, std::vector<ExprPtr> arguments);
    Value accept(ExprVisitor& visitor) override;
};

struct GroupingExpr : Expr {
    ExprPtr expression;
    GroupingExpr(ExprPtr expression);
    Value accept(ExprVisitor& visitor) override;
};

// Statements
struct ExpressionStmt : Stmt {
    ExprPtr expression;
    ExpressionStmt(ExprPtr expression);
    void accept(StmtVisitor& visitor) override;
};

struct LetStmt : Stmt {
    std::string name;
    ExprPtr initializer;
    LetStmt(const std::string& name, ExprPtr initializer);
    void accept(StmtVisitor& visitor) override;
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> statements);
    void accept(StmtVisitor& visitor) override;
};

struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;
    IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch);
    void accept(StmtVisitor& visitor) override;
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr condition, StmtPtr body);
    void accept(StmtVisitor& visitor) override;
};

struct FunctionStmt : Stmt {
    std::string name;
    std::vector<std::string> params;
    std::vector<StmtPtr> body;
    FunctionStmt(const std::string& name, std::vector<std::string> params, std::vector<StmtPtr> body);
    void accept(StmtVisitor& visitor) override;
};

struct ReturnStmt : Stmt {
    ExprPtr value;
    ReturnStmt(ExprPtr value);
    void accept(StmtVisitor& visitor) override;
};

struct ImportStmt : Stmt {
    std::string module;
    ImportStmt(const std::string& module);
    void accept(StmtVisitor& visitor) override;
};
