#include <string.h> 
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symbol.h"




//typedef struct layer_link_ * layer_link;
FieldList Struct_Prototype_List;// all struct type link to list

struct declare_node * p_Declare_List;
//int Current_Layer = 0;


int Current_Context;
struct context Context_List[CONF_MAX_CONTEXT] = {{0,0}};



struct Type_  type_int = {BASIC,{0}};
struct Type_  type_float = {BASIC,{1}};

unsigned int hash_pjw(char* name)  
 {  
   unsigned int val = 0, i;  
   for (; *name; ++name)  
   {  
     val = (val << 2) + *name;  
     if (i = val & ~0x3fff) 
        val = (val ^ (i >> 12)) & CONF_MOD_LEN;  
   }  
   return val; 
 }
FieldList Hash_Table[CONF_MOD_LEN] = {0};

struct Type_   type_int;
struct Type_   type_float;
/*Point to current function*/
FieldList Current_Func;

#define get_terminal_val_str(p_node) ((struct terminal_tokens *)p_node)->value.sval

void  process_error(int err_no,Token_Node p_node)
{
  printf("Error type %d at Line %d: ",err_no,p_node->loc.first_line);
  switch (err_no)
  {
  case 1:
    //
    printf("Undefined variable  \"%s\"\n",p_node->value.sval);
    break;
  case 2: 
    printf("Undefined function  \"%s\"\n",p_node->value.sval);
    break;
  case 3:
    printf("Redefined variable \"%s\"\n",p_node->value.sval);
    break;
  case 4:
  //Error type 4 at Line 6: Redefined function "func".
    printf("Redefined function \"%s\"\n",p_node->value.sval);
    break;   
  case 5:
    /* code */
    printf("Type mismatched for assignment.\n");
    break;
  case 6:
    printf("he left-hand side of an assignment must be a variable.\n");
    break;
  case 7:
    printf("Type mismatched for operands\n");
    break;
  case 8:
    printf("Type mismatched for return.\n");
    break;
  case 9:
    printf("Function \"%s\" is not applicable for arguments .\n",p_node->value.sval);
    break;
  case 10:
    printf("Type is not an array.\n");
    break;
  case 11:
    printf("\"%s\"is not an function.\n",p_node->value.sval);
    break;
  case 12:
    printf("array index is not an integer.\n");
    break;
  case 13:
    printf("Illegal use of \".\"\n");
    break;
  case 14:
    printf("Non-existent field \"%s\".\n",p_node->value.sval);
    break;
  case 15:
    printf("Redefined field \"%s\"\n",p_node->value.sval);
    break;
  case 16:
    printf("Duplicated name\"%s\"\n",p_node->value.sval);
    break;  
  case 17:
    printf("Undefined structure \"%s\"\n",p_node->value.sval);
    break;
  case 18:
    printf("Undefined function \"%s\"\n",p_node->value.sval);
    break;    
  case 19:
    printf("Inconsistent declaration of function \"%s\"\n",p_node->value.sval);
    break;    
  default:
    break;
  }
  return;
}

Type find_struct_define(char * struct_name )
{
  FieldList p;
  p = Struct_Prototype_List;
  while(p != NULL)
  {
    if(strcmp(p->name,struct_name) == 0)
      return p->type;
    p = p->tail;
  }
  return NULL;
}
FieldList find_elem_by_name_type(Token_Node p_ID,int kind)
{
  int hash_index ;
  FieldList p_field;
  char * field_name = p_ID->value.sval;
  hash_index = hash_pjw(field_name);
  p_field = Hash_Table[hash_index];
  if(p_field == NULL)
    return p_field;
  while(p_field)
  {
    if(strcmp(p_field->name,field_name) == 0 && (p_field->type->kind == kind ) )
    {
      return p_field;
    }
    else
      p_field = p_field->tail;
  }
  return p_field; 
}
Type get_id_type(Token_Node p_ID)
{
  int hash_index ;
  FieldList p_field;
  char * field_name = p_ID->value.sval;
  hash_index = hash_pjw(field_name);
  p_field = Hash_Table[hash_index];

  while(p_field)
  {
    if(strcmp(p_field->name,field_name) == 0  )
    {
      break;
    }
    else
      p_field = p_field->tail;
  }
  if(p_field == NULL)
    return NULL;
  else
  {
    return p_field->type;
  }
}

