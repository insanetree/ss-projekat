#ifndef GPR_HPP
#define GPR_HPP

#define SP 14
#define PC 15

#include "global.hpp"

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