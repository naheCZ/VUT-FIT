/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   ilist.h
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

// typy jednotlivych instrukci, David definovat kody instrukci

#ifndef ILIST_H
#define ILIST_H

#include "parser.h"

typedef enum tInstCode {
 	// aritmeticke operace
	I_Add = 0,
	I_Sub,
	I_Mul,
	I_Div,
  I_Stop, // netreba vyplnovat op1,op2 a res
//	I_Start,// netreba vyplnovat op1,op2 a res

  // porovnavani
	I_Less,
  I_Greater,
  I_LessEq,
  I_GreaterEq,
  I_Equal,
  I_NotEqual, //10

	//prirazeni
	I_Assign, // res = op1;

	I_Func, // (NULL, NULL, symbol funkce)
	I_Ret,  // (NULL, NULL, symbol návratové hodnoty)
	// vestavene funkce
	I_ReadInt, //nacte hodnotu do op1
	I_ReadDouble, //nacte hodnotu do op1
	I_ReadString, //nacte hodnotu do op1
	I_Print, // vytiskne op1
	I_Lenght, // ulozi delku op1 do res
//	I_Substr, // !!! dodelavam
	I_Compare, // porovna retezce na adrese op1 a op2 a vysledek ulozi do res
	I_Find, //  vyhleda v retezci op1 podretzec op2, do res se ulozi jestli nasel
	I_Sort, // seradi retezec v op1 a ulozi ho do res

	//skoky
	I_GoTo, // skoci na adresu ulozenou v op2
	I_IfGoTo, // pokud je op1 == 0 (je splnena podminka), pak skoci na op2
	I_IfNGoTo, // pokud neni splnena podminka (op1 != 0), pak skoci na op2
	I_Lab, // (NULL, NULL, návěští (symbol fce))  instrukce navesti, nic se neprovadi

	I_NOP // prázdná instrukce, nic se neprovádí - kvuli ifu a whilu, kdyby byly prazdne
} tInstCode;

typedef struct
{
  tInstCode instCode;
  void *op1;
  void *op2;
  void *result;
	int line;
} tInstr;

typedef struct listItem
{
  tInstr Instruction;
  struct listItem *nextItem;
} tListItem;

typedef struct
{
  struct listItem *first;  // ukazatel na prvni prvek
  struct listItem *last;   // ukazatel na posledni prvek
  struct listItem *active; // ukazatel na aktivni prvek
} tListOfInstr;

tListOfInstr List;

void listInit(tListOfInstr *L);
void listFree(tListOfInstr *L);
void listInsertLast(tListOfInstr *L, tInstr I);
void listFirst(tListOfInstr *L);
void listNext(tListOfInstr *L);
void listGoto(tListOfInstr *L, void *gotoInstr);
void *listGetPointerLast(tListOfInstr *L);
tInstr *listGetData(tListOfInstr *L);
void generate(tInstCode instCode, void *op1, void *op2, void *res);
void InstToString(tInstCode instCode);

#endif
