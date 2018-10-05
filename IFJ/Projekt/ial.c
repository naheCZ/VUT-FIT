/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   ial.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David KollarS
             Roman Nahalka
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "parser.h"

int SIZE = MAX_SIZE; //Velikost tabulky

//quickSort
void partition(char *a, int left, int right, int* i, int* j)
{
  //Funkce rozdeli pole na dve casti, levou a pravou.

  char pm;
  char pom;

  *i = left;
  *j = right;
  pm = a[(*i + *j)/2]; //Pseudomedian

  do
  {
    while(a[(*i)] < pm) //Prvni i zleva, pro a[i] >= pm
      (*i)++;
    while(a[(*j)] > pm) //Prvni j zprava pro a[j] <= pm
      (*j)--;
    if((*i) <= (*j)) //Vymena nalazenych prvku
    {
      pom = a[(*i)];
      a[(*i)] = a[(*j)];
      a[(*j)] = pom;
      (*i)++;
      (*j)--;
    }
  } while((*i) <= (*j)); //Cyklus konci, kdyz se indexy i a j prekrizi
}

void quickSort(char* a, int left, int right) //Rekurzivni algoritmus quickSort
{
  int i;
  int j;

  partition(a, left, right, &i, &j); //Rozdeleni pole
  if(left < j)
    quickSort(a, left, j); //Rekurze doleva
  if(i < right)
    quickSort(a, i, right); //Rekurze doprava
}

//Knuth-Morris-Pratt algoritmus
int kmpMatch(char* text, char* pattern, int textLength, int patternLength)
{
  int i;
  int j;
  int fail[patternLength];

  i = 2;
  j = 0;
  fail[0] = -1;
  fail[1] = 0;

  while(i < patternLength)
  {
    if(pattern[i-1] == pattern[j])
    {
      j++;
      fail[i] = j;
      i++;
    }

    else if(j > 0)
      j = fail[j];

    else
    {
      fail[i] = 0;
      i++;
    }
  }

  i = j = 0;

  while((i + j) < textLength)
  {
    if(pattern[j] == text[i+j])
    {
      if(j == (patternLength - 1))
        return i;

      j++;
    }

    else
    {
      i = i + j - fail[j];

      if(fail[j] > -1)
        j = fail[j];
      else
        j = 0;
    }
  }
  return -1;
}

//Tabulka symbolu
int hashCode(char* key) //Rozptylovaci funkce
{
  int retval = 1;
  int keylen = strlen(key);

  for(int i = 0; i < keylen; i++)
    retval += key[i];

  return (retval % SIZE);
}


void initTable(tTable **table)
{
  tItem *item = (tItem *) my_malloc(sizeof(tItem));

  if(item != NULL)
	{
		item->key = "*UNDEF*";
		item->data.info = NONE;
		item->next = NULL;
    item->data.data_type = VOID;
    item->data.class_name = NULL;
    item->data.name = NULL;
    item->data.local_table_stack = NULL; // ukazatel na lokální tabulky, u proměnných raději nastavit NULL
    item->data.param_n = 0; // Poradi parametru funkce, pokud neni paramter hodnota je 0
    item->data.value.i = 0; // Nastavuje se pouze jedna hodnota z unionu
    item->data.defined = false;
	}

  else
    throwError(E_INTERNAL);

  *table = NULL;
  *table = (tTable*) my_malloc(sizeof(tTable));

  if(*table != NULL)
	{
		for (int i = 0; i < MAX_SIZE; (**table)[i++] = item);
		tPartialInit(*table);
	}

  else
    throwError(E_INTERNAL);
}

void tPartialInit(tTable* table) //Inicializace tabulky
{
  if(table != NULL)
  {
    for(int i = 0; i < SIZE; i++)
    {
      (*table)[i] = NULL;
    }
  }
}

//Vyhledavani prvku podle zadaneho klice
tItem* tSearch(tTable* table, char* key)
{
  if(table != NULL)
  {

    int i = hashCode(key);
    tItem *pom = (*table)[i];

    while(pom != NULL) //Dokud je kde hledat
    {
      if(strcmp(pom->key, key) == 0) //Klic nalezen
        return pom;
      else
        pom = pom->next; //Hledame dal
    }
    return NULL; //Klic nenalezen
  }

  else
    return NULL;
}

