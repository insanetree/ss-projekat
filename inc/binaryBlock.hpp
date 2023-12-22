#ifndef BINARYBLOCK_HPP
#define BINARYBLOCK_HPP

#include "global.hpp"

class BinaryBlock {
public:
	void putData(void*, size_t);
private:
	std::vector<uint8_t> binaryData;
};

#endif