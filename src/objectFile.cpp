#include "objectFile.hpp"
#include "symbol.hpp"
#include "section.hpp"

ObjectFile::ObjectFile(FILE* file) : file(file) {
    fread(&mh, sizeof(mainHeader), 1, file);
    populateSymbolTable();
    populateSectionTable();
}

void ObjectFile::populateSymbolTable() {
    
}

void ObjectFile::populateSectionTable() {

}