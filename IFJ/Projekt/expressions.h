/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   expressions.h
 * Datum:    11.11.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

#include <stdio.h>
#include <stdlib.h>
#include "scaner.h"
#include "ial.h"
#include "ilist.h"
#include "parser.h"

typedef enum Prec_symb {
  P_PLUS,        // 0 +
  P_MINUS,       // 1 -
  P_MUL,         // 2 *
  P_DIV,         // 3 /
  P_LESS,        // 4 <
  P_GREAT,       // 5 >
  P_LESS_EQUAL,  // 6 <=
  P_GREAT_EQUAL, // 7 >=
  P_EQUAL,       // 8 ==
  P_NOT_EQUAL,   // 9 !=
  P_LEFT_PAR,    // 10 (
  P_RIGHT_PAR,   // 11 )
  P_COMMA,       // 12 ,
  P_STRING,      // 13 string
  P_INT,         // 14 int
  P_DOUBLE,      // 15 double
  P_FUNCTION,    // 16 f //TODO
  P_DOLLAR,      // 17 $
  P_ID,          // 18 ID
  P_ASSIGN,      // 19 =
  Z_LESS,        // 20 < na zasobniku
  Z_NOT_TERM,    // 21 E na zasobniku
  less,	         // 22 < prec_tab
	great,	       // 23 > prec_tab
	equal,	       // 24 = prec_tab
	empty,	       // 25 E prec_tab
} Prec_symb;


//zasobnik
typedef struct Stack_Elem {
  struct Stack_Elem *Lptr;
  struct Stack_Elem *Rptr;
  Prec_symb symbol;
  T_value value; //i d s
  T_data_type type;
  //pridani pro test
  char *class_name;
  char *name;
  tStack *local_table_stack;
  bool defined; //Bude potreba vedet
} Stack_Elem;

typedef struct Stack{
  Stack_Elem *First;
  Stack_Elem *Act;
  Stack_Elem *Last;
} Stack;

Stack_Elem* expr_result;

//prace se zasobnikem
void Stack_init(Stack *); //inicializace
void Stack_dispose(Stack *); //zruseni
void Stack_insert_last(Stack *, Stack_Elem*);//PUSH na konec seznamu
void Stack_act_last(Stack *); //aktivita na posledni prvek
void Stack_value_last(Stack *, Stack_Elem **); //hodnota posledniho prvku
void Stack_delete_last(Stack *); //zrusi posledni prvek seznamu
void Stack_delete_before(Stack *); //zrusi predchozi prvek (pred aktivnim)
void Stack_push_after(Stack *, Stack_Elem*); //vlozi prvek za aktivnim
void Stack_push_before(Stack *, Stack_Elem*); //vlozi prvek pred aktivnim
void Stack_value_act(Stack *, Stack_Elem **); //hodnota aktivnho prvku
void Stack_act_before(Stack *); //posun aktivity na predchozi prvek


tError expr();//hlavni funkce
Stack_Elem* tok_to_term(tToken token);//prevod tokenu na term
tData* term_to_data(Stack_Elem* elem);//prevod term na data
tData* tmp_var(T_data_type typ, T_value *hodnota);
void symbolToString(Prec_symb symbol);

#endif
