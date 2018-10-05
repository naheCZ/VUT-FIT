/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   functions.h
 * Datum:    10.11.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "str.h"

//Deklarace vestavenych funkci pro praci s retezci
int length(string s);
string substr(string s, int i, int n);
int compare(string s1, string s2);
int find(string s, string search);
string sort(string s);


#endif
