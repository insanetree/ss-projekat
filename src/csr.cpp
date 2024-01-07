#include "csr.hpp"

Csr::Csr() : status(~0x7), handler(0xffffffff), cause(0) {}

std::atomic<uint32_t>& Csr::operator[] (uint32_t index) {
	switch(index) {
		case 0:
			return status;
			break;
		case 1:
			return handler;
			break;
		case 2:
			return cause;
			break;
		default:
			throw std::out_of_range("Invalid CSR register");
			break;
	}
}

uint32_t Csr::operator[] (uint32_t index) const {
	switch(index) {
		case 0:
			return static_cast<uint32_t>(status);
			break;
		case 1:
			return static_cast<uint32_t>(handler);
			break;
		case 2:
			return static_cast<uint32_t>(cause);
			break;
		default:
			throw std::out_of_range("Invalid CSR register");
			break;
	}
}