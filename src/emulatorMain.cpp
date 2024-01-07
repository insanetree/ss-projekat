#include "global.hpp"
#include "emulator.hpp"

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr<<"No input file provided"<<std::endl;
		return -1;
	}
	FILE* f = fopen(argv[1], "rb");
	if(!f) {
		std::cerr<<"File "<<argv[1]<<" could not be opened"<<std::endl;
		return -2;
	}
	Emulator::initEmulator(f);
	fclose(f);
	Emulator::cpuRoutine();
	Emulator::printRegisters();
	return 0;
}