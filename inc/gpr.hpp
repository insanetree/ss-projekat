#ifndef GPR_HPP
#define GPR_HPP

#include "global.hpp"

class Gpr {
public:
	Gpr();
	uint32_t& operator[](uint32_t);
	uint32_t operator[](uint32_t) const;
private:
	std::array<uint32_t, 15> registers;
};

#endif