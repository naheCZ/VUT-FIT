//============================================================================
// Jmeno        : analyzer.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : analyzer pro premdet ISA FIT VUT 2016/2017
// Datum        : 15.11.2016
//============================================================================

#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <netdb.h>
#include "analyzer.h"
#include <netinet/if_ether.h>

using namespace std;


int main(int argc, char** argv)
{
    type_parametry param;
    int hodnota1 = 0;
    int hodnota2 = 0;

    if(zpracovaniArgumentu(&param, argc, argv) == false) //Zpracujeme argumenty prikazove radky
        return 1;


    if(param.hodnota.compare("top10") != 0)
    {
        if(cteniSouboru(&param, &hodnota1, &hodnota2) == false)
            return 2;

        cout << hodnota1 << "_" << hodnota2 << endl;
    }

    else
    {
        vector <string> hodnoty;
        vector <type_top> nej;
        type_top pom;
        unsigned int pocet = 0;

        if(top(&param, &hodnoty) == false)
            return 3;

        for(unsigned int i = 0; i < hodnoty.size(); i++)
            cout << i << ": " << hodnoty[i] << endl;

        nej.reserve(hodnoty.size());

        //Cyklus, ve kterem projedem vsechny mozne hodnoty filtru
        for(unsigned int i = 0; i < hodnoty.size(); i++)
        {
            hodnota1 = 0;
            hodnota2 = 0;
            param.hodnota = hodnoty[i];
            if(cteniSouboru(&param, &hodnota1, &hodnota2) == false)
                return 2;
            pom.adresa = hodnoty[i];
            pom.hodnota1 = hodnota1;
            pom.hodnota2 = hodnota2;
            nej.push_back(pom);
        }

        sort(nej.begin(), nej.end(), porovnani); //Seradime podle velikosti
        pocet = nej.size();

        if(pocet > 10)
            pocet = 10;

        for(unsigned int i = 0; i < pocet; i++)
            cout << nej[i].adresa << "_" << nej[i].hodnota1 << "_" << nej[i].hodnota2 << endl;
    }

	return 0;
}

bool zpracovaniArgumentu(type_parametry* param, int argc, char** argv)
{
    /*
        Funkce zpracuje a zkontroluje argumenty prikazove radky. V pripade, ze behem zpracovani
        argumentu dojde k nejake chybe, funkce vypise patruicnou chzbovou hlasku a vrati hodnotu
        false.
    */

    string typy[5] = {"mac", "ipv4", "ipv6", "tcp", "udp"}; //Vsechny mozne typy, ktere uzivatel mohl zadat jako argument.
    char* znak;
    int c;

    if(argc < MIN_PARAMETRU || argc >> MAX_PARAMETRU)
    {
        error(SPATNY_POCET_ARGUMENTU);
        return false;
    }

    param->s = false;
    param->d = false;

    while((c = getopt(argc, argv, "i:f:v:s::d::")) != -1)
    {
        switch(c)
        {
            case 'i':
                param->soubor = optarg;
                break;
            case 'f':
                param->typ = optarg;
                break;
            case 'v':
                param->hodnota = optarg;
                break;
            case 's':
                param->s = true;
                break;
            case 'd':
                param->d = true;
                break;
        }
    }

    if((param->s == false) && (param->d == false))
    {
        error(SPATNY_POCET_ARGUMENTU);
        return false;
    }

    //Zmensime vsechny znaky v uzivatelove argumentu, aby jsme je mohli zkontrolovat.
    transform(param->typ.begin(), param->typ.end(), param->typ.begin(), ::tolower);

    for(int i = 0; i < 5; i++)
    {
        if(i != 4)
        {
            if(param->typ.compare(typy[i]) == 0)
                break;
        }

        else
        {
            if(param->typ.compare(typy[i]) == 0)
                break;

            else
            {
                error(SPATNY_ARGUMENT);
                return false;
            }
        }
    }

    //Zkontrolujeme, jestli uzivatel k typu filtru zadal spravnou hodnotu
    if(param->hodnota.compare("top10") != 0)
    {
        if(param->typ.compare(typy[0]) == 0)
        {
            //Pro lepsi kontrolu vsechny pismena z MAC adresy prevedeme na velka.
            transform(param->hodnota.begin(), param->hodnota.end(), param->hodnota.begin(), ::tolower);

            if(macKontrola(param->hodnota) == false)
            {
                error(SPATNY_ARGUMENT);
                return false;
            }
        }

        else if(param->typ.compare(typy[1]) == 0)
        {
            if(ipKontrola(param->hodnota, IPv4) == false)
            {
                error(SPATNY_ARGUMENT);
                return false;
            }
        }

        else if(param->typ.compare(typy[2]) == 0)
        {
            if(ipKontrola(param->hodnota, IPv6) == false)
            {
                error(SPATNY_ARGUMENT);
                return false;
            }
        }
        else
        {
            strtol(param->hodnota.c_str(), &znak, SOUSTAVA);
            if(*znak != '\0')
            {
                error(SPATNY_ARGUMENT);
                return false;
            }
        }
    }

    return true;
}

