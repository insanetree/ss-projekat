#ifndef SECTION_HPP
#define SECTION_HPP

#include "global.hpp"
#include "statement.hpp"
#include "binaryBlock.hpp"

class Section {
public:
	Section(const std::string&);
	Section(const Section&);
	int32_t getId() const;
	uint32_t getSize() const;
	uint32_t getSizeWithPools() const;
	const std::string& getName() const;
	void setSize(uint32_t);
	uint32_t getBaseAddr() const;
	void setBaseAddr(uint32_t);
	bool isBaseAddrSet() const;
	void addStatement(Statement*);
	void addSymbolToPool(std::string&, uint32_t = 0);
	void addLiteralToPool(uint32_t, uint32_t = 0);
	uint32_t getRelativeOffsetToSymbol(const std::string&);
	uint32_t getRelativeOffsetToLiteral(uint32_t);
	int32_t secondPass();
	void incrementLocationCounter(uint32_t);
	uint32_t getLocationCounter();
	void putData(void*, size_t);
	void putDataReverse(void*, size_t);
	void putRelocationData(uint32_t, Symbol*);
	std::vector<relData>& getRelocationTable();
	uint8_t* getData();

	static uint32_t getIdOffset();
private:
	static int32_t nextId;
	static int32_t copyId;
	int32_t id;
	std::string name;
	uint32_t base = 0;
	bool baseSet = false;
	uint32_t size = 0;
	uint32_t locationCounter = 0;
	std::vector<Statement*> statements;
	std::map<std::string, uint32_t> symbolPool;
	std::map<uint32_t, uint32_t> literalPool;
	BinaryBlock binaryData;
	std::vector<relData> relocationTable;
};

#endif