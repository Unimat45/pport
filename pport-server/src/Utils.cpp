#include "Utils.h"

#include <sstream>

std::string toUnstyledString(const Json::Value& value) {
    Json::FastWriter writer;
    return writer.write(value);
}

std::string toUpper(std::string src) {
    std::stringstream result;

    for (char c : src) {
        result << (char)toupper(c);
    }

    return result.str();
}