bool macKontrola(string hodnota)
{
    /*
        Funkce zkontroluje, jestli uzivatel zadal platnou MAC adresu.
        Jestli nezadal, funkce vrati hodnotu false, v opacnem pripade true.
    */

    unsigned int delka = 17;

    if(hodnota.length() != delka)
        return false;

    for(unsigned int i = 0; i < delka; i++)
    {
        if(i == 2 || i == 5 || i == 8 || i == 11 || i == 14) //Mista, kde se maji nachazet dvojtecky.
        {
            if(hodnota[i] != ':')
                return false;
        }

        else
        {
            if((hodnota[i] < '0' || hodnota[i] > '9') && (hodnota[i] < 'a' || hodnota[i] > 'f')) //Mista, kde se maji nachazet hexadecimalni znaky.
                return false;
        }
    }

    return true;
}

bool ipKontrola(string hodnota, ip_type typ)
{
    /*
        Funkce zkontroluje, jestli uzivatel zadal platnou IP adresu (IPv4 nebo IPv6).
        Jestli zadany argument neni IP adresa funkce vrati hodnotu false.
        V opacnem pripade funkce vrati hodnotu true.
    */

    struct addrinfo adresa, *adresa1 = NULL;

    memset(&adresa1, '\0', sizeof adresa);

    adresa.ai_family = PF_UNSPEC;
    adresa.ai_flags = AI_NUMERICHOST;

    if(getaddrinfo(hodnota.c_str(), NULL, &adresa, &adresa1) != 0)
        return false;

    if(typ == IPv4 && adresa1->ai_family == AF_INET)
        return true;

    else if(typ == IPv6 && adresa1->ai_family == AF_INET6)
        return true;
    else
        return false;
}

