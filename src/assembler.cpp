#include "global.hpp"
#include "assembler.hpp"
#include "section.hpp"
#include "instruction.hpp"
#include "directive.hpp"
#include "stringPool.hpp"

FILE* Assembler::input = nullptr;
FILE* Assembler::output = nullptr;
FILE* Assembler::outputText = nullptr;
std::unordered_map<std::string, Symbol*> Assembler::symbolTable;
std::unordered_map<std::string, Section*> Assembler::sectionTable;
std::vector<Statement*> Assembler::statements;
uint32_t Assembler::locationCounter = 0;
Section* Assembler::currentSection = nullptr;
StringPool Assembler::stringPool;

bool Assembler::setInput(std::string filename) {
	input = fopen(filename.c_str(), "r");
	if(input) {
		yyin = input;
		return true;
	}
	return false;
}

bool Assembler::setOutput(std::string filename) {
	output = fopen(filename.c_str(), "wb");
	std::string filenameText = filename + ".txt";
	outputText = fopen(filenameText.c_str(), "w");
	return output && outputText;
}

int32_t Assembler::firstPass() {
	return yyparse();
}

int32_t Assembler::secondPass() {
	//add section names as symbols
	for(auto& s : sectionTable) {
		symbolTable.insert({s.first, new Symbol(s.first, 0, false, s.second->getId(), SECTION)});
	}
	//TODO: resolve equ directives
	for(Statement* s : statements) {
		if(s->secondPass()) {
			return -1;
		}
	}
	for(auto& s : symbolTable) {
		stringPool.putString(s.first);
	}
	for(auto& s : sectionTable) {
		stringPool.putString(s.first);
	}
	for(auto& section : sectionTable) {
		if(section.second->secondPass()) {
			return -1;
		}
	}
	printTextFIle();
	fclose(input);
	fclose(output);
	fclose(outputText);
	return 0;
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

void Assembler::printTextFIle() {
	fprintf(outputText, "SYMBOL_TABLE\n");
	fprintf(outputText, "%10s %20s %10s %10s %10s %10s\n", "id", "name", "value", "section", "global", "type");
	for(auto& sym: symbolTable) {
			fprintf(outputText, "%10d %20s 0x%08x %10d %10d %10s\n", 
				sym.second->getID(),
				sym.second->getName().c_str(),
				sym.second->getValue(),
				sym.second->getSection(),
				sym.second->isGlobal(),
				(sym.second->getType() == NOTYPE)?("NOTYPE"):((sym.second->getType() == SECTION)?("SECTION"):("COMMON"))
			);
	}
	fprintf(outputText, "SECTION_TABLE\n");
	fprintf(outputText, "%10s %20s %10s %10s\n","id", "name", "baseAddr", "size");
	for(auto& sec : sectionTable){
		fprintf(outputText, "%10d %20s 0x%08x 0x%08x\n", 
			sec.second->getId(),
			sec.second->getName().c_str(),
			sec.second->getBaseAddr(),
			sec.second->getSizeWithPools()
		);
	}
	for(auto& sec : sectionTable) {
		Section* s = sec.second;
		fprintf(outputText, "%s:\n", s->getName().c_str());
		for(size_t i = 0 ; i < s->getSizeWithPools() ; i++) {
			fprintf(outputText, "%02x%c", s->getData()[i], (i%8 == 7)?('\n'):(' '));
		}
		fprintf(outputText, "%s rel data:\n", s->getName().c_str());
		fprintf(outputText, "%10s %10s %10s %10s\n","offset", "symbol", "addend", "type");
		for(Section::relData& r : s->getRelocationTable()) {
			fprintf(outputText, "0x%08x %10d 0x%08x %10s\n", r.offset, r.symbolId, r.addend, "R_32");
		}
	}
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

	if(Assembler::firstPass()) {
		std::cerr<<"First pass failed"<<std::endl;
		return 1;
	}
	if(Assembler::secondPass()) {
		std::cerr<<"Second pass failed"<<std::endl;
	}
	return 0;
}