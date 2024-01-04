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
	static int32_t placeSections(const std::map<std::string, uint32_t>&);
	static bool writeExecutableFile(const std::string&);
	
private:
	static int32_t importSymbols();
	static int32_t importAndMergeSections();

	static std::vector<ObjectFile*> objectFiles;
	static std::map<std::string, Symbol*> globalSymbolTableByName;
	static std::map<uint32_t, Symbol*> globalSymbolTableById;
	static std::map<std::string, Section*> globalSectionTableByName;
	static std::map<uint32_t, Section*> globalSectionTableById;

	static std::map<uint32_t, uint32_t> relDataSymbolIdReplacementTable;
	static std::map<uint32_t, uint32_t> relDataLocalSymbolAddendIncrement;
	static std::map<Section*, Section*> symbolSectionPointerReplacementTable;
};

#endif