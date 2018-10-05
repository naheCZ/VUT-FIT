//============================================================================
// Jmeno        : popser.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 20.11.2017
//============================================================================

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "server.h"
#include "arguments.h"

using namespace std;

int main(int argc, char** argv)
{
	/*
        Hlavní funkce programu, která spustí po zapnutí programu.
	*/

	User* usr = new User();
	Arguments args(usr);

    signal(SIGPIPE, SIG_IGN);

	if(!(args.parseArguments(argc, argv))) //Parsování zadaných argumentů
        return 1;

    Server ser(&args);

    if(args.getHelp()) //Vypsání nápovědy, pokud to uživatel požaduje.
    {
        args.printHelp();
        return 0;
    }

    if(args.getReset()) //Restartování serveru
    {
        if(!(ser.reset()))
            return 1;
        if(argc == 2)
            return 0;
    }

    if(!(args.checkValidity())) //Kontrola platnosti argumentů
        return 1;

    if(!(ser.connection())) //Spuštění Serveru
    {
        return 1;
    }


	delete usr;

	return 0;
}
