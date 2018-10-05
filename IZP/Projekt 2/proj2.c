/*
 * Soubor:  proj2.c
 * Datum:   2015/11/29
 * Autor:   Roman Nahalka, xnahal01@stud.fit.vubr.cz
 * Projekt: Iteracni vypocty, projekt c. 2 pro predmet IZP
 * Popis:   Program vypocitava hodnotu prirozeneho logaritmu urceneho cisla, zadaneho jako
 *			argument programu. Program provadi vypocet pomoci taylorova polynomu a zretezeneho zlomku
 *			pro dany pocet iteraci. Program take muze najit, kolik je potreba iteraci pro vypocet libovelneho
 *			cisla v zadaneme intervalu.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>

#define SOUSTAVA 10 //Soustava pro funkci strtol

typedef enum chyba //Vycet chyb
{
	MALO_ARGUMENTU,
	MNOHO_ARGUMENTU,
	SPATNY_PARAMETR,
	VETSI_MIN,
	MALE_EPS,
	NEPLATNY_POCET_ITERACI,
} cislo_chyby;

//Deklarace funkci
double taylor_log(double x, unsigned int n);
double cfrac_log(double x, unsigned int n);
int logaritmus(char *argv[]);
int iterace(char *argv[]);
int pocet_cf_iteraci(double min, double max, double eps, double min_pravy_vysledek,double max_pravy_vysledek);
int pocet_taylor_iteraci(double min, double max, double eps, double min_pravy_vysledek,double max_pravy_vysledek);
int presnost(double pravy, double jiny, double eps);
void error(cislo_chyby n);
int kontrola_argumentu(int argc, char *argv[]);


int main(int argc, char *argv[]) //Hlavni funkce programu
{

	if(kontrola_argumentu(argc, argv) == 0) //Zkontrolujeme, jestli uzivatel zadal spravne argumenty
		return 0;

	if(strcmp(argv[1], "--log") == 0)
		logaritmus(argv);

	if(strcmp(argv[1], "--iter") == 0)
		iterace(argv);

	return 0;
}

int kontrola_argumentu(int argc, char *argv[])
{
	/*
		Funkce zkontroluje, jestli byli spravne zadany argumenty. V pripade, ze nalezne chybu
	  	vrati hodnotu 0. Jestli vse probehne v poradku vrati hodnotu 1.
	*/

	if(argc < 4) // Byl zadan dostatecny pocet argumentu?
	{
		error(MALO_ARGUMENTU);
		return 0;
	}

	else if(strcmp(argv[1], "--log") != 0 && strcmp(argv[1], "--iter") != 0) //Odpovida prepinac nejake z moznosti?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	else if(strcmp(argv[1], "--log") == 0 && argc != 4) //Je zadan spravny pocet argumentu u argumentu log?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	else if(strcmp(argv[1], "--iter") == 0 && argc != 5) //Je zadan spravny pocet argumentu u argumentu iter?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	return 1;
}

int logaritmus(char *argv[])
{
	/*
		Funkce, ktera zpracuje argumenty programu pro vypocet logaritmu. Nalezne-li pri zpracovani
		argumentu chybu, vypise prislusnou chybovou hlasku a vrati 0. Probehne-li vse v poradku
		program spocita logartimus pomoci knihovny Math.h, taylorova polynomu, zretezenych zlomku a
		vrati hodnotu 1.
	*/
	char* znak;
	double x, pravy_vysledek = 0.0, tay_vysledek = 0.0, cf_vysledek = 0.0;
	long int n;

	x = strtod(argv[2], &znak); //Prevedeme string na cislo
	if(*znak != '\0') //Jedna se jen o cislo?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	n = strtol(argv[3], &znak, SOUSTAVA); //Prevedeme string na cislo
	if(*znak != '\0') //Jedna se jen o cislo?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	if(errno == ERANGE || n <= 0 || n > UINT_MAX) //Prevedeme string na cislo
	{
		error(NEPLATNY_POCET_ITERACI);
		return 0;
	}

	pravy_vysledek = log(x);
	printf("       log(%g) = %.12g\n", x, pravy_vysledek);
	cf_vysledek = cfrac_log(x, n);
	printf("    cf_log(%g) = %.12g\n", x, cf_vysledek);
	tay_vysledek = taylor_log(x, n);
	printf("taylor_log(%g) = %.12g\n", x, tay_vysledek);

	return 1;
}