void dec_analyze(Type p_type,Token_Node p_dec)
{
  /*
  Dec -> VarDec
	  | VarDec ASSIGNOP Exp
  */
 Type p_exp_type;
  Token_Node p_child = p_dec->first_child;
  
  vardec_analyze(p_type,p_child);
  p_child = p_child->next_bro;
  if(p_child != NULL)
  {
    /*
    Dec VarDec ASSIGNOP Exp
    */
   if(Context_List[Current_Context].type == CONTEXT_STRUCT)
     process_error(15,p_child); 
    p_child = p_child->next_bro;//p to exp
    p_exp_type = exp_analyze(p_child);
    if(compare_type(p_exp_type,p_type))
      process_error(5,p_child);
  }
  return ;
}
void declist_analyze(Type p_type,Token_Node p_declist)
{
  /*
  DecList -> Dec 
    | Dec COMMA DecList
  */
  Token_Node p_child;
  p_child = p_declist->first_child;
  dec_analyze(p_type,p_child);
  p_child = p_child->next_bro;
  if(p_child != NULL)
  {
    p_child = p_child->next_bro;
    declist_analyze(p_type,p_child);
  }
  return;

}
void def_analyze(Token_Node p_def)
{
  /*
  Def Specifier DecList SEMI
  */
  Token_Node p_child;
  Type p_spec_type;
  /* Spec tag*/
  assert(p_def != NULL);
  p_child = p_def->first_child;
  
  p_spec_type =  specifier_analyze(p_child);
  if(p_spec_type == NULL)
  {
    /*
     specifier not defined.
     err reported in specifier_analyze
    */
    return;
  }
  p_child =  p_child->next_bro;
  
  declist_analyze(p_spec_type,p_child);
  return;
}
void deflist_analyze(Token_Node p_deflist)
{
  /*
  DefList Def DefList |
       epslon  
  */
  Token_Node p_def; 
  if(p_deflist->first_child == NULL)
    return;
  p_def = p_deflist->first_child;
  def_analyze(p_def);
  deflist_analyze(p_def->next_bro);
  return;
}
FieldList link_current_stack_vars()
{
  context_vars p;
  FieldList h;
  FieldList list = NULL;
  int index;
  p = Context_List[Current_Context].p_var_head;
  while(p)
  {
    index = p->hash_index;
    h = Hash_Table[index];
    h->tail = list;
    list = h;
    p = p->next;
  }
  return list;
}
void enter_scope(int context_type)
{
  Current_Context++;
  assert(Current_Context < CONF_MAX_CONTEXT);
  Context_List[Current_Context].type = context_type;
  Context_List[Current_Context].p_var_head = NULL;
  return;
}
void leave_scope()
{
  context_vars p;
  FieldList q;
  p  = Context_List[Current_Context].p_var_head;
  while(p)
  {
    q  = Hash_Table[p->hash_index];
    assert(q != NULL);
    Hash_Table[p->hash_index] = q->tail;
    p = p->next;
  }
  Current_Context--;
  return;
}
Type specifier_analyze(Token_Node p_spec )
{
  /*
   Specifier =     TYPE 
                | StructSpecifier 
  */
  Token_Node p_child = p_spec->first_child;
  Type p_spec_type;
   
  assert(p_child != NULL);
  if (p_child->id == ID_TYPE)
  {
    // tackle basic type
    /*Specifier  -> TYPE */
   
    if(strcmp(p_child->value.sval,"int") == 0)
      p_spec_type =  &type_int;
    else
      p_spec_type =  &type_float;
  }
  else
  {
    
    /* StructSpecifier  STRUCT OptTag LC DefList RC 
        | STRUCT Tag 
         */
    /*p_child = StructSpecifier*/
    p_child = p_child->first_child;// /*p_child = STRUCT */
    /*p_child = STRUCT */
    p_child = p_child->next_bro;//  /*p_child = OptTag | Tag */
    if(p_child->id == ID_TAG) 
    {
      /*
      StructSpecifier -> STRUCT Tag
      Tag -> ID
      */
      Token_Node p_ID = p_child->first_child;
      p_spec_type = find_struct_define(p_ID->value.sval);
      if(p_spec_type == NULL)
      // struct proto-type not exist.
          process_error(17,p_ID);
      return p_spec_type;
    }
    else 
    {
      /*
        StructSpecifier -> STRUCT OptTag LC DefList RC 
        define a struct type
        1. find struct name,to check if exist in hash table
        2. analyze the DEFlist,and return a def-link\
        3.create a field ,kind is STRUCTPROTYPE,type point to the link in step 2 
      */
      char * struct_name = NULL;
      /* p_child = opt_Tag*/
      if (p_child->first_child ) 
      {
        /*
          OptTag  -> ID
          with a tag name 
        */
        Token_Node p_ID = p_child->first_child;
        struct_name = p_ID->value.sval;
        if(find_struct_define(struct_name) != NULL)
        {
          /* re-defetion of struct type*/
          process_error(16,p_ID);
          return NULL;
        }
      }
      p_child = p_child->next_bro;//p_child = LC
      enter_scope(CONTEXT_STRUCT);
      p_child = p_child->next_bro;// p_child = DFFLIST
      deflist_analyze(p_child); // all item stored in current-layer stack
      /*creat struct type*/
      
      p_spec_type = (Type)malloc(sizeof(struct Type_));
      p_spec_type->kind = STRUCTURE;
      p_spec_type->u.structure = link_current_stack_vars();
      leave_scope();
      /* if struct with a namem, 
        insert into struct prototype list.
      */
      if(struct_name != NULL)
      {
        FieldList   p_new_field =  (FieldList)malloc(sizeof(struct FieldList_ ));
        p_new_field->name = struct_name;
        p_new_field->type = p_spec_type;
        p_new_field->tail = Struct_Prototype_List;
        Struct_Prototype_List = p_new_field;
      }
      
    }
  }
  return p_spec_type;
  
}
/*
  find "name" var in current layer
  1 found.
  2 not found.
*/
int find_vars_in_current_context(char * name)
{
  // Bug warning hash-confict  
  // if within a context scope ,hash-conflict occurs.
  int index;
  context_vars p_var;
  index = hash_pjw(name);
  p_var = Context_List[Current_Context].p_var_head;
  while(p_var != NULL)
  {
    if(p_var->hash_index == index)
      return 1;
    p_var = p_var->next;
  }
  return 0;
}
int add_to_current_context(int hash_index)
{
  context_vars p = (context_vars)malloc(sizeof(struct context_vars_));
  p->hash_index = hash_index;
  p->next = Context_List[Current_Context].p_var_head;
  Context_List[Current_Context].p_var_head = p;
  return 0;

}
void vardec_analyze(Type p_spec_type,Token_Node p_vardec)
{
  /*
  VarDec  ID 
          | VarDec LB INT RB 
  1. child is start with ID:
    1.1 check in hash exist the same-name var in current layer
    1.2 link the new var to hash table
  2. child starts with VarDec
    2.1 a array defintion 
  */
  Token_Node p_child;
  p_child = p_vardec->first_child;
  if(p_child->id ==  ID_ID)
  {
    /* VarDec -> ID */
    char * id_name;
    id_name = p_child->value.sval;
    if(find_vars_in_current_context(id_name))
    {
      // free all memories malloc during this stage-"array"
      /*
      Type p_type_bk;
      while(p_type->kind == ARRAY)
      {
        p_type_bk = p_type->u.array.elem;
        free(p_type);
        p_type = p_type_bk;
      }
      */
     /* Error type 3 at Line 4: Redefined variable "i" */
     if(Context_List[Current_Context].type == CONTEXT_STRUCT)
        process_error(15,p_child);
      else
        process_error(3,p_child);
    }
    else
    {
      /* allocate field for var */
      FieldList p_var;
      int hash_index ;
      p_var = malloc(sizeof(struct FieldList_));
      hash_index = hash_pjw(id_name);
      p_var->name = id_name;
      p_var->type = p_spec_type;
      p_var->tail = Hash_Table[hash_index];
      Hash_Table[hash_index] = p_var;
      // add this var to current layer
      add_to_current_context(hash_index);
    }
  }
  else
  {
    /*
    VarDec -> VarDec LB INT RB
    */
    /* jump to INT */
    Type p_arr_type = malloc(sizeof(struct Type_));
    p_child =  p_child->next_bro->next_bro;
    p_arr_type->kind = ARRAY;
    p_arr_type->u.array.elem = p_spec_type;
    p_arr_type->u.array.size = p_child->value.ival;
    vardec_analyze(p_arr_type,p_vardec->first_child);
  }
  return ;
}
void ext_declist_analyze(Type p_type,Token_Node p_ext_declist )
{
  Token_Node p_child;
  p_child = p_ext_declist->first_child; 
  vardec_analyze(p_type,p_child);
  p_child = p_child->next_bro;
  if(p_child != NULL)
  {
    //now point to ExtDecList
    p_child = p_child->next_bro;
    ext_declist_analyze(p_type,p_child);
  }
  return;
}
FieldList find_func(Token_Node p_ID)
{
  int hash_index ;
  char * func_name;
  func_name = p_ID->value.sval;
  FieldList p_field;
  hash_index = hash_pjw(func_name);
  p_field = Hash_Table[hash_index];
  if(p_field == NULL)
    return p_field;
  while(p_field)
  {
    if(strcmp(p_field->name,func_name) == 0 && (p_field->type->kind == FUNC_DECLARE || p_field->type->kind == FUNC_DEFINTION ) )
    {
      return p_field;
    }
    else
      p_field = p_field->tail;
  }
  return p_field;
}

