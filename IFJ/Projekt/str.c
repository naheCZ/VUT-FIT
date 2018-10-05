/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   str.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <malloc.h>
#include "str.h"
#include "ial.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) my_malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
   my_free(s->str);
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
{
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
   return s->length;
}

string charToString(char* ch)
{
  string str;
  strInit(&str);

  for(int i = 0; ch[i] != '\0'; i++)
    strAddChar(&str, ch[i]);

  return str;
}

void strPrint(string *s)
{
  printf("%s\n", s->str);
}

int length_char(const char *text){
    int i = 0;
    while(text[i] != '\0'){
        i++;
    }
    return i;
}

char *substr_char(char *s, int i, int n){
    if (i + n > length_char(s))
        n = length_char(s) - i;
    if(i >= 0 && n >= 0){
        char *text = my_malloc(n+1);
        if(text == NULL)
            return NULL;
        else {
            memcpy(text, s+i, n);
            text[n] = '\0';
            return text;
        }
    }
    else
        return NULL;
}

char *concat_char(char *str_one, char *str_two){
    int len_one = length_char(str_one);
    int len_two = length_char(str_two);

    char *word = my_malloc(len_one + len_two + 1);
    if (word == NULL)
        return NULL;
    else {
        memcpy(word, str_one, len_one);
        memcpy(word+len_one, str_two, len_two+1);

        return word;
    }
}

char* sort_char(char text[])
{
    int l = strlen(text);
    char *news;
    if((news = my_malloc(l+1))){
        memcpy(news, text, l+1);
        if(l > 1)
            quickSort(news, 0, l-1);
    } else {news = NULL;}
    return news;
}

int find_char(char *text, char* pattern){
    int len_p = strlen(pattern);
    int len_t = strlen(text);

    if(len_p == 0)
        return 0;

    else if(len_t == 0)
        return -1;

    else if(len_p == 1){
        int position = 0;
        for(int i = 0; i < len_t; i++)
            if(text[i] == pattern[0])
                {position = i; break;}
        return position == 0 ? -1 : position;
    }
    else
        {
            int position = kmpMatch(pattern, text, len_p, len_t);
            return position < len_t ? position - 1: -1;
        }
}
