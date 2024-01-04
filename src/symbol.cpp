#include "symbol.hpp"
#include "section.hpp"

uint32_t Symbol::next_id = 1;
uint32_t Symbol::copyId = 1;

Symbol::Symbol(const std::string& name, uint32_t value, bool global, Section* section, symbolType type)
: id(next_id++), name(name), value(value), section(section), global(global), type(type)
{

}

Symbol::Symbol(const Symbol& symbol) {
	oldId = symbol.id;
	id = copyId++;
	name = symbol.name;
	value = symbol.value;
	section = symbol.section;
	global = symbol.global;
	type = symbol.type;
}

uint32_t Symbol::getId() const {
	return this->id;
}

uint32_t Symbol::getOldId() const {
	return this->oldId;
}

std::string Symbol::getName() const {
	return this->name;
}

void Symbol::setName(std::string name) {
	this->name = name;
}

uint32_t Symbol::getValue() const {
	uint32_t sectionBase = 0;
	if(section) {
		sectionBase = section->getBaseAddr();
	}
	return this->value + sectionBase;
}

void Symbol::setValue(uint32_t value) {
	this->value = value;
}

bool Symbol::isGlobal() const {
	return this->global;
}

void Symbol::setGlobal(bool global) {
	this->global = global;
}

Section* Symbol::getSection() {
	return this->section;
}

int32_t Symbol::getSectionId() {
	if(type == COMMON)
		return 0;
	if(section)
		return section->getId();
	return -1;
}

void Symbol::setSection(Section* section) {
	this->section = section;
}

symbolType Symbol::getType() const {
	return type;
}

void Symbol::setType(symbolType type) {
	this->type = type;
}

uint32_t Symbol::getIdOffset() {
	return next_id-1;
}