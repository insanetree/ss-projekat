#include "statement.hpp"

Statement::Statement(const std::string& keyword, arg* argList=nullptr, Section* section=nullptr) : keyword(keyword), section(section) {
	while(argList) {
		arguments.push_back(argList);
		argList = argList->next;
	}
}

Statement::~Statement() {
	for(arg* a: arguments){
		delete a;
	}
}

const std::string& Statement::getKeyword() const {
	return keyword;
}