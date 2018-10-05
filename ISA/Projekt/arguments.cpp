//============================================================================
// Jmeno        : arguments.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 19.11.2017
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include "arguments.h"
#include "error.h"

using namespace std;

Arguments::Arguments(User* usr)
{
       /*
            Konstruktor pro třídu Arguments.
       */

       this->help = false;
       this->filePath = "";
       this->c = false;
       this->portNumber = -1;
       this->mailDirPath = "";
       this->reset = false;
       this->usr = usr;
}

bool Arguments::parseArguments(int argc, char** argv)
{
    /*
        Funkce, která zpracuje uživatelem zadané argumenty.
    */

    int c;
    char* znak;
    Error err;
    opterr = 0;
    string bin;

    if(argc < 2)
    {
        err.printErr(SPATNY_POCET_ARGUMENTU);
        return false;
    }

    //Cyklus, ve kterém projedeme všechny argumenty
    while((c = getopt(argc, argv, "d:p:a:hcr")) != -1)
    {
        switch(c)
        {
            case '?':
                err.printErr(NEZNAMY_ARGUMENT);
                return false;

            case ':':
                err.printErr(CHYBEJICI_HODNOTA);
                return false;

            case 'h':
                this->help = true;
                break;

            case 'a':
                this->filePath = optarg;
                break;

            case 'c':
                this->c = true;
                break;

            case 'p':
                this->portNumber = strtol(optarg, &znak, SOUSTAVA);

                if(*znak != '\0')
                {
                    err.printErr(SPATNY_ARGUMENT);
                    return false;
                }

                break;
            case 'd':
                this->mailDirPath = optarg;

                if(this->mailDirPath.back() == '/')
                    this->mailDirPath.pop_back();

                break;

            case 'r':
                this->reset = true;
                break;
        }
    }

    bin = argv[0];

    for(int i = 0; i < 6; i++)
        bin.pop_back();

    this->binPath = bin;

    if(this->getHelp()) //Zadán přepínač pro nápovědu
        return true;

    else if(this->getReset() && argc == 2) //Zadán přepínač pro reset
        return true;

    else
    {
        if(argc < 7)
        {
            err.printErr(SPATNY_POCET_ARGUMENTU);
            return false;
        }

        if(argc > 9)
        {
            err.printErr(SPATNY_POCET_ARGUMENTU);
            return false;
        }
    }

    return true;
}

bool Arguments::checkValidity()
{
    /*
        Funkce, která zkontroluje platnost uživatelem zadaných argumentů.
    */

    if(!(this->checkAuthFile()))
        return false;

    if(!(this->checkPort()))
        return false;

    return true;
}

bool Arguments::checkAuthFile()
{
    /*
        Funkce, která zkontroluje, že je soubor s uživatelským jménem a heslem ve správném formátu.
        Funkce také heslo a uživatelské jméno uloží do objektu User.
    */

    Error err;
    ifstream soubor;
    char pom[256] = "";

    soubor.open(this->getFilePath().c_str());

    if(!(soubor.is_open()))
    {
        err.printErr(NELZE_OTEVRIT);
        return false;
    }

    soubor.read(pom, 11);

    if((strcmp(pom, "username = ")))
    {
        err.printErr(SPATNY_SOUBOR);
        return false;
    }

    soubor.getline(pom, 256);
    string use(pom);
    this->usr->setUsername(use);
    soubor.read(pom, 11);

    if((strcmp(pom, "password = ")))
    {
        err.printErr(SPATNY_SOUBOR);
        return false;
    }

    soubor.getline(pom, 256);
    string pss(pom);
    this->usr->setPassword(pss);
    soubor.get(pom[0]);
    soubor.get();

    if(!(soubor.eof()))
    {
        err.printErr(SPATNY_SOUBOR);
        return false;
    }

    soubor.close();

    return true;
}

bool Arguments::checkPort()
{
    /*
        Funkce, která zkontorluje, zda je zadán platný port.
    */

    Error err;

    if(this->portNumber < 0 || this->portNumber > 65535)
    {
        err.printErr(CISLO_PORTU);
        return false;
    }

    return true;
}

void Arguments::printHelp()
{
    /*
        Funkce, která na standartní výstup vytiskne nápovědu.
    */

    cout << endl << "\tPOP3 server" << endl;
    cout << "Vitejte u napovedy pro program POP3 server" << endl;
    cout << "Program se spousti s temito parametry:" << endl << endl;
    cout << "-h: \tVypise tuto napovedu" << endl;
    cout << "-a: \tCesta k souboru s prihl. udaji" << endl;
    cout << "-c: \tPrenos hesla nesifrovane" << endl;
    cout << "-p: \tCislo portu, na kterem bude server bezet" << endl;
    cout << "-d: \tCesta do slozky maildir" << endl;
    cout << "-r: \tResetovani" << endl << endl;
}

bool Arguments::getHelp()
{
    /*
        Funkce, která vrátí informaci o tom, zda byl zadán přepínač s nápovědou.
    */

    return this->help;
}

string Arguments::getFilePath()
{
    /*
        Funkce, která vrátí cestu k autentizačnímu souboru.
    */

    return this->filePath;
}

int Arguments::getPortNumber()
{
    /*
        Funkce, která vráti číslo portu.
    */

    return this->portNumber;
}

bool Arguments::getC()
{
    /*
        Funkce, která vrátí informaci o tom, zfa byl zadán přepínač -c.
    */

    return this->c;
}

User* Arguments::getUsr()
{
    /*
        Funkce, která vrátí místo do paměti, kde je uložen objekt User.
    */

    return this->usr;
}

bool Arguments::getReset()
{
    /*
        Funkce, která vrátí informaci o tom, zda byl zadán přepínač pro reset.
    */

    return this->reset;
}

string Arguments::getMailDirPath()
{
    /*
        Funkce, která vrátí cestu k maildiru.
    */

    return this->mailDirPath;
}

void Arguments::setMailDirPath(string path)
{
    /*
        Funkce, která nastaví ccestu k maildiru.
        Pomocná funkce, pokud bude program spuštěn pouze s přepínačem pro reset.
    */

    this->mailDirPath = path;
}

string Arguments::getBinPath()
{
    /*
        Funkce, která vrátí cestu ke složce, kde je uložena binárka programu.
    */

    return this->binPath;
}
