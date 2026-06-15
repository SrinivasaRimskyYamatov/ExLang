#include "ast.h"

// Expr implementations
BinaryExpr::BinaryExpr(ExprPtr left, Token op, ExprPtr right) : left(left), op(op), right(right) {}
Value BinaryExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

UnaryExpr::UnaryExpr(Token op, ExprPtr right) : op(op), right(right) {}
Value UnaryExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

LiteralExpr::LiteralExpr(Value value) : value(value) {}
Value LiteralExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

VariableExpr::VariableExpr(const std::string& name) : name(name) {}
Value VariableExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

AssignExpr::AssignExpr(const std::string& name, ExprPtr value) : name(name), value(value) {}
Value AssignExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

CallExpr::CallExpr(ExprPtr callee, std::vector<ExprPtr> arguments) : callee(callee), arguments(arguments) {}
Value CallExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

GroupingExpr::GroupingExpr(ExprPtr expression) : expression(expression) {}
Value GroupingExpr::accept(ExprVisitor& visitor) { return visitor.visit(*this); }

// Stmt implementations
ExpressionStmt::ExpressionStmt(ExprPtr expression) : expression(expression) {}
void ExpressionStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

LetStmt::LetStmt(const std::string& name, ExprPtr initializer) : name(name), initializer(initializer) {}
void LetStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

BlockStmt::BlockStmt(std::vector<StmtPtr> statements) : statements(statements) {}
void BlockStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

IfStmt::IfStmt(ExprPtr condition, StmtPtr thenBranch, StmtPtr elseBranch) : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}
void IfStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

WhileStmt::WhileStmt(ExprPtr condition, StmtPtr body) : condition(condition), body(body) {}
void WhileStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

FunctionStmt::FunctionStmt(const std::string& name, std::vector<std::string> params, std::vector<StmtPtr> body) : name(name), params(params), body(body) {}
void FunctionStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

ReturnStmt::ReturnStmt(ExprPtr value) : value(value) {}
void ReturnStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }

ImportStmt::ImportStmt(const std::string& module) : module(module) {}
void ImportStmt::accept(StmtVisitor& visitor) { visitor.visit(*this); }
