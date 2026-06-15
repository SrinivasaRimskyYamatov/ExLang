// ExLang C Extension Example
// Compile: g++ -shared -fPIC -o math.so math_ext.cpp -I.

#include "../interpreter.h"
#include <cmath>

extern "C" void exlang_init(Interpreter& interp) {
    // Simple add function
    class AddFunc : public Function {
    public:
        Value call(Interpreter& interp, const std::vector<Value>& args) override {
            if (args.size() != 2) return 0;
            // Support int or double
            auto getNum = [](const Value& v) -> double {
                if (std::holds_alternative<int>(v)) return std::get<int>(v);
                if (std::holds_alternative<double>(v)) return std::get<double>(v);
                return 0.0;
            };
            return (int)(getNum(args[0]) + getNum(args[1]));
        }
        int arity() const override { return 2; }
    };
    interp.defineNative("add", std::make_shared<AddFunc>());

    // sin function
    class SinFunc : public Function {
    public:
        Value call(Interpreter& interp, const std::vector<Value>& args) override {
            if (args.size() != 1) return 0.0;
            // Support int or double
            auto getNum = [](const Value& v) -> double {
                if (std::holds_alternative<int>(v)) return std::get<int>(v);
                if (std::holds_alternative<double>(v)) return std::get<double>(v);
                return 0.0;
            };
            return sin(getNum(args[0]));
        }
        int arity() const override { return 1; }
    };
    interp.defineNative("sin", std::make_shared<SinFunc>());
}
