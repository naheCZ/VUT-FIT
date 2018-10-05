/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   ilist.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */


#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "ilist.h"

int debug_ilist = 1;
int line = 0;



const char* tInstCode_debug[] =
{
  // aritmeticke operace
  "I_Add",
  "I_Sub",
  "I_Mul",
  "I_Div",
  "I_Stop", // netreba vyplnovat op1,op2 a res
//  I_Start,// netreba vyplnovat op1,op2 a res

  // porovnavani
  "I_Less",
  "I_Greater",
  "I_LessEq",
  "I_GreaterEq",
  "I_Equal",
  "I_NotEqual", //10

  //prirazeni
  "I_Assign", // res = op1;

  "I_Func", // (NULL, NULL, symbol funkce)
  "I_Ret",  // (NULL, NULL, symbol návratové hodnoty)
  // vestavene funkce
  "I_ReadInt", //nacte hodnotu do op1
  "I_ReadDouble", //nacte hodnotu do op1
  "I_ReadString", //nacte hodnotu do op1
  "I_Print", // vytiskne op1
  "I_Lenght", // ulozi delku op1 do res
//  I_Substr, // !!! dodelavam
  "I_Compare", // porovna retezce na adrese op1 a op2 a vysledek ulozi do res
  "I_Find", //  vyhleda v retezci op1 podretzec op2, do res se ulozi jestli nasel
  "I_Sort", // seradi retezec v op1 a ulozi ho do res

  //skoky
  "I_GoTo", // skoci na adresu ulozenou v op2
  "I_IfGoTo", // pokud je op1 == 0 (je splnena podminka), pak skoci na op2
  "I_IfNGoTo", // pokud neni splnena podminka (op1 != 0), pak skoci na op2
  "I_Lab", // (NULL, NULL, návěští (symbol fce))  instrukce navesti, nic se neprovadi

  "I_NOP" // prázdná instrukce, nic se neprovádí - kvuli ifu a whilu, kdyby byly prazdne
};

void InstToString(tInstCode instCode)
{
    printf("_instrukcia: %s\n", tInstCode_debug[instCode]);
}

void listInit(tListOfInstr *L)
// funkce inicializuje seznam instrukci
{
  L->first  = NULL;
  L->last   = NULL;
  L->active = NULL;
}

void listFree(tListOfInstr *L)
// funkce dealokuje seznam instrukci
{
  tListItem *ptr;
  while (L->first != NULL)
  {
    ptr = L->first;
    L->first = L->first->nextItem;
    // uvolnime celou polozku
    my_free(ptr);
  }
}

void listInsertLast(tListOfInstr *L, tInstr I)
// vlozi novou instruci na konec seznamu
{
  tListItem *newItem;
  newItem = my_malloc(sizeof (tListItem));
  newItem->Instruction = I;
  newItem->nextItem = NULL;
  newItem->Instruction.line = line;
  if (L->first == NULL)
     L->first = newItem;
  else
     L->last->nextItem=newItem;
  L->last=newItem;
}

void listFirst(tListOfInstr *L)
// zaktivuje prvni instrukci
{
  L->active = L->first;
}

void listNext(tListOfInstr *L)
// aktivni instrukci se stane nasledujici instrukce
{
  if (L->active != NULL)
  L->active = L->active->nextItem;
}

void listGoto(tListOfInstr *L, void *gotoInstr)
// nastavime aktivni instrukci podle zadaneho ukazatele
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  L->active = (tListItem*) gotoInstr;
}

void *listGetPointerAct(tListOfInstr *L)
{
  return (void*) L->active;
}

void *listGetPointerLast(tListOfInstr *L)
// vrati ukazatel na posledni instrukci
// POZOR, z hlediska predmetu IAL tato funkce narusuje strukturu
// abstraktniho datoveho typu
{
  return (void*) L->last;
}

tInstr *listGetData(tListOfInstr *L)
// vrati aktivni instrukci
{
  if (L->active == NULL)
  {
    printf("Chyba, zadna instrukce neni aktivni");
    return NULL;
  }
  else return &(L->active->Instruction);
}

void generate(tInstCode instCode, void *op1, void *op2, void *res)
{
  //if (debug_ilist > 0) printf("INSTRUKCE: %d\n", instCode);
	tInstr inst;
	inst.instCode = instCode;
	inst.op1 = op1;
	inst.op2 = op2;
	inst.result = res;
	listInsertLast(&List,inst);	// list instrukci bude vytvoreny v parseru
  line++;
}
