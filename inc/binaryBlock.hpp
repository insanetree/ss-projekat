#ifndef BINARYBLOCK_HPP
#define BINARYBLOCK_HPP

#include "global.hpp"

class BinaryBlock {
public:
	void putData(void*, size_t);
	void putDataReverse(void*, size_t);
	uint8_t* getData();
private:
	std::vector<uint8_t> binaryData;
};

#endif