#include "objectFile.hpp"

int main(int argc, char** argv) {
	FILE*f = fopen("handler.o", "rb");
	ObjectFile o(f);
	return 0;
}