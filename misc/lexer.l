/* this will include some C code directly, so we can pull in the
 * header files we want */
%{
#include "parser.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
%}

/* this tells us that we're going to output our lexer implementation
 * to `lexer.c` and the associated header to `lexer.h`.
 */
%option outfile="src/lexer.cpp" header-file="inc/lexer.hpp"

/* the int yywrap(void) is used internally by the lexer for the case
 *  where you want to parse multiple files in sequence with the same
 * parser state. Usually, you'd supply your own implementation that
 * would set up the next input stream and let the lexer know whether
 * to keep going. In our case, we disable it; this is effectively
 * the same as a dummy implementation like #define yywrap() 0.
 */
%option noyywrap

LPAR  "("
RPAR  ")"
PLUS  "+"
COMMA ","
DOT   "\."
COLON ":"
LBRACKET "["
RBRACKET "]"
DOLLAR "$"
PERCENT "%"

HEXADECIMAL   0[Xx][0-9A-Fa-f]+
DECIMAL [1-9][0-9]*
IDENT [A-Za-z_][A-Za-z0-9_]*
COM   "#"[^\n\r]*
WS    [ \r\n\t]*

%%
{WS} {}
{COM} {}
{LPAR} {
    return TOKEN_LPAR;
}
{RPAR} {
    return TOKEN_RPAR;
}
{LBRACKET} {
    return TOKEN_LBRACKET;
}
{RBRACKET} {
    return TOKEN_RBRACKET;
}
{PLUS} {
    return TOKEN_PLUS;
}
{COMMA} {
    return TOKEN_COMMA;
}
{DOT} {
    return TOKEN_DOT;
}
{COLON} {
    return TOKEN_COLON;
}
{DOLLAR} {
    return TOKEN_DOLLAR;
}
{PERCENT} {
    return TOKEN_PERCENT;
}
{IDENT} {
    size_t size = strlen(yytext);
    yylval.ident = (char*)malloc(size+1);
    strcpy(yylval.ident, yytext);
    yylval.ident[size] = '\0';
    return TOKEN_IDENT;
}
{HEXADECIMAL} {
    sscanf(yytext, "%x", &yylval.num);
    return TOKEN_NUM;
}
{DECIMAL} {
    sscanf(yytext, "%d", &yylval.num);
    return TOKEN_NUM;
}
%%

/* In the event of some kind of error, this will get called. */
int
yyerror(const char* msg)
{
    fprintf(stderr, "lexer error: %s\n", msg);
    return 0;
}