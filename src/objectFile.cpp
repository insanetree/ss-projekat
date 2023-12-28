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
	uint8_t* sectionData;
	relData rte;
	sectionTableEntry ste;
	Section* sec;
	for(uint32_t i = 0 ; i < mh.SECTION_TABLE_LENGTH ; i++) {
		fseek(file, mh.SECTION_TABLE_OFFSET+i*sizeof(sectionTableEntry), SEEK_SET);
		fread(&ste, sizeof(sectionTableEntry), 1, file);
		sec = new Section(std::string(stringPool+ste.SECTION_NAME_OFFSET));
		sec->setSize(ste.SECTION_SIZE);
		sectionData = new uint8_t[ste.SECTION_SIZE];
		fseek(file, ste.SECTION_DATA_OFFSET, SEEK_SET);
		fread(sectionData, sizeof(uint8_t), ste.SECTION_SIZE, file);
		sec->putData(sectionData, ste.SECTION_SIZE);
		fseek(file, ste.REL_DATA_TABLE_OFFSET, SEEK_SET);
		for(uint32_t j = 0 ; j < ste.REL_DATA_TABLE_LENGTH ; j++) {
			fread(&rte, sizeof(relData), 1, file);
			sec->getRelocationTable().push_back(rte);
		}
		delete[] sectionData;
	}
}