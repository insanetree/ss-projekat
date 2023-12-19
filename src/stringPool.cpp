#include "stringPool.hpp"

void StringPool::putString(const std::string& str) {
	if(stringIndex.find(str) != stringIndex.end())
		return;
	stringIndex.insert({str, stringPool.size()});
	const char* cstr = str.c_str();
	for(uint64_t i = 0 ; i <= str.length() ; i++)
		stringPool.push_back(*(cstr+i));
}

int32_t StringPool::getStringIndex(const std::string& str) {
	if(stringIndex.find(str) == stringIndex.end())
		return -1;
	return stringIndex[str];
}