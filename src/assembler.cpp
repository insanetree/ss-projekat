#include "global.hpp"
#include "assembler.hpp"


std::ifstream Assembler::input = std::ifstream();
std::ofstream Assembler::output = std::ofstream();
std::unordered_map<std::string, Symbol> Assembler::symbolTable;
unsigned int Assembler::locationCounter = 0;

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

int Assembler::firstPass() {
	return yyparse();
}

int Assembler::secondPass() {
}

extern FILE* yyin;

int main(int argc, char** argv) {
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

	Assembler::firstPass();

	return 0;
}