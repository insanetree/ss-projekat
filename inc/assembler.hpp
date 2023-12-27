#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "global.hpp"
#include "lexer.hpp"
#include "parser.hpp"

extern FILE* yyin;

class Section;
class Statement;
class Symbol;
class StringPool;

class Assembler {
public:	
	static bool setInput(std::string filename);
	static bool setOutput(std::string filename);
	static int32_t firstPass();
	static int32_t secondPass();
	static Section* getCurrentSection();
	static void setCurrentSection(Section*);
	static uint32_t getLocationCounter();
	static void setLocationCounter(uint32_t value);
	static void incrementLocationCounter(uint32_t increment);
	static void insertStatement(Statement*);
	static std::unordered_map<std::string, Symbol*>& getSymbolTable();
	static std::unordered_map<std::string, Section*>& getSectionTable();
private:
	static void printTextFIle();
	static void printBinaryFile();

	static FILE* input;
	static FILE* output;
	static FILE* outputText;
	static std::unordered_map<std::string, Symbol*> symbolTable;
	static std::unordered_map<std::string, Section*> sectionTable;
	static std::vector<Statement*> statements;
	static uint32_t locationCounter;
	static Section* currentSection;
	static StringPool stringPool;
};

#endif