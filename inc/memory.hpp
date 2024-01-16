#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "global.hpp"

const uint32_t TERM_OUT = 0xffffff00;
const uint32_t TERM_IN  = 0xffffff04;
const uint32_t TIM_CFG  = 0xffffff10;

class Memory {
public:
	Memory();
	uint8_t get8(uint32_t);
	void put8(uint32_t, uint8_t);
	uint32_t get32(uint32_t);
	void put32(uint32_t, uint32_t);
private:
	struct Page {
		std::array<uint8_t, 0x100> frame;
		Page();
	};
	struct PMT2 {
		std::array<Page*, 0x100> page2;
		PMT2();
	};
	struct PMT1 {
		std::array<PMT2*, 0x100> page1;
		PMT1();
	};
	struct PMT0 {
		std::array<PMT1*, 0x100> page0;
		PMT0();
	};
	PMT0* pmt0;
	std::recursive_mutex monitorMutex;
	std::condition_variable_any termOutRead;
	bool termOutSet;
	void allocatePage(uint32_t);
};

#endif