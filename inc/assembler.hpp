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
	static int getCurrentSection();
	static unsigned int getLocationCounter();
	static void setLocationCounter(unsigned int value);
	static void incrementLocationCounter(unsigned int increment);
	static std::unordered_map<std::string, Symbol*>& getSymbolTable();
private:
	static std::ifstream input;
	static std::ofstream output;
	static std::unordered_map<std::string, Symbol*> symbolTable;
	static std::vector<std::string> sectionTable;
	static unsigned int locationCounter;
	static int currentSection;
};

#endif