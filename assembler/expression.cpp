#include "expression.h"
#include "assembler.h"

#include <sstream>
#include <iostream>

using namespace std;

static EvalResult getSymbol(
    const string& token,
    const unordered_map<string, Symbol>& symtab)
{
    auto it = symtab.find(token);

    if (it == symtab.end())
    {
        cout << "[ERROR] Undefined symbol: " << token << endl;
        return { 0, true };
    }

    return {
        it->second.value,
        it->second.relative
    };
}

static vector<string> tokenize(const string& expr)
{
    vector<string> tokens;
    string cur;

    for (char c : expr)
    {
        if (c == '+' || c == '-')
        {
            if (!cur.empty())
            {
                tokens.push_back(cur);
                cur.clear();
            }

            tokens.push_back(string(1, c));
        }
        else if (!isspace(c))
        {
            cur += c;
        }
    }

    if (!cur.empty())
        tokens.push_back(cur);

    return tokens;
}

EvalResult Expression::evaluate(
    const string& expr,
    const unordered_map<string, Symbol>& symtab)
{
    vector<string> tokens = tokenize(expr);

    EvalResult result = { 0, false };

    char op = '+';

    for (auto& t : tokens)
    {
        if (t == "+" || t == "-")
        {
            op = t[0];
            continue;
        }

        EvalResult val;

        if (isdigit(t[0]))
        {
            val = { stoi(t), false, false };
        }
        else
        {
            val = getSymbol(t, symtab);
        }

        // propagate error
        if (val.error)
            result.error = true;

        if (op == '+')
        {
            // R + R invalid
            if (result.relative && val.relative)
            {
                cout << "[ERROR] R + R in EQU\n";
                result.error = true;
            }
            else
            {
                result.value += val.value;
                result.relative = result.relative || val.relative;
            }
        }
        else if (op == '-')
        {
            if (!result.relative && val.relative)
            {
                cout << "[ERROR] A - R not allowed\n";
                result.error = true;
            }
            else
            {
                result.value -= val.value;

                // R - R = A
                if (result.relative && val.relative)
                    result.relative = false;
            }
        }
    }

    return result;
}