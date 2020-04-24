#include "lex.yy.c" 
#include <stdarg.h>
#include <string.h>
#include "main.h"
char * token_name[] = {
                "INT","FLOAT","SEMI","COMMA","ASSIGNOP","RELOP",\
                "PLUS","MINUS","STAR","DIV","AND","OR","DOT","NOT",\
                "LP","RP","LB","RB","LC","RC","STRUCT","RETURN","IF",\
                "ELSE","WHILE","TYPE","ID","Program","ExtDefList","ExtDef",\
                "ExtDecList","Specifier","StructSpecifier","OptTag","Tag","VarDec",\
                "FunDec","VarList ","ParamDec","CompSt","StmtList","Stmt","DefList","Def",\
                "DecList","Dec","Exp","Args"\
              };


Token_Node  p_tree_start;
int syntax_err = 0;
#define YYSTYPE unsigned long 
YYSTYPE new_terminal_node(int token_id,YYLTYPE loc)
{
  Token_Node p ;
  p = malloc(sizeof(struct token_node_));
  p->id = token_id;
  p->type = TERMINAL;
  p->loc = loc;
  p->next_bro = NULL;
  p->first_child = NULL;
  return (YYSTYPE)p;
}
int hex_toint(char * hex_str)
{
  int hex_len =  strlen(hex_str) -2;
  hex_str = hex_str+2;
  int sum = 0 ;
  int i = 0;
  int digit;
  while(i < hex_len)
  {
    if(hex_str[i] >= 'a')
    {
      digit =  hex_str[i] - 'a'+10;
    }
    else if(hex_str[i] >= 'A')
    {
      digit = hex_str[i] - 'A' + 10;
    }
    else 
      digit = hex_str[i] - '0';
    sum  = sum * 16 + digit;
    i++;
  }
  return sum ;
}
int oct_toint(char * oct_str)
{
    int oct_len =  strlen(oct_str) -1;
  oct_str = oct_str+1;
  int sum = 0 ;
  int i = 0;
  int digit;
  while(i < oct_len)
  {

    digit = oct_str[i] - '0';
    sum  = sum * 8 + digit;
    i++;
  }
  return sum ;
}
YYSTYPE new_terminal_node_with_value(int token_id,YYLTYPE loc,char * value)
{
  Token_Node p ;
  p = malloc(sizeof(struct token_node_));
  p->id = token_id;
  p->type = TERMINAL;
  p->loc = loc;
  if(token_id == ID_FLOAT)
  {
    (p->value).fval = atof(value);
  }
  else if (token_id == ID_INT)
  {
    /* code */
    if(value[0] == '0')
    {
      if(value[1] == 'x' || value[1] == 'X')
        /* hex */
        (p->value).ival = hex_toint(value);
      else
        /* oct */
        (p->value).ival = oct_toint(value);      
    }
    else
      (p->value).ival = atoi(value);
    
  }
  else if (token_id == ID_TYPE || token_id == ID_ID)
  {
    (p->value).sval = (char *)(malloc(strlen(value)+1));
    strcpy((p->value).sval,value);
  }
  p->next_bro = NULL;
  p->first_child = NULL;
  return (YYSTYPE)p;

}

/*
YYSTYPE new_float_node(int token_id,float value)
{
  struct terminal_tokens * p ;
  p = malloc(sizeof(struct terminal_tokens));
  p->id = token_id;
  p->type = TERMINAL;
  (p->value).fval = value;
  p->next_bro = NULL;
  return (YYSTYPE)p;
}

YYSTYPE new_int_node(int token_id,int value)
{
  struct terminal_tokens * p ;
  p = malloc(sizeof(struct terminal_tokens));
  p->id = token_id;
  p->type = TERMINAL;
  (p->value).ival = value;
  p->next_bro = NULL;
  return (YYSTYPE)p;
}

YYSTYPE new_type_node(int token_id, char * str)
{
  struct terminal_tokens * p ;
  p = malloc(sizeof(struct terminal_tokens));
  p->id = token_id;
  p->type = TERMINAL;  
  (p->value).sval = (char *)(malloc(strlen(str)+1));
  strcpy((p->value).sval,str);
  p->next_bro = NULL;
  return (YYSTYPE)p;
}
*/
YYSTYPE new_non_terminal_node(int token_id,YYLTYPE loc,int child_num,...)
{
  /*
  struct non_terminal_tokens * p;
  struct basic_tokens * p_var;
  */
  Token_Node p;
  Token_Node p_child;
  int i;
  va_list valist;
  p = malloc(sizeof(struct token_node_));
  p->type = NON_TERMINAL;
  p->id = token_id;
  p->loc = loc;
  va_start(valist, child_num);
  p_child = va_arg(valist, struct token_node_ *);
  p->first_child = p_child;
  for (i = 0; i < (child_num-1); i++)
  {
    p_child->next_bro = va_arg(valist, struct token_node_ *);
    p_child = p_child->next_bro;
    if(p_child == NULL)
      break;
  }
  va_end(valist);
  return (YYSTYPE) p;

}
void pretty_print(Token_Node p_node ,int space_num)
{

  if (p_node == NULL)
    return ;
  Token_Node p =  p_node;
  if(p_node->type == NON_TERMINAL)
  {
   if(p_node->first_child == NULL)
      return; 
  //  printf("$$next bro addr%ld ",(unsigned long)p->next_bro);
    if(space_num > 0)
      printf("%*s",space_num," ");
    printf("%s",token_name[p->id]);
    printf("%s"," ");
    printf("(%d)\n",p->loc.first_line);
   Token_Node p_child = p->first_child;
   while(p_child)
   {
     pretty_print(p_child,space_num + 2);
     p_child = p_child->next_bro;
   }
  }
  else
  {
    /* terminal node */
 //   printf("$$next bro addr%ld ",(unsigned long)p->next_bro);
    printf("%*s",space_num," ");
    printf("%s",token_name[p->id]);
     if( p_node->id == ID_INT)
      printf(": %d",p->value.ival);
     if(p_node->id == ID_FLOAT)
       printf(": %f",p->value.fval);
     if(p_node->id  == ID_ID|| p_node->id == ID_TYPE )
       printf(": %s",p->value.sval);
    printf("\n");
  }
  return ;
  
    
} 
void semantic_parse(Token_Node p_tree_start);
int main(int argc, char **argv) 
{ 
  FILE* f; 
  if (argc > 1) {       
    if (!(f = fopen(argv[1], "r"))) 
      {   perror(argv[1]);         
        return 1;       
      }     
    }
  syntax_err = 0;
  yyrestart(f);
  //yydebug =  1;
  yyparse();
  if(syntax_err == 0)
  {
    pretty_print(p_tree_start,0);
    semantic_parse(p_tree_start);
  }


  return 0;
}
void yyerror(const char* msg) 
{ 
  syntax_err = 1;
	fprintf(stderr, "Error type B at line %d : %s\n",yylineno, msg); 
} 
