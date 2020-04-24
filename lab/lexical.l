
%{
  #define YYSTYPE unsigned long 
#include "syntax.tab.h" 
#include "main.h"
int yycolumn = 1;
 #define YY_USER_ACTION \
 yylloc.first_line = yylloc.last_line = yylineno; \
 yylloc.first_column = yycolumn; \
 yylloc.last_column = yycolumn + yyleng - 1; \
yycolumn += yyleng;  
%}

%%
"//".*   {}
\/\*([^*]|(\*+[^*/]))*\*+\/ {int i = 0;while(i<yyleng){if(yytext[i] == '\n') yylineno++;i++;}}
[1-9][0-9]*|0[xX][0-9A-Fa-f]+|0[0-7]*		{yylval = new_terminal_node_with_value(ID_INT,yylloc,yytext) ;return INT;}
((([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[eE][+-]?[0-9]+)|((0|[1-9][0-9]*)\.[0-9]+) { yylval = new_terminal_node_with_value(ID_FLOAT,yylloc,yytext); return FLOAT;}
";"     {yylval = new_terminal_node(ID_SEMI,yylloc);return SEMI;}
","     {yylval = new_terminal_node(ID_COMMA,yylloc);return COMMA;}
"="     {yylval = new_terminal_node(ID_ASSIGNOP,yylloc);return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {yylval = new_terminal_node(ID_RELOP,yylloc);return RELOP;}
"+"     {yylval = new_terminal_node(ID_PLUS,yylloc);return PLUS;}
"-"     {yylval = new_terminal_node(ID_MINUS,yylloc);return MINUS;}
"*"	    {yylval = new_terminal_node(ID_STAR,yylloc);return STAR;}
"/"     {yylval = new_terminal_node(ID_DIV,yylloc);return DIV;}
"&&"    {yylval = new_terminal_node(ID_AND,yylloc);return AND;}
"||"    {yylval = new_terminal_node(ID_OR,yylloc);return OR;}
"."     {yylval = new_terminal_node(ID_DOT,yylloc);return DOT;}
"!"     {yylval = new_terminal_node(ID_NOT,yylloc);return NOT;}
"("     {yylval = new_terminal_node(ID_LP,yylloc);return LP;}
")"     {yylval = new_terminal_node(ID_RP,yylloc);return RP;}
"["     {yylval = new_terminal_node(ID_LB,yylloc);return LB;}
"]"     {yylval = new_terminal_node(ID_RB,yylloc);return RB;}
"{"     {yylval = new_terminal_node(ID_LC,yylloc);return LC;}
"}"     {yylval = new_terminal_node(ID_RC,yylloc);return RC;}
"struct"    {yylval = new_terminal_node(ID_STRUCT,yylloc);return STRUCT;}
"return"    {yylval = new_terminal_node(ID_RETURN,yylloc);return RETURN;}
"if"        {yylval = new_terminal_node(ID_IF,yylloc);return IF;}
"else"      {yylval = new_terminal_node(ID_ELSE,yylloc);return ELSE;}
"while"     {yylval = new_terminal_node(ID_WHILE,yylloc);return WHILE;}
"int"|"float" {yylval = new_terminal_node_with_value(ID_TYPE,yylloc,yytext);return TYPE;}
[_A-Za-z]([_0-9A-Za-z])* {yylval = new_terminal_node_with_value(ID_ID,yylloc,yytext);return ID;}
\n      {yycolumn = 1;yylineno++; }
\t|" "  {}
.     { printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext); }
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