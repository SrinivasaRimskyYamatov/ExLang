#pragma once
#include <string>
#include <variant>
#include <memory>
#include <vector>

class Function;
class Environment;

using Value = std::variant<int, double, std::string, bool, std::monostate, std::shared_ptr<Function>>;

std::string valueToString(const Value& value);
bool isTruthy(const Value& value);
bool valuesEqual(const Value& a, const Value& b);
