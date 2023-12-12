#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "global.hpp"

class Symbol{
public:
	Symbol(const std::string& name, unsigned int value, bool global, int section);
	unsigned int getID() const;
	std::string getName() const;
	void setName(std::string name);
	unsigned int getValue() const;
	void setValue(unsigned int value);
	bool isGlobal() const;
	void setGlobal(bool global);
	int getSection() const;
	void setSection(int section);
protected:
private:
	static unsigned int next_id;
	unsigned int id = next_id++;
	std::string name;
	unsigned int value;
	bool global;
	int section; //-1 if UNKNOWN 
};

#endif