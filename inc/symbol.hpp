#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "global.hpp"

class Section;

class Symbol{
public:
	Symbol(const std::string& name, uint32_t value, bool global, Section* section, symbolType type);
	Symbol(const Symbol&);
	uint32_t getId() const;
	uint32_t getOldId() const;
	std::string getName() const;
	void setName(std::string name);
	uint32_t getValue() const;
	void setValue(uint32_t value);
	bool isGlobal() const;
	void setGlobal(bool global);
	Section* getSection();
	int32_t getSectionId();
	void setSection(Section* section);
	symbolType getType() const;
	void setType(symbolType);

	static uint32_t getIdOffset();
protected:
private:
	static uint32_t next_id;
	static uint32_t copyId;
	uint32_t oldId;
	uint32_t id;
	std::string name;
	uint32_t value;
	Section* section;
	bool global;
	symbolType type;
};

#endif