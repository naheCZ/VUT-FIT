/*
*   Projekt 2 do predmetu IPK 2015/2016
*   Autor: Roman Nahalka, xnahal01@stud.fit.vutbr.cz
*   Datum : 21.4.2016
*/

//Potrebne knihovny
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

//Definice konstant
#define SOUSTAVA 10 //Soustava pro funkci strtol
#define BUFFER 512 //Buffer pro prijmani
#define MIN_PORT 1024 //Minimalni cislo portu, ktery se da pouzit
#define MAX_PORT 65534 //Maximalni cislo portu, ktery se da pouzit
#define ARGUMENTY 7 //Pocet ocekavanych argumentu programu

using namespace std;

typedef enum chyba //Vycet chyb
{
    SPATNY_POCET_ARGUMENTU,
    SPATNY_ARGUMENT,
    CHYBA_SOCKET,
    CHYBA_BIND,
    CHYBA_LISTEN,
    CHYBA_ACCEPT,
    CHYBA_RECV,
    CHYBA_OPEN,
    CHYBA_SEND,
    CHYBA_ADRESA,
    CHYBA_CONNECT,
    CHYBA_WRITE
} cislo_chyby;

typedef struct data //Struktura parametru programu
{
    int port; //Port, na ktery se pripojime
    string adresa; //Cilova adresa serveru
    string soubor; //Soubor, ktry chceme stahnout nebo nahrat
} type_data;

const char *chybova_hlaseni[] = //Chybova hlaseni
{
    "Pocet zadanych argumentu je chybny!",
    "Jeden z argumentu je zadan spatne!",
    "Chyba pri vytvareni socketu!",
    "Nastala chyba pri bindu!",
    "Nastala chyba pri listen!",
    "Nastala chyba pri accept!",
    "Nastala chyba pri prijmani dat!",
    "Nelze otevrit soubor!",
    "Chyba pri odesilani dat!",
    "Byla zadana spatna adresa!",
    "Nelze se pripojit!",
    "Chyba pri zapisu do souboru!"
};

void error(cislo_chyby n);
bool kontrola_argumentu(int argc, char **argv, type_data *data, int *parametr);
bool nahrani(type_data *data);
bool stahnuti(type_data *data);

int main(int argc, char **argv)
{
    type_data data;
    int parametr;

    if(kontrola_argumentu(argc, argv, &data, &parametr) == false)
        return 1;
    if(parametr == 0)
    {
        if(nahrani(&data) == false)
            return 1;
    }

    else if(parametr == 1)
    {
        if(stahnuti(&data) == false)
            return 1;
    }

    else
    {
        error(SPATNY_ARGUMENT);
        return 1;
    }

    return 0;
}

bool kontrola_argumentu(int argc, char **argv, type_data *data, int *parametr)
{
    /*
        Funkce, ktera zkontroluje a zpracoju argumenty zadane uzivatelem. V pripade
        chybi funkce vrati hodnotu FALSE, v pripade, ze vse probehne v poradku vrati
        hodnotu TRUE;
    */

    char *znak;

    if(argc != ARGUMENTY)
    {
        error(SPATNY_POCET_ARGUMENTU);
        return false;
    }

    if(strcmp(argv[1], "-h") != 0)
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    data->adresa = argv[2];

    if(strcmp(argv[3], "-p") != 0)
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    data->port = strtol(argv[4], &znak, SOUSTAVA);
    if(*znak != '\0')
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    if(strcmp(argv[5], "-u") == 0)
        *parametr = 0;
    else if(strcmp(argv[5], "-d") == 0)
        *parametr = 1;
    else
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    data->soubor = argv[6];

    return true;
}

