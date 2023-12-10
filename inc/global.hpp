#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <cstdio>
#include <cstdlib>
#include <cstdio>

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

struct arg {
    argumentType type;
    int registerNumber;
    int literal;
    std::string symbol;
};

#endif