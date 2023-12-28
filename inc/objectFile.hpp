#ifndef OBJECTFILE_HPP
#define OBJECTFILE_HPP

#include "global.hpp"

class Symbol;
class Section;

class ObjectFile {
public:
	ObjectFile(FILE*);

private:
	void populateSymbolTable();
	void populateSectionTable();

	FILE* file;
	mainHeader mh;
	char* stringPool;
	std::map<std::string, Symbol*> localSymbolTable;
	std::map<std::string, Section*> localSectionTable;
};

#endif