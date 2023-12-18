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