void paramdec_analyze(Token_Node p_paramdec)
{
  /*
  ParamDec -> Specifier VarDec
  */
  Type p_spec_type;
  Token_Node p_child;
  p_child = p_paramdec->first_child;
  p_spec_type = specifier_analyze(p_child);//Warning! p_spec_type could be None.
  //p_child = vardec
  p_child = p_child->next_bro;
  vardec_analyze(p_spec_type,p_child);
  return;
}
void varlist_analyze(Token_Node p_varlist)
{
  /*
  VarList  ParamDec COMMA VarList 
          | ParamDec
  */
  Token_Node p_child;
  if(p_varlist != NULL)
  {
    /*
    VarList ParamDec COMMA VarList
            ParamDec
    */
   // p_child = ParamDec
    p_child = p_varlist->first_child;
    paramdec_analyze(p_child);
    if(p_child->next_bro)
    {
      //p_child = Varlist
      p_child = p_child->next_bro->next_bro;
      varlist_analyze(p_child);
    }
  }
  return;
}


FieldList func_attribute(Type return_type,Token_Node p_varlist)
{
  FieldList p_return_type_field;
  varlist_analyze(p_varlist);
  p_return_type_field = malloc(sizeof(struct FieldList_));
  p_return_type_field->type = return_type;
  p_return_type_field->tail = link_current_stack_vars();
  return p_return_type_field;
}

