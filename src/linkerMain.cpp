#include "objectFile.hpp"
#include "linker.hpp"
#include "global.hpp"

int main(int argc, char** argv) {
	bool hex = false;
	bool relocatable = false;
	bool output = false;
	std::map<std::string, uint32_t> sectionPlacement;
	std::string outputFilename;
	for(int i = 1 ; i < argc ; i++) {
		if(strcmp(argv[i], "-hex")==0) {
			if(hex)
				return -1;
			hex = true;
		}
		else if(strcmp(argv[i], "-relocatable")==0) {
			if(relocatable)
				return -2;
			relocatable = true;
		}
		else if(strcmp(argv[i], "-o")==0) {
			if(output)
				return -3;
			output = true;
			outputFilename = argv[++i];
		} else if(strncmp(argv[i], "-place=", 7)==0) {
			std::string sectionName = strtok(argv[i]+7, "@");
			uint32_t placement = strtoul(strtok(NULL, "@"), nullptr, 16);
			if(sectionPlacement.find(sectionName)!=sectionPlacement.end())
				return -4;
			sectionPlacement[sectionName] = placement;
		} else if(output) {
			FILE* fin = fopen(argv[i], "rb");
			if(!fin) {
				std::cerr<<"Could not open file "<<argv[i]<<std::endl;
				return -5;
			}
			Linker::loadObjectFile(fin);
		}
	}

	if(!(hex^relocatable)) {
		std::cerr<<"Exactly one of options -hex and -relocatable need to be present"<<std::endl;
		return -6;
	}

	if(Linker::linkObjectFiles()) {
		std::cerr<<"Linker error"<<std::endl;
		return 1;
	}

	if(Linker::placeSections(sectionPlacement)) {
		std::cerr<<"Linker error"<<std::endl;
		return 2;
	}

	if(relocatable) {
		Linker::writeRelocatableFile(outputFilename);
	} else if(hex) {
		Linker::writeExecutableFile(outputFilename);
	}
	
	return 0;
}