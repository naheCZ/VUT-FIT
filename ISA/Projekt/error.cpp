//============================================================================
// Jmeno        : error.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 17.11.2017
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

const char *err_report[] = //Patricne hlasky k chybovym stavum
{
    "Pocet zadanych argumentu je chybny!",
    "Nektery z argumentu je chybne zadan!",
    "Byl zadan neznamy argument!",
    "K argumentu nebyla zadana povinna hodnota!",
    "Nelze otevrit autentizacni soubor!",
    "Autentizacni soubor je ve spatnem formatu!",
    "Zadane cislo portu neni v platnem rozsahu!",
    "Nastala chyba pri vytvareni socketu!",
    "Bind chyba, obsazeny port!",
    "Nastala chyba pri listen!",
    "Nastala chyba pri accpetu",
    "Nastala chyba pri sendu!",
    "Nastala chyba pri recv!"
};

using namespace std;

void Error::printErr(cislo_chyby n)
{
    /*
        Funkce, která na standartní chyboví výstup vypíše patřičnou chybovou hlášku.
    */

    cerr << err_report[n] << endl;
}
