#pragma once
#include "ast.h"
#include "environment.h"
#include "visitor.h"
#include <unordered_map>
#include <dlfcn.h>

class __attribute__((visibility("default"))) Function {
public:
    virtual ~Function() = default;
    virtual Value call(class Interpreter& interpreter, const std::vector<Value>& arguments) = 0;
    virtual int arity() const = 0;
};

class __attribute__((visibility("default"))) Interpreter : public ExprVisitor, public StmtVisitor {
public:
    Interpreter();
    void interpret(const std::vector<StmtPtr>& statements);
    Value evaluate(ExprPtr expr);
    void execute(StmtPtr stmt);

    // ExprVisitor
    Value visit(BinaryExpr& expr) override;
    Value visit(UnaryExpr& expr) override;
    Value visit(LiteralExpr& expr) override;
    Value visit(VariableExpr& expr) override;
    Value visit(AssignExpr& expr) override;
    Value visit(CallExpr& expr) override;
    Value visit(GroupingExpr& expr) override;

    // StmtVisitor
    void visit(ExpressionStmt& stmt) override;
    void visit(LetStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(FunctionStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(ImportStmt& stmt) override;

    void defineNative(const std::string& name, std::shared_ptr<Function> function) __attribute__((visibility("default")));
    void loadExtension(const std::string& moduleName);

private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    Value returnValue;
    bool hasReturn = false;
    std::vector<void*> loadedLibs;

    Value lookUpVariable(const std::string& name);
};
