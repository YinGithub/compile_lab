#ifndef _SYMBOL_H
#define _SYMBOL_H
#include "main.h"
#define CONF_MOD_LEN 0x3fff
#define CONF_MAX_CONTEXT 10
#define CONF_DECLARE_MAX_LIST_LEN 100
  
typedef struct Type_* Type;  
typedef struct FieldList_* FieldList; 
  
struct Type_  
{  
  enum { BASIC, ARRAY, STRUCTURE,FUNC_DECLARE,FUNC_DEFINTION} kind;  
  union 
  {  
    // 基本类型 
    int basic; 
    // 数组类型信息包括元素类型与数组大小构成 
    struct { Type elem; int size; } array; 
    // 结构体类型信息是一个链表 
    FieldList structure; 
  } u; 
}; 
struct FieldList_ 
{ 
  char* name;  // 域的名字 
  Type type;  // 域的类型 
  FieldList tail;  // 下一个域 
};
struct context_vars_
{
  int hash_index;
  struct context_vars_ * next;
};
typedef struct context_vars_ * context_vars;
struct context
{
  enum {CONTEXT_FUNCTION, CONTEXT_STRUCT} type;
  context_vars p_var_head;  
};
struct declare_node
{
  Token_Node p_func_id_node;
  struct declare_node * next;
};

/*#########func#######*/
int compare_type(Type type_a,Type type_b);
/*######### main #######*/
void varlist_analyze(Token_Node p_varlist);
void vardec_analyze(Type p_spec_type,Token_Node p_vardec);
void stmtlist_analyze(Token_Node p_stmtlist);
void stmt_analyze(Token_Node p_stmt);
Type specifier_analyze(Token_Node p_spec );
void paramdec_analyze(Token_Node p_paramdec);
FieldList func_dec_analyze(Type p_return_type,Token_Node p_func_dec);
void extdeflist_analyze(Token_Node p_deflist );
void extdef_analyze(Token_Node p_extdef);
void ext_declist_analyze(Type p_type,Token_Node p_ext_declist );
Type exp_analyze(Token_Node p_exp);
void deflist_analyze(Token_Node p_deflist);
void def_analyze(Token_Node p_def);
void declist_analyze(Type p_type,Token_Node p_declist);
void dec_analyze(Type p_type,Token_Node p_dec);
void compst_analyze(Token_Node p_compst);
FieldList args_analyze(Token_Node p_args);

#endif