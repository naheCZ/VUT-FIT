/*
*   Projekt 1 do predmetu IPK 2015/2016
*   Autor: Roman Nahalka, xnahal01@stud.fit.vutbr.cz
*   Datum : 24.2.2016
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

//Definice promennych
#define PORT 80
#define SOUBOR "index.html"
#define BUFFER 1024
#define SOUSTAVA 10
#define MAX_PORT 1023
#define CESTA "/"
#define OK "200"
#define PRESMEROVANI1 "301"
#define PRESMEROVANI2 "302"
#define MAX_PRESMEROVANI 5

using namespace std;

typedef enum chyba //Vycet chyb
{
    SPATNY_POCET_ARGUMENTU,
    CHYBI_HTTP,
    CHYBA_SOCKET,
    SPATNA_ADRESA,
    NEZADANA_ADRESA,
    SPATNY_PORT,
    CHYBA_SPOJENI,
    NELZE_ODESLAT_DATA,
    CHYBA_POZADAVKU,
    NENALEZENA_ADRESA_PRESMEROVANI,
    NELZE_OTEVRIT_SOBUOR
} cislo_chyby;

const char *chybova_hlaseni[] = //Chybova hlaseni
{
    "Pocet zadanych argumentu je chybny!",
    "Zadana adresa nezacina http://! Program umi pracovat pouze s HTPP!",
    "Chyba pri vytvareni socketu!",
    "Chyba, byla zadana neplatna adresa!",
    "Za http nebyla zadana zadna adresa!",
    "Chybne zadany port!",
    "Nelze navazat spojeni!",
    "Chyba pri odesilani dat!",
    "Byl zadan spatny pozadavek!",
    "Nelze najit adresu presmerovani!",
    "Nelze ulozit soubor!",
};

typedef struct Url //Struktura URL
{
    int mujSocket; //Socket
    int port; //Cislo portu
    string adresa; // Adresa, ke ktere se budeme pripojovat
    string soubor; //Nazev souboru, ktery budeme stahovat
    string cesta; //Cesta k danemu souboru
    string odpoved; //Odpoved, kterou jsme dostali od serveru
    int kod; //Kod odpovedi
} type_url;

//Deklarace funkci
void error(cislo_chyby n);
bool zpracovani_url(char* argv, type_url *data, bool presmer);
bool spojeni(type_url *data);
int odpoved(string text);
bool stahnuti(type_url *data);
bool presmerovani(type_url *data);

int main(int argc, char **argv) //Hlavni funkce programu
{
    type_url data = {0, PORT, "", SOUBOR, "/", "", 0};

    if(argc != 2) //Zkontrolujeme, jestli byl zadan jediny argument
    {
        error(SPATNY_POCET_ARGUMENTU);
        return -1;
    }

    char *url = argv[1];

    if(zpracovani_url(url, &data, false) == false) //Zkontrolujeme, zda jsou argumenty zadany spravne
        return -1; //Jestlize nebyli, ukoncime program

    if(spojeni(&data) == false)
        return -1;

    if(data.kod == 301 || data.kod == 302)
    {
        char* lokace;
        string pom;

        for(int i = 0; i<MAX_PRESMEROVANI; i++)
        {
            if(presmerovani(&data) == false)
                return -1;
            pom = data.odpoved;
            lokace = (char*)pom.c_str();
            data.odpoved = "";

            if(zpracovani_url(lokace, &data, true) == false)
                return -1;

            if(spojeni(&data) == false)
                return -1;
            if(data.kod != 301 && data.kod != 302)
                break;
        }
    }

    if(data.kod == 200)
    {
        if(stahnuti(&data) == false)
            return -1;
    }

    else if (data.kod == 400)
        return -1;

    return 0;
}


bool zpracovani_url(char* argv, type_url *data, bool presmer) //Funkce pro zpracovani argumentu
{
    string url = argv;
    char *znak;
    size_t delka;

    if(url.find("http://") != 0) //Kontrola, jestli zadany argument zacina "http://"
    {
        error(CHYBI_HTTP);
        return false;
    }

    url.erase(0, 7); //Smazeme "http://" z adresy
    if(url.length() == 0) //Zkontorlujeme, jestli nam neco zustalo
    {
        error(NEZADANA_ADRESA);
        return false;
    }

    if((delka = url.find(":")) == string::npos) //Zjistime, jestli uzivatel zadal i port
    {
        //Nezadal port

        data->port = PORT; //Nezadal, port bude vychozi pro HTTP, tedy 80
        if((delka = url.find("/")) == string::npos) //Nachazi se dale v adrese lomitko?
        {
            //Nenachazi....vychozi hodnoty
            data->adresa = url;
            if( ! presmer)
                data->soubor = SOUBOR;
            data->cesta = CESTA;
        }
        else //Nachazi, musime nastavit cestu a nazev souboru
        {
            data->adresa = url.substr(0, delka);
            url.erase(0, delka);
            data->cesta = url;
            if(url.length() == 1)
                data->soubor = SOUBOR;
            else
            {
                url.erase(0, 1);

                if((delka = url.rfind("/")) != string::npos)
                {
                    url.erase(0, delka);
                    if(url.length() == 1)
                    {
                        if( ! presmer)
                            data->soubor = SOUBOR;
                    }
                    else
                    {
                        url.erase(0, 1);
                        if( ! presmer)
                            data->soubor = url;
                    }
                }
                else
                {
                    if( ! presmer)
                        data->soubor = url;
                }
            }
        }
    }
    else //Uzivatel zadal port
    {
        int delkaCisla = 0;

        data->adresa = url.substr(0, delka);
        url.erase(0, delka+1);
        data->port = strtol(url.c_str(), &znak, SOUSTAVA);
        if(data->port <= 0 || data->port > MAX_PORT)
        {
            error(SPATNY_PORT);
            return false;
        }

        while(isdigit(url[delkaCisla]))
            delkaCisla++;

        url.erase(0, delkaCisla);
        if((delka = url.find("/")) == string::npos) //Nachazi se dale v adrese lomitko?
        {
            //Nenachazi, vychozi hodnoty
            if( ! presmer)
                data->soubor = SOUBOR;
            data->cesta = CESTA;
        }
        else //Nachazi se lomitko, nastavemi cestu a nezev souboru
        {
            data->cesta = url;
            if(delka == url.length()-1)
            {
                if( ! presmer)
                    data->soubor = SOUBOR;
            }
            else
            {
                url.erase(0, delka+1);

                if((delka = url.rfind("/")) != string::npos)
                {
                    url.erase(0, delka);
                    if(url.length() == 1)
                    {
                        if( ! presmer)
                            data->soubor = SOUBOR;
                    }
                    else
                    {
                        url.erase(0, 1);
                        if( ! presmer)
                            data->soubor = url;
                    }
                }
                else
                {
                    if( ! presmer)
                        data->soubor = url;
                }
            }
        }
    }

    while((delka = data->cesta.find(" ")) != string::npos)
        data->cesta.replace(delka, 1, "%20");

    return true;
}

bool spojeni(type_url *data) //Funkce na navazani spojeni se serverem, v pripade neuspechu vrati FALSE
{
    sockaddr_in serverSocket;
    hostent *host;
    int bytest;
    int prvniRadek;
    string text("GET " + data->cesta + " HTTP/1.1\r\n"
                "Host: " + data->adresa + "\r\n"
                "Connection: close\r\n\r\n");
    char buf[BUFFER];
    string kodOdpovedi;

    data->mujSocket = socket(AF_INET, SOCK_STREAM, 0); //Vytvorime socket
    if(data->mujSocket <= 0) //Podarilo se socket vytvorit?
    {
        error(CHYBA_SOCKET);
        return false;
    }

    host = gethostbyname(data->adresa.c_str()); //Zjistime si adresu, ke ktere se mame pripojit
    if(host == NULL)
    {
        error(SPATNA_ADRESA);
        return false;
    }

    serverSocket.sin_family = AF_INET; //Typ protokolu socketu
    serverSocket.sin_port = htons(data->port); //Nastaveni portu, ke kteremu se pripojime
    memcpy(&serverSocket.sin_addr, host->h_addr, host->h_length); //Nastaveni IP adresy socketu

    if(connect(data->mujSocket, (sockaddr *)&serverSocket, sizeof(serverSocket)) != 0) //Zkusime se pripojit
    {
        error(CHYBA_SPOJENI); //Nepovedlo se, vratime chybu a skoncime
        return false;
    }

    bytest = send(data->mujSocket, text.c_str(), text.length(), 0); //Odesleme data
    if(bytest < 0) //Zkontorluje, jestli se odeslani podarilo
    {
        error(NELZE_ODESLAT_DATA);
        return false;
    }

    while((bytest = recv(data->mujSocket, buf, BUFFER, 0)) > 0)
    {
        data->odpoved.append(buf, bytest);
    }
    close(data->mujSocket);//Ukoncime socket

    kodOdpovedi = data->odpoved;
    prvniRadek = kodOdpovedi.find("\n");
    kodOdpovedi.erase(prvniRadek);
    data->kod = odpoved(kodOdpovedi);
    if(data->kod == 400) //Nastala nejaka chyba, vypiseme ji
    {
        kodOdpovedi.erase(0, 9);
        cerr << kodOdpovedi << endl;
    }

    return true;
}

int odpoved(string text) //Zjistime si, co nam server odpovedel za stav
{
    if(text.find(OK) != string::npos)
        return 200;

    else if(text.find(PRESMEROVANI1) != string:: npos)
        return 301;

    else if(text.find(PRESMEROVANI2) != string::npos)
        return 302;

    else
        return 400;
}

bool stahnuti(type_url *data)
{
    ofstream soubor;
    size_t hlavicka;

    if((hlavicka = data->odpoved.find("\r\n\r\n")) != string::npos)
        data->odpoved.erase(0, hlavicka+4);

    soubor.open(data->soubor.c_str(), ios::out | ios::binary);
    if(soubor.fail())
    {
        error(NELZE_OTEVRIT_SOBUOR);
        return false;
    }
    soubor.write(data->odpoved.c_str(), data->odpoved.length());
    soubor.close();

    return true;
}

bool presmerovani(type_url *data)
{
    size_t lokace;

    if((lokace = data->odpoved.find("Location: ")) != string::npos)
    {
        data->odpoved.erase(0, lokace);
        lokace = data->odpoved.find("\n");
        data->odpoved.erase(lokace);
        data->odpoved.erase(0, 10);
    }
    else
    {
        error(NENALEZENA_ADRESA_PRESMEROVANI);
        return false;
    }

    return true;
}

void error(cislo_chyby n)
{
    cerr << chybova_hlaseni[n] << endl;
}
