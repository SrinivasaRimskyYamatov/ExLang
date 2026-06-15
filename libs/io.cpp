// ExLang IO Extension
// Functions: input, print (reinforce), readFile, writeFile

#include "../interpreter.h"
#include <iostream>
#include <fstream>
#include <string>

extern "C" void exlang_init(Interpreter& interp) {
    // input() - read line from stdin
    class InputFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            std::string line;
            std::getline(std::cin, line);
            return line;
        }
        int arity() const override { return 0; }
    };
    interp.defineNative("input", std::make_shared<InputFunc>());

    // readFile(filename)
    class ReadFileFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            if (args.size() != 1) return "";
            std::string filename;
            if (std::holds_alternative<std::string>(args[0])) {
                filename = std::get<std::string>(args[0]);
            } else {
                return "Error: filename must be string";
            }
            std::ifstream file(filename);
            if (!file.is_open()) return "Error: cannot open file";
            std::string content((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
            return content;
        }
        int arity() const override { return 1; }
    };
    interp.defineNative("readFile", std::make_shared<ReadFileFunc>());

    // writeFile(filename, content)
    class WriteFileFunc : public Function {
    public:
        Value call(Interpreter&, const std::vector<Value>& args) override {
            if (args.size() != 2) return false;
            std::string filename, content;
            if (std::holds_alternative<std::string>(args[0])) {
                filename = std::get<std::string>(args[0]);
            } else return false;
            if (std::holds_alternative<std::string>(args[1])) {
                content = std::get<std::string>(args[1]);
            } else return false;

            std::ofstream file(filename);
            if (file.is_open()) {
                file << content;
                file.close();
                return true;
            }
            return false;
        }
        int arity() const override { return 2; }
    };
    interp.defineNative("writeFile", std::make_shared<WriteFileFunc>());
}
