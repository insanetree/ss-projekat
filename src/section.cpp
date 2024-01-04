#include "section.hpp"

int32_t Section::nextId = 1;
int32_t Section::copyId = 1;

Section::Section(const std::string& name) : id(nextId++), name(name) {}

Section::Section(const Section& section) {
	id = copyId++;
	name = section.name;
	base = section.base;
	size = section.size;
	binaryData = section.binaryData;
	relocationTable = section.relocationTable;
}

uint32_t Section::getSize() const {
	return size;
}

void Section::setSize(uint32_t size) {
	this->size = size;
}

int32_t Section::getId() const {
	return id;
}

const std::string& Section::getName() const {
	return name;
}

void Section::addStatement(Statement* statement) {
	statements.push_back(statement);
}

void Section::addSymbolToPool(std::string& symbol, uint32_t offset) {
	symbolPool[symbol] = offset;
}

void Section::addLiteralToPool(uint32_t literal, uint32_t offset) {
	literalPool[literal] = offset;
} 

uint32_t Section::getRelativeOffsetToSymbol(const std::string& symbol) {
	return size - locationCounter + symbolPool[symbol];
}

uint32_t Section::getRelativeOffsetToLiteral(uint32_t literal) {
	return size - locationCounter + symbolPool.size()*sizeof(uint32_t) + literalPool[literal];
}

int32_t Section::secondPass() {
	uint32_t offset = 0, value;
	for(auto& sym : symbolPool) {
		sym.second = offset;
		if(Assembler::getSymbolTable().find(sym.first) == Assembler::getSymbolTable().end()) {
			std::cerr<<"Symbol "<<sym.first<<" is not defined"<<std::endl;
			return -1;
		}
		Symbol* s = Assembler::getSymbolTable()[sym.first];
		putRelocationData(offset+size, s);
		offset += sizeof(uint32_t);
	}
	offset = 0;
	for(auto& lit : literalPool) {
		lit.second = offset;
		offset += sizeof(uint32_t);
	}

	for(Statement* s : statements) {
		if(s->secondPass()) {
			return -1;
		}
	}

	for(auto& sym : symbolPool) {
		Symbol* s = Assembler::getSymbolTable()[sym.first];
		value = (s->getType()==COMMON)?(s->getValue()):(0);
		binaryData.putData(&value, sizeof(uint32_t));
	}
	for(auto& lit : literalPool) {
		value = lit.first;
		binaryData.putData(&value, sizeof(uint32_t));
	}
	return 0;
}

void Section::incrementLocationCounter(uint32_t increment) {
	locationCounter += increment;
}

uint32_t Section::getLocationCounter() {
	return locationCounter;
}

void Section::putData(void* ptr, size_t size) {
	binaryData.putData(ptr, size);
}

void Section::putDataReverse(void* ptr, size_t size) {
	binaryData.putDataReverse(ptr, size);
}

void Section::putRelocationData(uint32_t offset, Symbol* symbol) {
	if(symbol->getType() == COMMON)
		return;
	if(symbol->isGlobal()) {
		relocationTable.push_back({offset, symbol->getId(), 0});
	} else {
		relocationTable.push_back({offset, Assembler::getSymbolTable()[getName()]->getId(), symbol->getValue()});
	}
}

uint32_t Section::getBaseAddr() const {
	return base;
}

void Section::setBaseAddr(uint32_t base) {
	this->base = base;
	baseSet = true;
}

bool Section::isBaseAddrSet() const {
	return baseSet;
}

uint32_t Section::getSizeWithPools() const {
	return size + (literalPool.size() + symbolPool.size())*sizeof(uint32_t);
}

uint8_t* Section::getData() {
	return binaryData.getData();
}

std::vector<relData>& Section::getRelocationTable() {
	return relocationTable;
}

uint32_t Section::getIdOffset() {
	return nextId-1;
}