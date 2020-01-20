
%{
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
[1-9][0-9]*|0[xX][0-9A-Fa-f]+|0[0-7]*		{yylval = new_terminal_node_with_value(ID_INT,yytext) ;return INT;}
((([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[eE][+-]?[0-9]+)|((0|[1-9][0-9]*)\.[0-9]+) { yylval = new_terminal_node_with_value(ID_FLOAT,yytext); return FLOAT;}
";"     {yylval = new_terminal_node(ID_SEMI);return SEMI;}
","     {yylval = new_terminal_node(ID_COMMA);return COMMA;}
"="     {yylval = new_terminal_node(ID_ASSIGNOP);return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {yylval = new_terminal_node(ID_RELOP);return RELOP;}
"+"     {yylval = new_terminal_node(ID_PLUS);return PLUS;}
"-"     {yylval = new_terminal_node(ID_MINUS);return MINUS;}
"*"	    {yylval = new_terminal_node(ID_STAR);return STAR;}
"/"     {yylval = new_terminal_node(ID_DIV);return DIV;}
"&&"    {yylval = new_terminal_node(ID_AND);return AND;}
"||"    {yylval = new_terminal_node(ID_OR);return OR;}
"."     {yylval = new_terminal_node(ID_DOT);return DOT;}
"!"     {yylval = new_terminal_node(ID_NOT);return NOT;}
"("     {yylval = new_terminal_node(ID_LP);return LP;}
")"     {yylval = new_terminal_node(ID_RP);return RP;}
"["     {yylval = new_terminal_node(ID_LB);return LB;}
"]"     {yylval = new_terminal_node(ID_RB);return RB;}
"{"     {yylval = new_terminal_node(ID_LC);return LC;}
"}"     {yylval = new_terminal_node(ID_RC);return RC;}
"struct"    {yylval = new_terminal_node(ID_STRUCT);return STRUCT;}
"return"    {yylval = new_terminal_node(ID_RETURN);return RETURN;}
"if"        {yylval = new_terminal_node(ID_IF);return IF;}
"else"      {yylval = new_terminal_node(ID_ELSE);return ELSE;}
"while"     {yylval = new_terminal_node(ID_WHILE);return WHILE;}
"int"|"float" {yylval = new_terminal_node_with_value(ID_TYPE,yytext);return TYPE;}
[_A-Za-z]([_0-9A-Za-z])* {yylval = new_terminal_node_with_value(ID_ID,yytext);return ID;}
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