tItem* tSearchByInfo(tTable* table, char* key, char* class_name, T_data_info info)
{
  if(table != NULL)
  {
    if(info == FUNC) //Hledame funkci
    {
      int i = hashCode(key);
      tItem* pom = (*global_table)[i]; //prohledavame pouze globalni tabulku

      while(pom != NULL)
      {
        if(strcmp(pom->key, key) == 0 && strcmp(pom->data.class_name, class_name) == 0 && pom->data.info == info)
          return pom;
        else
          pom = pom->next;
      }

      return NULL;
    }

    else if(info == VAR) //Hledame promennou
    {
      int i = hashCode(key);
      tItem* pom = (*table)[i]; //Nejdrive prohledame lokalni tabulku

      
      while(pom != NULL)
      {
        if(strcmp(pom->key, key) == 0)
          return pom;
        else
          pom = pom->next;
      }

      pom = (*global_table)[i]; //Pokud neni promenna v lokalni tabulce, prohledame globalni

      while(pom != NULL)
      {
        if(strcmp(pom->key, key) == 0 && strcmp(pom->data.class_name, class_name) == 0 && pom->data.info == info)
          return pom;
        else
          pom = pom->next;
      }

      return NULL;
    }

    else
      return NULL;
  }

  else
    return NULL;
}

void tInsert(tTable* table, char* key, tData data) //Vlozeni prvku do tabulky
{
  if(table != NULL)
  {
    int i = hashCode(key);
    tItem *pom = tSearch(table, key);

    if(pom != NULL) //Prvek nalazen
      pom->data = data; //Aktualizujeme jen jeho data

    else //Prvek nenalezen, vlozime novy
    {

      tItem* novy = my_malloc(sizeof(struct tItem));
      if (novy == NULL)
        throwError(E_INTERNAL);

      if ((novy->key = my_malloc(sizeof(char) * strlen(key))) == NULL)
    		throwError(E_INTERNAL);

      novy->data = data;
      strcpy(novy->key, key);
      novy->next = (*table)[i];
      (*table)[i] = novy;
    }
  }

  else
    throwError(E_INTERNAL);
}

//Zjistovani hodnoty datove casti polozky zadane klicem
tData* tRead(tTable* table, char* key)
{
  if(table != NULL)
  {
    tItem *pom = tSearch(table, key);

    if(pom == NULL) //Prvek nenalezen, nemame co cist
      return NULL;

    else //prvek nalezen, vratime ukazatel na jeho data
      return &(pom->data);
  }

  else
    return NULL;
}

tData* tReadByInfo(tTable* table, char* key, char* class_name, T_data_info info)
{

  if(table != NULL)
  {
    tItem *pom = tSearchByInfo(table, key, class_name, info);

    if(pom == NULL)
      return NULL;

    else
    {

      return &(pom->data);
    }
  }

  else
    return NULL;
}

void tRewrite(tTable* table, char* key, char* class_name, T_data_info info, tData data)
{
  if(table != NULL)
  {
    tItem *pom = tSearchByInfo(table, key, class_name, info);

    if(pom == NULL)
      throwError(E_INTERNAL);

    else
      pom->data = data;
  }

  else
    throwError(E_INTERNAL);
}

//Vyjmuti polozky dane klicem z tabulky
void tDelete(tTable* table, char* key)
{
  if(table != NULL)
  {
    int i = hashCode(key);
    tItem* pom = (*table)[i];
    tItem* pom2 = (*table)[i];

    if(pom != NULL)
    {
      if(pom->key == key) //Nalezli jsme prvek, ktery chceme smazat
      {
        (*table)[i] = pom->next;
        my_free(pom);
      }

      else //Zatim jsme ho nenalezli, budeme hledat dal
      {
        while(pom->next != NULL)
        {
          pom2 = pom;
          pom = pom->next;

          if(pom->key == key) //Uz jsme prvek nalezli
          {
            pom = pom->next;
            pom2->next = pom;
            my_free(pom);
            return;
          }
        }

        pom2 = pom;

      }
    }
  }

  else
    throwError(E_INTERNAL);
}

//Zrusi vsechny polozky tabulky a uvolni prostor
void tClearAll(tTable* table)
{
  if(table != NULL)
  {
    tItem* pom;
    tItem* pom2;

    for(int i = 0; i < SIZE; i++) //Projedeme celou tabulku
    {
      pom = (*table)[i];

      while(pom != NULL)
      {
        pom2 = pom->next;
        my_free(pom);
        pom = pom2;
      }

      (*table)[i] = NULL; //Zinicilizujeme prvek
    }
  }

  else
    throwError(E_INTERNAL);
}

void tPrint(tTable* table)
{
  int sum = 0;

  printf("---------TABULKA---------\n");

  for(int i = 0; i < SIZE; i++)
  {
    tItem* ptr = (*table)[i];

    while(ptr != NULL)
    {
      if(strcmp(ptr->key, "*UNDEF*"))
      {
        printf("Klic: %s, ", ptr->key);
        if(ptr->data.data_type == INT)
          printf("data: %d, ", ptr->data.value.i);
        else if(ptr->data.data_type == STRING)
          printf("data: %s, ", ptr->data.value.s);
        else if(ptr->data.data_type == DOUBLE)
          printf("data: %f", ptr->data.value.d);
        ptr = ptr->next;
        sum++;
        printf("\n");
      }

      else
        ptr = ptr->next;
    }
  }

  printf("\nPocet polozek v tabulce: %d\n", sum);
}
