#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>

std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v");
std::string toTitleCase(const std::string &input);

#endif // STRING_UTILS_HPP
