%{
#include "output.hpp"
#include "nodes.hpp"
#include "parser.tab.h"
%}

%option noyywrap

digit [0-9]
first_digit [1-9]
whitespace [ \t\r]+  
letter [a-zA-Z]
string \"([^\n\r\"\\]|\\[rnt\"\\])+\"  
comment \/\/[^\r\n]*[\r]?
commentline \/\/[^\r\n]*[\n|\r\n]
%%

void        return VOID;
int         return INT;
byte        return BYTE;
bool        return BOOL;
and         return AND;
or          return OR;
not         return NOT;
true        return TRUE;
false       return FALSE;
return      return RETURN;
if          return IF;
else        return ELSE;
while       return WHILE;
break       return BREAK;
continue    return CONTINUE;

;           return SC;
,           return COMMA;
\(          return LPAREN;
\)          return RPAREN;
\{          return LBRACE;
\}          return RBRACE;
=           return ASSIGN;

\+              return PLUS;
\-              return MINUS;
\*              return MULT;
\/              return DIV;

==              return EQUAL;
!=              return NEQUAL;
\<               return LESS;
>               return GREATER;
\<=              return LEQ;
>=              return GEQ;

[a-zA-Z][a-zA-Z0-9]*   {yylval = std::make_shared<ast::ID>(yytext);
                        return ID;}

[1-9][0-9]*b|0b        {yylval = std::make_shared<ast::NumB>(yytext);
                        return NUM_B;}  // Binary numbers
[1-9][0-9]*|0          {yylval = std::make_shared<ast::Num>(yytext);
                        return NUM;}    // Decimal numbers

{string}    {yylval = std::make_shared<ast::String>(yytext);
            return STRING;} //not sure if i need to increase yylineno
{comment}   { } //need to check
{commentline}   { yylineno++; } //need to check
{whitespace} {  }

\n { yylineno++; }

. { output::errorLex(yylineno); }

%%