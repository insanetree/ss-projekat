#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include "statement.hpp"

class Instruction : public Statement {
public:
	Instruction(const std::string&, arg*);
    virtual uint32_t getSize() override;
    virtual bool isValid() override;
    virtual int32_t firstPass() override;
    virtual int32_t secondPass() override;
protected:
private:
    static std::set<std::string> validKeywords;
};
#endif