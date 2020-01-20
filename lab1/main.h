#ifndef _MAIN_H
#define _MAIN_H
#include "syntax.tab.h" 
#ifndef  YYSTYPE 
#define YYSTYPE unsigned long 
#endif
enum tokens_id {  ID_INT,ID_FLOAT,ID_SEMI,ID_COMMA,ID_ASSIGNOP,ID_RELOP,\
                ID_PLUS,ID_MINUS,ID_STAR,ID_DIV,ID_AND,ID_OR,ID_DOT,ID_NOT,\
                ID_LP,ID_RP,ID_LB,ID_RB,ID_LC,ID_RC,ID_STRUCT,ID_RETURN,ID_IF,\
                ID_ELSE,ID_WHILE,ID_TYPE,ID_ID,ID_PROGRAM,ID_EXTDEFLIST,ID_EXTDEF,\
                ID_EXTDECLIST,ID_SPECIFIER,ID_STRUCTSPECIFIER,ID_OPTTAG,ID_TAG,ID_VARDEC,\
                ID_FUNDEC,ID_VARLIST,ID_PARAMDEC,ID_COMPST,ID_STMTLIST,ID_STMT,ID_DEFLIST,\
                ID_DEF,ID_DECLIST,ID_DEC,ID_EXP,ID_ARGS};
enum token_types {TERMINAL,NON_TERMINAL};
enum type_types  {TYPE_INT,TYPE_FLOAT};
typedef union  
{
  int ival; 
  float fval;
  char * sval;
  }
u_val;
struct basic_tokens
{
  int type;
  int id;
  struct basic_tokens * next_bro;
};
struct terminal_tokens
{
  int type;
  int id;
  struct basic_tokens * next_bro;
  u_val value;
};

struct non_terminal_tokens
{
  int type;
  int id;
  struct basic_tokens * next_bro;
  YYLTYPE loc;
  struct basic_tokens * children;
};

extern char * token_name[] ;
struct non_terminal_tokens *  p_tree_start;

YYSTYPE new_terminal_node(int token_id);
YYSTYPE new_float_node(int token_id,float value);
YYSTYPE new_int_node(int token_id,int value);
YYSTYPE new_type_node(int token_id, char * type);
YYSTYPE new_non_terminal_node(int token_id,YYLTYPE loc,int child_num,...);
YYSTYPE new_terminal_node_with_value(int token_id,char * value);

#endif