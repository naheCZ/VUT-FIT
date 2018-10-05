/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   functions.c
 * Datum:    10.11.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#include <stdio.h>
#include <stdlib.h>
#include "ial.h"
#include "functions.h"
#include "ilist.h"

int length(string s) //Funkce vraci delku retezce
{
  return strGetLength(&s);
}

string substr(string s, int i, int n) //Funkce najde a vrati podretezec zadany indexi
{
  string sub;
  strInit(&sub);

  while(s.str[i] != '\0' && i <= n) //Cyklus na ziskani podretezce znak po znaku
  {
    strAddChar(&sub, s.str[i]);
    i++;
  }

  return sub;
}

int compare(string s1, string s2) //Funkce porovna dva retezce
{
  int cmp;
  cmp = strCmpString(&s1, &s2);

  if(cmp == 0) //Shonde retezce
    return 0;

  else if(cmp > 0)
    return 1;

  else
    return -1;
}

int find(string s, string search) //Funkce vyhleda zadany podretezec
{
  int match;

  if(s.length == 0) //Mame kde hledat?
    return -1;

  if(search.length == 0) //Hledame prazdny retezec
    return 0;

  match = kmpMatch(s.str, search.str, s.length, search.length);

  return match;
}

string sort(string s) //Funkce vraci abecedne serazeny retezec
{
  int length;
  string sort;

  strInit(&sort);
  strCopyString(&sort, &s);
  length = strGetLength(&sort);

  if(length > 1) //Mame co serazovat?
    quickSort(sort.str, 0, length - 1);

  return sort;
}
