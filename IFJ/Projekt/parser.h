/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   parser.h
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <stdio.h>
#include "scaner.h"
#include "error.h"
#include "str.h"
#include "ial.h"
#include "expressions.h"
#include "ilist.h"


// 1 - vypis pruchodu fcí, 2 - tokeny
int debug;

#define EXPR 0
#define COND 1

tToken token;
tTable *global_table;

T_data_info curr_info;
T_data_type curr_type;

char* curr_name;
char* curr_class;
char* curr_func;

extern int line;

// výčet parsovacích funkcí podle LL gramatiky
tError parse();
tError program();
tError class();
tError func();
tError var_or_func();
tError params();
tError params_n();
tError stmt_list();
tError stmt();
tError var_dec();
tError var_init();
tError els();
tError init();
tError expression(int expr_or_cond);
tError read();
tError print();
tError concat();

// výčet pomocných funkcí parseru
void get_token();
void check_token(T_Type token_type);
void get_token_and_check(T_Type tok);
T_data_type get_data_type();
int loadName(char* s);
int loadClass(char* s);
int loadFunc(char* s);
tError insert_to_global();
tError insert_to_local();
int ganerate_key();
int set_value_int(char* name, int int_val);
int set_value_double(char* name, double double_val);
int set_value_string(char* name, char* string_val);
int set_value_string_id(char* name);
int insert_tmp_var(T_data_type data_type, char* name);
tTable* get_table(char* funcName, char* className);

#endif
