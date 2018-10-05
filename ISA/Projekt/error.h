//============================================================================
// Jmeno        : error.h
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 25.10.2017
//============================================================================

#ifndef ERROR_H
#define ERROR_H

typedef enum chyba
{
    SPATNY_POCET_ARGUMENTU,
    SPATNY_ARGUMENT,
    NEZNAMY_ARGUMENT,
    CHYBEJICI_HODNOTA,
    NELZE_OTEVRIT,
    SPATNY_SOUBOR,
    CISLO_PORTU,
    SOCKET_CHYBA,
    BIND_CHYBA,
    LISTEN_CHYBA,
    ACCEPT_CHYBA,
    SEND_CHYBA,
    RECV_CHYBA
} cislo_chyby;

extern const char* err_report[];

class Error
{
    public:
        void printErr(cislo_chyby number);
};

#endif
