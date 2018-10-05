/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   stack.h
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

 #ifndef STACK_H
 #define STACK_H

#include <string.h>
#include <stdbool.h>

#include "error.h"

typedef struct {
	void **bottom;
	void **top;
} tStack;

tStack* stackInit();
void *stackPop(tStack *stack);
void *stackPopBottom(tStack *stack);
void *stackTop(tStack *stack);
void *stackBottom(tStack *stack);
tError stackPush(tStack *stack, void *data);
void stackDispose(tStack *stack);
bool isStackEmpty(tStack *stack);

#endif
