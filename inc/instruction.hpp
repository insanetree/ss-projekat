#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "statement.hpp"

class Instruction : public Statement {
	Instruction(const std::string&, arg*);
    virtual uint32_t getSize() override;
    virtual bool isValid() override;
    virtual int32_t firstPass() override;
protected:
private:
    static std::set<std::string> validKeywords;
};
#endif