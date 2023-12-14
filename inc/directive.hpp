#ifndef DIRECTIVE_HPP
#define DIRECTIVE_HPP

#include "statement.hpp"
#include "assembler.hpp"

class Directive : public Statement {
public:
    Directive(const std::string&, arg*);
    virtual uint32_t getSize() override;
    virtual bool isValid() override;
    virtual int32_t firstPass() override;
protected:
private:
    static std::set<std::string> validKeywords;
};
#endif