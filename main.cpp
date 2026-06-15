#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    Interpreter interpreter;
    
    if (argc > 1) {
        // Run file
        std::ifstream file(argv[1]);
        if (!file) {
            std::cerr << "Could not open file: " << argv[1] << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        
        Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        
        Parser parser(tokens);
        auto statements = parser.parse();
        
        interpreter.interpret(statements);
    } else {
        // REPL
        std::string line;
        std::cout << "ExLang REPL. Type 'exit' to quit.\n";
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            if (line == "exit") break;
            if (line.empty()) continue;
            
            try {
                Lexer lexer(line + ";"); // Ensure semicolon
                auto tokens = lexer.scanTokens();
                Parser parser(tokens);
                auto statements = parser.parse();
                interpreter.interpret(statements);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }
    return 0;
}
