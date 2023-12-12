/* If we want to use other functions, we have to put the relevant
 * header includes here. */
%{
  #include "global.hpp"
  #include "assembler.hpp"
  #include "symbol.hpp"
	int yylex(void);
	void yyerror(const char*);
%}

/* These declare our output file names. */
%output "src/parser.cpp"
%defines "inc/parser.hpp"
%language "c"

/* This union defines the possible return types of both lexer and
 * parser rules. We'll refer to these later on by the field name */
%union {
	int         num;
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
 * (in this case) is an int. */
%token <num>   TOKEN_NUM
%token <ident> TOKEN_IDENT

/* These are non-terminals in our grammar, by which I mean, parser
 * rules down below. Each of these also has a meaningful return type,
 * which is declared in the same way. */
%type <arg> arg;
%type <ident> rname;

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
      exit(1);
    }
    std::string label($1);
    if(Assembler::getSymbolTable().count(label)) {
      std::cerr<<"label "<<label<<" already defined"<<std::endl;
      exit(1);
    }
    Symbol* newSymbol = new Symbol(label, Assembler::getLocationCounter(), false, Assembler::getCurrentSection());
    Assembler::getSymbolTable().insert({label, newSymbol});
  }
  ;

  instruction:
  ;

  directive:
  ;
%%