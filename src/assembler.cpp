#include "global.hpp"
#include "assembler.hpp"

ifstream Assembler::input = ifstream();
ofstream Assembler::output = ofstream();
unordered_map<string, Symbol> Assembler::symbolTable;
unsigned int Assembler::locationCounter = 0;

bool Assembler::setInput(string filename) {
	input.open(filename, ios::in);
	return input.is_open();
}

bool Assembler::setOutput(string filename) {
	output.open(filename, ios::out | ios::binary);
	return output.is_open();
}

void Assembler::firstPass() {
	string line;
	smatch m;
	bool patternFound;
	regex clearComments("\\s*#.*");
	while(!input.eof()) {
		getline(input, line);
		if(regex_search(line, m, clearComments)) {
			line = m.prefix().str();
		}
		if(line.empty()) {
			continue;
		}
		cout << line << endl;
	}
	cout<<"**************"<<endl;
}

void Assembler::secondPass() {
	input.clear();
	input.seekg(0);

	string line;
	while(!input.eof()) {
		getline(input, line);
		cout << line << endl;
	}
}

int main(int argc, char** argv) {
	if(argc != 4) {
		cerr<<"Incorrect number of arguments"<<endl \
		<<"Usage: assembler -o <output file> <input file>"<<endl;
		return -1;
	}

	if(!Assembler::setInput(argv[3])) {
		cerr<<"Input file not found"<<endl;
		return -2;
	}
	if(!Assembler::setOutput(argv[2])) {
		cerr<<"Output file can not be generated"<<endl;
		return -3;
	}

	Assembler::firstPass();
	Assembler::secondPass();

	return 0;
}