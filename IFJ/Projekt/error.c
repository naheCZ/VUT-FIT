/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   error.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */


#include <stdio.h>
#include <stdlib.h>
#include "error.h"

int CELKEM = 0;


struct T_Garbage *garbage = NULL;

int throwError(int error_code){
	switch(error_code){
		case OK:
			break;
		case E_LEX:
			fprintf(stderr,"ERROR %d: Chyba v programu v ramci lexikalni analyzy (chybna struktura aktualniho lexemu).\n", error_code);
			break;
		case E_SYNTAX:
			fprintf(stderr,"ERROR %d: Chyba v programu v ramci syntakticke analyzy (chybna syntaxe programu).\n", error_code);
			break;
		case E_SEMANTIC_UNDEFINED :
			fprintf(stderr,"ERROR %d: Semanticka chyba v programu – nedefinovana trida/funkce/promenna, pokus o redefinici tridy/funkce/promenne, atd.\n", error_code);
			break;
		case E_SEMANTIC_INCOMPATIBLE:
			fprintf(stderr,"ERROR %d: Semanticka chyba typove kompatibility v aritmetickych, retezcovych a relacnich vyrazech, prip. spatny pocetci typ parametru u volani funkce.\n", error_code);
			break;
		case E_SEMANTIC_OTHER:
			fprintf(stderr,"ERROR %d: Ostatni semanticke chyby.\n", error_code);
			break;
		case E_RUN_IN_DIGIT:
			fprintf(stderr,"ERROR %d: Behova chyba pri nacitani ciselne hodnoty ze vstupu.\n", error_code);
			break;
		case E_RUN_UNINITIALIZED_VAR:
			fprintf(stderr,"ERROR %d: Behova chyba pri praci s neinicializovanou promennou.\n", error_code);
			break;
		case E_RUN_DIV_BY_ZERO:
			fprintf(stderr,"ERROR %d: Behova chyba deleni nulou. \n", error_code);
			break;
		case E_RUN_OTHER:
			fprintf(stderr,"ERROR %d: Ostatni behove chyby.\n", error_code);
			break;
		case E_INTERNAL:
			fprintf(stderr,"ERROR %d: Interni chyba interpretu tj. neovlivnena vstupnim programem (napr. chyba alokace pameti, chyba pri otvirani souboru s ridicim programem, spatne parametry prikazove radky atd.\n", error_code);
			break;
	}
	free_garbage_collector();
	exit(error_code);
	return error_code;
}

int init_garbage_collector()
{
	garbage = malloc(sizeof(T_Garbage));

	if (garbage == NULL) {
		throwError(E_INTERNAL);
	}
	else
	{
		garbage->first = NULL;
		garbage->last = NULL;
		return OK;
	}
	return OK;
}

T_GarbageItem *findItem(void *tmpPtr)
{
	T_GarbageItem *ptrItem = garbage->first;

	while (ptrItem != NULL)
	{
		if (tmpPtr == ptrItem->ptr)
			return ptrItem;

		ptrItem = ptrItem->next;
	}

	return NULL;
}

void *my_malloc(size_t size)
{
	CELKEM += size;
	void *tmpPtr = malloc(size);
	T_GarbageItem *ptrItem = malloc(sizeof(T_GarbageItem));

	if (ptrItem == NULL || tmpPtr == NULL) {
		if (tmpPtr != NULL)
			free(tmpPtr);
			throwError(E_INTERNAL);
	}

	ptrItem->ptr = tmpPtr;
	ptrItem->next = NULL;

	if (garbage->first == NULL)
	{
		garbage->first = ptrItem;
		garbage->last = ptrItem;
	}
	else
	{
		garbage->last->next = ptrItem;
		garbage->last = ptrItem;
	}

	return tmpPtr;
}

// Our memory reallocation. If function found an allocation in garbage, reallocate it and makes pointer in garbage.
void* my_realloc(void *ptr, size_t size)
{

	T_GarbageItem *ptrItem = findItem(ptr);
	void *tmpPtr = NULL;

	if (ptrItem == NULL)					// no item in garbage
		tmpPtr = my_malloc(size);
	else
	{
		CELKEM += size;
		tmpPtr = realloc(ptr, size);
		ptrItem->ptr = tmpPtr;
	}
	if (tmpPtr == NULL) {
		throwError(E_INTERNAL);
	}

	return tmpPtr;
}

// Clearing of garbage collector. In this function is all memory released.
void free_garbage_collector()
{
	T_GarbageItem *ptrItem = garbage->first;
	T_GarbageItem *ptrItemNext = NULL;

	while (ptrItem != NULL)
	{
		free(ptrItem->ptr);
		ptrItem->ptr = NULL;
		ptrItemNext = ptrItem->next;
		free(ptrItem);
		ptrItem = ptrItemNext;
	}

	free(garbage);
}


// Clearing of allocated memory, which is allocated on address at pointer.
void my_free(void *ptr)
{
	T_GarbageItem *ptrItem = findItem(ptr);
	T_GarbageItem *ptrItemActual = NULL;
	T_GarbageItem *ptrItemPrev = NULL;

	if (ptrItem == garbage->first)  // pointer is first item of garbage
	{
		garbage->first = ptrItem->next;
		free(ptrItem->ptr);
		free(ptrItem);
	}
	else
	{
		ptrItemActual = garbage->first->next;
		ptrItemPrev = garbage->first;
		while (ptrItemActual->next != NULL)
		{

			if (ptrItem == ptrItemActual)				// we found allocated memory
			{
				ptrItemPrev->next = ptrItemActual->next;
				free(ptrItem->ptr);
				free(ptrItem);
			}

			ptrItemActual = ptrItemActual->next;		// move to next item
			ptrItemPrev = ptrItemPrev->next;
		}
	}
}
