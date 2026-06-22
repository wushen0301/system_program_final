#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>

inline std::string toHex(int value, int width)
{
    std::stringstream ss;

    ss << std::uppercase
        << std::hex
        << std::setw(width)
        << std::setfill('0')
        << value;

    return ss.str();
}

#endif