void add_to_hash_table(FieldList p_item)
{
  int index;
  FieldList p_head;
  assert(p_item != NULL);
  index  = hash_pjw(p_item->name);
  p_head =  Hash_Table[index]; 
  p_item->tail = p_head;
  Hash_Table[index] = p_item;
  return;
}
void add_to_delcare_list(Token_Node p_ID)
{
 
  struct declare_node * p;
  p = (struct declare_node *)malloc(sizeof(struct declare_node));
  p->p_func_id_node = p_ID;
  p->next = p_Declare_List;
  p_Declare_List = p;
  return;
}
int compare_type(Type type_a,Type type_b)
{
  FieldList f_a,f_b;
  assert(type_a != NULL);
  assert(type_b!= NULL);
  if(type_a->kind != type_b->kind && (type_a->kind != FUNC_DECLARE && type_a->kind !=  FUNC_DEFINTION))
    return 1;
  if(type_a->kind == BASIC)
  {
    if(type_a->u.basic == type_b->u.basic )
      return 0;
    else
      return 1;
  }
  else 
  {
    if(type_a->kind == ARRAY)
    {
      if(type_a->u.array.size != type_b->u.array.size)
        return 1;
      return compare_type(type_a->u.array.elem,type_b->u.array.elem);
    }
    else
    {
      f_a = type_a->u.structure;
      f_b = type_b->u.structure;
      while(1)
      {
        if(f_a == NULL && f_b == NULL)
          return 0;
        if(f_a == NULL || f_b == NULL)
        {
          return 1;
        }
          
        if(compare_type(f_a->type,f_b->type))
        {
          return 1;
        } 
        f_a = f_a->tail;
        f_b = f_b->tail; 
      }
    }
  }
  return 0;
}
int compare_structure(FieldList f_a,FieldList f_b)
{
  while(1)
  {
    if(f_a == NULL && f_b == NULL)
      return 0;
    if(f_a == NULL || f_b == NULL)
      return 1;
    if(compare_type(f_a->type,f_b->type))
      return 1;
    f_a = f_a->tail;
    f_b = f_b->tail; 
  }
  return 0;
}
// void remove_from_hashtable(FieldList p_field)
// {
//   int index;
//   FieldList p_head;
//   FieldList p,p_pre;
//   index  = hash_pjw(p_field->name);
//   p =  Hash_Table[index];
//   p_pre = p;
//   while(p)
//   {
//     if(strcmp(p->name,p_field->name))
//     {
//       p = p->tail;
//     }
//     else
//     {
//         break;
//     }
//   }
//   if(p != (FieldList)NULL)
//   {
//     p = 
//   }
//   p_item->tail = p_head;
//   Hash_Table[index] = p_item;
//   return;
// }
FieldList func_dec_analyze(Type p_return_type,Token_Node p_func_dec)
{
  /*
  ExtDef Specifier FunDec CompSt ->1
        | Specifier FunDec SEMI ->2
  Specifier FunDec CompSt
  FunDec  ID LP VarList RP 
          | ID LP RP 
  */
  Token_Node p_bro =  p_func_dec->next_bro;
  Token_Node p_child,p_ID;
  FieldList p_new_func_field,p_pre_func_field,p_param;
  assert(p_bro != NULL);
  if(p_bro->id == ID_SEMI)//Productin 1
  {
    /*
    Specifier FunDec SEMI
    FunDec ID LP VarList RP 
          | ID LP RP 
    */
    //p_child = ID 
    p_child = p_func_dec->first_child;
    p_ID = p_child;
    /* 
    no pre-declare or pre-define found.
    1. verify the parameter(in VarList or empty),then create a strucure linking all param.
    2. add return param to the param link list
    3. malloc a FieldList struct ,with type-kind "FUNC_DECLARE", filled with "step 2" data, link to hash table
    4. add to "decare-list"
    */
    /* p_child = VARLIST | RP   */
    p_child = p_child->next_bro->next_bro;
    assert(p_child != NULL);
    if(p_child->id == ID_VARLIST)
    {
        /*
        FunDec -> ID LP VarList RP
        */
        p_param = func_attribute(p_return_type,p_child);
    }
    else
    {
      /*
      FunDec -> ID LP RP 
      */
      p_param = func_attribute(p_return_type,(Token_Node)NULL);
    }
    p_new_func_field = (FieldList)malloc(sizeof(struct FieldList_));
    p_new_func_field->name = p_ID->value.sval;
    p_new_func_field->type = (Type)malloc(sizeof(struct Type_));
    p_new_func_field->type->kind = FUNC_DECLARE;
    p_new_func_field->type->u.structure = p_param;

    p_pre_func_field = find_func(p_ID);
    if(p_pre_func_field == NULL)
    {
      add_to_hash_table(p_new_func_field);
      add_to_delcare_list(p_ID);
    }
    else
    {
      /*
        with the func-struct in hash table
        1. Check the return type & param (in VarList or empty) with the exist func-struct.
        2. if func-struct is "FUNC_DECLARE" type or "FUNC_DEFINE".if declare go to step 3
        3. add the new func to "declare list"  
       */
      
      if(compare_type(p_new_func_field->type,p_pre_func_field->type))
      {
        /*
        func param list & return type  not identical between two delaration(defetion).
        */
        process_error(19,p_ID);
      }
      else
      {
        if(p_pre_func_field->type->kind == FUNC_DECLARE)
          add_to_delcare_list(p_ID);
    
      }
    }
  }
  else
  {
    /*
      FunDec  ID LP VarList RP 
          | ID LP RP 
      define a func
      1. find in hash table ,if null ,step 2,if not null,step 5.
      2. verify the param(in VarList or empty),then link all the params as fieldlist link
      3. add the return type to step 2 link
      4. malloc a FieldList struct ,with type-kind "FUNC_DEFINE",add the step 3 data 
      5. check if func type is FUNC_DEFINE,go to error process,otherwise go to step 6
      6. now as the func type is FUNC_DECLARE, check the "func define"'s 
        "return type" and the "param" same as declare struct 
      7. change the type kind from FUNC_DECLARE to FUNC_DEFINE
     */ 
    p_child = p_func_dec->first_child;
    p_ID = p_child;
    assert(p_child !=NULL);

    /* p_child = VARLIST | RP   */
    p_child = p_child->next_bro->next_bro;
    if(p_child->id == ID_VARLIST)
    {
        /*
        FunDec -> ID LP VarList RP
        */
        p_param = func_attribute(p_return_type,p_child);
    }
    else
    {
      /*
      FunDec -> ID LP RP 
      */
      p_param = func_attribute(p_return_type,(Token_Node)NULL);
    }
    
    p_new_func_field = (FieldList)malloc(sizeof(struct FieldList_));
    p_new_func_field->name = p_ID->value.sval;
    p_new_func_field->type = (Type)malloc(sizeof(struct Type_));
    p_new_func_field->type->kind = FUNC_DEFINTION;
    p_new_func_field->type->u.structure = p_param;
    p_pre_func_field = find_func(p_ID);
    if(p_pre_func_field != NULL)
    {
      if(p_pre_func_field->type->kind == FUNC_DEFINTION)
      {
        /*
        func re-defition
        */
       //Error type 4 at Line 6: Redefined function "func".
       process_error(4,p_ID);
      }
      else
      {
        /* code */

        if(compare_type(p_new_func_field->type,p_pre_func_field->type))
        {
          /*
          func param list & return type  not identical between two delaration(defetion).
          */
          process_error(19,p_ID);
        }
        else
        {
          struct declare_node * p1;
          struct declare_node * p2;
          Token_Node p_decare_ID;
          p2 = p1 = p_Declare_List ; 
          /* modified as func defition in hash table  */

          p_pre_func_field->type->kind = FUNC_DEFINTION;
          //delete all declaration 
          while(p2)
          {
            p_decare_ID = p2->p_func_id_node;
            if(strcmp(p_decare_ID->value.sval,p_ID->value.sval) == 0)
            {
              if(p2 == p_Declare_List)
              {
                  p2 = p2->next;
                  p1 = p2;
                  p_Declare_List = p2;
                  continue;
              }
              else
                p1->next = p2->next;
            }
            p1 = p2;
            p2 = p2->next;
          }
        }
      }
      
    }
    else
    {
      /* no preious defition or delaration */
      add_to_hash_table(p_new_func_field);
    }
  }
  return p_new_func_field;
}
int match_production(Token_Node p_father_node, int * case_list,int case_len)
{
  int i;
  Token_Node p_child;
  p_child = p_father_node->first_child;
  for(i = 0;i<case_len;i++ )
  {
    if(p_child == NULL)
      return 1;
    if(p_child->id != case_list[i])
      return 1;
    p_child = p_child->next_bro;
  }
  return 0;
}
int legal_left_hand(Token_Node p_exp)
{
  /*
    legal left-hand 
    1. ID
    2. EXP Dot ID
    3. Exp LB Exp RB
  */
  int case_1[1] = {ID_ID};
  int case_2[3] = {ID_EXP,ID_DOT,ID_ID};
  int case_3[4] = {ID_EXP,ID_LB,ID_EXP,ID_RB};
  if(match_production(p_exp,case_1,1) == 0)
    return 0;
  if(match_production(p_exp,case_2,3) == 0)
    return 0;
  if(match_production(p_exp,case_3,4) == 0)
    return 0;
  return 1;
}
int is_id(Token_Node p_father)
{
  int id_prod[1]= {ID_ID};
  return match_production(p_father,id_prod,1);
}
Type find_struct_item(Type p_struct,char * id)
{
  FieldList p ; 
  p = p_struct->u.structure;
  while(p != NULL)
  {
    if(strcmp(p->name,id) == 0 )
      return p->type;
    p = p->tail;
  }
  return NULL;
}
FieldList args_analyze(Token_Node p_args)
{
  /*
   args-> Exp COMMA Args 
        | Exp  
  */
  Token_Node p_child ;
  Type p_type;
  FieldList p_new_field;
  assert(p_args != NULL );
  p_child =  p_args->first_child;
  assert(p_child != NULL );
  p_type =  exp_analyze(p_child);
  p_new_field =  malloc(sizeof(struct FieldList_));
  p_new_field->type = p_type;
  p_child = p_child->next_bro;
  if(p_child)
  {
    p_new_field->tail = args_analyze(p_child->next_bro);
  }
  return p_new_field;
}
Type exp_analyze(Token_Node p_exp)
{
  /*
    Exp 
  | Exp ASSIGNOP Exp 
  | Exp AND Exp 
  | Exp OR Exp 
  | Exp RELOP Exp 
  | Exp PLUS Exp 
  | Exp MINUS Exp 
  | Exp STAR Exp 
  | Exp DIV Exp 
  | LP Exp RP 
  | MINUS Exp 
  | NOT Exp 
  | ID LP Args RP 
  | ID LP RP 
  | Exp LB Exp RB 
  | Exp DOT ID 
  | ID 
  | INT 
  | FLOAT 
  */
  Type p_op1_type,p_op2_type;
  Token_Node p_op1,p_op2,p_oprator;
  p_op1 = p_exp->first_child;//First first_child of exp
  switch(p_op1->id)
  {
    case ID_EXP:
      p_op1_type = exp_analyze(p_op1);
      if(p_op1_type == NULL)
        return NULL;
      // p_child = ASSIGNOP | AND | OR | RELOP | PLUS | MINUS | STAR | DIV | LB | DOT
      p_oprator = p_op1->next_bro;
      switch (p_oprator->id)
      {
      case ID_PLUS:
      case ID_MINUS:
      case ID_STAR:
      case ID_DIV:
      case ID_RELOP:
        /*
        arith "+","-","*","/"
        exp_1 exp_2 are both int,or both float 
        */
        if(p_op1_type->kind !=BASIC)
        {
          process_error(7,p_op1);
          return NULL;
        }
        p_op2 = p_oprator->next_bro;//p_child point to 2nd Exp
        p_op2_type = exp_analyze(p_op2);
        if(p_op2_type == NULL )
          return NULL;
        if(compare_type(p_op1_type,p_op2_type))
        {
          process_error(7,p_op2);
          return NULL;
        }
        if(p_oprator->id ==  ID_RELOP)
            return &type_int;
        else
          return p_op2_type; 
      case ID_ASSIGNOP:
        /* 
        1.left hand check
        2.left & right should have same type
        3.
          */
        if(legal_left_hand(p_op1))
        {
          process_error(6,p_op1);
          return NULL;
        }
        p_op2 = p_oprator->next_bro;
        p_op2_type = exp_analyze(p_op2);
        if(p_op2_type == NULL)
        {
          return NULL;
        }
        
        if(compare_type(p_op1_type,p_op2_type))
        {
          process_error(5,p_op1);
          return NULL;          
        }
        return p_op2_type;
      case ID_AND:
      case ID_OR:
        /*
          logical exp only allows type "INT". 
        */
        if(compare_type(p_op1_type,&type_int))
        {
          process_error(7,p_op1);
          return NULL;
        }
        p_op2 = p_oprator->next_bro;
        p_op2_type = exp_analyze(p_op2);
        if(p_op2_type == NULL)
          return NULL;
        if(compare_type(p_op2_type,&type_int))
        {
          process_error(7,p_op2);
          return NULL;
        }
        return &type_int;
      case ID_DOT:
        /*
        exp = EXP dot ID
        struct items
        */ 
        if(p_op1_type->kind != STRUCTURE)
        {
          process_error(13,p_op1);
          return NULL;
        }
        p_op2 = p_oprator->next_bro;
        p_op2_type = find_struct_item(p_op1_type,p_op2->value.sval);
        if(p_op2_type == NULL)
        {
          process_error(14,p_op2);
          return NULL;
        }
        else 
          return p_op2_type;
      case ID_LB:
        /*
        Exp LB Exp RB
        */
          if(p_op1_type->kind != ARRAY)
          {
            process_error(10,p_op1);
            return NULL;
          }
          p_op2 = p_oprator->next_bro;
          p_op2_type = exp_analyze(p_op2);
          if(p_op2_type == NULL || compare_type(p_op2_type,&type_int))
          {
            process_error(12,p_op2);
            return NULL;
          }
          return p_op1_type->u.array.elem;
      default:
        return NULL;
      }
    case ID_ID:
      p_op1_type = get_id_type(p_op1);
      if(p_op1->next_bro == NULL)
      {
        /*
        EXP -> ID 
        1.Find id in hashtable
        2. check type not func_deaclare or func defetion
        3. return the id type
        */
        if(p_op1_type ==NULL || p_op1_type->kind == FUNC_DECLARE || p_op1_type->kind == FUNC_DEFINTION)
        {
          process_error(1,p_op1);
          return NULL;
        }
        else
          return p_op1_type;
      }
      else
      {
        /*
          | ID LP Args RP 
        | ID LP RP 
        */
        FieldList p_arg_list_field,p_return_type_field;
        if(p_op1_type == NULL)
        {
            process_error(2,p_op1);
            return NULL;
        }
        if(p_op1_type->kind == BASIC || p_op1_type->kind == ARRAY || p_op1_type->kind == STRUCTURE)
        {
          process_error(11,p_op1);
          return NULL;
        }
        p_op2 =  p_op1->next_bro->next_bro;// point to ARGS or RP
        if(p_op2->id ==  ID_ARGS)
        {
          p_arg_list_field = args_analyze(p_op2);
        }
        else 
          p_arg_list_field = NULL;
        //compare after the return type field
        p_return_type_field = p_op1_type->u.structure;
        
        assert(p_return_type_field != NULL);
        if(compare_structure(p_return_type_field->tail,p_arg_list_field))
        {
          process_error(9,p_op1);
        }
        /* return function "return type"*/
        return p_return_type_field->type;
      }
    case ID_NOT:

      /*
      logical ,exp type should be INT
      NOT Exp 
      */  
      p_op2 = p_op1->next_bro;
      p_op2_type = exp_analyze(p_op2);
      if(p_op2_type == NULL)
        return NULL;
        
      if(compare_type(p_op2_type,&type_int))
      {
        process_error(7,p_op2);
        return NULL;
      }
      return p_op2_type;
    case ID_MINUS:
      /*
        Exp -> minus exp
        exp should be int or float
      */    
      p_op2 = p_op1->next_bro;
      p_op2_type = exp_analyze(p_op2);
      if(p_op2_type == NULL)
        return NULL;
      if(p_op2_type->kind != BASIC)
      {
        process_error(7,p_op2);
        return NULL;
      }
      return p_op2_type;
    case ID_LP:
      /*
      Exp = LP EXP RP
      */
     p_op2 = p_op1->next_bro;
     return exp_analyze(p_op2);
    case ID_FLOAT:
      return &type_float;
    case ID_INT:
      return &type_int;
    default:
      break;
  }
  return NULL;
}

