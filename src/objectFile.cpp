#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"

ObjectFile::ObjectFile(FILE* file) : file(file) {
	fread(&mh, sizeof(mainHeader), 1, file);
	stringPool = new char[mh.STRING_POOL_SIZE];
	fseek(file, mh.STRING_POOL_OFFSET, SEEK_SET);
	fread(stringPool, sizeof(char), mh.STRING_POOL_SIZE, file);
	populateSymbolTable();
	populateSectionTable();
}

void ObjectFile::populateSymbolTable() {
	fseek(file, mh.SYMBOL_TABLE_OFFSET, SEEK_SET);
	symTableEntry ste;
	Symbol* sym;
	for(uint32_t i = 0 ; i < mh.SYMBOL_TABLE_LENGTH ; i++) {
		fread(&ste, sizeof(symTableEntry), 1, file);
		sym = new Symbol(std::string(stringPool+ste.SYMBOL_NAME_OFFSET), ste.SYMBOL_VALUE, (bool)ste.GLOBAL, ste.SECTION_ID, (symbolType)ste.SYMBOL_TYPE);
		localSymbolTable.insert({sym->getName(), sym});
	}
}

void ObjectFile::populateSectionTable() {

}