#include "value.h"
#include <sstream>

std::string valueToString(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::monostate>(value)) {
        return "nil";
    } else if (std::holds_alternative<std::shared_ptr<Function>>(value)) {
        return "<function>";
    }
    return "unknown";
}

bool isTruthy(const Value& value) {
    if (std::holds_alternative<std::monostate>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    return true;
}

bool valuesEqual(const Value& a, const Value& b) {
    if (a.index() != b.index()) return false;
    return a == b;
}
