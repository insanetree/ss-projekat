#ifndef STRINGPOOL_HPP
#define STRINGPOOL_HPP

#include "global.hpp"

class StringPool {
public:
	void putString(const std::string&);
	int32_t getStringIndex(const std::string&);
	std::vector<uint8_t>& getStringPool();
private:
	std::vector<uint8_t> stringPool;
	std::map<std::string, int32_t> stringIndex;
};

#endif