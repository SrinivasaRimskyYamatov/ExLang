#pragma once
#include "value.h"
#include "token.h"
// ast.h forward is enough but include for completeness

// Forward declarations for AST
struct BinaryExpr;
struct UnaryExpr;
struct LiteralExpr;
struct VariableExpr;
struct AssignExpr;
struct CallExpr;
struct GroupingExpr;

struct ExpressionStmt;
struct LetStmt;
struct BlockStmt;
struct IfStmt;
struct WhileStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ImportStmt;

class Interpreter;

// ExprVisitor
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual Value visit(BinaryExpr& expr) = 0;
    virtual Value visit(UnaryExpr& expr) = 0;
    virtual Value visit(LiteralExpr& expr) = 0;
    virtual Value visit(VariableExpr& expr) = 0;
    virtual Value visit(AssignExpr& expr) = 0;
    virtual Value visit(CallExpr& expr) = 0;
    virtual Value visit(GroupingExpr& expr) = 0;
};

// StmtVisitor
class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visit(ExpressionStmt& stmt) = 0;
    virtual void visit(LetStmt& stmt) = 0;
    virtual void visit(BlockStmt& stmt) = 0;
    virtual void visit(IfStmt& stmt) = 0;
    virtual void visit(WhileStmt& stmt) = 0;
    virtual void visit(FunctionStmt& stmt) = 0;
    virtual void visit(ReturnStmt& stmt) = 0;
    virtual void visit(ImportStmt& stmt) = 0;
};
