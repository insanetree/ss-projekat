#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "global.hpp"
#include "assembler.hpp"
#include "section.hpp"
#include "symbol.hpp"

class Statement {
public:
	Statement(const std::string&, arg*, Section*);
	virtual ~Statement();
	const std::string& getKeyword() const;
	virtual uint32_t getSize() = 0;
	virtual bool isValid() = 0;
	virtual int32_t firstPass() = 0;
	virtual int32_t secondPass() = 0;
protected:
	std::string keyword;
	std::vector<arg*> arguments;
	Section* section;
};
#endif