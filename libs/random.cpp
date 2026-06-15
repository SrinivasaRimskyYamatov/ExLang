// ExLang Random Extension
// Save as libs/random.cpp

#include "../interpreter.h"
#include <random>
#include <ctime>
#include <iostream>

extern "C" void exlang_init(Interpreter& interp) {
    // Global random generator
    static std::mt19937 gen(std::time(nullptr));

    // random() -> double between 0.0 and 1.0
    class RandomFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            std::uniform_real_distribution<double> dis(0.0, 1.0);
            return dis(gen);
        }
        int arity() const override { return 0; }
    };
    interp.defineNative("random", std::make_shared<RandomFunc>());

    // randInt(min, max)
    class RandIntFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            if (args.size() != 2) return 0;
            int minv = 0, maxv = 100;
            if (std::holds_alternative<int>(args[0])) minv = std::get<int>(args[0]);
            else if (std::holds_alternative<double>(args[0])) minv = (int)std::get<double>(args[0]);
            if (std::holds_alternative<int>(args[1])) maxv = std::get<int>(args[1]);
            else if (std::holds_alternative<double>(args[1])) maxv = (int)std::get<double>(args[1]);
            std::uniform_int_distribution<int> dis(minv, maxv);
            return dis(gen);
        }
        int arity() const override { return 2; }
    };
    interp.defineNative("randInt", std::make_shared<RandIntFunc>());

}
