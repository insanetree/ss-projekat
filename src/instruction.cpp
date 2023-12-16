#include "instruction.hpp"

std::set<std::string> Instruction::validKeywords{
	"halt",
	"int",
	"iret",
	"call",
	"ret",
	"jmp",
	"beq",
	"bne",
	"bgt",
	"push",
	"pop",
	"xchg",
	"add",
	"sub",
	"mul",
	"div",
	"not",
	"and",
	"or",
	"xor",
	"shl",
	"shr",
	"ld",
	"st",
	"csrrd",
	"csrwr"
};

uint32_t Instruction::getSize() {
	return 4;
}

int32_t Instruction::firstPass() {
	Section* currentSection = Assembler::getCurrentSection();
	if(!currentSection)
		return -1;
	currentSection->addStatement(this);
	Assembler::incrementLocationCounter(this->getSize());
	return 0;
}

bool Instruction::isValid() {
	if(Instruction::validKeywords.find(keyword) == Instruction::validKeywords.end())
		return false;
	if(keyword == "halt" || keyword == "int" || keyword == "iret" || keyword == "ret") {
		if(arguments.size()>0){
			return false;
		}
	}
	else if(keyword == "call" || keyword == "jmp") {
		if(arguments.size()!=1)
			return false;
	}
	else if(keyword == "push" || keyword == "pop" || keyword == "not") {
		if(arguments.size()!=1)
			return false;
		if(arguments[0]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "beq" || keyword == "bne" || keyword == "bgt") {
		if(arguments.size()!=3)
			return false;
		if(arguments[0]->type != REGISTER_VALUE || arguments[1]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "xchg" || keyword == "add" || keyword == "sub" || keyword == "mul" ||
				keyword == "div" || keyword == "and" || keyword == "or" || keyword == "xor" ||
				keyword == "shl" || keyword == "shr") {
		if(arguments.size()!=2)
			return false;
		if(arguments[0]->type != REGISTER_VALUE || arguments[1]->type != REGISTER_VALUE)
			return false;	
	}
	else if(keyword == "ld") {
		if(arguments.size()!=2)
			return false;
		if(arguments[1]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "st") {
		if(arguments.size()!=2)
			return false;
		if(arguments[0]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "csrrd") {
		if(arguments.size()!=2)
			return false;
		if(arguments[0]->type != REGISTER_VALUE_CSR || arguments[1]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "csrwr") {
		if(arguments.size()!=2)
			return false;
		if(arguments[0]->type != REGISTER_VALUE || arguments[1]->type != REGISTER_VALUE_CSR)
			return false;
	}
	return true;
}