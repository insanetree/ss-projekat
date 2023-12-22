#include "binaryBlock.hpp"

void BinaryBlock::putData(void* ptr, size_t size) {
	for(size_t i = 0 ; i < size ; i++) {
		binaryData.push_back(*(static_cast<uint8_t*>(ptr)+i));
	}
}