void stmt_analyze(Token_Node p_stmt)
{
  /*
  Stmt
  | Exp SEMI 
  | CompSt 
  | RETURN Exp SEMI 
  | IF LP Exp RP Stmt 
  | IF LP Exp RP Stmt ELSE Stmt 
  | WHILE LP Exp RP Stmt 
  */
  Token_Node p_child;
  Type p_type;
  assert(p_stmt != NULL);
  p_child = p_stmt->first_child;
  assert(p_child != NULL);
  switch (p_child->id)
  {
  case ID_EXP:
    /* code */
    exp_analyze(p_child);
    break;
  case ID_COMPST:
    enter_scope(CONTEXT_FUNCTION);
    
    compst_analyze(p_child);
    leave_scope();
    break;
  case ID_RETURN:
    p_child = p_child->next_bro;//p_child point to exp
    p_type = exp_analyze(p_child);
    /* The return type of current func stored in first palace of the field list */
    if(Current_Func == NULL || p_type == NULL || compare_type(Current_Func->type->u.structure->type,p_type))
    {
      /* return type not comply with func-defition */
      /* Error type 8 at Line 4: Type mismatched for return.  */
      process_error(8,p_child);
    }
    break;
  case ID_IF:
    p_child = p_child->next_bro->next_bro; // Exp
    p_type = exp_analyze(p_child);
    if(compare_type(p_type,&type_int))
      process_error(7,p_child);
    p_child = p_child->next_bro->next_bro;
    stmt_analyze(p_child);//p_child = stmt
    if(p_child->next_bro)// p_child->next_bro = ELSE
    {
      p_child = p_child->next_bro->next_bro;//p_child = Stmt
      stmt_analyze(p_child);
    }
    break;
  case ID_WHILE:
    /*  | WHILE LP Exp RP Stmt */
    p_child = p_child->next_bro->next_bro;
    p_type = exp_analyze(p_child);
    if(compare_type(p_type,&type_int))
      process_error(7,p_child);
    p_child = p_child->next_bro->next_bro;
    stmt_analyze(p_child);
    break;
  default:
    break;
  }
  return;
}
void stmtlist_analyze(Token_Node p_stmtlist)
{
  /*
  StmtList -> 
      Stmt StmtList 
    | epslon 
  */
  Token_Node p_stmt;
  if(p_stmtlist->first_child == NULL)
    return;
  p_stmt = p_stmtlist->first_child;
  stmt_analyze(p_stmt);
  stmtlist_analyze(p_stmt->next_bro);
  return;
}
// void delete_current_context_vars()
// {
//   layer_link p,p_bk;
//   FieldList h;
//   int index;
//   p = Layer_Stack[Current_Layer];
//   while(p)
//   {
//     index = p->hash_index;
//     h = Hash_Table[index];
//     Hash_Table[index] = h->tail;
//     free(h);
//     p_bk = p;
//     p = p->next;
//     free(p_bk);
//   }
//   return;  
// }
void compst_analyze(Token_Node p_compst)
{
  /*
  CompSt -> LC DefList StmtList RC 
  */
 Token_Node p_child;
 p_child = p_compst->first_child;
 /* p_child = deflist */
 p_child = p_child->next_bro;//p to Deflist
 deflist_analyze(p_child);
 p_child = p_child->next_bro;
  /* p_child = stmtlist */
 stmtlist_analyze(p_child);
return ;
}

