#include "memory.hpp"

Memory::PMT0::PMT0() {
	page0.fill(nullptr);
}

Memory::PMT1::PMT1() {
	page1.fill(nullptr);
}

Memory::PMT2::PMT2() {
	page2.fill(nullptr);
}

Memory::Page::Page() {
	frame.fill(0);
}

Memory::Memory() {
	pmt0 = new PMT0();
}

void Memory::allocatePage(uint32_t address) {
	uint8_t page0, page1, page2;
	PMT1** pmt1;
	PMT2** pmt2;
	Page** page;
	page0 = (address & 0xff000000) >> 24;
	page1 = (address & 0x00ff0000) >> 16;
	page2 = (address & 0x0000ff00) >> 8;
	pmt1 = &(pmt0->page0[page0]);
	if(!(*pmt1))
		*pmt1 = new PMT1();
	pmt2 = &((*pmt1)->page1[page1]);
	if(!(*pmt2))
		*pmt2 = new PMT2();
	page = &((*pmt2)->page2[page2]);
	if(!(*page))
		*page = new Page(); 
}

uint8_t Memory::get8(uint32_t address) {
	std::unique_lock<std::recursive_mutex> lock(monitorMutex);
	allocatePage(address);
	uint8_t page0, page1, page2, offset;
	page0 = (address & 0xff000000) >> 24;
	page1 = (address & 0x00ff0000) >> 16;
	page2 = (address & 0x0000ff00) >> 8;
	offset = (address & 0x000000ff);
	return pmt0->page0[page0]->page1[page1]->page2[page2]->frame[offset];
}

void Memory::put8(uint32_t address, uint8_t value) {
	std::unique_lock<std::recursive_mutex> lock(monitorMutex);
	allocatePage(address);
	uint8_t page0, page1, page2, offset;
	page0 = (address & 0xff000000) >> 24;
	page1 = (address & 0x00ff0000) >> 16;
	page2 = (address & 0x0000ff00) >> 8;
	offset = (address & 0x000000ff);
	pmt0->page0[page0]->page1[page1]->page2[page2]->frame[offset] = value;
}

uint32_t Memory::get32(uint32_t address) {
	std::unique_lock<std::recursive_mutex> lock(monitorMutex);
	return (get8(address+3)<<24) | (get8(address+2)<<16) | (get8(address+1)<<8) | (get8(address));
}

void Memory::put32(uint32_t address, uint32_t value) {
	std::unique_lock<std::recursive_mutex> lock(monitorMutex);
	uint32_t value0, value1, value2, value3;
	value0 = (value & 0x000000ff);
	value1 = (value & 0x0000ff00) >> 8;
	value2 = (value & 0x00ff0000) >> 16;
	value3 = (value & 0xff000000) >> 24;
	put8(address, value0);
	put8(address+1, value1);
	put8(address+2, value2);
	put8(address+3, value3);
}