bool cteniSouboru(type_parametry* param, int* hodnota1, int* hodnota2)
{
    /*
        Funkce bude ze zadaneho pcap souboru cist data, dokud nedojde na konec souboru.
        Nepujde-li soubor otevrit, ci dojde k nejake jine chybe, funkce vrati hodnotu
        false. Pokud vse probehne v poradku, vrati hodnotu true.
    */

    pcap_hdr_s sHlavicka;
    pcaprec_hdr_t pHlavicka;
    struct ethhdr eth;
    struct iphdr ip;
    struct ipv6hdr ip6;
    struct udphdr udp;
    struct tcphdr tcp;
    struct arphdr arp;
    struct sockaddr_in zdroj, cil;
    ifstream soubor(param->soubor.c_str(), ios::in | ios::binary);
    int pocet = 0;
    int misto = 0;
    int argPort = 0;
    int port = 0;
    int velikost = 0;
    string pom;
    char mac[18];
    char buff[16];

    if(param->typ.compare("udp") == 0 || param->typ.compare("tcp") == 0)
        argPort = atoi(param->hodnota.c_str());

    memset(&zdroj, 0, sizeof(zdroj));
    memset(&cil, 0, sizeof(cil));

    if(soubor.is_open()) //Podarilo se otevrit soubor?
    {
        soubor.read((char*)&sHlavicka, sizeof(sHlavicka));

        //Cyklus, ve kterem precteme vsechny packety*/
        while(soubor.eof() == false)
        {
            pocet++;

            soubor.read((char*)&pHlavicka, sizeof(pHlavicka));
            if(soubor.eof() == true)
                break;
            soubor.read((char*)&eth, sizeof(struct ethhdr));

            if(eth.h_proto == IPV4) //Nasleduje IPv4 hlavicka
            {
                soubor.read((char*)&ip, sizeof(struct iphdr));

                velikost = pHlavicka.orig_len - (ntohs(ip.tot_len) + sizeof(struct ethhdr));

                if(param->typ.compare("mac") == 0)
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_dest[0], eth.h_dest[1],
                        eth.h_dest[2], eth.h_dest[3], eth.h_dest[4], eth.h_dest[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(iphdr);
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_source[0], eth.h_source[1],
                        eth.h_source[2], eth.h_source[3], eth.h_source[4], eth.h_source[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(iphdr);
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if(param->typ.compare("ipv4") == 0 ) //Pokud mame filtr IPv4
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true) //Je filtr aplikovany na cilove adresy?
                    {
                        cil.sin_addr.s_addr = ip.daddr;
                        pom = inet_ntoa(cil.sin_addr);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true) //Je filtr aplikovany na zdrojove adresy?
                    {
                        zdroj.sin_addr.s_addr = ip.saddr;
                        pom = inet_ntoa(zdroj.sin_addr);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if((param->typ.compare("udp") == 0) && ((unsigned int)ip.protocol == UDP))
                {
                    soubor.read((char*)&udp, sizeof(struct udphdr));

                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(udp.dest);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += sizeof(struct udphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        port = ntohs(udp.source);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += sizeof(struct udphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if((param->typ.compare("tcp") == 0) && ((unsigned int)ip.protocol == TCP))
                {

                    soubor.read((char*)&tcp, sizeof(struct tcphdr));

                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct tcphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(tcp.dest);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += sizeof(struct tcphdr);
                            *hodnota1 += misto;
                            if((tcp.doff*4) > 20)
                                misto -= tcp.doff*4 - 20;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        port = ntohs(tcp.source);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct iphdr);
                            *hodnota1 += sizeof(struct tcphdr);
                            *hodnota1 += misto;
                            if((tcp.doff*4) > 20)
                                misto -= tcp.doff*4 - 20;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr)));
                    soubor.seekg(misto, soubor.cur);
                }
            }

            else if(eth.h_proto == IPV6) //Nasleduje IPv6 hlavicka
            {
                soubor.read((char*)&ip6, sizeof(struct ipv6hdr));

                velikost = pHlavicka.orig_len - (ntohs(ip6.payload_len) + sizeof(struct ipv6hdr) + sizeof(struct ethhdr));

                if(param->typ.compare("mac") == 0)
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_dest[0], eth.h_dest[1],
                        eth.h_dest[2], eth.h_dest[3], eth.h_dest[4], eth.h_dest[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(ipv6hdr);
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_source[0], eth.h_source[1],
                        eth.h_source[2], eth.h_source[3], eth.h_source[4], eth.h_source[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(ipv6hdr);
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if(param->typ.compare("ipv6") == 0)
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        pom = inet_ntop(AF_INET6, &ip6.daddr, buff, sizeof(ip6.daddr));
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        pom = inet_ntop(AF_INET6, &ip6.saddr, buff, sizeof(ip6.saddr));
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if((param->typ.compare("udp") == 0) && ((unsigned int)ip6.nexthdr == UDP))
                {
                    soubor.read((char*)&udp, sizeof(struct udphdr));

                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr) + sizeof(struct udphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(udp.dest);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += sizeof(struct udphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        port = ntohs(udp.dest);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += sizeof(struct udphdr);
                            *hodnota1 += misto;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else if((param->typ.compare("tcp") == 0) && ((unsigned int)ip6.nexthdr == TCP))
                {
                    soubor.read((char*)&tcp, sizeof(struct tcphdr));

                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr) + sizeof(struct tcphdr)));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(tcp.dest);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += sizeof(struct tcphdr);
                            *hodnota1 += misto;
                            if((tcp.doff*4) > 20)
                                misto -= tcp.doff*4 - 20;
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        port = ntohs(tcp.source);
                        if(port == argPort)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct ipv6hdr);
                            *hodnota1 += sizeof(struct tcphdr);
                            *hodnota1 += misto;
                            if((tcp.doff*4) > 20)
                                misto -= tcp.doff*4 - 20;
                            *hodnota2 += misto - velikost;
                        }
                    }
                }

                else
                {
                    misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr)));
                    soubor.seekg(misto, soubor.cur);
                }
            }

            else if(eth.h_proto == ARP)
            {
                soubor.read((char*)&arp, sizeof(struct arphdr));
                velikost = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct arphdr) + ZBYTEK);

                misto = (pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct arphdr)));
                soubor.seekg(misto, soubor.cur);

                if(param->typ.compare("mac") == 0)
                {
                    if(param->d == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_dest[0], eth.h_dest[1],
                        eth.h_dest[2], eth.h_dest[3], eth.h_dest[4], eth.h_dest[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct arphdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(struct arphdr);
                            *hodnota2 += misto - velikost;
                        }
                    }

                    if(param->s == true)
                    {
                        snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_source[0], eth.h_source[1],
                        eth.h_source[2], eth.h_source[3], eth.h_source[4], eth.h_source[5]);
                        pom = string(mac);
                        if(pom.compare(param->hodnota) == 0)
                        {
                            *hodnota1 += sizeof(struct ethhdr);
                            *hodnota1 += sizeof(struct arphdr);
                            *hodnota1 += misto;
                            *hodnota2 += sizeof(struct arphdr);
                            *hodnota2 += misto - velikost;
                        }
                    }
                }
            }

            else
            {
                misto = pHlavicka.orig_len - sizeof(ethhdr);
                soubor.seekg(misto, soubor.cur);
            }
        }
    }

    else //Soubor se nepodarilo otevrit
    {
        error(NELZE_OTEVRIT);
        return false;
    }

    soubor.close();

    return true;
}

