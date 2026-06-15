#include "interpreter.h"
#include "visitor.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <functional>

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
    
    // Built-in print
    class PrintFunction : public Function {
    public:
        Value call(Interpreter& interpreter, const std::vector<Value>& arguments) override {
            if (!arguments.empty()) {
                std::cout << valueToString(arguments[0]) << std::endl;
            }
            return std::monostate{};
        }
        int arity() const override { return 1; }
    };
    defineNative("print", std::make_shared<PrintFunction>());
}

void Interpreter::interpret(const std::vector<StmtPtr>& statements) {
    try {
        for (const auto& stmt : statements) {
            execute(stmt);
        }
    } catch (const std::exception& e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
    }
}

Value Interpreter::evaluate(ExprPtr expr) {
    return expr->accept(*this);
}

void Interpreter::execute(StmtPtr stmt) {
    stmt->accept(*this);
}

Value Interpreter::visit(BinaryExpr& expr) {
    Value left = evaluate(expr.left);
    Value right = evaluate(expr.right);
    
    // Helper to get numeric value
    auto toDouble = [](const Value& v) -> double {
        if (std::holds_alternative<int>(v)) return std::get<int>(v);
        if (std::holds_alternative<double>(v)) return std::get<double>(v);
        return 0.0;
    };
    
    switch (expr.op.type) {
        case TokenType::PLUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                return std::get<int>(left) + std::get<int>(right);
            }
            if (std::holds_alternative<double>(left) || std::holds_alternative<double>(right) ||
                std::holds_alternative<int>(left) || std::holds_alternative<int>(right)) {
                return toDouble(left) + toDouble(right);
            }
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            break;
        case TokenType::MINUS:
            return toDouble(left) - toDouble(right);
        case TokenType::STAR:
            return toDouble(left) * toDouble(right);
        case TokenType::SLASH:
            return toDouble(left) / toDouble(right);
        case TokenType::MODULO:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                return std::get<int>(left) % std::get<int>(right);
            }
            break;
        case TokenType::EQ:
            return valuesEqual(left, right);
        case TokenType::NEQ:
            return !valuesEqual(left, right);
        case TokenType::LT:
            return toDouble(left) < toDouble(right);
        case TokenType::GT:
            return toDouble(left) > toDouble(right);
        case TokenType::LTE:
            return toDouble(left) <= toDouble(right);
        case TokenType::GTE:
            return toDouble(left) >= toDouble(right);
    }
    return std::monostate{};
}

Value Interpreter::visit(UnaryExpr& expr) {
    Value right = evaluate(expr.right);
    if (expr.op.type == TokenType::MINUS) {
        if (std::holds_alternative<int>(right)) return -std::get<int>(right);
        if (std::holds_alternative<double>(right)) return -std::get<double>(right);
    }
    return std::monostate{};
}

Value Interpreter::visit(LiteralExpr& expr) {
    return expr.value;
}

Value Interpreter::visit(VariableExpr& expr) {
    return lookUpVariable(expr.name);
}

Value Interpreter::visit(AssignExpr& expr) {
    Value value = evaluate(expr.value);
    environment->assign(expr.name, value);
    return value;
}

Value Interpreter::visit(CallExpr& expr) {
    Value callee = evaluate(expr.callee);
    std::vector<Value> arguments;
    for (const auto& arg : expr.arguments) {
        arguments.push_back(evaluate(arg));
    }
    
    if (std::holds_alternative<std::shared_ptr<Function>>(callee)) {
        auto function = std::get<std::shared_ptr<Function>>(callee);
        if (arguments.size() != static_cast<size_t>(function->arity())) {
            throw std::runtime_error("Incorrect number of arguments.");
        }
        return function->call(*this, arguments);
    }
    throw std::runtime_error("Can only call functions.");
}

Value Interpreter::visit(GroupingExpr& expr) {
    return evaluate(expr.expression);
}

// Stmt visits
void Interpreter::visit(ExpressionStmt& stmt) {
    evaluate(stmt.expression);
}

void Interpreter::visit(LetStmt& stmt) {
    Value value = std::monostate{};
    if (stmt.initializer) {
        value = evaluate(stmt.initializer);
    }
    environment->define(stmt.name, value);
}

void Interpreter::visit(BlockStmt& stmt) {
    auto previous = environment;
    environment = std::make_shared<Environment>(previous);
    for (const auto& s : stmt.statements) {
        execute(s);
    }
    environment = previous;
}

void Interpreter::visit(IfStmt& stmt) {
    if (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.thenBranch);
    } else if (stmt.elseBranch) {
        execute(stmt.elseBranch);
    }
}

void Interpreter::visit(WhileStmt& stmt) {
    while (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.body);
    }
}

void Interpreter::visit(FunctionStmt& stmt) {
    class UserFunction : public Function {
    public:
        std::string name;
        std::vector<std::string> params;
        std::vector<StmtPtr> body;
        std::shared_ptr<Environment> closure;
        
        UserFunction(const std::string& n, const std::vector<std::string>& p, const std::vector<StmtPtr>& b, std::shared_ptr<Environment> c) 
            : name(n), params(p), body(b), closure(c) {}
        
        Value call(Interpreter& interpreter, const std::vector<Value>& arguments) override {
            auto env = std::make_shared<Environment>(closure);
            for (size_t i = 0; i < params.size(); ++i) {
                env->define(params[i], arguments[i]);
            }
            interpreter.environment = env; // Temporary
            // Execute body, handle return later
            for (const auto& s : body) {
                interpreter.execute(s);
                if (interpreter.hasReturn) {
                    interpreter.hasReturn = false;
                    Value ret = interpreter.returnValue;
                    interpreter.environment = closure; // Restore?
                    return ret;
                }
            }
            interpreter.environment = closure;
            return std::monostate{};
        }
        
        int arity() const override { return params.size(); }
    };
    
    auto function = std::make_shared<UserFunction>(stmt.name, stmt.params, stmt.body, environment);
    environment->define(stmt.name, function);
}

void Interpreter::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        returnValue = evaluate(stmt.value);
    } else {
        returnValue = std::monostate{};
    }
    hasReturn = true;
}

void Interpreter::visit(ImportStmt& stmt) {
    loadExtension(stmt.module);
}

void Interpreter::defineNative(const std::string& name, std::shared_ptr<Function> function) {
    globals->define(name, function);
}

void Interpreter::loadExtension(const std::string& moduleName) {
    // Try current directory first
    std::string path = "./" + moduleName + ".so";
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    
    if (!handle) {
        // Fallback to just module name
        path = moduleName + ".so";
        handle = dlopen(path.c_str(), RTLD_LAZY);
    }
    
    if (!handle) {
        std::cerr << "Failed to load " << moduleName << ".so: " << dlerror() << std::endl;
        return;
    }
    
    loadedLibs.push_back(handle);
    
    // Call init function
    using InitFunc = void(*)(Interpreter&);
    InitFunc init = (InitFunc)dlsym(handle, "exlang_init");
    if (init) {
        init(*this);
    } else {
        std::cerr << "Warning: exlang_init not found in " << moduleName << ".so" << std::endl;
    }
}

Value Interpreter::lookUpVariable(const std::string& name) {
    try {
        return environment->get(name);
    } catch (...) {
        return globals->get(name);
    }
}
