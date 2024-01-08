#include "linker.hpp"
#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"
#include "stringPool.hpp"

std::vector<ObjectFile*> Linker::objectFiles;
std::map<std::string, Symbol*> Linker::globalSymbolTableByName;
std::map<uint32_t, Symbol*> Linker::globalSymbolTableById;
std::map<std::string, Section*> Linker::globalSectionTableByName;
std::map<uint32_t, Section*> Linker::globalSectionTableById;
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
		for(auto& sym : globalSymbolTableByName)
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
			if(globalSymbolTableByName.find(localSymbol->getName()) != globalSymbolTableByName.end()) {
				std::cerr<<"Multiple definitions of global symbol "<<localSymbol->getName()<<std::endl;
				status += 1;
			}
			copySymbol = new Symbol(*localSymbol);
			globalSymbolTableByName.insert({localSymbol->getName(), copySymbol});
			globalSymbolTableById.insert({copySymbol->getId(), copySymbol});
			relDataSymbolIdReplacementTable.insert({copySymbol->getOldId(), copySymbol->getId()});
			relDataLocalSymbolAddendIncrement.insert({copySymbol->getOldId(), 0});
		}
	}
	for(ObjectFile* obj : objectFiles) {
		for(auto& sym : obj->getUnresolvedSymbols()) {
			localSymbol = sym.second;
			if(globalSymbolTableByName.find(localSymbol->getName()) == globalSymbolTableByName.end()) {
				std::cerr<<"Unresolved symbol "<<localSymbol->getName()<<std::endl;
				status += 1;
				continue;
			}
			copySymbol = globalSymbolTableByName[localSymbol->getName()];
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
			if(globalSymbolTableByName.find(localSectionSymbol->getName()) == globalSymbolTableByName.end()) {
				newSectionSymbol = new Symbol(*localSectionSymbol);
				globalSymbolTableByName.insert({localSectionSymbol->getName(), newSectionSymbol});
				globalSymbolTableById.insert({newSectionSymbol->getId(), newSectionSymbol});
				relDataLocalSymbolAddendIncrement.insert({localSectionSymbol->getId(), 0});
			} else {
				newSectionSymbol = globalSymbolTableByName[localSectionSymbol->getName()];
				relDataLocalSymbolAddendIncrement.insert({localSectionSymbol->getId(), globalSectionTableByName[localSectionSymbol->getName()]->getSize()});
			}
			relDataSymbolIdReplacementTable.insert({localSectionSymbol->getId(), newSectionSymbol->getId()});
		}
		for(auto& sec : obj->exportSections()) {
			localSection = sec.second;
			if(globalSectionTableByName.find(localSection->getName()) == globalSectionTableByName.end()) {
				newSection = new Section(*localSection);
				for(relData& rel : newSection->getRelocationTable()) {
					rel.ADDEND += relDataLocalSymbolAddendIncrement[rel.SYMBOL_ID];
					rel.SYMBOL_ID = relDataSymbolIdReplacementTable[rel.SYMBOL_ID];
				}
				globalSectionTableByName.insert({newSection->getName(), newSection});
				globalSectionTableById.insert({newSection->getId(), newSection});
			} else {
				newSection = globalSectionTableByName[localSection->getName()];
				for(relData rel : localSection->getRelocationTable()) {
					rel.OFFSET += newSection->getSize();
					rel.ADDEND += relDataLocalSymbolAddendIncrement[rel.SYMBOL_ID];
					rel.SYMBOL_ID = relDataSymbolIdReplacementTable[rel.SYMBOL_ID];
					newSection->getRelocationTable().push_back(rel);
				}
				for(auto& sym : obj->exportGlobalSymbols()) { //update global symbols
					Symbol* globalSymbol = globalSymbolTableByName[sym.second->getName()];
					globalSymbol->setValue(globalSymbol->getValue() + newSection->getSize());
				}
				newSection->putData(localSection->getData(), localSection->getSize());
				newSection->setSize(newSection->getSize() + localSection->getSize());
			}
			symbolSectionPointerReplacementTable.insert({localSection, newSection});
		}
	}
	return status;
}

