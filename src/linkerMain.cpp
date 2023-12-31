#include "objectFile.hpp"
#include "linker.hpp"

int main(int argc, char** argv) {
	FILE* f1 = fopen("output.hex", "rb");
	//FILE* f2 = fopen("main.o", "rb");
	Linker::loadObjectFile(f1);
	//Linker::loadObjectFile(f2);

	if(Linker::linkObjectFiles()) {
		std::cerr<<"Linker error"<<std::endl;
		return 1;
	}
	Linker::writeRelocatableFile(std::string("nigga.hex"));
	return 0;
}