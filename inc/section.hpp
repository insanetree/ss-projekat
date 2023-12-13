#ifndef SECTION_HPP
#define SECTION_HPP

#include "global.hpp"

class Section {
public:
private:
	int32_t id;
	std::string name;
	uint32_t base;
	uint32_t size;
};

#endif