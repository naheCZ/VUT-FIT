//============================================================================
// Jmeno        : supermarket.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : Model supermarketu, projekt pro predmet IMS 2016/2017
// Datum        : 29.11.2016
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

const int dobaSimulace = 7 * DEN; //Doba simulace

//Konstanty cekacich dob
const double minDobaNakupu = 1.0 * (double)MINUTA;
const double maxDobaNakupu = 10.0 * (double)MINUTA;
const double minDobaObsluhy = 17.0 * (double)SEKUNDA;
const double maxDobaObsluhy = 122.0 * (double)SEKUNDA;
const double minDobaUzeniny = 7.0 * (double)SEKUNDA;
const double maxDobaUzeniny = 1.0 * (double)MINUTA;
const double prichodZakaznikaSpicka = 14.55 * (double)SEKUNDA;
const double prichodZakaznikaNespicka = 40.0 * (double)SEKUNDA;
const int pocetPokladen = 4;
const int pocetUzeniny = 2;
const double procentoUzeniny = 0.33;
const int otevreno = 13 * HODINA;
const int zavreno = 11 * HODINA;
const int spicka = 4 * HODINA;
const double nespicka =  4.5 * (double)HODINA;

//Citace
unsigned int pocetNakupujicich = 0;
unsigned int volnePokladny = pocetPokladen;
unsigned int volneUzeniny = pocetUzeniny;
unsigned int pocetSpicka = 0;
unsigned int pocetNespicka = 0;
unsigned int lidiUzenarstvi = 0;
unsigned int pocetCekajich = 0;
unsigned int muc = 0;

Facility pokladny[pocetPokladen];
Facility uzeniny[pocetUzeniny];
Queue frontaPok1;
Queue frontaPok2;
Queue frontaPok3;
Queue frontaPok4;
Queue frontaUze1;
Queue frontaUze2;

bool jeOtevreno = false;
bool jeSpicka = true;

Histogram casNakupovani("Celkova doba zakaznika v obchode", 0, 10 * MINUTA, 12);
Histogram casObsluhy("Doba obsluhy", 0, 2 * MINUTA, 4);

class Zakaznik : public Process
{
    void Behavior()
    {
        int cislo = 0;
        double prichod = Time;

        pocetNakupujicich++;

        if(Random() <= procentoUzeniny)
        {
            int cislo1 = 0;

            lidiUzenarstvi++;

            if(volneUzeniny < pocetUzeniny)
            {
                for(int i = 0; i < pocetUzeniny; i++)
                {
                    if(uzeniny[i].QueueLen() < uzeniny[cislo1].QueueLen())
                        cislo1 = i;
                }
            }

            Seize(uzeniny[cislo1]);
            volneUzeniny--;
            Wait(Uniform(minDobaUzeniny, maxDobaUzeniny));
            Release(uzeniny[cislo1]);
            volneUzeniny++;
        }

        Wait(Uniform(minDobaNakupu, maxDobaNakupu));

        if(volnePokladny < pocetPokladen)
        {
            for(int i = 0; i < pocetPokladen; i++)
            {
                if(pokladny[i].QueueLen() < pokladny[cislo].QueueLen())
                    cislo = i;
            }
        }

        Seize(pokladny[cislo]);
        if(!jeOtevreno)
            muc++;
        volnePokladny--;
        double platim = Time;
        Wait(Uniform(minDobaObsluhy, maxDobaObsluhy));
        Release(pokladny[cislo]);
        casObsluhy(Time - platim);
        pocetNakupujicich--;
        casNakupovani(Time - prichod);
        volnePokladny++;
    }
};

class GenZakazniku : public Event
{
    void Behavior()
    {
        if(jeOtevreno && jeSpicka)
        {
            (new Zakaznik)->Activate();
            pocetSpicka++;
            Activate(Time + Exponential(prichodZakaznikaSpicka));
        }

        else if(jeOtevreno && !jeSpicka)
        {
            (new Zakaznik)->Activate();
            pocetNespicka++;
            Activate(Time + Exponential(prichodZakaznikaNespicka));
        }
    }
};

class OteviraciDoba : public Event
{
    void Behavior()
    {
        if(jeOtevreno)
        {
            jeOtevreno = false;
            Activate(Time + otevreno);
        }

        else
        {
            jeOtevreno = true;
            (new GenZakazniku)->Activate();
            Activate(Time + zavreno);
        }

    }
};

class Spicka : public Event
{
    void Behavior()
    {
        if(jeOtevreno && jeSpicka)
        {
            jeSpicka = false;
            Activate(Time + nespicka);
        }

        else if(jeOtevreno && !jeSpicka)
        {
            jeSpicka = true;
            Activate(Time + spicka);
        }

        else
        {
            jeSpicka = false;
            Activate(Time + zavreno + nespicka);
        }
    }
};

int main()
{
    RandomSeed(time(NULL));

    SetOutput(SOUBOR);

    pokladny[0].SetQueue(frontaPok1);
    pokladny[0].SetName("Pokladna 1");
    pokladny[1].SetQueue(frontaPok2);
    pokladny[1].SetName("Pokladna 2");
    pokladny[2].SetQueue(frontaPok3);
    pokladny[2].SetName("Pokladna 3");
    pokladny[3].SetQueue(frontaPok4);
    pokladny[3].SetName("Pokladna 4");
    uzeniny[0].SetQueue(frontaUze1);
    uzeniny[1].SetQueue(frontaUze2);

    Init(0, dobaSimulace);
    (new OteviraciDoba)->Activate();
    (new Spicka)->Activate();

    Run();

    Print("Pocet zakazniku ve spicce: %d\n", pocetSpicka);
    Print("Pocet zakazniku mimo spicce: %d\n", pocetNespicka);
    Print("Jeste nakupuje: %d\n", pocetNakupujicich);
    Print("Cas simulace %.02f\n", Time);
    Print("Pocet lidi, kteri byli ve fronte: %d\n", pocetCekajich);
    Print("Lidi, kteri navstivili pult s uzeninami: %d\n", lidiUzenarstvi);
    Print("Kolik lidi nakupovalo mimo oteviracku: %d\n", muc);
    Print("\n\n");
    pokladny[0].Output();
    pokladny[1].Output();
    pokladny[2].Output();
    pokladny[3].Output();
    casNakupovani.Output();
    casObsluhy.Output();

    return 0;
}
