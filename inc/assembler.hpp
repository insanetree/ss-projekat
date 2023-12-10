#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "global.hpp"
#include "symbol.hpp"
#include "lexer.hpp"
#include "parser.hpp"

extern FILE* yyin;

class Assembler {
public:	
	static bool setInput(std::string filename);
	static bool setOutput(std::string filename);
	static int firstPass();
	static int secondPass();
private:
	static std::ifstream input;
	static std::ofstream output;
	static std::unordered_map<std::string, Symbol> symbolTable;
	static unsigned int locationCounter;
};

#endif