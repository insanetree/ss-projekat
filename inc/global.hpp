#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <algorithm>
#include <array>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <map>
#include <mutex>
#include <vector>
#include <set>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <cstdint>

class Symbol;

typedef enum {
	LITERAL_DOLLAR,
	SYMBOL_DOLLAR,
	LITERAL,
	SYMBOL,
	REGISTER_VALUE,
	REGISTER_MEMORY,
	REGISTER_LITERAL_MEMORY,
	REGISTER_SYMBOL_MEMORY,
	REGISTER_VALUE_CSR
}argumentType;

typedef enum : uint32_t {
	NOTYPE = 0,
	SECTION,
	COMMON
}symbolType;

typedef enum : uint32_t {
	R_32
}relType;

struct arg {
	argumentType type;
	int32_t registerNumber;
	int32_t literal;
	std::string symbol;
	struct arg* next;
};

struct mainHeader {
	uint32_t SYMBOL_TABLE_OFFSET;
	uint32_t SYMBOL_TABLE_LENGTH;
	uint32_t SECTION_TABLE_OFFSET;
	uint32_t SECTION_TABLE_LENGTH;
	uint32_t STRING_POOL_OFFSET;
	uint32_t STRING_POOL_SIZE;
};

struct symTableEntry {
	uint32_t SYMBOL_ID;
	uint32_t SYMBOL_NAME_OFFSET;
	uint32_t SYMBOL_VALUE;
	uint32_t SECTION_ID;
	uint32_t GLOBAL;
	uint32_t SYMBOL_TYPE;
};

struct sectionTableEntry {
	uint32_t SECTION_ID;
	uint32_t SECTION_NAME_OFFSET;
	uint32_t SECTION_BASE_ADDRESS;
	uint32_t SECTION_DATA_OFFSET;
	uint32_t SECTION_SIZE;
	uint32_t REL_DATA_TABLE_OFFSET;
	uint32_t REL_DATA_TABLE_LENGTH;
};

struct relData {
	uint32_t OFFSET;
	uint32_t SYMBOL_ID;
	uint32_t ADDEND;
};

int32_t getRegisterNum(const std::string&);

#endif