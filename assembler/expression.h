#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <unordered_map>

struct EvalResult
{
    int value;
    bool relative;
    bool error = false;
};

class Expression
{
public:
    static EvalResult evaluate(
        const std::string& expr,
        const std::unordered_map<std::string, struct Symbol>& symtab);
};

#endif
