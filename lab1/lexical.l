%{
#include "syntax.tab.h" 

%}

%%
[1-9][0-9]*|0[xX][0-9A-Fa-f]+|0[0-7]*		{return INT;}
((([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[eE][+-]?[0-9]+)|((0|[1-9][0-9]*)\.[0-9]+) {return FLOAT;}
";"     {return SEMI;}
","     {return COMMA;}
"="     {return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {return RELOP;}
"+"     {return PLUS;}
"-"     {return MINUS;}
"*"	    {return STAR;}
"/"     {return DIV;}
"&&"    {return AND;}
"||"    {return OR;}
"."     {return DOT;}
"!"     {return NOT;}
"("     {return LP;}
")"     {return RP;}
"["     {return LB;}
"]"     {return RB;}
"{"     {return LC;}
"}"     {return RC;}
"struct"    {return STRUCT;}
"return"    {return RETURN;}
"if"        {return IF;}
"else"      {return ELSE;}
"while"     {return WHILE;}
"int"|"float" {return TYPE;}
[_A-Za-z]([_0-9A-Za-z])* {return ID;}
%%
/*
int main(int argc, char **argv) 
{ 
  FILE* f; 
  if (argc > 1) {       
    if (!(yyin = fopen(argv[1], "r"))) 
      {   perror(argv[1]);         
        return 1;       
      }     
    }
    yylex();
    return 0;
}
*/