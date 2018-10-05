/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   error.h
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#ifndef ERROR_H
#define	ERROR_H

#include <malloc.h>

extern int CELKEM;

typedef enum tError {
	OK = 0,
	E_LEX = 1,
	E_SYNTAX = 2,
	E_SEMANTIC_UNDEFINED = 3,
	E_SEMANTIC_INCOMPATIBLE = 4,
	E_SEMANTIC_OTHER = 6,
	E_RUN_IN_DIGIT = 7,
	E_RUN_UNINITIALIZED_VAR = 8,
	E_RUN_DIV_BY_ZERO = 9,
	E_RUN_OTHER = 10,
	E_INTERNAL = 99,
} tError;

int throwError(int error_code);

 //list for garbage collector
typedef struct T_GarbageItem {
	void *ptr;
	struct T_GarbageItem *next;
}T_GarbageItem;

//struct of list for garbage collector
typedef struct T_Garbage {
	T_GarbageItem *first;
	T_GarbageItem *last;
}T_Garbage;

int init_garbage_collector();
T_GarbageItem *findRecord(void *tmpPtr);
void *my_malloc(size_t size);
void* my_realloc(void *ptr, size_t size);
void free_garbage_collector();
void my_free(void *ptr);

#endif
