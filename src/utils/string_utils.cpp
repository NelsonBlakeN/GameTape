#include "string_utils.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

std::string &rtrim(std::string &s, const char *t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// TODO: Perform in place
std::string toTitleCase(const std::string &input)
{
    if (input.empty())
    {
        return input;
    }

    std::istringstream stream(input);
    std::ostringstream result;
    std::string word;

    while (stream >> word)
    {
        if (!word.empty())
        {
            word[0] = std::toupper(word[0]);
            std::transform(word.begin() + 1, word.end(), word.begin() + 1, ::tolower);
        }
        if (!result.str().empty())
        {
            result << " ";
        }
        result << word;
    }

    return result.str();
}
