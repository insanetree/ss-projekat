#include "symbol.hpp"
#include "section.hpp"

uint32_t Symbol::next_id = 1;

Symbol::Symbol(const std::string& name, uint32_t value, bool global, Section* section, symbolType type)
: name(name), value(value), section(section), global(global), type(type)
{

}

uint32_t Symbol::getID() const {
	return this->id;
}

std::string Symbol::getName() const {
	return this->name;
}

void Symbol::setName(std::string name) {
	this->name = name;
}

uint32_t Symbol::getValue() const {
	return this->value;
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