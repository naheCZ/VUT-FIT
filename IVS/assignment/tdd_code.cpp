//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Roman Nahálka <xnahal01@stud.fit.vutbr.cz>
// $Date:       $2018-02-25
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Roman Nahálka
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
	first = NULL;
}

PriorityQueue::~PriorityQueue()
{
	Element_t *element = first;
	Element_t *del;

	while (element != NULL)
	{
		del = element;
		element = element->pNext;
		free(del);
	}
}

void PriorityQueue::Insert(int value)
{
	Element_t *element = (struct Element_t*) malloc(sizeof(struct Element_t));

	if (first == NULL) //Prazdny seznam, vlozeni prvniho prvku
	{
		element->pNext = NULL;
		element->pPrev = NULL;
		element->value = value;
		first = element;
	}

	else //Neprazdny seznam, umistime spravne prvek podle hodnoty
	{
		Element_t *cur = first;
		Element_t *preCur;
		bool found = false;

		while (cur != NULL)
		{
			if (value <= cur->value)
			{
				found = true;
				break;
			}

			preCur = cur;
			cur = cur->pNext;
		}

		if (found) //Vlozeni do seznamu
		{
			Element_t *prev = cur->pPrev;

			if (prev == NULL) //Vlozeni pred prvni prvek
			{
				element->pNext = cur;
				element->pPrev = NULL;
				element->value = value;
				cur->pPrev = element;
				first = element;
			}

			else //Vlozeni mezi dva prvky
			{
				element->pNext = cur;
				element->pPrev = prev;
				element->value = value;
				prev->pNext = element;
				cur->pPrev = element;
			}
		}

		else //Vlozeni na konec seznamu
		{
			element->pNext = NULL;
			element->pPrev = preCur;
			element->value = value;
			preCur->pNext = element;
		}
	}
}

bool PriorityQueue::Remove(int value)
{
	Element_t *element;
	bool found = false;

	element = Find(value); //Nalezneme prvek

	if (element != NULL) //Prvek nalezen, odstranime
	{
		found = true;
		Element_t *next = element->pNext;
		
		if (element->pPrev == NULL) //Prvni prvek
		{

			if (next != NULL) //Vic jak jeden prvek
			{
				first = next;
				next->pPrev = NULL;
			}

			else
				first = NULL;

			free(element);
		}

		else
		{
			Element_t *prev = element->pPrev;

			if (next == NULL) //Posledni prvek
			{
				prev->pNext = NULL;
				free(element);
			}

			else //Meziprvek
			{
				prev->pNext = next;
				next->pPrev = prev;
				free(element);
			}
		}
	}

	return found;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
	Element_t *element = first;

	while (element != NULL) //Projdeme cely seznam
	{
		if (element->value == value) //Nalezen hledany prvek
			return element;

		element = element->pNext;
	}
	
	return NULL;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return first;
}

/*** Konec souboru tdd_code.cpp ***/
