#ifndef SECTION_HPP
#define SECTION_HPP

#include "global.hpp"
#include "statement.hpp"

class Section {
public:
	Section(const std::string&);
	int32_t getId() const;
	uint32_t getSize() const;
	const std::string& getName() const;
	void setSize(uint32_t);
	void addStatement(Statement*);
	void addSymbolToPool(std::string&, uint32_t = 0);
	void addLiteralToPool(uint32_t, uint32_t = 0);
private:
	static int32_t nextId;
	int32_t id = nextId++;
	std::string name;
	uint32_t base = 0;
	uint32_t size = 0;
	std::vector<Statement*> statements;
	std::map<std::string, uint32_t> symbolPool;
	std::map<uint32_t, uint32_t> literalPool;
};

#endif