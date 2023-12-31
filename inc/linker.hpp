#ifndef LINKER_HPP
#define LINKER_HPP

#include "global.hpp"

class ObjectFile;
class Symbol;
class Section;

class Linker {
public:
	static void loadObjectFile(FILE*);
	static int32_t linkObjectFiles();
	static void writeRelocatableFile(const std::string&);
	static void writeExecutableFile(const std::string&);
	
private:
	static int32_t importSymbols();
	static int32_t importAndMergeSections();

	static std::vector<ObjectFile*> objectFiles;
	static std::map<std::string, Symbol*> globalSymbolTable;
	static std::map<std::string, Section*> globalSectionTable;

	static std::map<uint32_t, uint32_t> relDataSymbolIdReplacementTable;
	static std::map<uint32_t, uint32_t> relDataLocalSymbolAddendIncrement;
	static std::map<Section*, Section*> symbolSectionPointerReplacementTable;
};

#endif