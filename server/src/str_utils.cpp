#include "str_utils.h"
#include <algorithm>

bool istrcmp(std::string a, std::string b) {
    return istrcmp(a.c_str(), b.c_str());
}

bool istrcmp(const char *a, const char *b) {
    for (; *a != 0 && *b != 0; a++, b++) {
        if (::toupper(*a) != ::toupper(*b)) {
            return false;
        }
    }
    return *a == 0 && *b == 0;
}

std::string str_to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](const char c) { return (char)std::toupper(c); });

    return s;
}

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    [](char c) { return !std::isspace(c); }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](char c) { return !std::isspace(c); })
                .base(),
            s.end());
}

void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}