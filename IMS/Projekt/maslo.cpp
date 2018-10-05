//============================================================================
// Jmeno        : maslo.cpp
// Autori       : xnahal01, xmorav33
// Popis        : Model vyrobny masla, projekt pro predmet IMS 2017/2018
// Datum        : 30.11.2016
//============================================================================

#include "simlib.h"
#include <ctime>
#include <iostream>

#define SOUBOR "output.out"

//Casove konstanty
const int SEKUNDA = 1;
const int MINUTA =  60 * SEKUNDA;
const int HODINA = 60 * MINUTA;
const int DEN = 24 * HODINA;
const int TYDEN = 7 * DEN;
const int MESIC = 30 * DEN;
const int ROK = 365 * DEN;

const int dobaSimulace = 1 * ROK; //Doba simulace

//Konstanty cekacich dob
const double minKontrola = 3.0 * MINUTA;
const double maxKontrola = 7.0 * MINUTA;
const double minCerpani = 42.0 * MINUTA;
const double maxCerpani = 50.0 * MINUTA;
const double dobaOdstredeni = 2.5 * HODINA;
const double minZrani = 10.0 * HODINA;
const double maxZrani = 12.0 * HODINA;
const double minPrijezdCisterny = 30.0 * MINUTA;
const double maxPrijezdCisterny = 40.0 * MINUTA;
const double novaKostka = 10 * SEKUNDA;
const double minBaleni = 0.625 * SEKUNDA;
const double maxBaleni = 0.875 * SEKUNDA;
const double dobaStloukani = 434.8 * SEKUNDA;

//pocet zarizeni
const int pocetCerpadel = 2;
const int pocetStanic = 1;
const int pocetOdstredivek = 5;
const int pocetZrajiren = 20;
const int pocetStloukaren = 2;
const int pocetBalicek = 5;

//Citace
int pocetKostek = 0;
int pocetCisteren = 0;
int pocetChyb = 0;
int volnaCerpadla = pocetCerpadel;
int volneOdstredivky = pocetOdstredivek;
int volneStanice = pocetStanic;
int volneZrajirny = pocetZrajiren;
int volneStloukarny = pocetStloukaren;
int volneBalicky = pocetBalicek;

//Pravdepodobnosti
const double procentZavadnehoMleka = 0.0003;

//Zarizeni
Facility stanice[pocetStanic];
Facility cerpadlo[pocetCerpadel];
Facility odstredivka[pocetOdstredivek];
Facility zrajirna[pocetZrajiren];
Facility stloukarna[pocetStloukaren];
Facility balicka[pocetBalicek];

class Kostka : public Process
{
    void Behavior()
    {
        int cislo = 0;

        if(volneBalicky < pocetBalicek)
        {
            for(int i = 0; i < pocetBalicek; i++)
            {
                if(balicka[i].QueueLen() < balicka[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(balicka[cislo]);
        volneBalicky--;
        Wait(Uniform(minBaleni, maxBaleni));
        Release(balicka[cislo]);
        volneBalicky++;

        pocetKostek++;
    }
};

class genKostky : public Event
{
    int pocet = 2174;

    void Behavior()
    {

        if(pocet > 0)
        {
            pocet--;
            (new Kostka)->Activate();
            Activate(Time + novaKostka);
        }
    }
};

class Smetana : public Process
{
    void Behavior()
    {
        int cislo = 0;

        if(volneZrajirny < pocetZrajiren)
        {
            for(int i = 0; i < pocetZrajiren; i++)
            {
                if(zrajirna[i].QueueLen() < zrajirna[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(zrajirna[cislo]);
        volneZrajirny--;
        Wait(Uniform(minZrani, maxZrani));
        Release(zrajirna[cislo]);
        volneZrajirny++;

        cislo = 0;

        if(volneStloukarny < pocetStloukaren)
        {
            for(int i = 0; i < pocetStloukaren; i++)
            {
                if(stloukarna[i].QueueLen() < stloukarna[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(stloukarna[cislo]);
        volneStloukarny--;
        Wait(dobaStloukani);
        Release(stloukarna[cislo]);
        volneStloukarny++;

        (new genKostky)->Activate();
    }
};

class Mleko : public Process
{
    void Behavior()
    {
        int cislo = 0;

        if(volneOdstredivky < pocetOdstredivek)
        {
            for(int i = 0; i < pocetOdstredivek; i++)
            {
                if(odstredivka[i].QueueLen() < odstredivka[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(odstredivka[cislo]);
        volneOdstredivky--;
        Wait(dobaOdstredeni);
        Release(odstredivka[cislo]);
        volneOdstredivky++;

        (new Smetana)->Activate();
    }
};

class Cisterna : public Process
{
    void Behavior()
    {
        int cislo = 0;

        if(volneStanice < pocetStanic)
        {
            for(int i = 0; i < pocetStanic; i++)
            {
                if(stanice[i].QueueLen() < stanice[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(stanice[cislo]);
        volneStanice--;
        Wait(Uniform(minKontrola, maxKontrola));
        Release(stanice[cislo]);
        volneStanice++;

		if(Random() <= procentZavadnehoMleka)
		{
			pocetChyb++;
			return;
		}

        cislo = 0;

        if(volnaCerpadla < pocetCerpadel)
        {
            for(int i = 0; i < pocetCerpadel; i++)
            {
                if(cerpadlo[i].QueueLen() < cerpadlo[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(cerpadlo[cislo]);
        volnaCerpadla--;
        Wait(Uniform(minCerpani, maxCerpani));
        Release(cerpadlo[cislo]);
        volnaCerpadla++;

        (new Mleko)->Activate();
    }
};

class GenCisteren : public Event
{
    void Behavior()
    {
        (new Cisterna)->Activate();
        pocetCisteren++;
        Activate(Time + Uniform(minPrijezdCisterny, maxPrijezdCisterny));
    }
};

int main()
{
    RandomSeed(time(NULL));

    SetOutput(SOUBOR);

    stanice[0].SetName("Stanice");
    zrajirna[0].SetName("Zrajirna");
    odstredivka[0].SetName("Odstredivka");

    Init(0, dobaSimulace);
    (new GenCisteren)->Activate();

    Run();

    Print("Pocet spatnejch cisteren: %d\n", pocetChyb);
	Print("Pocet cisteren: %d\n", pocetCisteren);
    Print("Pocet kostek: %d\n", pocetKostek);
	Print("Tun masla: %d t\n", pocetKostek/4000);
	Print("Litru mleka: %d l\n", pocetCisteren*25000);
    stanice[0].Output();
    zrajirna[0].Output();
    odstredivka[0].Output();

    return 0;
}
