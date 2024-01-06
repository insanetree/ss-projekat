#ifndef CSR_HPP
#define CSR_HPP

#include "global.hpp"

class Csr {
public:
	Csr();
	std::atomic<uint32_t>& operator[] (uint32_t);
	uint32_t operator[] (uint32_t) const;
private:
	std::atomic<uint32_t> status; // 0
	std::atomic<uint32_t> handler; // 1
	std::atomic<uint32_t> cause; // 2
};

#endif