int iterace(char *argv[])
{
	/*
		Funkce, ktera zpracuje argumenty pro vypocet pozadoveneho poctu iteraci pro urcity interval.
		Nalezne-li pri tom chybu, vypise prislusnou chybovou hlasku a vrati hodnotu 0. Nebude-li
		nalezena chyba, program spocita pocet potrebnych iteraci pro tayloruv polynom i zretezeny
		zlomek a vrati hodnotu 1.
	*/
	char* znak;
	double min, max, eps;
	double min_pravy_vysledek, max_pravy_vysledek;
	double min_taylor_vysledek, max_taylor_vysledek;
	double min_cf_vysledek, max_cf_vysledek;
	unsigned int taylor_iterace, cf_iterace;

	min = strtod(argv[2], &znak); //Prevedeme string na cislo
	if(*znak != '\0') //Jedna se jen o cislo?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	max = strtod(argv[3], &znak); //Prevedeme string na cislo
	if(*znak != '\0') //Jedna se jen o cislo?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	if(min > max) //Je zacatek intervalu mensi nez jeho konec?
	{
		error(VETSI_MIN);
		return 0;
	}

	eps = strtod(argv[4], &znak); //Prevedeme string na cislo
	if(*znak != '\0') //Jedna se jen o cislo?
	{
		error(SPATNY_PARAMETR);
		return 0;
	}

	if(eps < 1e-12) //Nperesahuje presnost nejmensi povolenou?
	{
		error(MALE_EPS);
		return 0;
	}

	min_pravy_vysledek = log(min);
	printf("       log(%g) = %.12g\n", min, min_pravy_vysledek);
	max_pravy_vysledek = log(max);
	printf("       log(%g) = %.12g\n", max, max_pravy_vysledek);
	cf_iterace = pocet_cf_iteraci(min, max, eps, min_pravy_vysledek, max_pravy_vysledek);
	printf("continued fraction iterations = %d\n", cf_iterace);
	min_cf_vysledek = cfrac_log(min, cf_iterace);
	printf("    cf_log(%g) = %.12g\n", min, min_cf_vysledek);
	max_cf_vysledek = cfrac_log(max, cf_iterace);
	printf("    cf_log(%g) = %.12g\n", max, max_cf_vysledek);
	taylor_iterace = pocet_taylor_iteraci(min, max, eps, min_pravy_vysledek, max_pravy_vysledek);
	printf("taylor polynomial iterations = %d\n", taylor_iterace);
	min_taylor_vysledek = taylor_log(min, taylor_iterace);
	printf("taylor_log(%g) = %.12g\n", min, min_taylor_vysledek);
	max_taylor_vysledek = taylor_log(max, taylor_iterace);
	printf("taylor_log(%g) = %.12g\n", max, max_taylor_vysledek);

	return 1;
}

double taylor_log(double x, unsigned int n)
{
	/*
		Funkce pro vypocet logaritmu pomoci taylorova polynomu. Funkce obsahuje 2 vzorce, jeden pro
		vypocet hodnot mensich jak 1 a druhy pro vypocet hodnot vetsich jak 1. Na zacatku jeste funkce
		zkontroluje, zda se nejedna o jednu z moznych vyjimek pri vypoctu logaritmu. Navratova hodnota
		funkce je vypocitana hodnota logaritmu z argumentu x v zadanych n iteraci.
	*/

	double vysledek = 0.0;
	double cinitel;
	unsigned int i;

	if(x == 0.0)
		return -INFINITY;

	else if(x < 0.0)
		return NAN;

	else if(isinf(x) == 1)
		return INFINITY;

	else if(isnan(x) == 1)
		return NAN;

	else if(x == 1)
		return 0;

	if(x > 1.0) //Prvni vzorec, pro hodnoty vetsi jak 1
	{
		cinitel = 1.0; //Pocatecni cinitel

		for(i=1; i<=n; i++)
		{
			cinitel = cinitel*((x-1)/x); //Vypocet noveho cinitele
			vysledek = vysledek + cinitel/i; //Vzorec
		}
	}

	else //Druhy vzorec, pro hodnoty mensi jak 1.
	{
		double upravena = 1-x; //Vypocet hodnoty, se kterou budeme skutecne pocitat

		cinitel = -upravena; //Pocatecni cinitel

		for(i=1; i<=n; i++)
		{
			vysledek = vysledek + cinitel/i; //Vzorec
			cinitel = cinitel*upravena; //Vypocet noveho cinitele
		}

	}

	return vysledek;
}

double cfrac_log(double x, unsigned int n)
{
	/*
		Funkce pro vypocet logaritmu pomoci zretezenych zlomku. Na zacatku jeste funkce zkontroluje
		zda se nejedna o jednu z vyjimek pri pocitani logaritmu. Navratova hodnota funkce je vypocitana
		hodnota logaritmu z argumentu x v zadanych n iteraci.
	*/

	double vysledek = 0.0;
	double z = (x-1)/(x+1); //Vypocet hodnoty, se kterou budeme skutecne pocitat
	double zz = z*z; //Budeme potrebovat jeji druhou mocninu
	unsigned int i; //Pro cyklus
	double ak = 0.0; //Promenna pro cinitel
	double bk = 0.0; //Promenna pro jmenovatel
	double s = 0.0, t = 0.0;

	if(x == 0.0)
		return -INFINITY;

	else if(x < 0.0)
		return NAN;

	else if(isinf(x) == 1)
		return INFINITY;

	else if(isnan(x) == 1)
		return NAN;

	else if(x == 1)
		return 0;

	for(i=n; i>0; i--) //Zretezeny zlomek se pocita od jeho konce
	{
		if (i == 1) //Prvni zlomek nema urcitou posloupnost
			ak = 2*z; //Cinitel se v tomto pripade rovna 2z
		else //Ve vsech pripadech, krome prvniho zlomku
		{
			t = i-1; //Pred dalsim zlomkem je hodnota i-1
			s = (t*t); //V citateli zlomku je vzdy (i-1)^2
			ak = (-s*zz); //Vzorec pro citatel pro vsechny zlomky, az na ten prvni
		}
		bk = 2*i - 1; //Vzorec pro jmenovatel
		vysledek = ak/(bk + vysledek); //Vydelime
	}

	return vysledek;

}

