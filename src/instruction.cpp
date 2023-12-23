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

Instruction::Instruction(const std::string& keyword, arg* argList, Section* section) : Statement(keyword, argList, section) {}

uint32_t Instruction::getSize() {
	if(keyword == "iret")
		return 8;
	return 4;
}

int32_t Instruction::firstPass() {
	if(!section)
		return -1;
	section->addStatement(this);
	Assembler::incrementLocationCounter(this->getSize());
	if(keyword == "ld") {
		if(arguments.front()->type == SYMBOL || arguments.front()->type == SYMBOL_DOLLAR) {
			if(Assembler::getSymbolTable()[arguments[0]->symbol]->getSection() != section->getId())
				section->addSymbolToPool(arguments.front()->symbol);
		}
		else if(arguments.front()->type == LITERAL || arguments.front()->type == LITERAL_DOLLAR) {
			section->addLiteralToPool(arguments.front()->literal);
		}
	}
	else if (keyword == "call" || keyword == "jmp" || keyword == "beq" ||
             keyword == "bne" || keyword == "bgt" || keyword == "st") {
		if(arguments.back()->type == SYMBOL || arguments.back()->type == SYMBOL_DOLLAR) {
			if(Assembler::getSymbolTable()[arguments.back()->symbol]->getSection() != section->getId())
				section->addSymbolToPool(arguments.back()->symbol);
		}
		else if(arguments.back()->type == LITERAL || arguments.back()->type == LITERAL_DOLLAR) {
			section->addLiteralToPool(arguments.back()->literal);
		}
	}
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
		if(arguments.size()!=1 || arguments[0]->type != SYMBOL || arguments[0]->type != LITERAL)
			return false;
	}
	else if(keyword == "push" || keyword == "pop" || keyword == "not") {
		if(arguments.size()!=1)
			return false;
		if(arguments[0]->type != REGISTER_VALUE)
			return false;
	}
	else if(keyword == "beq" || keyword == "bne" || keyword == "bgt") {
		if(arguments.size()!=3 || arguments[2]->type != SYMBOL || arguments[2]->type != LITERAL)
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

int32_t Instruction::secondPass() {
	uint32_t op, mod, regA, regB, regC, disp, write;
	section->incrementLocationCounter(4);
	if (keyword == "halt") {
		write = 0;
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "int") {
		op = 0b0001;
		mod = regA = regB = regC = disp = 0;
		write = (op<<28);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "iret") {
		//TODO
	}
	else if (keyword == "call") {
		op = 0b0010;
		regA = 15;
		regB = 0;
		regC = 0;
		if(arguments[0]->type == LITERAL) {
			mod = 0b0001;
			disp = section->getRelativeOffsetToLiteral(arguments[0]->literal);
		}
		else if(Assembler::getSymbolTable()[arguments[0]->symbol]->getSection() == section->getId()) {
			mod = 0b0000;
			disp = section->getRelativeOffsetToSymbol(arguments[0]->symbol);
		}
		else {
			mod = 0b0001;
			disp = section->getRelativeOffsetToSymbol(arguments[0]->symbol);
		}
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "ret") {
		op = 0b1001;
		mod = 0b0011;
		regA = 15;
		regB = 14;
		regC = 0;
		disp = 4;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "jmp") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1000;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else if(Assembler::getSymbolTable()[arguments.back()->symbol]->getSection() == section->getId()) {
			mod = 0b0000;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		else {
			mod = 0b1000;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}		
		regA = 15;
		regB = regC = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "beq") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1001;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else if(Assembler::getSymbolTable()[arguments.back()->symbol]->getSection() == section->getId()) {
			mod = 0b0001;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		else {
			mod = 0b1001;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "bne") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1010;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else if(Assembler::getSymbolTable()[arguments.back()->symbol]->getSection() == section->getId()) {
			mod = 0b0010;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		else {
			mod = 0b1010;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "bgt") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1011;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else if(Assembler::getSymbolTable()[arguments.back()->symbol]->getSection() == section->getId()) {
			mod = 0b0011;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		else {
			mod = 0b1011;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "push") {
	}
	else if (keyword == "pop") {
		op = 0b1001;
		mod = 0b0011;
		regA = arguments[0]->registerNumber;
		regB = 14;
		regC = 0;
		disp = 4;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "xchg") {
		op = 0b0100;
		mod = 0b0000;
		regA = 0;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "add") {
		op = 0b0101;
		mod = 0b0000;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "sub") {
		op = 0b0101;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));	
	}
	else if (keyword == "mul") {
		op = 0b0101;
		mod = 0b0010;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "div") {
		op = 0b0101;
		mod = 0b0011;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "not") {
		op = 0b0110;
		mod = 0b0000;
		regA = regB = arguments[0]->registerNumber;
		regC = 0;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "and") {
		op = 0b0110;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "or") {
		op = 0b0110;
		mod = 0b0010;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "xor") {
		op = 0b0110;
		mod = 0b0011;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "shl") {
		op = 0b0111;
		mod = 0b0000;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "shr") {
		op = 0b0111;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
	}
	else if (keyword == "ld") {
	}
	else if (keyword == "st") {
	}
	else if (keyword == "csrrd") {
	}
	else if (keyword == "csrwr") {
	}
	return 0;
}