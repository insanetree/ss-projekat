#include "global.hpp"
#include "assembler.hpp"
#include "section.hpp"
#include "instruction.hpp"
#include "directive.hpp"
#include "stringPool.hpp"

extern FILE* yyin;

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
		symbolTable.insert({s.first, new Symbol(s.first, 0, false, s.second, SECTION)});
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
	printBinaryFile();
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
	std::map<uint32_t, Symbol*> symTablePrint;
	std::map<uint32_t, Section*> secTablePrint;
	for(auto& s : symbolTable)
		symTablePrint.insert({s.second->getID(), s.second});
	for(auto& s : sectionTable)
		secTablePrint.insert({s.second->getId(), s.second});

	fprintf(outputText, "SYMBOL_TABLE\n");
	fprintf(outputText, "%10s %20s %10s %10s %10s %10s\n", "id", "name", "value", "section", "global", "type");
	for(auto& sym: symTablePrint) {
			fprintf(outputText, "%10d %20s 0x%08x %10d %10d %10s\n", 
				sym.second->getID(),
				sym.second->getName().c_str(),
				sym.second->getValue(),
				sym.second->getSectionId(),
				sym.second->isGlobal(),
				(sym.second->getType() == NOTYPE)?("NOTYPE"):((sym.second->getType() == SECTION)?("SECTION"):("COMMON"))
			);
	}
	fprintf(outputText, "SECTION_TABLE\n");
	fprintf(outputText, "%10s %20s %10s %10s\n","id", "name", "baseAddr", "size");
	for(auto& sec : secTablePrint){
		fprintf(outputText, "%10d %20s 0x%08x 0x%08x\n", 
			sec.second->getId(),
			sec.second->getName().c_str(),
			sec.second->getBaseAddr(),
			sec.second->getSizeWithPools()
		);
	}
	for(auto& sec : secTablePrint) {
		Section* s = sec.second;
		fprintf(outputText, "\n%s:\n", s->getName().c_str());
		for(size_t i = 0 ; i < s->getSizeWithPools() ; i++) {
			fprintf(outputText, "%02x%c", s->getData()[i], (i%8 == 7)?('\n'):(' '));
		}
		fprintf(outputText, "\n%s rel data:\n", s->getName().c_str());
		fprintf(outputText, "%10s %10s %10s %10s\n","offset", "symbol", "addend", "type");
		for(relData& r : s->getRelocationTable()) {
			fprintf(outputText, "0x%08x %10d 0x%08x %10s\n", r.OFFSET, r.SYMBOL_ID, r.ADDEND, "R_32");
		}
	}
}

void Assembler::printBinaryFile() {
	std::map<uint32_t, Symbol*> symTablePrint;
	std::map<uint32_t, Section*> secTablePrint;
	for(auto& s : symbolTable)
		symTablePrint.insert({s.second->getID(), s.second});
	for(auto& s : sectionTable)
		secTablePrint.insert({s.second->getId(), s.second});
	
	mainHeader* mh = new mainHeader();
	mh->SYMBOL_TABLE_OFFSET = sizeof(mainHeader);
	mh->SYMBOL_TABLE_LENGTH = symbolTable.size();
	mh->SECTION_TABLE_OFFSET = mh->SYMBOL_TABLE_OFFSET + mh->SYMBOL_TABLE_LENGTH*sizeof(symTableEntry);
	mh->SECTION_TABLE_LENGTH = sectionTable.size();
	mh->STRING_POOL_OFFSET = 0; //TODO
	mh->STRING_POOL_SIZE = stringPool.getStringPool().size();

	std::vector<symTableEntry*> symTableOutput;
	symTableEntry* symte = nullptr;
	for(auto& s : symTablePrint) {
		symte = new symTableEntry();
		symte->SYMBOL_ID = s.second->getID();
		symte->SYMBOL_NAME_OFFSET = stringPool.getStringIndex(s.second->getName());
		symte->SYMBOL_VALUE = s.second->getValue();
		symte->SECTION_ID = s.second->getSectionId();
		symte->GLOBAL = s.second->isGlobal();
		symte->SYMBOL_TYPE = s.second->getType();
		symTableOutput.push_back(symte);
	}

	std::vector<sectionTableEntry*> secTableOutput;
	sectionTableEntry* secte = nullptr;
	uint32_t sectionDataOffset = sizeof(mainHeader) + sizeof(symTableEntry)*symbolTable.size() + sizeof(sectionTableEntry)*sectionTable.size();
	for(auto& s : secTablePrint) {
		secte = new sectionTableEntry();
		secte->SECTION_ID = s.second->getId();
		secte->SECTION_NAME_OFFSET = stringPool.getStringIndex(s.second->getName());
		secte->SECTION_BASE_ADDRESS = s.second->getBaseAddr();
		secte->SECTION_DATA_OFFSET = sectionDataOffset;
		secte->SECTION_SIZE = s.second->getSizeWithPools();
		sectionDataOffset += secte->SECTION_SIZE;
		secte->REL_DATA_TABLE_OFFSET = sectionDataOffset;
		secte->REL_DATA_TABLE_LENGTH = s.second->getRelocationTable().size();
		sectionDataOffset += secte->REL_DATA_TABLE_LENGTH * sizeof(relData);
		secTableOutput.push_back(secte);
	}
	mh->STRING_POOL_OFFSET = sectionDataOffset;

	fwrite(mh, sizeof(mainHeader), 1, output);
	for(symTableEntry* s : symTableOutput) {
		fwrite(s, sizeof(symTableEntry), 1, output);
		delete s;
	}
	for(sectionTableEntry* s : secTableOutput) {
		fwrite(s, sizeof(sectionTableEntry), 1, output);
		delete s;
	}
	for(auto& s : secTablePrint) {
		fwrite(s.second->getData(), sizeof(uint8_t), s.second->getSizeWithPools(), output);
		fwrite(s.second->getRelocationTable().data(), sizeof(relData), s.second->getRelocationTable().size(), output);
	}
	fwrite(stringPool.getStringPool().data(), sizeof(uint8_t), stringPool.getStringPool().size(), output);

	delete mh;
}


