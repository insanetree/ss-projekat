#ifndef STRINGPOOL_HPP
#define STRINGPOOL_HPP

#include "global.hpp"

class StringPool {
public:
	void putString(const std::string&);
	int32_t getStringIndex(const std::string&);
private:
	std::vector<char> stringPool;
	std::map<std::string, int32_t> stringIndex;
};

#endif