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
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <fcntl.h>

//Definice konstant
#define SOUSTAVA 10 //Soustava pro funkci strtol
#define BUFFER 512 //Buffer pro prijmani
#define MIN_PORT 1024 //Minimalni cislo portu, ktery se da pouzit
#define MAX_PORT 65534 //Maximalni cislo portu, ktery se da pouzit
#define UP 8 //Pocet znaku, ktere se maji smazat z prijate zpravy v pripade uploadu
#define DOWN 10 //Pocet znaku, ktere se maji smazat z prijate zpravy v pripade downloadu

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
    CHYBA_WRITE
} cislo_chyby;

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
    "Chyba pri zapisu do souboru!"
};

//Deklarace funkci
void error(cislo_chyby n);
bool kontrola_argumentu(int argc, char **argv, int *port);
bool spojeni(int *port);
bool odeslani(int socket, string soubor);
bool ulozeni(int socket, string soubor);
void sig(int signal);

int main(int argc, char **argv)
{
    int port;

    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, sig);
    signal(SIGINT, sig);

    if(kontrola_argumentu(argc, argv, &port) == false)
        return 1;

    if(spojeni(&port) == false)
        return 1;

    return 0;
}

bool kontrola_argumentu(int argc, char **argv, int *port)
{
    /*
        Funkce zkontroluje a zpracuje uzivatalem zadany argument. V pripade
        nejake chyby funkce vrati hodnotu FALSE, v pripade uspechu TRUE.
    */

    char *znak;

    if(argc != 3)
    {
        error(SPATNY_POCET_ARGUMENTU);
        return false;
    }

    if(strcmp(argv[1], "-p") != 0)
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    *port = strtol(argv[2], &znak, SOUSTAVA);
    if(*znak != '\0')
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    if(*port < MIN_PORT || *port > MAX_PORT)
    {
        error(SPATNY_ARGUMENT);
        return false;
    }

    return true;
}

bool spojeni(int *port)
{
    /*
        Funkce, ktera navaze spojeni s klientem a zacne s nim kominukovat. Zjisti, co
        chce klient provadet a podle toho zavola prislusnou funkci. V pripade nejakeho
        neuspechu funkce vrati hodnotu FALSE.
    */

    int rc, mujSocket, komSocket, res;
    struct sockaddr_in serverSocket, klientSocket;
    socklen_t klientVel;
    pid_t pid;
    char zprava[BUFFER];
    string soubor;
    string odpoved = "OK";
    bool operace;
    ssize_t odeslano;

    mujSocket = socket(PF_INET, SOCK_STREAM, 0);
    if(mujSocket <= 0)
    {
        error(CHYBA_SOCKET);
        return false;
    }

    serverSocket.sin_family = PF_INET;
    serverSocket.sin_port = htons(*port);
    serverSocket.sin_addr.s_addr = INADDR_ANY;

    rc = bind(mujSocket, (struct sockaddr *)&serverSocket, sizeof(serverSocket));
    if(rc < 0)
    {
        error(CHYBA_BIND);
        return false;
    }

    if(listen(mujSocket, 1) < 0)
    {
        error(CHYBA_LISTEN);
        return false;
    }

    while(1)
    {
        klientVel = sizeof(klientSocket);
        komSocket = accept(mujSocket, (struct sockaddr*)&klientSocket, &klientVel);

        if(komSocket < 0)
        {
            error(CHYBA_ACCEPT);
            return false;
        }

        pid = fork();
        if(pid == 0)
        {
            close(mujSocket);
            if((res = recv(komSocket, zprava, BUFFER, 0)) < 0)
            {
                error(CHYBA_RECV);
                return false;
            }

            soubor.append(zprava, res);
            if(soubor.find("Download: ") == 0)
            {
                cout << "Prijata zadost o odeslani souboru" << endl;
                operace = false;
                soubor.erase(0, DOWN);
            }

            else if(soubor.find("Upload: ") == 0)
            {
                cout << "Prijata zadost o ulozeni souboru" << endl;
                operace = true;
                if((odeslano = send(komSocket, odpoved.data(), odpoved.size(), 0)) < 0)
                {
                    error(CHYBA_SEND);
                    return false;
                }
                soubor.erase(0, UP);
            }

            if(!operace)
                odeslani(komSocket, soubor);
            else if(operace)
                ulozeni(komSocket, soubor);

            close(komSocket);
            exit(0);
        }

        close(komSocket);
    }

    close(mujSocket);

    return true;
}

bool odeslani(int socket, string soubor)
{
    /*
        Funkce, ktera klientovi odesle pozadovany soubor. V pripade nejakeho neuspechu
        vrati funkce hodnotu FALSE a posle klientovi zpravu "FAIL".
    */

    ssize_t cteni = 0, odeslane = 0;
    char buf[BUFFER];
    string chyba = "FAIL";
    int file;

    file = open(soubor.data(), O_RDONLY);
    if(file < 0)
    {
        error(CHYBA_OPEN);
        odeslane = send(socket, chyba.data(), chyba.size(), 0);
        if(odeslane < 0)
            error(CHYBA_SEND);

        return false;
    }

    while((cteni = read(file, buf, BUFFER)) > 0)
    {
        odeslane = send(socket, buf, cteni, 0);
        if(odeslane < cteni)
        {
            error(CHYBA_SEND);
            return false;
        }
    }

    cout << "Odeslan soubor: " << soubor << endl;

    close(file);

    return true;
}

bool ulozeni(int socket, string soubor)
{
    /*
        Funkce, ktera prijme od klienta zadany soubor. V pripade nejakeho neuspechu
        vrati funkce hodnotu FALSE a posle klientovi zpravu "FAIL".
    */

    int file;
    string chyba = "FAIL";
    char buf[BUFFER];
    ssize_t prijato = 0, odeslane = 0;

    file = open(soubor.data(), O_WRONLY|O_CREAT, 0644);
    if(file < 0)
    {
        error(CHYBA_OPEN);
        odeslane = send(socket, chyba.data(), chyba.size(), 0);
        if(odeslane < 0)
            error(CHYBA_SEND);

        return false;
    }

    while((prijato = recv(socket, buf, BUFFER, 0)) > 0)
    {
        if(write(file, buf, prijato) < 0)
        {
            error(CHYBA_WRITE);
            return false;
        }
    }

    cout << "Prijat soubor: " << soubor << endl;

    close(file);

    return true;
}

void sig(int signal)
{
    cerr << "Ukoncuji server, dostal jsem signal " << signal << endl;
    exit(0);
}

void error(cislo_chyby n) //Funkce na vypsani chybi na standratdni chybovi vystup
{
    cerr << chybova_hlaseni[n] << endl;
}
