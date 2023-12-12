/* If we want to use other functions, we have to put the relevant
 * header includes here. */
%{
  #include "global.hpp"
  #include "assembler.hpp"
  #include "symbol.hpp"
	int32_t yylex(void);
	void yyerror(const char*);
%}

/* These declare our output file names. */
%output "src/parser.cpp"
%defines "inc/parser.hpp"
%language "c"

/* This union defines the possible return types of both lexer and
 * parser rules. We'll refer to these later on by the field name */
%union {
	int32_t         num;
	char*       ident;
	struct arg *arg;
}

/* These define the tokens that we use in the lexer.
 * All of these have no meaningful return value. */
%token TOKEN_LPAR
%token TOKEN_RPAR
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_PLUS
%token TOKEN_COMMA
%token TOKEN_DOT
%token TOKEN_COLON
%token TOKEN_DOLLAR
%token TOKEN_PERCENT


/* These are ALSO used in the lexer, but in addition to
 * being tokens, they also have return values associated
 * with them. We name those according to the names we used
 * above, in the %union declaration. So, the TOKEN_NUM
 * rule will return a value of the same type as num, which
 * (in this case) is an int32_t. */
%token <num>   TOKEN_NUM
%token <ident> TOKEN_IDENT

/* These are non-terminals in our grammar, by which I mean, parser
 * rules down below. Each of these also has a meaningful return type,
 * which is declared in the same way. */
%type <arg> arg;
%type <arg> argList;

%%
  prog:
  | line prog
  ;

  line: label
  | instruction
  | directive
  ;

  label: TOKEN_IDENT TOKEN_COLON {
    if(Assembler::getCurrentSection()<0){
      std::cerr<<"label "<<$1<<" defined out of any section"<<std::endl;
      YYABORT;
    }
    std::string label($1);
    if(Assembler::getSymbolTable().count(label)) {
      std::cerr<<"label "<<label<<" already defined"<<std::endl;
      YYABORT;
    }
    Symbol* newSymbol = new Symbol(label, Assembler::getLocationCounter(), false, Assembler::getCurrentSection());
    Assembler::getSymbolTable().insert({label, newSymbol});
  }
  ;

  instruction:
  ;

  directive: TOKEN_DOT TOKEN_IDENT {
    std::string directive($2);
    if(directive == "end") YYACCEPT;
    YYABORT;
  }
  | TOKEN_DOT TOKEN_IDENT argList {

  }
  ;

  argList: arg {
    $$ = $1;
  }
  | arg argList {
    struct arg *argument = $1;
    argument->next = $2;
    $$ = argument;
  }
  ;

  arg: TOKEN_DOLLAR TOKEN_NUM {
    struct arg* newArgument = new arg();
    newArgument->type = LITERAL_VALUE;
    newArgument->literal = $2;
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_DOLLAR TOKEN_IDENT {
    struct arg* newArgument = new arg();
    newArgument->type = SYMBOL_VALUE;
    newArgument->symbol = std::string($2);
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_NUM {
    struct arg* newArgument = new arg();
    newArgument->type = LITERAL_MEMORY;
    newArgument->literal = $1;
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_IDENT {
    struct arg* newArgument = new arg();
    newArgument->type = SYMBOL_MEMORY;
    newArgument->symbol = std::string($1);
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_PERCENT TOKEN_IDENT {
    struct arg* newArgument = new arg();
    newArgument->type = REGISTER_VALUE;
    uint32_t reg = getRegisterNum(std::string($2));
    if(reg < 0) YYABORT;
    newArgument->registerNumber = reg;
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_LBRACKET TOKEN_PERCENT TOKEN_IDENT TOKEN_RBRACKET {
    struct arg* newArgument = new arg();
    newArgument->type = REGISTER_MEMORY;
    uint32_t reg = getRegisterNum(std::string($3));
    if(reg < 0) YYABORT;
    newArgument->registerNumber = reg;
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  | TOKEN_LBRACKET TOKEN_PERCENT TOKEN_IDENT TOKEN_PLUS TOKEN_NUM TOKEN_RBRACKET {
    struct arg* newArgument = new arg();
    newArgument->type = REGISTER_LITERAL_MEMORY;
    uint32_t reg = getRegisterNum(std::string($3));
    if(reg < 0) YYABORT;
    newArgument->registerNumber = reg;
    newArgument->literal = $5;
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  |TOKEN_LBRACKET TOKEN_PERCENT TOKEN_IDENT TOKEN_PLUS TOKEN_IDENT TOKEN_RBRACKET {
    struct arg* newArgument = new arg();
    newArgument->type = REGISTER_SYMBOL_MEMORY;
    uint32_t reg = getRegisterNum(std::string($3));
    if(reg < 0) YYABORT;
    newArgument->registerNumber = reg;
    newArgument->symbol = std::string($5);
    newArgument->next = nullptr;
    $$ = newArgument;
  }
  ;
%%