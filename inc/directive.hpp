#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include "statement.hpp"
#include "assembler.hpp"
#include "section.hpp"

class Directive : public Statement {
public:
    Directive(const std::string&, arg*);
    virtual uint32_t getSize() override;
    virtual bool isValid() override;
    virtual int32_t firstPass() override;
    virtual int32_t secondPass() override;
protected:
private:
    static std::set<std::string> validKeywords;
};
#endif