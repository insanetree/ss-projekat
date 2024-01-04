#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"

ObjectFile::ObjectFile(FILE* file) : file(file) {
	fread(&mh, sizeof(mainHeader), 1, file);
	stringPool = new char[mh.STRING_POOL_SIZE];
	fseek(file, mh.STRING_POOL_OFFSET, SEEK_SET);
	fread(stringPool, sizeof(char), mh.STRING_POOL_SIZE, file);
	symbolIdOffset = Symbol::getIdOffset();
	sectionIdOffset = Section::getIdOffset();
	populateSectionTable();
	populateSymbolTable();
	fclose(file);
	file = nullptr;
}

void ObjectFile::populateSymbolTable() {
	fseek(file, mh.SYMBOL_TABLE_OFFSET, SEEK_SET);
	symTableEntry ste;
	Symbol* sym;
	for(uint32_t i = 0 ; i < mh.SYMBOL_TABLE_LENGTH ; i++) {
		fread(&ste, sizeof(symTableEntry), 1, file);
		sym = new Symbol(std::string(stringPool+ste.SYMBOL_NAME_OFFSET), ste.SYMBOL_VALUE, (bool)ste.GLOBAL, nullptr, (symbolType)ste.SYMBOL_TYPE);
		if(localSectionTableById.find(ste.SECTION_ID + sectionIdOffset) != localSectionTableById.end()) 
			sym->setSection(localSectionTableById[ste.SECTION_ID+sectionIdOffset]);
		localSymbolTableByName.insert({sym->getName(), sym});
		localSymbolTableById.insert({sym->getId(), sym});
		if(sym->isGlobal() && sym->getSection() != nullptr) exportedSymbols.insert({sym->getId(), sym});
		if(sym->isGlobal() && sym->getSectionId() == -1) importedSymbols.insert({sym->getName(), sym});
		if(sym->getType() == SECTION) sectionSymbols.insert({sym->getId(), sym});
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
			rte.SYMBOL_ID += symbolIdOffset;
			sec->getRelocationTable().push_back(rte);
		}
		localSectionTableByName.insert({sec->getName(), sec});
		localSectionTableById.insert({sec->getId(), sec});
		delete[] sectionData;
	}
}

std::map<uint32_t, Symbol*>& ObjectFile::exportGlobalSymbols() {
	return exportedSymbols;
}

std::map<uint32_t, Section*>& ObjectFile::exportSections() {
	return localSectionTableById;
}

std::map<uint32_t, Symbol*>& ObjectFile::exportSectionSymbols() {
	return sectionSymbols;
}

std::map<std::string, Symbol*>& ObjectFile::getUnresolvedSymbols() {
	return importedSymbols;
}