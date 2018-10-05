/*
 * Predmet:  IFJ/IAL
 * Projekt:  Implementace interpretu jazyka IFJ16
 * Varianta: a/1/II
 * Soubor:   main.c
 * Datum:    27.10.2016
 * Autori:   Patrik Vlk
             Stepan Matalik
             Vlad Sherstobitov
             David Kollar
             Roman Nahalka
 */

#include <stdio.h>
#include "error.h"
#include "parser.h"
#include "interpret.h"

int main(int argc, char** argv)
{
   char *c = NULL;
   printf("%s", c);
   FILE *f;
   tError error = 0;
   debug = 11;

   init_garbage_collector();

   if (argc == 1)
   {
      printf("Neni zadan vstupni soubor\n");
      return E_INTERNAL;
   }
   if ((f = fopen(argv[1], "r")) == NULL)
   {
      printf("Soubor se nepodarilo otevrit\n");
      return E_INTERNAL;
   }
 //---------- spusteni
 	setSourceFile(f);
 	if (debug > 1) printf("main\n");

 	initTable(&global_table);   //---------- inicializace pomocnych struktur

 	listInit(&List);
  if (debug > 1) printf("\n-------------------------- Parser ------------------------\n\n");
 	get_token();                 //---------- spusteni parseru
 	error = parse();

  if (debug > 1)
  {
    printf("\n");
    tPrint(global_table);
    tPrint(get_table("run", "Main"));
  }

 	if ( error != OK)
 		throwError(error);

  if (debug > 1) printf("\n------------------------- Interpret -----------------------\n\n");

  error = inter(&List);             //---------- spusteni interpretu

 	if ( error != OK)
 		throwError(error);

    if (debug > 1)
    {
      printf("\n");
      tPrint(global_table);
      tPrint(get_table("run", "Main"));
    }
  
  printf("Celkem naalokovano: %d\n", CELKEM);
  fclose(f);
  free_garbage_collector(); 
 	return error;
}
