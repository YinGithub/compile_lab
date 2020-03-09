#include <string.h> 
#include "main.h"
#define MOD_LEN 0x3fff
#define MAX_LAYERS 10
  
  typedef struct Type_* Type;  
  typedef struct FieldList_* FieldList;  
  
  struct Type_  
  {  
    enum { BASIC, ARRAY, STRUCTURE } kind;  
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

typedef struct layer_link_ * layer_link;


int stack_top = 0;
layer_link Layer_Stack[MAX_LAYERS] = {0};
struct layer_link_
{
    /* data */
    int pos ;
    layer_link next;
};



unsigned int hash_pjw(char* name)  
 {  
   unsigned int val = 0, i;  
   for (; *name; ++name)  
   {  
     val = (val << 2) + *name;  
     if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & MOD_LEN;  
   }  
   return val; 
 }
FieldList Hash_Table[MOD_LEN] = {0};

 void start()
 {
     //create
     if (node == "DEF")
     { 
         layer_link p;
         FieldList f ;
         // first check if already exist in current layer scope

         p = Layer_Stack[stack_top];
         while (p != NULL)
         {
             if(strcmp(Hash_Table[p->pos]->name,node.ID) == 0 )
                return ERR;
            p = p->next;
         }
         while()
         f =  (FieldList)malloc(sizeof(struct FieldList_));
         f->name = node.ID;
         if 
         type = malloc
         f->type = 
     }


 }



void pretty_print(struct basic_tokens * p_node ,int space_num)
{

  if (p_node == NULL)
    return ;
  if(p_node->type == NON_TERMINAL)
  {
    struct non_terminal_tokens * p =  (struct non_terminal_tokens * )p_node;
  //  printf("$$next bro addr%ld ",(unsigned long)p->next_bro);
    if(space_num > 0)
      printf("%*s",space_num," ");
    printf("%s",token_name[p->id]);
    printf("%s"," ");
    printf("(%d)\n",p->loc.first_line);
   struct basic_tokens * p_child = p->children;
   while(p_child)
   {
     pretty_print(p_child,space_num + 2);
     p_child = p_child->next_bro;
   }
  }
  else
  {
    /* terminal node */
    struct terminal_tokens * p = (struct terminal_tokens *)p_node;
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


int analyze(struct basic_tokens * p_node)
{
    
}

void main()
{
    func(p_tree_start);
    return;
}