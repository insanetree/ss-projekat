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
	static int32_t firstPass();
	static int32_t secondPass();
	static int32_t getCurrentSection();
	static uint32_t getLocationCounter();
	static void setLocationCounter(uint32_t value);
	static void incrementLocationCounter(uint32_t increment);
	static std::unordered_map<std::string, Symbol*>& getSymbolTable();
private:
	static std::ifstream input;
	static std::ofstream output;
	static std::unordered_map<std::string, Symbol*> symbolTable;
	static std::vector<std::string> sectionTable;
	static uint32_t locationCounter;
	static int32_t currentSection;
};

#endif