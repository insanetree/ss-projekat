#include "section.hpp"

int32_t Section::nextId = 1;

Section::Section(const std::string& name) : name(name) {}

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
		Symbol* s = Assembler::getSymbolTable()[sym.first];
		relocationTable.push_back({size+offset, s});
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
		value = 0;
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

void Section::putRelocationData(uint32_t offset, Symbol* symbol) {
	relocationTable.push_back({offset, symbol});
}