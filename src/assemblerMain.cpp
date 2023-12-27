#include "global.hpp"
#include "assembler.hpp"

int32_t main(int32_t argc, char** argv) {
	if(argc != 4) {
		std::cerr<<"Incorrect number of arguments"<<std::endl \
		<<"Usage: assembler -o <output file> <input file>"<<std::endl;
		return -1;
	}

	if(!Assembler::setInput(argv[3])) {
		std::cerr<<"Input file not found"<<std::endl;
		return -2;
	}
	if(!Assembler::setOutput(argv[2])) {
		std::cerr<<"Output file can not be generated"<<std::endl;
		return -3;
	}

	if(Assembler::firstPass()) {
		std::cerr<<"First pass failed"<<std::endl;
		return 1;
	}
	if(Assembler::secondPass()) {
		std::cerr<<"Second pass failed"<<std::endl;
	}
	return 0;
}