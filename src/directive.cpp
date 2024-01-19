#include "directive.hpp"

/*
KEWORD SWITCH:
	if(keyword == "global") {

	} 
	else if(keyword == "extern") {

	} 
	else if(keyword == "section") {

	}
	else if(keyword == "word") {

	} 
	else if(keyword == "skip") {

	} 
	else if(keyword == "ascii") {

	} 
	else if(keyword == "equ") {

	} 
	else if(keyword == "end") {

	}
*/

std::set<std::string> Directive::validKeywords{
	"global",
	"extern",
	"section",
	"word",
	"skip",
	"ascii",
	"equ",
	"end"
};

Directive::Directive(const std::string& keyword, arg* argList, Section* section) : Statement(keyword, argList, section) {}

uint32_t Directive::getSize() {
	if(keyword == "global") {
		return 0;
	} 
	else if(keyword == "extern") {
		return 0;
	} 
	else if(keyword == "section") {
		return 0;
	} 
	else if(keyword == "word") {
		return 4*arguments.size();
	} 
	else if(keyword == "skip") {
		return arguments[0]->literal;
	} 
	else if(keyword == "ascii") {
		return arguments[0]->ascii.size();
	} 
	else if(keyword == "equ") {
		return 0;
	} 
	else if(keyword == "end") {
		return 0;
	}
	return 0;
}

bool Directive::isValid() {
	if(validKeywords.find(keyword) == validKeywords.end()) 
		return false;
	if(keyword == "global") {
		if(arguments.size() == 0)
			return false;
		for(arg* a : arguments) {
			if(a->type != SYMBOL)
				return false;
		}
	} else if(keyword == "extern") {
		if(arguments.size() == 0)
			return false;
		for(arg* a : arguments) {
			if(a->type != SYMBOL)
				return false;
		}
	} 
	else if(keyword == "section") {
		if(arguments.size() != 1)
			return false;
		if(arguments[0]->type != SYMBOL)
			return false;
	} 
	else if(keyword == "word") {
		if(arguments.size() == 0 || section == nullptr)
			return false;
		for(arg* a : arguments) {
			if(a->type != SYMBOL && a->type != LITERAL)
				return false;
		}
	} 
	else if(keyword == "skip") {
		if(arguments.size() != 1 || section == nullptr)
			return false;
		if(arguments[0]->type != LITERAL)
			return false;
	} 
	else if(keyword == "ascii") {
		if(arguments.size() != 1 || section == nullptr)
			return false;
		if(arguments[0]->type != ASCII)
			return false;
	} 
	else if(keyword == "equ") {
		if(arguments.size() != 2) 
			return false;
		if(arguments[0]->type != SYMBOL)
			return false;
		if(arguments[1]->type != LITERAL && arguments[1]->type != EXPRESSION && arguments[1]->type != SYMBOL)
			return false;
	} 		
	else if(keyword == "end") {

	}
	return true;
}

int32_t Directive::firstPass() {
	if(keyword == "global") {
		Assembler::insertStatement(this);
	} 
	else if(keyword == "extern") {
		for(uint32_t i = 0 ; i < arguments.size() ; i++)
			Assembler::getSymbolTable().insert({arguments[i]->symbol, new Symbol(arguments[i]->symbol, 0, true, nullptr, NOTYPE)});
	} 
	else if(keyword == "section") {
		std::string sectionName(arguments[0]->symbol);
		Section* newSection;
		if(!Assembler::getCurrentSection()) {
			newSection = new Section(sectionName);
			Assembler::getSectionTable().insert({sectionName, newSection});
			Assembler::getSymbolTable().insert({sectionName, new Symbol(sectionName, 0, false, newSection, SECTION)});
			Assembler::setCurrentSection(newSection);
			Assembler::setLocationCounter(0);
			return 0;
		}
		
		Section* currentSection = Assembler::getCurrentSection();
		currentSection->setSize(Assembler::getLocationCounter());

		auto sectionTableIter = Assembler::getSectionTable().find(sectionName);
		if(sectionTableIter == Assembler::getSectionTable().end()) {
			newSection = new Section(sectionName);
			Assembler::getSectionTable().insert({sectionName, newSection});
			Assembler::getSymbolTable().insert({sectionName, new Symbol(sectionName, 0, false, newSection, SECTION)});
		} else {
			newSection = sectionTableIter->second;
		}
		Assembler::setLocationCounter(newSection->getSize());
		Assembler::setCurrentSection(newSection);
		return 0;
	} 
	else if(keyword == "word") {
		Section* currentSection = Assembler::getCurrentSection();
		if(!currentSection)
			return -1;
		
		Assembler::incrementLocationCounter(this->getSize());
		currentSection->addStatement(this);
		return 0;
	} 
	else if(keyword == "skip") {
		Section* currentSection = Assembler::getCurrentSection();
		if(!currentSection)
			return -1;
		
		Assembler::incrementLocationCounter(this->getSize());
		currentSection->addStatement(this);
		return 0;
	} 
	else if(keyword == "ascii") {
		Section* currentSection = Assembler::getCurrentSection();
		if(!currentSection)
			return -1;
		Assembler::incrementLocationCounter(this->getSize());
		currentSection->addStatement(this);
		return 0;
	} 
	else if(keyword == "equ") {
		if(arguments[1]->type == LITERAL) {
			char number[20];
			sprintf(number, "0x%08x", arguments[1]->literal);
			arguments[1]->expression = std::string(number);
		}
		if(arguments[1]->type == SYMBOL) {
			arguments[1]->expression = arguments[1]->symbol;
		}
		if(Assembler::getUnresolvedExpressions().find(arguments[0]->symbol) != Assembler::getUnresolvedExpressions().end())
			return -1;
		Assembler::getUnresolvedExpressions().insert({arguments[0]->symbol, arguments[1]->expression});
		return 0;
	} 
	else if(keyword == "end") {
		return 0;
	}
	return 0;
}

int32_t Directive::secondPass() {
	uint32_t value, offset;
	if(keyword == "global") {
		for(arg* a : arguments) {
			Assembler::getSymbolTable()[a->symbol]->setGlobal(true);
		}
	} 
	else if(keyword == "extern") {

	} 
	else if(keyword == "section") {

	}
	else if(keyword == "word") {
		for(arg* a : arguments) {
			if(a->type == SYMBOL) {
				offset = section->getLocationCounter();
				value = 0;
				section->putRelocationData(offset, Assembler::getSymbolTable()[a->symbol]);
			}
			else if(a->type == LITERAL) {
				value = a->literal;
			}
			section->putData(&value, sizeof(uint32_t));
			section->incrementLocationCounter(sizeof(uint32_t));
		}
	} 
	else if(keyword == "skip") {
		uint8_t skip = 0;
		for(int32_t i = 0 ; i<arguments[0]->literal ; i++) {
			section->putData(&skip, sizeof(uint8_t));
			section->incrementLocationCounter(sizeof(uint8_t));
		}
	} 
	else if(keyword == "ascii") {
		for(char c : arguments[0]->ascii) {
			section->putData(&c, sizeof(char));
			section->incrementLocationCounter(sizeof(char));
		}
	} 
	else if(keyword == "equ") {

	} 
	else if(keyword == "end") {

	}
	return 0;
}