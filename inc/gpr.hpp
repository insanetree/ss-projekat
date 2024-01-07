#ifndef GPR_HPP
#define GPR_HPP

#include "global.hpp"

const uint32_t SP = 14;
const uint32_t PC = 15;

class Gpr {
public:
	Gpr();
	int32_t get(uint32_t) const;
	void set(uint32_t, uint32_t);
	void inc(uint32_t, uint32_t);
private:
	std::array<int32_t, 15> registers;
};

#endif