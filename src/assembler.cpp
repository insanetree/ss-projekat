#include "global.hpp"

int main(int argc, char** argv) {
	if(argc != 4) {
		cerr<<"Incorrect number of arguments"<<endl \
		<<"Usage: assembler -o <output file> <input file>"<<endl;
		return -1;
	}
	ifstream input(argv[2], ios::in);
	if(!input.is_open()) {
		cerr<<"Input file not found"<<endl;
		return -2;
	}
	ofstream output(argv[3], ios::out | ios::binary);
	return 0;
}