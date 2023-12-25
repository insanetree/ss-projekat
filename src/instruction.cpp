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
	if(keyword == "ld" && (arguments[0]->type == SYMBOL || arguments[0]->type == LITERAL))
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
		if(arguments[1]->type == SYMBOL_DOLLAR || arguments[1]->type == LITERAL_DOLLAR || arguments[1]->type == REGISTER_VALUE)
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
	int32_t op, mod, regA, regB, regC, disp;
	uint32_t write;
	section->incrementLocationCounter(4);
	if (keyword == "halt") {
		op = mod = regA = regB = regC = disp = 0;
	}
	else if (keyword == "int") {
		op = 0b0001;
		mod = regA = regB = regC = disp = 0;
		write = (op<<28);
	}
	else if (keyword == "iret") {
		op = 0b1001;
		mod = 0b0011;
		regA = 15;
		regB = 14;
		regC = 0;
		disp = 4;
		write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
		section->putDataReverse(&write, sizeof(uint32_t));
		section->incrementLocationCounter(4);
		mod = 0b0111;
		regA = 0;
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
		else {
			mod = 0b0001;
			disp = section->getRelativeOffsetToSymbol(arguments[0]->symbol);
		}
	}
	else if (keyword == "ret") {
		op = 0b1001;
		mod = 0b0011;
		regA = 15;
		regB = 14;
		regC = 0;
		disp = 4;
	}
	else if (keyword == "jmp") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1000;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else {
			mod = 0b1000;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}		
		regA = 15;
		regB = regC = 0;
	}
	else if (keyword == "beq") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1001;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else {
			mod = 0b1001;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
	}
	else if (keyword == "bne") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1010;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else {
			mod = 0b1010;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
	}
	else if (keyword == "bgt") {
		op = 0b0011;
		if(arguments.back()->type == LITERAL) {
			mod = 0b1011;
			disp = section->getRelativeOffsetToLiteral(arguments.back()->literal);
		}
		else {
			mod = 0b1011;
			disp = section->getRelativeOffsetToSymbol(arguments.back()->symbol);
		}
		regA = 15;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
	}
	else if (keyword == "push") {
		op = 0b1000;
		mod = 0b0001;
		regA = 14;
		regB = 0;
		regC = arguments[0]->registerNumber;
		disp = -4;
	}
	else if (keyword == "pop") {
		op = 0b1001;
		mod = 0b0011;
		regA = arguments[0]->registerNumber;
		regB = 14;
		regC = 0;
		disp = 4;
	}
	else if (keyword == "xchg") {
		op = 0b0100;
		mod = 0b0000;
		regA = 0;
		regB = arguments[0]->registerNumber;
		regC = arguments[1]->registerNumber;
		disp = 0;
	}
	else if (keyword == "add") {
		op = 0b0101;
		mod = 0b0000;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "sub") {
		op = 0b0101;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "mul") {
		op = 0b0101;
		mod = 0b0010;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "div") {
		op = 0b0101;
		mod = 0b0011;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "not") {
		op = 0b0110;
		mod = 0b0000;
		regA = regB = arguments[0]->registerNumber;
		regC = 0;
		disp = 0;
	}
	else if (keyword == "and") {
		op = 0b0110;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "or") {
		op = 0b0110;
		mod = 0b0010;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "xor") {
		op = 0b0110;
		mod = 0b0011;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "shl") {
		op = 0b0111;
		mod = 0b0000;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "shr") {
		op = 0b0111;
		mod = 0b0001;
		regA = regB = arguments[1]->registerNumber;
		regC = arguments[0]->registerNumber;
		disp = 0;
	}
	else if (keyword == "ld") {
		op = 0b1001;
		regA = arguments[1]->registerNumber;
		switch(arguments[0]->type) {
			case SYMBOL:
				mod = 0b0010;
				regB = 15;
				regC = 0;
				disp = section->getRelativeOffsetToSymbol(arguments[0]->symbol);
				write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
				section->putDataReverse(&write, sizeof(uint32_t));
				section->incrementLocationCounter(4);
				regB = regA;
				disp = 0;
			break;
			case SYMBOL_DOLLAR:
				mod = 0b0010;
				regB = 15;
				regC = 0;
				disp = section->getRelativeOffsetToSymbol(arguments[0]->symbol);
			break;
			case LITERAL:
				mod = 0b0010;
				regB = 15;
				regC = 0;
				disp = section->getRelativeOffsetToLiteral(arguments[0]->literal);
				write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
				section->putDataReverse(&write, sizeof(uint32_t));
				section->incrementLocationCounter(4);
				regB = regA;
				disp = 0;
			break;
			case LITERAL_DOLLAR:
				mod = 0b0010;
				regB = 15;
				regC = 0;
				disp = section->getRelativeOffsetToLiteral(arguments[0]->literal);
			break;
			case REGISTER_VALUE:
				mod = 0b0001;
				regB = arguments[0]->registerNumber;
				regC = 0;
				disp = 0;
			break;
			case REGISTER_MEMORY:
				mod = 0b0010;
				regB = arguments[0]->registerNumber;
				regC = 0;
				disp = 0;
			break;
			case REGISTER_SYMBOL_MEMORY:
				if(Assembler::getSymbolTable()[arguments[0]->symbol]->getSection() != 0) {
					std::cerr<<"Value of symbol "<<arguments[0]->symbol<<" can't be determined"<<std::endl;
					return -1;
				}
				mod = 0b0010;
				regB = arguments[0]->registerNumber;
				regC = 0;
				disp = Assembler::getSymbolTable()[arguments[0]->symbol]->getValue();
				if(disp < -0x800 || disp > 0x7ff) {
					std::cerr<<"Value of symbol "<<arguments[0]->symbol<<" larger than 12 bits signed"<<std::endl;
					return -1;
				}
			break;
			case REGISTER_LITERAL_MEMORY:
				mod = 0b0010;
				regB = arguments[0]->registerNumber;
				regC = 0;
				disp = arguments[0]->literal;
				if(disp & 0xfffff000) {
					std::cerr<<"Value of literal "<<arguments[0]->literal<<" larger than 12 bits signed"<<std::endl;
					return -1;
				}
			break;
			default:
				std::cerr<<"ld argument type undefined"<<std::endl;
				return -1;
			break;
		}
	}
	else if (keyword == "st") {
		op = 0b1000;
		regC = arguments[0]->registerNumber;
		switch(arguments[1]->type) {
			case SYMBOL:
				mod = 0b0010;
				regA = 15;
				regB = 0;
				disp = section->getRelativeOffsetToSymbol(arguments[1]->symbol);
			break;
			case LITERAL:
				mod = 0b0010;
				regA = 15;
				regB = 0;
				disp = section->getRelativeOffsetToLiteral(arguments[1]->literal);
			break;
			case REGISTER_MEMORY:
				mod = 0b0000;
				regA = arguments[1]->registerNumber;
				regB = 0;
				disp = 0;
			break;
			case REGISTER_SYMBOL_MEMORY:
				if(Assembler::getSymbolTable()[arguments[1]->symbol]->getSection() != 0) {
					std::cerr<<"Value of symbol "<<arguments[1]->symbol<<" can't be determined"<<std::endl;
					return -1;
				}
				mod = 0b0000;
				regA = arguments[1]->registerNumber;
				regB = 0;
				disp = Assembler::getSymbolTable()[arguments[1]->symbol]->getValue();
				if(disp < -0x800 || disp > 0x7ff) {
					std::cerr<<"Value of symbol "<<arguments[1]->symbol<<" larger than 12 bits signed"<<std::endl;
					return -1;
				}
			break;
			case REGISTER_LITERAL_MEMORY:
				mod = 0b0000;
				regA = arguments[1]->registerNumber;
				regB = 0;
				disp = arguments[1]->literal;
				if(disp < -0x800 || disp > 0x7ff) {
					std::cerr<<"Value of literal "<<arguments[1]->literal<<" larger than 12 bits signed"<<std::endl;
					return -1;
				}
			break;
			default:
				std::cerr<<"st argument type undefined"<<std::endl;
				return -1;
			break;
		}
	}
	else if (keyword == "csrrd") {
		op = 0b1001;
		mod = 0b0000;
		regA = arguments[1]->registerNumber;
		regB = arguments[0]->registerNumber & 0xf;
		regC = 0;
		disp = 0;
	}
	else if (keyword == "csrwr") {
		op = 0b1001;
		mod = 0b0100;
		regA = arguments[1]->registerNumber & 0xf;
		regB = arguments[0]->registerNumber;
		regC = 0;
		disp = 0;
	}
	write = (op<<28) | (mod<<24) | (regA<<20) | (regB<<16) | (regC<<12) | (disp & 0xfff);
	section->putDataReverse(&write, sizeof(uint32_t));
	return 0;
}