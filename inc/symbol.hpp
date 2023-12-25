#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "global.hpp"

class Symbol{
public:
	Symbol(const std::string& name, uint32_t value, bool global, int32_t section, symbolType type);
	uint32_t getID() const;
	std::string getName() const;
	void setName(std::string name);
	uint32_t getValue() const;
	void setValue(uint32_t value);
	bool isGlobal() const;
	void setGlobal(bool global);
	int32_t getSection() const;
	void setSection(int32_t section);
	symbolType getType() const;
	void setType(symbolType);
protected:
private:
	static uint32_t next_id;
	uint32_t id = next_id++;
	std::string name;
	uint32_t value;
	int32_t section; //-1: UNKNOWN 0: COMMON else:sectionId
	bool global;
	symbolType type;
};

#endif