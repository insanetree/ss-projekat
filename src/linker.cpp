#include "linker.hpp"
#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"

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
		}
	}
	return status;
}