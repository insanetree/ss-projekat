#include "global.hpp"
#include "assembler.hpp"

std::ifstream Assembler::input = std::ifstream();
std::ofstream Assembler::output = std::ofstream();
std::unordered_map<std::string, Symbol*> Assembler::symbolTable;
std::unordered_map<std::string, Section*> Assembler::sectionTable;
std::vector<Statement*> Assembler::statements;
uint32_t Assembler::locationCounter = 0;
Section* Assembler::currentSection = nullptr;

bool Assembler::setInput(std::string filename) {
	input.open(filename, std::ios::in);
	if(input.is_open()) {
		yyin = fopen(filename.c_str(), "r");
	}
	return input.is_open();
}

bool Assembler::setOutput(std::string filename) {
	output.open(filename, std::ios::out | std::ios::binary);
	return output.is_open();
}

int32_t Assembler::firstPass() {
	return yyparse();
}

int32_t Assembler::secondPass() {
}

Section* Assembler::getCurrentSection() {
	return currentSection;
}

void Assembler::setCurrentSection(Section* section) {
	currentSection = section;
}

std::unordered_map<std::string, Symbol*>& Assembler::getSymbolTable() {
	return symbolTable;
}

std::unordered_map<std::string, Section*>& Assembler::getSectionTable() {
	return sectionTable;
}

uint32_t Assembler::getLocationCounter() {
	return locationCounter;
}

void Assembler::setLocationCounter(uint32_t value=0) {
	locationCounter = value;
}

void Assembler::incrementLocationCounter(uint32_t increment=4) {
	locationCounter += increment;
}

void Assembler::insertStatement(Statement* statement) {
	statements.push_back(statement);
}

extern FILE* yyin;

int32_t main(int32_t argc, char** argv) {
	if(argc != 4) {
		std::cerr<<"Incorrect number of arguments"<<std::endl \
		<<"Usage: assembler -o <output file> <input file>"<<std::endl;
		return -1;
	}

	if(!Assembler::setInput(argv[3])) {
		std::cerr<<"Input file not found"<<std::endl;
		return -2;
	}
	if(!Assembler::setOutput(argv[2])) {
		std::cerr<<"Output file can not be generated"<<std::endl;
		return -3;
	}

	std::cout<<Assembler::firstPass();

	return 0;
}