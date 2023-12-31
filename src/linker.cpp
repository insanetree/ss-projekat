#include "linker.hpp"
#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"
#include "stringPool.hpp"

std::vector<ObjectFile*> Linker::objectFiles;
std::map<std::string, Symbol*> Linker::globalSymbolTable;
std::map<std::string, Section*> Linker::globalSectionTable;
std::map<uint32_t, uint32_t> Linker::relDataSymbolIdReplacementTable;
std::map<Section*, Section*> Linker::symbolSectionPointerReplacementTable;
std::map<uint32_t, uint32_t> Linker::relDataLocalSymbolAddendIncrement;

void Linker::loadObjectFile(FILE* f) {
	objectFiles.push_back(new ObjectFile(f));
}

int32_t Linker::linkObjectFiles() {
	int32_t status = 0;
	status += importSymbols();
	if(!status)
		status += importAndMergeSections();
	if(!status) {
		for(auto& sym : globalSymbolTable)
			sym.second->setSection(symbolSectionPointerReplacementTable[sym.second->getSection()]);
	}
	return status;
}

int32_t Linker::importSymbols() {
	int32_t status = 0;
	Symbol* copySymbol;
	Symbol* localSymbol;
	for(ObjectFile* obj : objectFiles) {
		for(auto& sym : obj->exportGlobalSymbols()) {
			localSymbol = sym.second;
			if(globalSymbolTable.find(sym.first) != globalSymbolTable.end()) {
				std::cerr<<"Multiple definitions of global symbol "<<sym.first<<std::endl;
				status += 1;
			}
			copySymbol = new Symbol(*localSymbol);
			globalSymbolTable.insert({sym.first, copySymbol});
			relDataSymbolIdReplacementTable.insert({copySymbol->getOldId(), copySymbol->getId()});
			relDataLocalSymbolAddendIncrement.insert({copySymbol->getOldId(), 0});
		}
	}
	for(ObjectFile* obj : objectFiles) {
		for(auto& sym : obj->getUnresolvedSymbols()) {
			localSymbol = sym.second;
			if(globalSymbolTable.find(sym.first) == globalSymbolTable.end()) {
				std::cerr<<"Unresolved symbol "<<sym.first<<std::endl;
				status += 1;
				continue;
			}
			copySymbol = globalSymbolTable[sym.first];
			relDataSymbolIdReplacementTable.insert({localSymbol->getId(), copySymbol->getId()});
			relDataLocalSymbolAddendIncrement.insert({localSymbol->getId(), 0});
		}
	}
	return status;
}

int32_t Linker::importAndMergeSections() {
	int32_t status = 0;
	Section* newSection;
	Section* localSection;
	Symbol* newSectionSymbol;
	Symbol* localSectionSymbol;
	for(ObjectFile* obj : objectFiles) {
		for(auto& sym : obj->exportSectionSymbols()) {
			localSectionSymbol = sym.second;
			if(globalSymbolTable.find(sym.first) == globalSymbolTable.end()) {
				newSectionSymbol = new Symbol(*localSectionSymbol);
				globalSymbolTable.insert({sym.first, newSectionSymbol});
				relDataLocalSymbolAddendIncrement.insert({localSectionSymbol->getId(), 0});
			} else {
				newSectionSymbol = globalSymbolTable[sym.first];
				relDataLocalSymbolAddendIncrement.insert({localSectionSymbol->getId(), globalSectionTable[sym.first]->getSize()});
			}
			relDataSymbolIdReplacementTable.insert({localSectionSymbol->getId(), newSectionSymbol->getId()});
		}
		for(auto& sec : obj->exportSections()) {
			localSection = sec.second;
			if(globalSectionTable.find(sec.first) == globalSectionTable.end()) {
				newSection = new Section(*localSection);
				for(relData& rel : newSection->getRelocationTable()) {
					rel.ADDEND += relDataLocalSymbolAddendIncrement[rel.SYMBOL_ID];
					rel.SYMBOL_ID = relDataSymbolIdReplacementTable[rel.SYMBOL_ID];
				}
				globalSectionTable.insert({newSection->getName(), newSection});
			} else {
				newSection = globalSectionTable[sec.first];
				for(relData rel : localSection->getRelocationTable()) {
					rel.OFFSET += newSection->getSize();
					rel.ADDEND += relDataLocalSymbolAddendIncrement[rel.SYMBOL_ID];
					rel.SYMBOL_ID = relDataSymbolIdReplacementTable[rel.SYMBOL_ID];
					newSection->getRelocationTable().push_back(rel);
				}
				newSection->putData(localSection->getData(), localSection->getSize());
				newSection->setSize(newSection->getSize() + localSection->getSize());
			}
			symbolSectionPointerReplacementTable.insert({localSection, newSection});
		}
	}
	return status;
}

void Linker::writeExecutableFile(const std::string& filename) {

}

void Linker::writeRelocatableFile(const std::string& filename) {
	FILE* outputText = fopen((filename+".txt").c_str(), "w");
	FILE* output = fopen(filename.c_str(), "wb");

	StringPool stringPool;

	std::map<uint32_t, Symbol*> symTablePrint;
	std::map<uint32_t, Section*> secTablePrint;
	for(auto& s : globalSymbolTable) {
		symTablePrint.insert({s.second->getId(), s.second});
		stringPool.putString(s.first);
	}
	for(auto& s : globalSectionTable){
		secTablePrint.insert({s.second->getId(), s.second});
		stringPool.putString(s.first);
	}

	fprintf(outputText, "SYMBOL_TABLE\n");
	fprintf(outputText, "%10s %20s %10s %10s %10s %10s\n", "id", "name", "value", "section", "global", "type");
	for(auto& sym: symTablePrint) {
			fprintf(outputText, "%10d %20s 0x%08x %10d %10d %10s\n", 
				sym.second->getId(),
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
	fclose(outputText);

	mainHeader* mh = new mainHeader();
	mh->SYMBOL_TABLE_OFFSET = sizeof(mainHeader);
	mh->SYMBOL_TABLE_LENGTH = globalSymbolTable.size();
	mh->SECTION_TABLE_OFFSET = mh->SYMBOL_TABLE_OFFSET + mh->SYMBOL_TABLE_LENGTH*sizeof(symTableEntry);
	mh->SECTION_TABLE_LENGTH = globalSectionTable.size();
	mh->STRING_POOL_OFFSET = 0; //TODO
	mh->STRING_POOL_SIZE = stringPool.getStringPool().size();

	std::vector<symTableEntry*> symTableOutput;
	symTableEntry* symte = nullptr;
	for(auto& s : symTablePrint) {
		symte = new symTableEntry();
		symte->SYMBOL_ID = s.second->getId();
		symte->SYMBOL_NAME_OFFSET = stringPool.getStringIndex(s.second->getName());
		symte->SYMBOL_VALUE = s.second->getValue();
		symte->SECTION_ID = s.second->getSectionId();
		symte->GLOBAL = s.second->isGlobal();
		symte->SYMBOL_TYPE = s.second->getType();
		symTableOutput.push_back(symte);
	}

	std::vector<sectionTableEntry*> secTableOutput;
	sectionTableEntry* secte = nullptr;
	uint32_t sectionDataOffset = sizeof(mainHeader) + sizeof(symTableEntry)*globalSymbolTable.size() + sizeof(sectionTableEntry)*globalSectionTable.size();
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
	fclose(output);
}