bool stahnuti(type_data *data)
{
    /*
        Funkce, ktera zahaji komunikaci ze serverem za ucelem stahnuti souboru.
        V pripade nejake chyby funkce vrati hodnotu FALSE, v opacnem pripade vrati
        hodnotu TRUE.
    */

    int mujSocket, file;
    struct sockaddr_in sock;
    string zprava = "Download: ";
    char prijata_zprava[BUFFER];
    ssize_t odeslano = 0, prijato = 0;
    size_t delka;
    hostent *host;

    mujSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(mujSocket < 0)
    {
        error(CHYBA_SOCKET);
        return false;
    }

    sock.sin_family = PF_INET;
    host = gethostbyname(data->adresa.data()); //Zjistime si adresu, ke ktere se mame pripojit
    if(host == NULL)
    {
        error(CHYBA_ADRESA);
        return false;
    }

    sock.sin_port = htons(data->port);
    memcpy(&sock.sin_addr, host->h_addr, host->h_length);

    if(connect(mujSocket, (sockaddr *)&sock, sizeof(sock)) < 0)
    {
        error(CHYBA_CONNECT);
        return false;
    }

    zprava.append(data->soubor);
    delka = zprava.size();

    if((odeslano = send(mujSocket, zprava.data(), delka, 0)) < 0)
    {
        error(CHYBA_SEND);
        return false;
    }

    file = open(data->soubor.data(), O_WRONLY|O_CREAT, 0644);
    if(file < 0)
    {
        error(CHYBA_OPEN);
        return false;
    }

    while((prijato = recv(mujSocket, prijata_zprava, BUFFER, 0)) > 0)
    {
        if(strncmp(prijata_zprava, "FAIL", 4) == 0)
        {
            error(CHYBA_RECV);
            break;
        }

        if(write(file, prijata_zprava, prijato) < 0)
        {
            error(CHYBA_WRITE);
            return false;
        }
    }

    close(file);

    return true;
}

bool nahrani(type_data *data)
{
    /*
        Funkce, ktera zahaji komunikaci ze serverem za ucelem nahrani souboru.
        V pripade nejake chyby funkce vrati hodnotu FALSE, v opacnem pripade vrati
        hodnotu TRUE.
    */

    int mujSocket, file, res;
    struct sockaddr_in sock;
    string zprava = "Upload: ";
    string odpoved;
    hostent *host;
    char buf[BUFFER];
    char prijmani[BUFFER];
    ssize_t cteni = 0, odeslane = 0, odeslano = 0;
    size_t delka;

    mujSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(mujSocket < 0)
    {
        error(CHYBA_SOCKET);
        return false;
    }

    sock.sin_family = PF_INET;
    host = gethostbyname(data->adresa.data()); //Zjistime si adresu, ke ktere se mame pripojit
    if(host == NULL)
    {
        error(CHYBA_ADRESA);
        return false;
    }

    sock.sin_port = htons(data->port);
    memcpy(&sock.sin_addr, host->h_addr, host->h_length);

    if(connect(mujSocket, (sockaddr *)&sock, sizeof(sock)) < 0)
    {
        error(CHYBA_CONNECT);
        return false;
    }

    zprava.append(data->soubor);
    delka = zprava.size();

    if((odeslano = send(mujSocket, zprava.data(), delka, 0)) < 0)
    {
        error(CHYBA_SEND);
        return false;
    }

    if((res = recv(mujSocket, prijmani, BUFFER, 0)) < 0)
    {
        error(CHYBA_RECV);
        return false;
    }

    odpoved.append(prijmani, res);
    if(odpoved.find("OK") != 0)
    {
        error(CHYBA_RECV);
        return false;
    }

    file = open(data->soubor.data(), O_RDONLY);
    if(file < 0)
    {
        error(CHYBA_OPEN);
        return false;
    }

    while((cteni = read(file, buf, BUFFER)) > 0)
    {
        odeslane = send(mujSocket, buf, cteni, 0);
        if(odeslane < cteni)
        {
            error(CHYBA_SEND);
            return false;
        }
    }

    close(file);

    return true;
}

void error(cislo_chyby n) //Funkce, ktera na standartni chybovi vystup vypise chybovou hlasku.
{
    cerr << chybova_hlaseni[n] << endl;
}
