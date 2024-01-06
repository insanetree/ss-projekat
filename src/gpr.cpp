#include "gpr.hpp"

Gpr::Gpr() {
	registers.fill(0);
}

uint32_t& Gpr::operator[](uint32_t index) {
	if(index > 15)
		throw std::out_of_range("Invalid register used");
	if(index == 0)
		throw std::out_of_range("Register 0 can not be used as lvalue");
	return registers[index-1];
}

uint32_t Gpr::operator[](uint32_t index) const {
	if(index > 15)
		throw std::out_of_range("Invalid register used");
	if(index == 0)
		return 0;
	return registers[index-1];
}