int pocet_cf_iteraci(double min, double max, double eps, double min_pravy_vysledek, double max_pravy_vysledek)
{
	/*
		Funkce spocita potrebny pocet iteraci pro vypocet logaritmu pomoci zretezeneho zlomku pro
		libovolnou hodnotu z intervalu zadaneho pomoci argumentu min a max pri zadane presnosti
		eps. Funkce spocitanou hodntou kontroluje s hodnotou z funkce log z kinhovny Math.h.
	*/

	unsigned int i, hodnota = 0;
	double vysledek = 0.0;

	for(i = 1; ; i++) //Nekoncenemu zacykleni zabrunuje podminka uvnitr cyklu
	{
		vysledek = cfrac_log(min, i);
		if(presnost(min_pravy_vysledek, vysledek, eps) == 1) //Dokud nedosahneme urcite presnosti
			break;
	}

	hodnota = i; //Ulozime si pocet iteraci pro zacatek intervalu

	for(i = 1; ; i++) //Nekoncenemu zacykleni zabrunuje podminka uvnitr cyklu
	{
		vysledek = cfrac_log(max, i);
		if(presnost(max_pravy_vysledek, vysledek, eps) == 1) //Dokud nedosahneme urcite presnosti
			break;
	}

	if(i > hodnota) //Porovname pocty iteraci pro min a max
		hodnota = i;

	return hodnota;
}

int pocet_taylor_iteraci(double min, double max, double eps, double min_pravy_vysledek, double max_pravy_vysledek)
{
	/*
		Funkce spocita potrebny pocet iteraci pro vypocet logaritmu pomoci taylorova polynomu pro
		libovolnou hodnotu z intervalu zadaneho pomoci argumentu min a max pri zadane presnosti
		eps. Funkce spocitanou hodntou kontroluje s hodnotou z funkce log z kinhovny Math.h.
	*/

	unsigned int i, hodnota = 0;
	double vysledek = 0.0;

	for(i = 1; ; i++) //Nekoncenemu zacykleni zabrunuje podminka uvnitr cyklu
	{
		vysledek = taylor_log(min, i);
		if(presnost(min_pravy_vysledek, vysledek, eps) == 1)
			break;
	}

	hodnota = i; //Ulozime si pocet iteraci pro zacatek intervalu

	for(i=1; ; i++) //Nekoncenemu zacykleni zabrunuje podminka uvnitr cyklu
	{
		vysledek = taylor_log(max, i);
		if(presnost(max_pravy_vysledek, vysledek, eps) == 1)
			break;
	}

	if(i > hodnota) //Porovname pocty iteraci pro min a max
		hodnota = i;

	return hodnota;
}

int presnost(double pravy, double jiny, double eps)
{
	/*
		Funkce spocita, jestli je uz vysledek dostatecne presny pri zadane presnosti eps. Jestlize
		uz je vysledek dostatecne presny, vrati hodnotu 1, jestlize neni, vrati hodnotu 0.
	*/

	double vysledek;

	if(pravy < 0.0) //Pouzivame absolutni hodnoty
		pravy = -pravy;
	if(jiny < 0.0) //Pouzivame absolutni hodnoty
		jiny = -jiny;
	vysledek = pravy - jiny; //Porovname vysledky
	if(vysledek < 0.0) //Pouzivame absolutni hodnoty
		vysledek = -vysledek;
	if(vysledek > eps) //Je to uz dostatecne presne?
		return 0;
	return 1;
}

void error(cislo_chyby n)
{
	/*
		Funkce, ktera na chybovy vystup vypise prislusnou chybovou hlasku.
	*/

	const char *chybova_hlaseni[] = //Chybove hlasky
	{
		"Zadal jsi prilis malo argumentu!\n",
		"Zadal jsi prilis mnoho argumentu!\n",
		"Argument je zadany spatne!\n",
		"Zacatek intervalu je vetsi nez konec intervalu!\n",
		"Zadana presnost je prilis mala!\n",
		"Byl zadan neplatny pocet iteraci!\n",
	};

	fprintf(stderr, chybova_hlaseni[n]);
}
