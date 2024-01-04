#ifndef OBJECTFILE_HPP
#define OBJECTFILE_HPP

#include "global.hpp"

class Symbol;
class Section;

class ObjectFile {
public:
	ObjectFile(FILE*);
	std::map<uint32_t, Symbol*>& exportGlobalSymbols();
	std::map<uint32_t, Symbol*>& exportSectionSymbols();
	std::map<std::string, Symbol*>& getUnresolvedSymbols();
	std::map<uint32_t, Section*>& exportSections();

private:
	void populateSymbolTable();
	void populateSectionTable();

	FILE* file;
	mainHeader mh;
	char* stringPool;
	uint32_t symbolIdOffset;
	uint32_t sectionIdOffset;

	std::map<std::string, Symbol*> localSymbolTableByName;
	std::map<uint32_t, Symbol*> localSymbolTableById;
	std::map<uint32_t, Symbol*> exportedSymbols;
	std::map<uint32_t, Symbol*> sectionSymbols;
	std::map<std::string, Symbol*> importedSymbols;

	std::map<std::string, Section*> localSectionTableByName;
	std::map<uint32_t, Section*> localSectionTableById;
};

#endif