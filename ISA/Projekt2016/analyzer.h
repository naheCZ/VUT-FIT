//============================================================================
// Jmeno        : analyzer.h
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : analyzer pro premdet ISA FIT VUT 2016/2017
// Datum        : 15.11.2016
//============================================================================

#ifndef ANALYZER_H
#define ANALYZER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

//Definice konstant
#define MIN_PARAMETRU 8 //Minimalni pocet parametru, ktery muze byt zadan
#define MAX_PARAMETRU 9 //Maximalni pocet parametru, ktery muze byt zadan
#define SOUSTAVA 10 //Soustava pro funkci strtol
#define TCP 6 //Ciselne oznaceni TCP protokolu v IP hlavicce
#define UDP 17 //Ciselne oznaceni UDP protokolu v IP hlavicce
#define ICMP 1 //Ciselne oznaceni ICMP protokolu v IP hlavicce
#define IPV4 8 //Cislene oznaceni IPv4 v ethernetove hlavicce
#define IPV6 56710 //Ciselne oznaceni IPv6 v ethernetove hlavicce
#define ARP 1544 //Cislene oznaceni ARP/RARP v ethernetove hlavicce
#define ZBYTEK 20 //ARP hlavicka ma 28 bytu, struktura arphdr pouze 8

using namespace std;

//Deklarace struktur
typedef enum chyba //Chybove stavy programu
{
    SPATNY_POCET_ARGUMENTU,
    SPATNY_ARGUMENT,
    NELZE_OTEVRIT
} cislo_chyby;

const char *chybova_hlaseni[] = //Patricne hlasky k chybovym stavum
{
    "Pocet zadanych argumentu je chybny!",
    "Nektery z argumentu je chybne zadan!",
    "Zadany soubor nelze otevrit!"
};

typedef enum ip_type //Mozne varianty ip adres
{
    IPv4,
    IPv6
} ip_type;

typedef struct parametry //Struktura, do ktere si ulozime parametry
{
  string soubor; //Nazev souboru
  string typ; //Typ filtru...mac/IPv4/IPv6/tcp/udp
  string hodnota; //Hodnota filtru
  bool s; //Zdrojove adresy
  bool d; //Cilove adresy
} type_parametry;

typedef struct pcap_hdr_s //Struktura hlavicky pcap souboru
{
    unsigned int magic_number;
    unsigned short version_major;
    unsigned short version_minor;
    signed int thiszone;
    unsigned int sigfigs;
    unsigned int snaplen;
    unsigned int network;
} pcap_hdr_s;

typedef struct pcaprec_hdr_s //Struktura hlavicky packetu
{
    unsigned int ts_sec;
    unsigned int ts_usec;
    unsigned int incl_len;
    unsigned int orig_len;
} pcaprec_hdr_t;

typedef struct nejvetsi //Struktura pro filtr top10
{
    string adresa; //Adresa/port zadaneho filtru
    int hodnota1;
    int hodnota2;
} type_top;

//Oficialni struktura z linux/ipv6.h, na testovacim virtualnim stroji se knihovna hadala s jinou.
struct ipv6hdr
{
#if defined(__LITTLE_ENDIAN_BITFIELD)
	__u8			priority:4,
                    version:4;
#elif defined(__BIG_ENDIAN_BITFIELD)
	__u8			version:4,
                    priority:4;
#else
#endif
	__u8			flow_lbl[3];

	__be16			payload_len;
	__u8			nexthdr;
	__u8			hop_limit;

	struct	in6_addr	saddr;
	struct	in6_addr	daddr;
};

//Deklarace funkci
void error(cislo_chyby n);
bool zpracovaniArgumentu(type_parametry* param, int argc, char** argv);
bool macKontrola(string hodnota);
bool ipKontrola(string hodnota, ip_type typ);
bool cteniSouboru(type_parametry* param, int* hodnota1, int* hodnota2);
bool top(type_parametry* param, vector <string>* hodnoty);
bool porovnani(const type_top &top1, const type_top &top2);
string intToString(int x);

#endif
