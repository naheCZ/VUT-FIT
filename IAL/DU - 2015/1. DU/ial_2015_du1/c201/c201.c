
/* c201.c *********************************************************************}
{* T�ma: Jednosmirn� line�rn� seznam
**
**                     N�vrh a referenen� implementace: Petr Poikryl, o�jen 1994
**                                          �pravy: Andrea Nimcov� listopad 1996
**                                                   Petr Poikryl, listopad 1997
**                                Poepracovan� zad�n�: Petr Poikryl, boezen 1998
**                                  Poepis do jazyka C: Martin Tueek, o�jen 2004
**	                                      �pravy: Bohuslav Koena, o�jen 2015
**
** Implementujte abstraktn� datov� typ jednosmirn� line�rn� seznam.
** U3iteen�m obsahem prvku seznamu je cel� e�slo typu int.
** Seznam bude jako datov� abstrakce reprezentov�n prominnou typu tList.
** Definici konstant a typu naleznete v hlaviekov�m souboru c201.h.
**
** Va1�m �kolem je implementovat n�sleduj�c� operace, kter� spolu s v�1e
** uvedenou datovou e�st� abstrakce tvoo� abstraktn� datov� typ tList:
**
**      InitList ...... inicializace seznamu poed prvn�m pou3it�m,
**      DisposeList ... zru1en� v1ech prvku seznamu,
**      InsertFirst ... vlo3en� prvku na zae�tek seznamu,
**      First ......... nastaven� aktivity na prvn� prvek,
**      CopyFirst ..... vrac� hodnotu prvn�ho prvku,
**      DeleteFirst ... zru1� prvn� prvek seznamu,
**      PostDelete .... ru1� prvek za aktivn�m prvkem,
**      PostInsert .... vlo3� nov� prvek za aktivn� prvek seznamu,
**      Copy .......... vrac� hodnotu aktivn�ho prvku,
**      Actualize ..... poep�1e obsah aktivn�ho prvku novou hodnotou,
**      Succ .......... posune aktivitu na dal1� prvek seznamu,
**      Active ........ zji1�uje aktivitu seznamu.
**
** Poi implementaci funkc� nevolejte 3�dnou z funkc� implementovan�ch v r�mci
** tohoto po�kladu, nen�-li u dan� funkce explicitni uvedeno nico jin�ho.
**
** Nemus�te o1etoovat situaci, kdy m�sto leg�ln�ho ukazatele na seznam
** poed� nikdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodni komentujte!
**
** Terminologick� pozn�mka: Jazyk C nepou3�v� pojem procedura.
** Proto zde pou3�v�me pojem funkce i pro operace, kter� by byly
** v algoritmick�m jazyce Pascalovsk�ho typu implemenov�ny jako
** procedury (v jazyce C procedur�m odpov�daj� funkce vracej�c� typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornin� na to, 3e do1lo k chybi.
** Tato funkce bude vol�na z nikter�ch d�le implementovan�ch operac�.
**/
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* glob�ln� prominn� -- po�znak chyby */
}

void InitList (tList *L)
{
/*
** Provede inicializaci seznamu L poed jeho prvn�m pou3it�m (tzn. 3�dn�
** z n�sleduj�c�ch funkc� nebude vol�na nad neinicializovan�m seznamem).
** Tato inicializace se nikdy nebude prov�dit nad ji3 inicializovan�m
** seznamem, a proto tuto mo3nost neo1etoujte. V3dy poedpokl�dejte,
** 3e neinicializovan� prominn� maj� nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;
}

void DisposeList (tList *L) {
/*
** Zru1� v1echny prvky seznamu L a uvede seznam L do stavu, v jak�m se nach�zel
** po inicializaci. Ve1ker� pami� pou3�van� prvky seznamu L bude korektni
** uvolnina vol�n�m operace free.
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
** Vlo3� prvek s hodnotou val na zae�tek seznamu L.
** V po�padi, 3e nen� dostatek pamiti pro nov� prvek poi operaci malloc,
** vol� funkci Error().
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
** Nastav� aktivitu seznamu L na jeho prvn� prvek.
** Funkci implementujte jako jedin� po�kaz, ani3 byste testovali,
** zda je seznam L pr�zdn�.
**/

    L->Act = L->First;

}

void CopyFirst (tList *L, int *val) {
/*
** Prostoednictv�m parametru val vr�t� hodnotu prvn�ho prvku seznamu L.
** Pokud je seznam L pr�zdn�, vol� funkci Error().
**/
    if(L->First == NULL) // Pr�zdn� seznam = ERROR
        Error();
    else
        *val = L->First;
}

void DeleteFirst (tList *L) {
/*
** Zru1� prvn� prvek seznamu L a uvoln� j�m pou3�vanou pami�.
** Pokud byl ru1en� prvek aktivn�, aktivita seznamu se ztr�c�.
** Pokud byl seznam L pr�zdn�, nic se nedije.
**/

    tElemPtr pom;

    if(L->First == L->Act)
        L->Act == NULL;

    if(L->First != NULL) //Pouze pro nepr�zdn� seznam
    {
        pom = L->First;
        L->First = pom->ptr;
        free(pom);
    }
}

void PostDelete (tList *L) {
/*
** Zru1� prvek seznamu L za aktivn�m prvkem a uvoln� j�m pou3�vanou pami�.
** Pokud nen� seznam L aktivn� nebo pokud je aktivn� posledn� prvek seznamu L,
** nic se nedije.
**/

    tElemPtr pom;
    if(L->Act != NULL) //Pouze pro nepr�zdn� seznam
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
** Vlo3� prvek s hodnotou val za aktivn� prvek seznamu L.
** Pokud nebyl seznam L aktivn�, nic se nedije!
** V po�padi, 3e nen� dostatek pamiti pro nov� prvek poi operaci malloc,
** zavol� funkci Error().
**/

    tElemPtr pom = (tElemPtr)malloc(sizeof(struct tElem));
    if(pom == NULL)
    {
        Error();
        return;
    }

    if(L->Act != NULL) //Pouze pro nepr�zdn� seznam
    {
        pom->data = val;
        pom->ptr = L->Act->ptr;
        L->Act->ptr = pom;
    }
}

void Copy (tList *L, int *val) {
/*
** Prostoednictv�m parametru val vr�t� hodnotu aktivn�ho prvku seznamu L.
** Pokud seznam nen� aktivn�, zavol� funkci Error().
**/
    if(L->Act == NULL)
        Error();
    else
        *val = L->Act->data;
}

void Actualize (tList *L, int val) {
/*
** Poep�1e data aktivn�ho prvku seznamu L hodnotou val.
** Pokud seznam L nen� aktivn�, nedil� nic!
**/
    if (L->Act != NULL) //Pouze pro nepr�zdn� seznam
        L->Act->data = val;
}

void Succ (tList *L) {
/*
** Posune aktivitu na n�sleduj�c� prvek seznamu L.
** V1imnite si, 3e touto operac� se mu3e aktivn� seznam st�t neaktivn�m.
** Pokud nen� poedan� seznam L aktivn�, nedil� funkce nic.
**/
    if(L->Act != NULL) //Pouze pro nepr�zdn� seznam
        L->Act = L->Act->ptr;

}

int Active (tList *L) {
/*
** Je-li seznam L aktivn�, vrac� nenulovou hodnotu, jinak vrac� 0.
** Tuto funkci je vhodn� implementovat jedn�m po�kazem return.
**/

    return L->Act != NULL;
}

/* Konec c201.c */
