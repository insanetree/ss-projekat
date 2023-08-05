#include "symbol.hpp"

unsigned int Symbol::next_id = 0;

unsigned int Symbol::getID() const {
	return this->id;
}

string Symbol::getName() const {
	return this->name;
}

void Symbol::setName(string name) {
	this->name = name;
}

unsigned int Symbol::getValue() const {
	return this->value;
}

void Symbol::setValue(unsigned int value) {
	this->value = value;
}

unsigned int Symbol::getSize() const {
	return this->size;
}

void Symbol::setSize(unsigned int size) {
	this->size = size;
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