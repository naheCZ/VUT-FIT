/*
 * Soubor:  proj1.c
 * Datum:   2015/10/03
 * Autor:   Roman Nahalka, xnahal01@stud.fit.vubr.cz
 * Projekt: Prace s textem, projekt c. 1 pro predmet IZP
 * Popis:   Program zpracovava slova v textu. Program v textu detekuje cisla, kalendarni datum,
 *			u cisla provede test na prvocislo a u textu provede test, jestli se nejedna o palindrome.
 *			Vstupem programu je standartni vstup (stdin).
 */

#include <stdio.h> //Prace se vstupem/vystupem.
#include <stdlib.h> //Obecne funkce jazyka C.
#include <limits.h> //Limity cislenych typu.
#include <time.h> //Pro praci s datem.

#define DELKA 101 //Budeme pracovat se slovy o maximalne delce 100 znaku.
#define SOUSTAVA 10 //Pro funkci strol(l) si vytvorime soustavu, kterou budeme pouzivat. V tomto pripade desitkovou.

typedef enum chyba //Vyctovy typ pro chyby v programu.
{
    CHYBNA_VSTUPNI_DATA,
} cislo_chyby;

const char *chybova_hlaseni[]= //Chybove hlasky.
{
    "Neocekavana vstupni data!\n",
};


//    Deklarace funkci:
void popis(void); // Funkce, ktera vypise popis programu.
int skenovani(void); // Funkce pro skenovani dalsiho slova.
int prvocislo(long long cislo); // Funkce, ktera zjisti jestli je cislo prvocislem.
int datum(int rok, char* slovo); // Funkce na zjisteni dne v tydnu, pokud se bude jednat o datum.
int kontrola_data (char slovo[DELKA]); // Funkce, ktera zkontroluje, jestli uzivatel napsal datum ve spravnem formatu.
int kontrola_dnu(int den, int mesic, int rok); // Funkce, ktera zkontroluje zda-li jsou strukture tm zadana spravna data.
void vypis_slova(char slovo[DELKA]); // Funkce na vypsani slova.
int palindrome(char slovo[DELKA]); // Funkce, pro detekci palindromu.
void error(cislo_chyby n); //Vypise chybovou hlasku.
int delka_slova(char slovo[DELKA]); // Funkce na zjisteni delky slova.
int kontrola_chyb(char slovo[]);

// Zacatek programu:
int main(int argc, char** argv) // Hlavni funkce programu.
{
    (void) argv;
    if (argc > 1) // Zjisteni, jestli uzivatel pouzil nejake dalsi argumenty.
    {
        popis(); // Vypsani popisu aplikace.
        return 0;
    }
    skenovani(); // Spusteni skenovani slov.
    return 0;
}

int skenovani(void) // Funkce pro skenovani dalsiho slova, skonci az s koncem souboru.
{
    char slovo[DELKA];
    long long hodnota;
    char *znak;

    while (scanf("%100s", slovo)!=EOF) // Program hleda slova, dokud nedojde na konec programu.
    {
        if(kontrola_chyb(slovo) == 0)
            return 0;
        hodnota = strtoll(slovo, &znak, SOUSTAVA); // Prevod retezce na cislo, konkretne do typu long long.
        if (*znak == '-') //Po poslednim cisle ve slovu se nachazi pomlcka, muze se tedy jednat o datum.
        {
            if (kontrola_data(slovo) == 1) // Zkontrolujeme, jestli ma datum v poradku znaky.
            {
                if (datum(hodnota, znak+1) == 0) // Nastane-li nekde v date chyba, program bude zadane datum povazovat za slovo.
                    vypis_slova(slovo);
                else
                    printf(" %s\n", slovo); // Probehlo-li vse spravne, program vypise zadane datum.
            }

            else
                vypis_slova(slovo);
        }

        else if (*znak != '\0') // Kontrola, jestli se jedna ciste o cislo.
            vypis_slova(slovo);
        else if (hodnota <= INT_MAX && hodnota >= 0)
        {
            if(prvocislo(hodnota) == 1)
                printf("number: %lld (prime)\n", hodnota);
            else
                printf("number: %lld\n", hodnota);
        }
        else
            vypis_slova(slovo);
    }

    return 0;
}

int prvocislo(long long cislo) //Funkce, ktera zjisti jestli je cislo prvocislem.
{
    /*
        Funkce zjisti, zda-li je cislo zadane v argumentu funkce prvocislo nebo ne.
        V pripade, ze se o prvocislo jedna, vrati funkce hodntu 1.
        V opacne pripade funkce vrati hodnotu 0.
    */

    int i;
    int konec = cislo/2;

    if (cislo <= 1) //Jednicka neni prvocislo.
        return 0;
    if (cislo == 2) //Dvojka je prvocislo.
        return 1;
    if (cislo % 2 == 0) //Je cislo delitelne 2? Jestli ano, tak se nejedna o prvocislo.
        return 0;
    for (i=3; i<=konec; i+=2) //Projizdime vsechny cisla od 3 do n-1...nemuzeme pouzit odmocninu (nemame povlednou knihovnu Math.h), tudiz to bude trvat dele.
    {
        if (cislo % i == 0)
            return 0;
    }

    return 1;
}

