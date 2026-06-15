#pragma once
#include "value.h"
#include <unordered_map>
#include <string>
#include <memory>

class Environment {
public:
    Environment();
    Environment(std::shared_ptr<Environment> enclosing);
    
    void define(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    void assign(const std::string& name, const Value& value);

private:
    std::unordered_map<std::string, Value> values;
    std::shared_ptr<Environment> enclosing;
};
