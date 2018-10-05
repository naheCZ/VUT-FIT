/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   stack.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "stack.h"

tStack* stackInit()
{
	tStack* stack;

	stack = my_malloc(sizeof(tStack));

	if (stack == NULL)
		return NULL;

	stack->bottom = my_malloc(sizeof(void*));

	if (stack->bottom == NULL)
		return NULL;

	stack->top = stack->bottom;

	return stack;
}

void stackDispose(tStack *stack)
{
	if (stack != NULL)
	{
		my_free(stack->bottom);
		my_free(stack->top);
		my_free(stack);
	}
}

void *stackTop(tStack *stack)
{
	if (stack == NULL || isStackEmpty(stack))
		return NULL;

	return *(stack->top - 1);
}

void *stackBottom(tStack *stack)
{
	if(stack == NULL || isStackEmpty(stack))
		return NULL;

	return *(stack->bottom);
}

void* stackPop(tStack *stack)
{
	if (stack == NULL || isStackEmpty(stack))
		return NULL;

	(stack->top)--;
	return *(stack->top);
}

void *stackPopBottom(tStack *stack)
{
	if(stack == NULL || isStackEmpty(stack))
		return NULL;

	(stack->bottom)++;
	return *(stack->bottom - 1);
}

tError stackPush(tStack *stack, void *data)
{
	if (stack == NULL)
		return E_INTERNAL;

	int size = (stack->top - stack->bottom);

	stack->bottom = my_realloc(stack->bottom, sizeof(void*) * (size + 1));

	if (stack->bottom == NULL)
		return E_INTERNAL;

  stack->top = stack->bottom + size;
	*(stack->top) = data;
	(stack->top)++;

	return OK;
}

bool isStackEmpty(tStack *stack)
{
	return (stack->bottom == stack->top);
}
