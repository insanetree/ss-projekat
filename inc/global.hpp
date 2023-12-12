#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <cstdint>

class Symbol;

typedef enum{
    LITERAL_VALUE,
    SYMBOL_VALUE,
    LITERAL_MEMORY,
    SYMBOL_MEMORY,
    REGISTER_VALUE,
    REGISTER_MEMORY,
    REGISTER_LITERAL_MEMORY,
    REGISTER_SYMBOL_MEMORY
}argumentType;

struct arg{
    argumentType type;
    int32_t registerNumber;
    int32_t literal;
    std::string symbol;
    struct arg* next;
};

int32_t getRegisterNum(const std::string&);

#endif