bool Linker::writeExecutableFile(const std::string& filename) {
	std::map<uint32_t, Section*> sectionByAddress;
	for(auto& s : globalSectionTableById) {
		sectionByAddress.insert({s.second->getBaseAddr(), s.second});
	}
	FILE* outputText;
	FILE* outputBinary;
	outputText = fopen((filename+".txt").c_str(), "w");
	outputBinary = fopen(filename.c_str(), "wb");
	if(!outputBinary || !outputText)
		return false;
	
	constexpr uint32_t impossibleAddress = 0xffffffff;
	uint8_t* sectionData;
	uint32_t loadAddress;
	uint32_t printAddressNext = impossibleAddress, printAddressPrev;
	uint32_t sectionSize;
	Section* section;
	Symbol* symbol;
	for(auto& s : sectionByAddress) {
		section = s.second;
		loadAddress = s.first;
		sectionSize = section->getSize();
		sectionData = new uint8_t[section->getSize()];
		memcpy(sectionData, section->getData(), section->getSize());
		for(relData& r : section->getRelocationTable()) {
			symbol = globalSymbolTableById[r.SYMBOL_ID];
			*((uint32_t*)(sectionData+r.OFFSET)) = (symbol->getValue() + r.ADDEND);
		}
		fwrite(&loadAddress, sizeof(uint32_t), 1, outputBinary);
		fwrite(&sectionSize, sizeof(uint32_t), 1, outputBinary);
		fwrite(sectionData, sizeof(uint8_t), section->getSize(), outputBinary);


		printAddressPrev = printAddressNext;
		printAddressNext = loadAddress;
		for(uint32_t i = 0 ; i < section->getSize() ; i++) {
			if(printAddressNext%8 == 0 || printAddressNext != printAddressPrev+1 || printAddressNext < printAddressPrev) {
				if(printAddressPrev != impossibleAddress)
					fprintf(outputText, "\n");
				fprintf(outputText, "%08x:", printAddressNext);
			}
			fprintf(outputText, " %02x", sectionData[i]);
			printAddressPrev = printAddressNext;
			printAddressNext++;
		}

		delete sectionData;
	}
	fwrite(&impossibleAddress, sizeof(uint32_t), 1, outputBinary);
	fwrite(&impossibleAddress, sizeof(uint32_t), 1, outputBinary);
	fclose(outputBinary);
	fclose(outputText);
	return true;
}

void Linker::writeRelocatableFile(const std::string& filename) {
	FILE* outputText = fopen((filename+".txt").c_str(), "w");
	FILE* output = fopen(filename.c_str(), "wb");

	StringPool stringPool;

	std::map<uint32_t, Symbol*> symTablePrint;
	std::map<uint32_t, Section*> secTablePrint;
	for(auto& s : globalSymbolTableByName) {
		symTablePrint.insert({s.second->getId(), s.second});
		stringPool.putString(s.first);
	}
	for(auto& s : globalSectionTableByName){
		secTablePrint.insert({s.second->getId(), s.second});
		stringPool.putString(s.first);
		s.second->setBaseAddr(0);
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
	mh->SYMBOL_TABLE_LENGTH = globalSymbolTableByName.size();
	mh->SECTION_TABLE_OFFSET = mh->SYMBOL_TABLE_OFFSET + mh->SYMBOL_TABLE_LENGTH*sizeof(symTableEntry);
	mh->SECTION_TABLE_LENGTH = globalSectionTableByName.size();
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
	uint32_t sectionDataOffset = sizeof(mainHeader) + sizeof(symTableEntry)*globalSymbolTableByName.size() + sizeof(sectionTableEntry)*globalSectionTableByName.size();
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

int32_t Linker::placeSections(const std::map<std::string, uint32_t>& sectionPlacement) {
	int32_t status = 0;
	uint32_t nextBaseAddress = 0;
	Section* sec1;
	Section* sec2;
	for(auto& it1 : sectionPlacement) {
		if(globalSectionTableByName.find(it1.first)==globalSectionTableByName.end()) {
			std::cerr<<"Placing section "<<it1.first<<" which does not exist"<<std::endl;
			status++;
			continue;
		}
		sec1 = globalSectionTableByName[it1.first];
		sec1->setBaseAddr(it1.second);
		for(auto& it2 : sectionPlacement) {
			if(it1.first == it2.first)
				break;
			if(globalSectionTableByName.find(it2.first)==globalSectionTableByName.end())
				continue;
			sec2 = globalSectionTableByName[it2.first];
			if(
				sec1->getBaseAddr() == sec2->getBaseAddr()
				|| (sec1->getBaseAddr() < sec2->getBaseAddr() && sec1->getBaseAddr()+sec1->getSize() > sec2->getBaseAddr())
				|| (sec1->getBaseAddr() > sec2->getBaseAddr() && sec1->getBaseAddr() < sec2->getBaseAddr()+sec2->getSize())
				) {
				std::cerr<<"Sections "<<it2.first<<" and "<<it1.first<<" overlap"<<std::endl;
				status++;
				break;
			}
		}
		nextBaseAddress = std::max(nextBaseAddress, sec1->getBaseAddr()+sec1->getSize());
	}
	for(auto& it : globalSectionTableById) {
		sec1 = it.second;
		if(sec1->isBaseAddrSet()) continue;
		sec1->setBaseAddr(nextBaseAddress);
		nextBaseAddress += sec1->getSize();
		if(nextBaseAddress < sec1->getBaseAddr() || nextBaseAddress >= 0xffffff00) {
			std::cerr<<"Out of available memory for section "<<sec1->getName()<<std::endl;
			status++;
		}
	}
	return status;
}
