#include "symbol.hpp"

unsigned int Symbol::next_id = 0;

Symbol::Symbol(const std::string& name, unsigned int value, bool global=false, int section=-1)
: name(name), value(value), global(global), section(section)
{

}

unsigned int Symbol::getID() const {
	return this->id;
}

std::string Symbol::getName() const {
	return this->name;
}

void Symbol::setName(std::string name) {
	this->name = name;
}

unsigned int Symbol::getValue() const {
	return this->value;
}

void Symbol::setValue(unsigned int value) {
	this->value = value;
}

bool Symbol::isGlobal() const {
	return this->global;
}

void Symbol::setGlobal(bool global) {
	this->global = global;
}

int Symbol::getSection() const {
	return this->section;
}

void Symbol::setSection(int section) {
	this->section = section;
}