void extdef_analyze(Token_Node p_extdef)
{
  /*
  ExtDef Specifier ExtDecList SEMI 
        | Specifier SEMI 
        | Specifier FunDec CompSt 
        |Specifier FunDec SEMI
  */
  Token_Node p_child;
  Type p_def_type;
  /* p_child   =  Specifier  tag*/
  assert(p_extdef != NULL);
  p_child = p_extdef->first_child;
  assert(p_child != NULL);
  p_def_type = specifier_analyze(p_child);
  /* p_child = ExtDecList |  SEMI | FunDec  */
  p_child = p_child->next_bro; //point to tag sibling to spec_tag
  /*
    ext-def:Specifier SEMI 
  */
 assert(p_child != NULL);
  switch (p_child->id)
  {
  case ID_EXTDECLIST:
    /* 
      extdef -> Specifier ExtDecList SEMI  
    */
    ext_declist_analyze(p_def_type,p_child);
    break;
  case ID_FUNDEC:
    /*
      extdef->Specifier FunDec CompSt
     */
    enter_scope(CONTEXT_FUNCTION);
    Current_Func  = func_dec_analyze(p_def_type,p_child);
    p_child = p_child->next_bro;
    if(p_child->id == ID_COMPST)
      compst_analyze(p_child);
    leave_scope();
  default:
    break;
  }
  return;
}
void extdeflist_analyze(Token_Node p_deflist )
{
  // ext-def dispatch
  Token_Node p_extdef = NULL ;
  if( p_deflist->first_child == NULL )
    return;
  assert(p_deflist != NULL);
  p_extdef = p_deflist->first_child;
  extdef_analyze(p_extdef);
  extdeflist_analyze(p_extdef->next_bro);
}
void context_init()
{
  Current_Context = 0;
  Context_List[Current_Context].type = CONTEXT_FUNCTION;
  Context_List[Current_Context].p_var_head = NULL;
  return;
}
void env_init()
{
  Current_Func = NULL;
  p_Declare_List = NULL;
  context_init();
  return;
}
void check_func_declare()
{
  
  while(p_Declare_List)
  {
    process_error(18,p_Declare_List->p_func_id_node);
    p_Declare_List  = p_Declare_List->next;
  }
  return ;
}
void semantic_parse(Token_Node p_tree_start)
{
   env_init();
   extdeflist_analyze(p_tree_start->first_child);
   check_func_declare();
    /*
    post_process 
    check all func declare list to found un-defition function.
    */
   
  return;

}