// ExLang Time Extension
// Save as libs/time.cpp

#include "../interpreter.h"
#include <chrono>
#include <thread>
#include <iostream>

extern "C" void exlang_init(Interpreter& interp) {
    // time() -> current unix timestamp (seconds)
    class TimeFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            return (int)std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        }
        int arity() const override { return 0; }
    };
    interp.defineNative("time", std::make_shared<TimeFunc>());

    // sleep(ms)
    class SleepFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            if (args.size() != 1) return false;
            int ms = 0;
            if (std::holds_alternative<int>(args[0])) {
                ms = std::get<int>(args[0]);
            } else if (std::holds_alternative<double>(args[0])) {
                ms = (int)std::get<double>(args[0]);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            return true;
        }
        int arity() const override { return 1; }
    };
    interp.defineNative("sleep", std::make_shared<SleepFunc>());
}