bool top(type_parametry* param, vector <string>* hodnoty)
{
    /*
        Funkce nalezne v souboru vsechny packety se zadanym filtrem. Cilem je nalezt vsechny hodnoty
        filtru, ktere se v souboru nachazeji. Ty se pak pouziji pro filtr top10. V pripade, ze funkce
        probehne bez chyby vrati hodnotu true, v opacnem pripade vrati hodnotu false.
    */

    ifstream soubor(param->soubor.c_str(), ios::in | ios::binary);
    pcap_hdr_s sHlavicka;
    pcaprec_hdr_t pHlavicka;
    struct ethhdr eth;
    struct iphdr ip;
    struct ipv6hdr ip6;
    struct udphdr udp;
    struct tcphdr tcp;
    struct sockaddr_in zdroj, cil;
    int misto = 0;
    char mac[18];
    string pom;
    int port = 0;
    char buff[16];

    memset(&zdroj, 0, sizeof(zdroj));
    memset(&cil, 0, sizeof(cil));

    if(soubor.is_open()) //Podarilo se otevrit soubor?
    {
        soubor.read((char*)&sHlavicka, sizeof(sHlavicka));

        //Cyklus, ve kterem precteme vsechny packety
        while(soubor.eof() == false)
        {
            soubor.read((char*)&pHlavicka, sizeof(pHlavicka));
            if(soubor.eof() == true) //Jinak se nekonecne zacykli...
                break;
            soubor.read((char*)&eth, sizeof(struct ethhdr));

            if(param->typ.compare("mac") == 0)
            {
                misto = pHlavicka.orig_len - sizeof(struct ethhdr);
                soubor.seekg(misto, soubor.cur);

                if(param->d == true)
                {
                    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_dest[0], eth.h_dest[1],
                    eth.h_dest[2], eth.h_dest[3], eth.h_dest[4], eth.h_dest[5]);
                    pom = string(mac);

                    if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                        hodnoty->push_back(pom);
                }

                if(param->s == true)
                {
                    snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x", eth.h_source[0], eth.h_source[1],
                    eth.h_source[2], eth.h_source[3], eth.h_source[4], eth.h_source[5]);
                    pom = string(mac);

                    if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                        hodnoty->push_back(pom);
                }
            }

            else if(eth.h_proto == IPV4)
            {
                soubor.read((char*)&ip, sizeof(struct iphdr));

                if(param->typ.compare("ipv4") == 0)
                {
                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        cil.sin_addr.s_addr = ip.daddr;
                        pom = inet_ntoa(cil.sin_addr);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        zdroj.sin_addr.s_addr = ip.saddr;
                        pom = inet_ntoa(zdroj.sin_addr);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }
                }

                else if(param->typ.compare("udp") == 0 && (unsigned int)ip.protocol == UDP)
                {
                    soubor.read((char*)&udp, sizeof(struct udphdr));

                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(udphdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(udp.dest);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        port = ntohs(udp.source);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }
                }

                else if(param->typ.compare("tcp") == 0 && (unsigned int)ip.protocol == TCP)
                {
                    soubor.read((char*)&tcp, sizeof(struct tcphdr));

                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(tcphdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(tcp.dest);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        port = ntohs(tcp.source);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }
                }

                else
                {
                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct iphdr));
                    soubor.seekg(misto, soubor.cur);
                }
            }

            else if(eth.h_proto == IPV6)
            {
                soubor.read((char*)&ip6, sizeof(struct ipv6hdr));

                if(param->typ.compare("ipv6") == 0)
                {
                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        pom = inet_ntop(AF_INET6, &ip6.daddr, buff, sizeof(ip6.daddr));

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        pom = inet_ntop(AF_INET6, &ip6.saddr, buff, sizeof(ip6.saddr));

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }
                }

                else if(param->typ.compare("udp") == 0 && (unsigned int)ip6.nexthdr == UDP)
                {
                    soubor.read((char*)&udp, sizeof(struct udphdr));

                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr) + sizeof(struct udphdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(udp.dest);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        port = ntohs(udp.source);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                             hodnoty->push_back(pom);
                    }
                }

                else if(param->typ.compare("tcp") == 0 && (unsigned int)ip6.nexthdr == TCP)
                {
                    soubor.read((char*)&tcp, sizeof(struct tcphdr));

                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr) + sizeof(struct tcphdr));
                    soubor.seekg(misto, soubor.cur);

                    if(param->d == true)
                    {
                        port = ntohs(tcp.dest);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }

                    if(param->s == true)
                    {
                        port = ntohs(tcp.source);
                        pom = intToString(port);

                        if(find(hodnoty->begin(), hodnoty->end(), pom) == hodnoty->end())
                            hodnoty->push_back(pom);
                    }
                }

                else
                {
                    misto = pHlavicka.orig_len - (sizeof(struct ethhdr) + sizeof(struct ipv6hdr));
                    soubor.seekg(misto, soubor.cur);
                }
            }

            else
            {
                misto = pHlavicka.orig_len - sizeof(ethhdr);
                soubor.seekg(misto, soubor.cur);
            }
        }

    }

    else
    {
        error(NELZE_OTEVRIT);
        return false;
    }

    return true;
}

bool porovnani(const type_top &top1, const type_top &top2)
{
    /*
        Funkce porovnava dve hodnoty a vraci true, pokud je prvni vetsi, v opacnem priapde vraci false.
    */

    return top1.hodnota1 > top2.hodnota1;
}

string intToString(int x)
{
    /*
        Funkce, ktera prevadi cislo typu Integer do typu String. Funkce vraci retezec vytvoreny z cisla.
    */

    ostringstream s;
    s << x;
    return s.str();
}

void error(cislo_chyby n)
{
    /*
        Funkce, ktera na stantartni chybovi vystup vypisuje chybovou hlasku.
    */

    cerr << chybova_hlaseni[n] << endl;
}