int datum(int rok, char* slovo) // Funkce na zjisteni dne v tydnu, pokud se bude jednat o datum.
{
    time_t rawtime;
    int mesic, den;
    struct tm * cas; // Odkaz na strukturu casu.
    char* d;
    const char * dny[] = { "Sun", "Mon", "Tue", "Wed","Thu", "Fri", "Sat"}; // Dny v tydnu.

    mesic = strtol(slovo, &d, SOUSTAVA);
    den = strtol(d+1, &d, SOUSTAVA);
    if(kontrola_dnu(den, mesic, rok) == 0) //Zkontrolujeme, jestli zadana hodnoty jsou v souladu se strukturou tm
        return 0;
    // Nastaveni momentalniho casu.
    time(&rawtime);
    cas = localtime(&rawtime);
    // Nastaveni casu, podle zadaneho data.
    cas->tm_year = rok - 1900;
    cas->tm_mon = mesic - 1;
    cas->tm_mday = den;
    if(mktime(cas) == -1)
        return 0;
    printf("date: %s", dny[cas->tm_wday]);

    return 1;
}

int kontrola_data (char slovo[DELKA]) // Funkce, ktera zkontroluje, jestli uzivatel napsal datum ve spravnem formatu.
{
    /*
        Funkce kontroluje, jestli datum obsahuje pouze spravne znaky.
        Zjisti, jestli jsou spravne zadana cisla a pomlcky.
        Jestlize jsou, funkce vrati hodnotu 1.
        V opacnem pripade vrati hodnotu 0.
    */

    int i;

    for (i = 0; i<=9; i++)
    {
        if (i == 4 || i == 7) // Mista v datum, kde se maji nachazet pomlcky.
        {
            if (slovo[i] == '-')
                continue;
            else
                return 0;
        }
        if ((slovo[i] >= '0' && slovo[i] <= '9') == 0) // Mista v datum, kde sa maji nachazet cislice.
            return 0;
    }

    return 1;
}

int kontrola_dnu(int den, int mesic, int rok) // Funkce, ktera zkontroluje zda-li jsou strukture tm zadana spravna data.
{
    /*
		Funkce zkontroluje, jestli jsou zadane hodnty v poradku dle definice struktury tm.
	*/
	
	if(rok<1900)
		return 0;
	else if(den>31)
        return 0;
    else if(mesic>12)
        return 0;
    else
    	return 1;
}

void vypis_slova(char slovo[DELKA]) // Program na vypsani slovo a zjisteni palindromu.
{
    printf("word: %s", slovo);
    if (palindrome(slovo) == 1)
        printf(" (palindrome)\n");
    else
        printf("\n");

}

int palindrome(char slovo[DELKA]) //Funkce, pro detekci palindromu.
{
    /*
        Funkce, ktera zjisti, zda je zadane slovo palindrome ci ne.
        V pripade kladne odpovedi funkce vrati hodnotu 1, v opacnem pripade hodnotu 0.
    */

    int delka, konec, stred, i;

    delka = delka_slova(slovo); //Zjistime si delku slova.
    konec = delka - 1; //Konec naseho slova...pole pracuje od indexu 0, proto musime od poctu znaku odecist 1.
    stred = delka/2; //Stred naseho slova.
    for (i = 0; i < stred; i++)
    {
        if (slovo[i] != slovo[konec]) //Jestlize se znaky nebudou schodovat, nejde o palindrome.
            return 0;
        konec--;
    }

    return 1;
}

int delka_slova(char slovo[DELKA]) // Funkce na zjisteni delky slova.
{
    /*
		Funkce zjisti delku slova. Delka slovo je navratova hodnota funkce.
	*/

	int delka = 0;

    while(slovo[delka] != '\0') //Zjistime si delku slova.
        delka++;

    return delka;
}

int kontrola_chyb(char slovo[DELKA])
{
	int i = 0;

	if(slovo[0] == '\0')
	{
		error(CHYBNA_VSTUPNI_DATA);
		return 0;
	}
	
	while(slovo[i] != '\0')
	{
		if(slovo[i] < 0)
		{
			error(CHYBNA_VSTUPNI_DATA);
			return 0;
		}
		i++;
	}

    return 1;
}

void error(cislo_chyby n) //Vypise chybovou hlasku.
{
    fprintf(stderr, chybova_hlaseni[n]);
}

void popis(void) // Funkce, ktera vypise popis programu.
{
    printf("\t\t\tPopis programu\n"
                    "Program zpracovava text ulozeny v souboru data.txt, ktery je dany jako argument programu.\n"
                    "\nProgram v textu nalezne slova, cisla, prvocisla, datum a detekuje palindrom.\n"
                    "\nDelka slova je omezena na maximalne 100 znaku.\n"
                    "\nDatum musi byt napsano ve formatu ROK-MESIC-DEN a rok musi byt 1900 a vyssi.\n"
                    "\nVypsanim jakehokoliv argumentu k programu vyvola tento popis aplikace.\n\n");
}
