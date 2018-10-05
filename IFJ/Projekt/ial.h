/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   ial.h
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */




#ifndef IAL_H
#define IAL_H

#include <stdbool.h>
#include "scaner.h"
#include "stack.h"

#define MAX_SIZE 101 //Maximalni velikost tabulky

//Definice Struktur tabulky symbolu


typedef enum T_data_info //Typ polozky
{
  CLASS,
	FUNC,
	VAR,
	NONE
} T_data_info;


typedef enum T_data_type {
  INT,
  DOUBLE,
  STRING,
  VOID
} T_data_type;

typedef struct tData //Data polozky v tabulce
{
  T_data_info info;
	T_data_type data_type;
  char *class_name;
  char *name;
  tStack *local_table_stack; // ukazatel na lokální tabulky, u proměnných raději nastavit NULL
  int param_n; // Poradi parametru funkce, pokud neni paramter hodnota je 0
  T_value value;
  bool defined; //Bude potreba vedet
  //Todo: U funkce by mel byt odkaz na skok nebo pro kazdou funkci samostatna posloupnost instrukci
} tData;

typedef struct tItem //Polozka v tabulce
{
  char* key; //Klic a nazev
  tData data; //Obsah
  struct tItem* next; //Ukazatel na dalsi polozku
} tItem;


typedef tItem* tTable[MAX_SIZE]; //Samotna tabulka
extern int SIZE;

//Deklarace funkci pro tabulku symbolu
int hashCode(char* key);
void tPartialInit(tTable* table);
tItem* tSearch(tTable* table, char* key);
tItem* tSearchByInfo(tTable* table, char* key, char* class_name, T_data_info info);
void tInsert(tTable* table, char* key, tData data);
void tRewrite(tTable* table, char* key, char* class_name, T_data_info info, tData data);
tData* tRead(tTable* table, char* key);
tData* tReadByInfo(tTable* table, char* key, char* class_name, T_data_info info);
void tDelete(tTable* table, char* key);
void tClearAll(tTable* table);
void tPrint(tTable* table);
void initTable(tTable **table);

//Deklarace funkci pro quicksort
void partition(char* a, int left, int right, int* i, int* j);
void quickSort(char* a, int left, int right);

//Deklarace funkci pro Knuth-Morris-Pratt algoritmus
int kmpMatch(char* t, char* p, int tl, int pl);


#endif
