#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <string>

bool istrcmp(std::string a, std::string b);
bool istrcmp(const char *a, const char *b);
std::string str_to_upper(std::string s);
void trim(std::string& s);

#endif // STR_UTILS_H