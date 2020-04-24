# <center>实验报告</center>
## 1. 编译指令
make 后生成 **parser** 解析程序
## 2. 运行解析器
*./parser test/**.cmm* 运行
## 3. 代码解析
### a. 代码
增加了symbol.c 和symbol.h 用以解析语法树
### b. 入口
main.c 中提供了解析的根入口`semantic_parse(p_tree_start);`
`p_tree_start`为经过lab1 syntax 解析后的语法树。`semantic_parse`主题
### c. 解析结构
extdeflist_analyze --> extdef_analyze --> pecifier_analyze --> int 
                                                           --> float
                                                           --> struct --> deflist_analyze   
                                                                          ... --> def_analyze 
                                                                              ... --> specifier_analyze 
                                                                                  ... --> declist_analyze
                                      --> ext_declist_analyze --> vardec_analyze 
                                      --> func_dec_analyze   varlist_analyze paramdec_analyze vardec_analyze
                                      --> compst_analyze   deflist_analyze def_analyze specifier_analyze declist_analyze
                                                              stmtlist_analyze stmt_analyze    exp_analyze  
                                                                                              compst_analyze
                                                                                                             
                                                                        
### d. 解析函数列表
- dec_analyze
- declist_analyze
- def_analyze
- deflist_analyze
- specifier_analyze
- vardec_analyze
- ext_declist_analyze
- paramdec_analyze
- varlist_analyze
- func_dec_analyze
- args_analyze
- exp_analyze
- stmt_analyze
- stmtlist_analyze
- compst_analyze
- extdef_analyze
- extdeflist_analyze
### e. 核心数据结构
Hashtable

