
/* c201.c *********************************************************************}
{* Téma: Jednosmirný lineární seznam
**
**                     Návrh a referenení implementace: Petr Poikryl, oíjen 1994
**                                          Úpravy: Andrea Nimcová listopad 1996
**                                                   Petr Poikryl, listopad 1997
**                                Poepracované zadání: Petr Poikryl, boezen 1998
**                                  Poepis do jazyka C: Martin Tueek, oíjen 2004
**	                                      Úpravy: Bohuslav Koena, oíjen 2015
**
** Implementujte abstraktní datový typ jednosmirný lineární seznam.
** U3iteeným obsahem prvku seznamu je celé eíslo typu int.
** Seznam bude jako datová abstrakce reprezentován prominnou typu tList.
** Definici konstant a typu naleznete v hlaviekovém souboru c201.h.
**
** Va1ím úkolem je implementovat následující operace, které spolu s vý1e
** uvedenou datovou eástí abstrakce tvooí abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu poed prvním pou3itím,
**      DisposeList ... zru1ení v1ech prvku seznamu,
**      InsertFirst ... vlo3ení prvku na zaeátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zru1í první prvek seznamu,
**      PostDelete .... ru1í prvek za aktivním prvkem,
**      PostInsert .... vlo3í nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... poepí1e obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na dal1í prvek seznamu,
**      Active ........ zji1Ÿuje aktivitu seznamu.
**
** Poi implementaci funkcí nevolejte 3ádnou z funkcí implementovaných v rámci
** tohoto poíkladu, není-li u dané funkce explicitni uvedeno nico jiného.
**
** Nemusíte o1etoovat situaci, kdy místo legálního ukazatele na seznam
** poedá nikdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodni komentujte!
**
** Terminologická poznámka: Jazyk C nepou3ívá pojem procedura.
** Proto zde pou3íváme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozorniní na to, 3e do1lo k chybi.
** Tato funkce bude volána z nikterých dále implementovaných operací.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální prominná -- poíznak chyby */
}

void InitList (tList *L)
{
/*
** Provede inicializaci seznamu L poed jeho prvním pou3itím (tzn. 3ádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádit nad ji3 inicializovaným
** seznamem, a proto tuto mo3nost neo1etoujte. V3dy poedpokládejte,
** 3e neinicializované prominné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zru1í v1echny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Ve1kerá pamiŸ pou3ívaná prvky seznamu L bude korektni
** uvolnina voláním operace free.
***/

    L->Act = NULL;
    tElemPtr pom = NULL;

    while(L->First != NULL)
    {
        pom = L->First->ptr;
        if(L->First != NULL)
            free(L->First);
        L->First = pom;
    }
}

void InsertFirst (tList *L, int val) {
/*
** Vlo3í prvek s hodnotou val na zaeátek seznamu L.
** V poípadi, 3e není dostatek pamiti pro nový prvek poi operaci malloc,
** volá funkci Error().
**/

    tElemPtr pom = (tElemPtr)malloc(sizeof(struct tElem));
    if(pom == NULL)
    {
        Error();
        return;
    }

    pom->data = val;
    pom->ptr = L->First;
    L->First = pom;
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný poíkaz, ani3 byste testovali,
** zda je seznam L prázdný.
**/

    L->Act = L->First;

}

void CopyFirst (tList *L, int *val) {
/*
** Prostoednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
    if(L->First == NULL) // Prázdný seznam = ERROR
        Error();
    else
        *val = L->First;
}

void DeleteFirst (tList *L) {
/*
** Zru1í první prvek seznamu L a uvolní jím pou3ívanou pamiŸ.
** Pokud byl ru1ený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se nedije.
**/

    tElemPtr pom;

    if(L->First == L->Act)
        L->Act == NULL;

    if(L->First != NULL) //Pouze pro neprázdný seznam
    {
        pom = L->First;
        L->First = pom->ptr;
        free(pom);
    }
}

void PostDelete (tList *L) {
/*
** Zru1í prvek seznamu L za aktivním prvkem a uvolní jím pou3ívanou pamiŸ.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se nedije.
**/

    tElemPtr pom;
    if(L->Act != NULL) //Pouze pro neprázdný seznam
    {
        pom = L->Act->ptr;
        if(pom != NULL)
        {
            L->Act->ptr = pom->ptr;
            free(pom);
        }
    }
}

void PostInsert (tList *L, int val) {
/*
** Vlo3í prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se nedije!
** V poípadi, 3e není dostatek pamiti pro nový prvek poi operaci malloc,
** zavolá funkci Error().
**/

    tElemPtr pom = (tElemPtr)malloc(sizeof(struct tElem));
    if(pom == NULL)
    {
        Error();
        return;
    }

    if(L->Act != NULL) //Pouze pro neprázdný seznam
    {
        pom->data = val;
        pom->ptr = L->Act->ptr;
        L->Act->ptr = pom;
    }
}

void Copy (tList *L, int *val) {
/*
** Prostoednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
**/
    if(L->Act == NULL)
        Error();
    else
        *val = L->Act->data;
}

void Actualize (tList *L, int val) {
/*
** Poepí1e data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedilá nic!
**/
    if (L->Act != NULL) //Pouze pro neprázdný seznam
        L->Act->data = val;
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** V1imnite si, 3e touto operací se mu3e aktivní seznam stát neaktivním.
** Pokud není poedaný seznam L aktivní, nedilá funkce nic.
**/
    if(L->Act != NULL) //Pouze pro neprázdný seznam
        L->Act = L->Act->ptr;

}

int Active (tList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním poíkazem return.
**/

    return L->Act != NULL;
}

/* Konec c201.c */
