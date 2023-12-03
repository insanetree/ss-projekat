#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "global.hpp"
#include "symbol.hpp"
#include "lexer.hpp"
#include "parser.hpp"

extern FILE* yyin;

class Assembler {
public:	
	static bool setInput(string filename);
	static bool setOutput(string filename);
	static int firstPass();
	static int secondPass();
private:
	static ifstream input;
	static ofstream output;
	static unordered_map<string, Symbol> symbolTable;
	static unsigned int locationCounter;
};

#endif