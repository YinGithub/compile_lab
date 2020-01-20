%locations 
%define parse.error verbose
%{

	#define YYSTYPE unsigned long 
	#include "main.h"
  #include <stdio.h>

int yylex();
void yyerror(const char *s);


%}

%token INT 
%token FLOAT 
%token SEMI 
%token COMMA 
%token ASSIGNOP
%token RELOP 
%token PLUS 
%token MINUS 
%token STAR 
%token DIV 
%token AND 
%token OR 
%token DOT
%token NOT 
%token LP 
%token RP 
%token LB 
%token RB 
%token LC 
%token RC 
%token STRUCT 
%token RETURN 
%token IF 
%token ELSE 
%token WHILE 
%token TYPE
%token ID 
%right ASSIGNOP
%left OR
%left AND 
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left COMMA DOT LP RP LB RB
%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE 


%%
//High Level
Program: ExtDefList	{p_tree_start = (struct non_terminal_tokens *)new_non_terminal_node(ID_PROGRAM,@$,1,$1);}
	;
ExtDefList: ExtDef ExtDefList {$$ = new_non_terminal_node(ID_EXTDEFLIST,@$,2,$1,$2);}
	|		{$$ = (YYSTYPE)NULL;}
	;
ExtDef: Specifier ExtDecList SEMI {$$ = new_non_terminal_node(ID_EXTDEF,@$,3,$1,$2,$3);}
	| Specifier SEMI 	{$$ = new_non_terminal_node(ID_EXTDEF,@$,2,$1,$2);}
	| Specifier FunDec CompSt 	  {$$ = new_non_terminal_node(ID_EXTDEF,@$,3,$1,$2,$3);}
	| error SEMI 
	; 
ExtDecList:VarDec	{$$ = new_non_terminal_node(ID_EXTDECLIST,@$,1,$1);}
	| VarDec COMMA ExtDecList  {$$ = new_non_terminal_node(ID_EXTDECLIST,@$,3,$1,$2,$3);}
	;

//Specifiers
Specifier: TYPE	 {$$ = new_non_terminal_node(ID_SPECIFIER,@$,1,$1);}
	| StructSpecifier  {$$ = new_non_terminal_node(ID_SPECIFIER,@$,1,$1);}
	; 
StructSpecifier: STRUCT OptTag LC DefList RC {$$ = new_non_terminal_node(ID_STRUCTSPECIFIER,@$,5,$1,$2,$3,$4,$5);}
	| STRUCT Tag  {$$ = new_non_terminal_node(ID_STRUCTSPECIFIER,@$,2,$1,$2);}
	;
OptTag: ID  {$$ = new_non_terminal_node(ID_OPTTAG,@$,1,$1);}
	|	{$$ = (YYSTYPE)NULL;}
	;
Tag:ID   {$$ = new_non_terminal_node(ID_TAG,@$,1,$1);}
	;

//Declarators 
VarDec: ID    {$$ = new_non_terminal_node(ID_VARDEC,@$,1,$1);}
	| VarDec LB INT RB {$$ = new_non_terminal_node(ID_VARDEC,@$,4,$1,$2,$3,$4);}
	;
FunDec: ID LP VarList RP {$$ = new_non_terminal_node(ID_FUNDEC,@$,4,$1,$2,$3,$4);}
	| ID LP RP  {$$ = new_non_terminal_node(ID_FUNDEC,@$,3,$1,$2,$3);}
	| LP error RP
	; 
VarList : ParamDec COMMA VarList {$$ = new_non_terminal_node(ID_VARLIST,@$,3,$1,$2,$3);}
	| ParamDec  {$$ = new_non_terminal_node(ID_VARLIST,@$,1,$1);}
	; 
ParamDec: Specifier VarDec {$$ = new_non_terminal_node(ID_PARAMDEC,@$,2,$1,$2);}
	;

//Statements 
CompSt: LC DefList StmtList RC {$$ = new_non_terminal_node(ID_COMPST,@$,4,$1,$2,$3,$4);}
	| error RC
	;
StmtList: Stmt StmtList {$$ = new_non_terminal_node(ID_STMTLIST,@$,2,$1,$2);}
	|	{$$ = (YYSTYPE)NULL;}
	;
Stmt: Exp SEMI  {$$ = new_non_terminal_node(ID_STMT,@$,2,$1,$2);}
	| CompSt {$$ = new_non_terminal_node(ID_STMT,@$,1,$1);}
	| RETURN Exp SEMI {$$ = new_non_terminal_node(ID_STMT,@$,3,$1,$2,$3);}
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE	{$$ = new_non_terminal_node(ID_STMT,@$,5,$1,$2,$3,$4,$5);}
	| IF LP Exp RP Stmt ELSE Stmt {$$ = new_non_terminal_node(ID_STMT,@$,6,$1,$2,$3,$4,$5,$6);}
	| WHILE LP Exp RP Stmt {$$ = new_non_terminal_node(ID_STMT,@$,5,$1,$2,$3,$4,$5);}
	| error SEMI { yyerrok;}
	| IF LP error RP Stmt %prec LOWER_THAN_ELSE
	| IF LP error RP Stmt ELSE Stmt 
	| WHILE LP error RP Stmt 
	; 

//Local Definitions 
DefList: Def DefList	 {$$ = new_non_terminal_node(ID_DEFLIST,@$,2,$1,$2);}
	|	{$$ = (YYSTYPE)NULL;}
	;
Def: Specifier DecList SEMI {$$ = new_non_terminal_node(ID_DEF,@$,3,$1,$2,$3);}
	| Specifier error SEMI {$$ = new_non_terminal_node(ID_DEF,@$,3,$1,$2,$3);}
	;
DecList: Dec {$$ = new_non_terminal_node(ID_DECLIST,@$,1,$1);}
	| Dec COMMA DecList {$$ = new_non_terminal_node(ID_DECLIST,@$,3,$1,$2,$3);}
	;
Dec: VarDec		{$$ = new_non_terminal_node(ID_DEC,@$,1,$1);}
	| VarDec ASSIGNOP Exp {$$ = new_non_terminal_node(ID_DEC,@$,3,$1,$2,$3);}
	; 

//Expressions 
Exp: Exp ASSIGNOP Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp AND Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp OR Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp RELOP Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp PLUS Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp MINUS Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp STAR Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp DIV Exp {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| LP Exp RP {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| MINUS Exp {$$ = new_non_terminal_node(ID_EXP,@$,2,$1,$2);}
	| NOT Exp %prec NOT  {$$ = new_non_terminal_node(ID_EXP,@$,2,$1,$2);}
	| ID LP Args RP {$$ = new_non_terminal_node(ID_EXP,@$,4,$1,$2,$3,$4);}
	| ID LP RP {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| Exp LB Exp RB {$$ = new_non_terminal_node(ID_EXP,@$,4,$1,$2,$3,$4);}
	| Exp DOT ID {$$ = new_non_terminal_node(ID_EXP,@$,3,$1,$2,$3);}
	| ID {$$ = new_non_terminal_node(ID_EXP,@$,1,$1);}
	| INT {$$ = new_non_terminal_node(ID_EXP,@$,1,$1);}
	| FLOAT {$$ = new_non_terminal_node(ID_EXP,@$,1,$1);}
	| LP error RP 
	; 
Args: Exp COMMA Args {$$ = new_non_terminal_node(ID_ARGS,@$,3,$1,$2,$3);}
	| Exp	{$$ = new_non_terminal_node(ID_ARGS,@$,1,$1);}
	; 

%%

	 
