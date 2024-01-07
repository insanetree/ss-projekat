#include "gpr.hpp"

Gpr::Gpr() {
	registers.fill(0);
}

void Gpr::set(uint32_t index, uint32_t value) {
	if(index > 15)
		throw std::out_of_range("Invalid register used");
	if(index == 0)
		throw std::out_of_range("Register 0 can not be used as lvalue");
	registers[index-1] = value;
}

void Gpr::inc(uint32_t index, uint32_t inc) {
	if(index > 15)
		throw std::out_of_range("Invalid register used");
	if(index == 0)
		throw std::out_of_range("Register 0 can not be used as lvalue");
	registers[index-1] += inc;
}

int32_t Gpr::get(uint32_t index) const {
	if(index > 15)
		throw std::out_of_range("Invalid register used");
	if(index == 0)
		return 0;
	return registers[index-1];
}