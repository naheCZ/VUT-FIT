//============================================================================
// Jmeno        : server.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 20.11.2017
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <dirent.h>
#include <time.h>
#include <algorithm>
#include "server.h"

using namespace std;

Server::Server(Arguments* arg)
{
    /*
        Konstruktor pro třídu server
    */

    this->arg = arg;
    this->usr = arg->getUsr();
    this->msgNum = 0;
    this->bytes = 0;
}

Arguments* Server::getArguments()
{
    /*
        Funkce, která vrátí ukazatel do paměti, kde se nachází objekt Argumenty.
    */

    return this->arg;
}

bool Server::connection()
{
    /*
        Funkce na inicializaci spojení mezi klientem a serverem.
        Při příchozím spojení funkce vytvoří pro každého klienta nové vlákno.
    */

    int bindCode, mySocket, comSocket;
    struct sockaddr_in serverSocket, clientSocket;
    socklen_t clientLen;

    mySocket = socket(PF_INET, SOCK_STREAM, 0);

    if(mySocket <= 0) //Chyba při vytváření SOCKETU
    {
        this->err.printErr(SOCKET_CHYBA);
        return false;
    }

    //Nastavení SOCKETU
    serverSocket.sin_family = PF_INET;
    serverSocket.sin_port = htons(this->getArguments()->getPortNumber());
    serverSocket.sin_addr.s_addr = INADDR_ANY;

    bindCode = bind(mySocket, (struct sockaddr *)&serverSocket, sizeof(serverSocket));

    if(bindCode < 0) //Chyba při bindu, nejspíše obsazený port.
    {
        this->err.printErr(BIND_CHYBA);
        return false;
    }

    if(listen(mySocket, 1) < 0)
    {
        this->err.printErr(LISTEN_CHYBA);
        return false;
    }

    while(42) //Hlavní cyklus, ve tkerém budeme příjmat požadavky od klientů
    {
        clientLen = sizeof(clientSocket);
        comSocket = accept(mySocket, (struct sockaddr*)&clientSocket, &clientLen);

        if(comSocket < 0) //Chyba při navázání spojení
        {
            this->err.printErr(ACCEPT_CHYBA);
            sleep(5);
            continue;
        }

        //Vytvoření nového vlákna, které se nechá žít vlastním životem
        this->thr = thread(&Server::authorization, this, comSocket);
        thr.detach();
    }

    close(comSocket);
    return true;
}

void Server::authorization(int sock)
{
    /*
        Funkce, pro ověření připojeného uživatele.
        Podle přepínače -c bude funkce očekávat příkaz APOP nebo kombinace příkazů USER/PASS.
    */

    string msg = "POP3 server pripraven ";
    ssize_t res;
    char zprava[BUFFER];
    size_t delka;
    bool tr = true;
    bool respond = true;
    timeval tv;
    fd_set myset;
    int ret;

    FD_ZERO(&myset);
    FD_SET(sock, &myset);

    msg.append(this->getTimeStamp());
    respond = this->respondOK(msg, sock);

    while(42)
    {
        if(!respond) //Klient ukončil spojení
            return;

        if(!(this->arg->getC())) //Šifrované spojení, očekává se příkaz APOP
        {
            string username;
            string password;

            tv.tv_sec = CAS;
            tv.tv_usec = 0;
            ret = select(sock + 1, &myset, NULL, NULL, &tv);

            if(ret <= 0)
            {
                cerr << "Hej hej" << endl;
                tr = false;
                break;
            }

            res = recv(sock, zprava, BUFFER, 0);

            if(res < 0)
            {
                this->err.printErr(RECV_CHYBA);
                return;
            }

            string command(zprava);

            for(int i = 0; i < 4; i++) //Case in-sensitive
                command[i] = tolower(command[i]);

            if(!(command.find("quit", 0, 4))) //Uživatel ukončil spojení
            {
                tr = false;
                break;
            }

            else if(!(command.find("apop ", 0, 5))) //Uživatel správně zadal příkaz apop
            {
                command.erase(0, 5);

                if((delka = command.find(" ")) == string::npos) //Uživatel nezadal heslo
                {
                    msg = "Nezadal jsi heslo";
                    respond = this->respondERR(msg, sock);
                    continue;
                }

                username = command.substr(0, delka);
                command.erase(0, delka + 1);

                password = command.substr(0, command.find("\r\n"));

                if((username.compare(this->usr->getUsername())) || (password.compare(this->usr->getEncryptedPassword()))) //Kontrola správnosti údajů
                {
                    msg = "Uzivatelske jmeno nebo heslo je spatne";
                    respond = this->respondERR(msg, sock);
                    continue;
                }

                else //Vše zadáno správně
                {
                    if(!this->mailDirControl()) //Kontrola správné struktury maildiru
                    {
                        msg = "nelze se dostat do maildiru";
                        respond = this->respondERR(msg, sock);
                        continue;
                    }

                    if(!(this->mtx.try_lock())) //Pokusíme se uzamknout přístup
                    {
                        msg = "nelze ziskat pristup k maildropu";
                        respond = this->respondERR(msg, sock);
                        continue;
                    }

                    break;
                }
            }

            else
            {
                msg = "Byl zadan neznami nebo nepovoleny prikaz";
                respond = this->respondERR(msg, sock);
                continue;
            }
        }

        else //Nešifrovaná komunikace
        {
            string username;
            string password;
            bool check = true;

            tv.tv_sec = CAS;
            tv.tv_usec = 0;
            ret = select(sock + 1, &myset, NULL, NULL, &tv);

            if(ret <= 0)
            {
                tr = false;
                break;
            }

            res = recv(sock, zprava, BUFFER, 0);

            if(res < 0)
            {
                this->err.printErr(RECV_CHYBA);
                return;
            }

            string command(zprava);

            for(int i = 0; i < 4; i++) //Case in-sensitive
                command[i] = tolower(command[i]);

            if(!(command.find("quit", 0, 4))) //Klient ukončil spojení
            {
                tr = false;
                break;
            }

            else if(!(command.find("user ", 0, 5))) //Klient správně zadal příkaz USER
            {
                command.erase(0, 5);

                username = command.substr(0, command.find("\r\n"));

                if(username.compare(this->usr->getUsername())) //Špatné uživatelské jméno...nicméně klient nesmí vědět, který z údajů byl špatně zadán, takže pokračujeme
                    check = false;

                msg = "Nyni zadejte heslo";
                respond = this->respondOK(msg, sock);

                tv.tv_sec = CAS;
                tv.tv_usec = 0;
                ret = select(sock + 1, &myset, NULL, NULL, &tv);

                if(ret <= 0)
                {
                    tr = false;
                    break;
                }

                res = recv(sock, zprava, BUFFER, 0);

                if(res < 0)
                {
                    this->err.printErr(RECV_CHYBA);
                    return;
                }

                command = zprava;

                for(int i = 0; i < 4; i++) //Case in-sensitive
                    command[i] = tolower(command[i]);

                if(command.find("pass ", 0, 5)) //Špatně zadaný příkaz
                {
                    msg = "Nezadal si prikaz PASS";
                    respond = this->respondERR(msg, sock);
                    continue;
                }

                command.erase(0, 5);

                password = command.substr(0, command.find("\r\n"));

                if(password.compare(this->usr->getPassword())) //Špatně zadané heslo
                    check = false;

                if(!check) //Některý z údajů byl chybný
                {
                    msg = "Uzivatelske jmeno nebo heslo je spatne";
                    respond = this->respondERR(msg, sock);
                    continue;
                }

                else //Vše proběhlo v pořádku
                {

                    if(!this->mailDirControl()) //Špatná struktura maildiru
                    {
                        msg = "Nastala chyba u maildiru";
                        respond = this->respondERR(msg, sock);
                        continue;
                    }

                    if(!(this->mtx.try_lock())) //Pokusíme se uzamknout přístup
                    {
                        msg = "Nelze ziskat pristup k maildropu";
                        respond = this->respondERR(msg, sock);
                        continue;
                    }

                    break;
                }
            }

            else
            {
                msg = "Byl zadan neznami nebo nepovoleny prikaz";
                respond = this->respondERR(msg, sock);
                continue;
            }
        }
    }


    if(tr)
        this->transaction(sock);

    else
    {
        close(sock);
        return;
    }

    this->quit(sock);

    return;
}

void Server::transaction(int sock)
{
    /*
        Funkce, která reprezentuje stav transakce u POP3 Serveru.
        V tomto stavu server očekává od klienta zadávání příkazů a server je vyhodnocuje.
    */

    string msg;
    string wholeCommand;
    string command;
    string rest;
    ssize_t res;
    char zprava[BUFFER];
    bool respond = true;
    bool param;
    fd_set myset;
    timeval tv;
    int ret;

    FD_ZERO(&myset);
    FD_SET(sock, &myset);

    this->newToCur();

    msg = "vase schranka ma ";
    msg.append(to_string(this->msgNum));
    msg.append(" zprav (");
    msg.append(to_string(this->bytes));
    msg.append(" oktetu)");
    this->respondOK(msg, sock);

    while(42) //Cyklus, který zpracovává příkazy zadané od klienta.
    {
        if(!respond)
        {
            return;
        }

        param = false;

        tv.tv_sec = CAS;
        tv.tv_usec = 0;
        ret = select(sock + 1, &myset, NULL, NULL, &tv);

        if(ret <= 0)
            return;

        res = recv(sock, zprava, BUFFER, 0);

        if(res < 0)
        {
            this->err.printErr(RECV_CHYBA);
            continue;
        }

        wholeCommand = zprava;

        transform(wholeCommand.begin(), wholeCommand.end(), wholeCommand.begin(), ::tolower);

        command = wholeCommand.substr(0, 4);
        wholeCommand.erase(0, 4);

        rest = wholeCommand.substr(0, wholeCommand.find("\r\n"));

        if(rest.size() != 0) //Příkazy mají 4 znaky, tady se nachází ještě nějaký znak
        {
            if(rest[0] != ' ')//Pokud se nejedná o mezeru, jedná se o neznámí příkaz
            {
                msg = "Neznami prikaz";
                respond = this->respondERR(msg, sock);
                continue;
            }

            rest.erase(0, 1);
        }

        if(rest.size() != 0) //Pokud se tam nenacházela jen mezera, klient zadal k příkazu i argument.
            param = true;


        switch(this->stringToCommand(command)) //Zjištění zadaného příkazu
        {
            case LIST:
                if(param) //Byl zadán argument?
                    this->listFunc(rest, sock);

                else
                    this->listFunc(sock);

                break;

            case STAT:
                this->statFunc(sock);
                break;

            case RETR:
                if(!param) //Příkaz má povinný argument, který nebyl zadán
                {
                    msg = "Chybi parametr u prikazu";
                    this->respondERR(msg, sock);
                }

                else
                    this->retr(rest, sock);

                break;

            case DELE:
                if(!param) //Příkaz má povinný argument, který nebyl zadán
                {
                    msg = "Chybi parametr u prikazu";
                    this->respondERR(msg, sock);
                }

                else
                    this->dele(rest, sock);

                break;

            case RSET:
                this->rset(sock);
                break;

            case NOOP:
                respond = this->respondOK("", sock);
                break;

            case UIDL:
                if(param) //Byl zadán argument?
                    this->uidl(rest, sock);

                else
                    this->uidl(sock);

                break;

            case QUIT:
                this->update(sock);
                return;

            case UNKNOWN:
                msg = "Zadal jsi neznami prikaz";
                respond = this->respondERR(msg, sock);
                break;
        }
    }
}

void Server::update(int sock)
{
    /*
        Funkce, která reprezentuje stav UPDATE u POP3 Serveru.
        Tento stav je stav těsně před ukončením spojení s klientem.
        V tomto stavu se smažou všechny zprávy označené ke smazání příkazem DELE.
    */

    string msg;
    string path;
    int num;
    bool removedAll = true;

    //Projedeme seznam smazaných mailů
    for(unsigned int i = 0; i < this->delMark.size(); i++)
    {
        path = this->arg->getMailDirPath();
        path.append("/cur/");
        num = this->delMark[i];
        path.append(this->mails[num-1]);

        if(remove(path.c_str()) != 0)
            removedAll = false;
    }

    if(removedAll) //Všechny emaily se v pořádku smazali
    {
        msg = "odpojuji klienta";
        this->respondOK(msg, sock);
    }

    else //Při mazání došlo k nějaké chybě.
    {
        msg = "nektere zpravy nebyli smazany";
        this->respondERR(msg, sock);
    }
}

string Server::getTimeStamp()
{
    /*
        Funkce, která vygeneruje timestamp pro příkaz APOP a uvítací zprávu při připojení klienta.
        Timestamp se skládá z čísla procesu, UNIXového času a název zařízení, na kterém server běží.
    */

    string msg = "<";
    char hostname[BUFFER];

    msg.append(to_string(getpid()));
    msg.append(".");
    msg.append(to_string(time(NULL)));
    msg.append("@");
    gethostname(hostname, BUFFER);
    msg.append(hostname);
    msg.append(">");

    this->usr->encryptPassword(msg);

    return msg;
}

bool Server::respondOK(string msg, int sock)
{
    /*
        Funkce, která uživateli zašle OK zprávu.
    */

    ssize_t snd;
    msg = "+OK " + msg + "\r\n";

    if((snd = send(sock, msg.data(), msg.size(), 0)) < 0)
        return false;

    return true;
}

bool Server::respondERR(string msg, int sock)
{
    /*
        Funkce, která uživateli zašle ERR zprávu.
    */

    ssize_t snd;
    msg = "-ERR " + msg + "\r\n";

    if((snd = send(sock, msg.data(), msg.size(), 0)) < 0)
        return false;

    return true;
}

void Server::respondEnd(int sock)
{
    /*
        Funkce, která uživateli zašle ukončující oktet.
        Tento oktet se uživateli zasílá, pokud se ukončilo posílání multi-line odpovědi.
    */

    string msg = ".\r\n";

    send(sock, msg.data(), msg.size(), 0);
}

void Server::quit(int sock)
{
    /*
        Funkce, která ukončí spojení s klientem.
        Funkce uzavře komunikační sokcet a odemče zámek pro dalšího klienta.
    */

    this->mails.clear();
    this->delMark.clear();
    close(sock);
    this->mtx.unlock();
    this->msgNum = 0;

    return;
}

commandNumber Server::stringToCommand(string com)
{
    /*
        Funkce, která převede string na mnou definovaný enum typ commandNumber, který značí zadaný příkaz.
    */

    if (com == "list")
        return LIST;

    else if (com == "stat")
        return STAT;

    else if (com == "retr")
        return RETR;

    else if (com == "dele")
            return DELE;

    else if (com == "rset")
        return RSET;

    else if (com == "noop")
        return NOOP;

    else if (com == "uidl")
        return UIDL;

    else if (com == "quit")
        return QUIT;

    else
        return UNKNOWN;
}

bool Server::mailDirControl()
{
    /*
        Kontrola struktury maildiru.
        Maildir musí obsahovat pdosložky cur a new, tmp v tomto případě není potřeba.
    */

    DIR* dirp;
    string folder;

    dirp = opendir(this->arg->getMailDirPath().c_str());

    if(dirp == NULL)
        return false;

    closedir(dirp);

    folder = this->arg->getMailDirPath() + "/cur";

    dirp = opendir(folder.c_str());

    if(dirp == NULL)
        return false;

    closedir(dirp);

    folder = this->arg->getMailDirPath() + "/new";

    dirp = opendir(folder.c_str());

    if(dirp == NULL)
        return false;

    closedir(dirp);

    return true;
}

void Server::newToCur()
{
    /*
        Funkce, která přenuse soubory z posložky Maildiru new do podsložky cur.
        Funkce také vytvoří pomocný soubor, pro informace, které si musíme uchovávat.
    */

    DIR* dirp;
    string folder;
    struct dirent* ent;
    struct stat st;
    int type;
    int oktet;
    string path;
    string from;
    string mail;
    ifstream fileIn;
    ofstream fileOut;
    string line;
    string help;
    string pom;
    string dir;
    size_t fileHash;

    folder = this->arg->getMailDirPath() + "/new/";

    dirp = opendir(folder.c_str());
    pom = this->arg->getBinPath();
    pom.append("pom.pop");
    fileOut.open(pom.c_str(), ios::app);

    while((ent = readdir(dirp)) != NULL) //Prohledáváme celou složku new.
    {
        type = (int)ent->d_type;
        oktet = 0;

        if(type == 8) //Type 8 značí, že se jedná o soubor, které nás jako jedinné zajímají
        {
            path = this->arg->getMailDirPath() + "/cur/" + ent->d_name;
            from = folder + ent->d_name;
            rename(from.c_str(), path.c_str());
            fileIn.open(path.c_str());

            while(getline(fileIn, line)) //Projíždíme soubor řádek po řádku
            {
                if(line.back() != '\r') //Pokud řádek končí jen LF, místo CRLF, je potřeba počítat s bajtem navíc.
                    oktet++;
            }

            fileIn.clear();

            //Kontrola, zda soubor končí koncem řádku
            fileIn.seekg(0, ios::end);
            int pos = fileIn.tellg();
            fileIn.seekg(pos-1, ios::beg);
            char lastChar = fileIn.get();

            if(lastChar != '\n') //Pokud ne, opět musíme přidat bajt
                oktet++;

            help = ent->d_name;
            fileHash = hash<string>()(help);
            help.append("\n");
            help.append(to_string(oktet));
            help.append("\n");
            help.append(to_string(fileHash));
            help.append("\n");

            fileOut.write(help.c_str(), help.size());
            fileIn.close();
        }
    }

    closedir(dirp);

    folder = this->arg->getMailDirPath() + "/cur/";

    dirp = opendir(folder.c_str());


    while((ent = readdir(dirp)) != NULL) //Po přesunu nových mailů projedeme i složku cur
    {
        type = (int)ent->d_type;

        if(type == 8) //U souborů si uložíme některé důležité informace
        {
            this->msgNum++;
            this->mails.reserve(this->msgNum);
            mail = ent->d_name;
            this->mails.push_back(mail);
            mail = folder + mail;
            stat(mail.c_str(), &st);
            this->bytes += st.st_size;
        }
    }

    closedir(dirp);

    fileOut.close();
    pom = this->arg->getBinPath();
    pom.append("dir.pop");
    fileOut.open(pom.c_str());

    path = this->arg->getMailDirPath();
    if(path[0] == '.')
    {
        path.erase(0, 1);
        help = getenv("PWD");
        help.append(path);
        path = help;
    }

    else if(path.find("/") == string::npos)
    {
        help = getenv("PWD");
        help.append("/");
        help.append(path);
        path = help;
    }

    fileOut.write(path.c_str(), path.size());
    fileOut.close();
}

void Server::curToNew()
{
    /*
        Funkce, která přenuse soubory z podsložky Maildiru cur do podsložky new.
        Tato funkce je využívána u resetu.
    */

    DIR* dirp;
    string folder;
    struct dirent* ent;
    int type;
    string path;
    string from;

    folder = this->arg->getMailDirPath() + "/cur/";

    dirp = opendir(folder.c_str());

    while((ent = readdir(dirp)) != NULL) //Prohledáváme celou složku cur.
    {
        type = (int)ent->d_type;
        if(type == 8) //Soubory přesouváme do new
        {
            path = this->arg->getMailDirPath() + "/new/" + ent->d_name;
            from = folder + ent->d_name;
            rename(from.c_str(), path.c_str());
        }
    }
}

void Server::listFunc(string param, int sock)
{
    /*
        Funkce, která reprezentuje příkaz LIST, konkrétně příkaz LIST s argumentem.
        Tento příkaz klientovi vypíše velikost mailu zadaného v argumentu param.
    */

    char* znak;
    int number;
    int oktet;
    string msg;
    string path;
    struct stat st;
    ifstream file;
    string line;
    string pom;

    number = strtol(param.c_str(), &znak, SOUSTAVA);

    if(*znak != '\0') //Kontrola, zda je zadaný argument číslo.
    {
        msg = "zadan neciselny argument!";
        this->respondERR(msg, sock);
        return;
    }

    if(number > this->msgNum || number < 1) //Kontrola, zda je není číslo větší, než počet mailů, popř. menší jak 1.
    {
        msg = "neexistujici zprava";
        this->respondERR(msg, sock);
        return;
    }

    //Kontrola, zda se mail nenachází mezi smazanýma.
    if(find(this->delMark.begin(), this->delMark.end(), number) != this->delMark.end())
    {
        msg = "zprava cislo ";
        msg.append(to_string(number));
        msg.append(" byla smazana");
        this->respondERR(msg, sock);
        return;
    }

    path = this->arg->getMailDirPath() + "/cur/";
    path.append(this->mails[number-1]);
    stat(path.c_str(), &st);
    oktet = st.st_size;

    pom = this->arg->getBinPath();
    pom.append("pom.pop");
    file.open(pom.c_str(), ios::app);

    while(getline(file, line)) //Z pomocného souboru zjistíme, počet přidávaných bajtů pro tento mail.
    {
        if(!(line.compare(this->mails[number-1])))
        {
            getline(file, line);
            oktet += strtol(line.c_str(), &znak, SOUSTAVA);
            break;
        }
    }

    file.close();

    msg = to_string(number);
    msg.append(" ");
    msg.append(to_string(oktet));
    this->respondOK(msg, sock);
}

void Server::listFunc(int sock)
{
    /*
        Funkce, která reprezentuje příkaz LIST, konkrétně příkaz LIST bez argumentu.
        Tento příkaz klientovi vypíše velikost všech mailů ve složce cur.
    */

    int oktet = 0;
    int number;
    string msg;
    string path;
    struct stat st;
    ifstream file;
    string line;
    char* znak;
    string pom;

    number = this->msgNum - this->delMark.size();

    if(number == 0) //Uživatel nemá žádnou nesmazanou zprávu
    {
        msg = "0 zprav v maildropu";
        this->respondOK(msg, sock);
        this->respondEnd(sock);
    }

    msg = to_string(number);
    msg.append(" zpravy ");
    this->respondOK(msg, sock);

    for(unsigned int i = 0; i < this->mails.size(); i++) //Projedeme všechny maily
    {
        if(find(this->delMark.begin(), this->delMark.end(), i+1) != this->delMark.end()) //Smazaný mail nečteme
            continue;

        pom = this->arg->getBinPath();
        pom.append("pom.pop");
        file.open(pom.c_str(), ios::app);
        int cr;

        path = this->arg->getMailDirPath() + "/cur/" + this->mails[i];
        stat(path.c_str(), &st);
        oktet += st.st_size;

        while(getline(file, line))
        {
            if(!(line.compare(this->mails[i])))
            {
                getline(file, line);
                cr = strtol(line.c_str(), &znak, SOUSTAVA);
                break;
            }
        }

        msg = to_string(i+1);
        msg.append(" ");
        msg.append(to_string(st.st_size + cr));
        msg.append("\r\n");
        send(sock, msg.c_str(), msg.size(), 0);

        file.close();
    }

    this->respondEnd(sock);
}

void Server::statFunc(int sock)
{
    /*
        Funkce, která reprezentuje příkaz STAT.
        Tento příkaz klientovi vypíše celkovou velikost mailů ve složce cur.
    */

    int number = 0;
    int oktet = 0;
    char* znak;
    string line;
    string path;
    string msg;
    ifstream file;
    struct stat st;
    string pom;

    for(unsigned int i = 0; i < this->mails.size(); i++) //Projedeme všechny maily
    {
        if(find(this->delMark.begin(), this->delMark.end(), i+1) != this->delMark.end()) //Vynecháme smazané maily
            continue;

        number++;
        path = this->arg->getMailDirPath() + "/cur/" + this->mails[i];
        stat(path.c_str(), &st);
        oktet += st.st_size;

        pom = this->arg->getBinPath();
        pom.append("pom.pop");
        file.open(pom.c_str(), ios::app);

        while(getline(file, line))
        {
            if(!(line.compare(this->mails[i])))
            {
                getline(file, line);
                oktet += strtol(line.c_str(), &znak, SOUSTAVA);
                break;
            }
        }

        file.close();
    }

    msg = to_string(number);
    msg.append(" ");
    msg.append(to_string(oktet));
    this->respondOK(msg, sock);
}

void Server::uidl(int sock)
{
    /*
        Funkce, která reprezentuje příkaz UIDL, konkrétně příkaz UIDL bez argumentu.
        Tento příkaz klientovi vypíše pro každý mail jeho unikátní identifikační číslo.
    */

    int number;
    string msg;
    ifstream file;
    string line;
    string pom;

    number = this->msgNum - this->delMark.size();

    if(number == 0) //Nezbývá žádná nesmazaná zpráva
    {
        msg = "0 zprav v maildropu";
        this->respondOK(msg, sock);
        return;
    }

    msg = to_string(number);
    msg.append(" zpravy");
    this->respondOK(msg, sock);

    for(unsigned int i = 0; i < this->mails.size(); i++) //Postupně projedeme všechny maily
    {
        pom = this->arg->getBinPath();
        pom.append("pom.pop");
        file.open(pom.c_str(), ios::app);

        if(find(this->delMark.begin(), this->delMark.end(), i+1) != this->delMark.end()) //Vynecháme smazané maily
            continue;

        while(getline(file, line))
        {
            if(!(line.compare(this->mails[i])))
            {
                getline(file, line);
                getline(file, line);
                break;
            }
        }

        msg = to_string(i+1);
        msg.append(" ");
        msg.append(line);
        msg.append("\r\n");
        send(sock, msg.c_str(), msg.size(), 0);

        file.close();
    }

    this->respondEnd(sock);
}

void Server::uidl(string param, int sock)
{
    /*
        Funkce, která reprezentuje příkaz UIDL, konkrétně příkaz UIDL s argumentem.
        Tento příkaz klientovi vypíše pro zadaný mail jeho unikátní identifikační číslo.
    */

    char* znak;
    int number;
    string msg;
    string mail;
    ifstream file;
    string line;
    string pom;

    number = strtol(param.c_str(), &znak, SOUSTAVA);

    if(*znak != '\0') //Kontrola správnosti argumentu
    {
        msg = "zadan neciselny argument";
        this->respondERR(msg, sock);
        return;
    }

    if(number > this->msgNum || number < 1) //Kontrola, zda je číslo mailu v platném rozsahu
    {
        msg = "neexistujici zprava";
        this->respondERR(msg, sock);
        return;
    }

    //Kontrola zda zadaný mail nebyl označen ke smazání
    if(find(this->delMark.begin(), this->delMark.end(), number) != this->delMark.end())
    {
        msg = "zprava cislo ";
        msg.append(to_string(number));
        msg.append(" byla smazana");
        this->respondERR(msg, sock);
        return;
    }

    pom = this->arg->getBinPath();
    pom.append("pom.pop");
    file.open(pom.c_str(), ios::app);
    mail = this->mails[number-1];

    while(getline(file, line))
    {
        if(!(line.compare(mail)))
        {
            getline(file, line);
            getline(file, line);
            break;
        }
    }

    file.close();

    msg = to_string(number);
    msg.append(" ");
    msg.append(line);
    this->respondOK(msg, sock);
}

void Server::retr(string param, int sock)
{
    /*
        Funkce, která reprezentuje příkaz RETR.
        Tento příkaz klientovi pošle obsah zadaného mailu.
    */

    char* znak;
    int number;
    string msg;
    string path;
    string line;
    struct stat st;
    int oktet;
    ifstream file;
    ifstream pomFile;
    string pom;
    string pom1;

    number = strtol(param.c_str(), &znak, SOUSTAVA);

    if(*znak != '\0') //Kontrola, zda byl zadán platný argument
    {
        msg = "zadan neciselny argument";
        this->respondERR(msg, sock);
        return;
    }

    if(number > this->msgNum || number < 1) //Kontrola, zda je zadaný argument v platném rozsahu
    {
        msg = "neexistujici zprava";
        this->respondERR(msg, sock);
        return;
    }

    //Kontrola, zda nebyl zadaný mail označen ke smazání
    if(find(this->delMark.begin(), this->delMark.end(), number) != this->delMark.end())
    {
        msg = "zprava cislo ";
        msg.append(to_string(number));
        msg.append(" byla smazana");
        this->respondERR(msg, sock);
        return;
    }

    path = this->arg->getMailDirPath() + "/cur/";
    path.append(this->mails[number-1]);
    stat(path.c_str(), &st);
    oktet = st.st_size;

    pom1 = this->arg->getBinPath();
    pom1.append("pom.pop");
    pomFile.open(pom1.c_str(), ios::app);

    while(getline(pomFile, pom))
    {
        if(!(pom.compare(this->mails[number-1])))
        {
            getline(pomFile, pom);
            oktet += strtol(pom.c_str(), &znak, SOUSTAVA);
            break;
        }
    }

    msg = to_string(number);
    msg.append(" ");
    msg.append(to_string(oktet));
    this->respondOK(msg, sock);

    file.open(path.c_str());

    while(getline(file, line))
    {
        if(line.back() == '\r')
            line.append("\n");

        else
            line.append("\r\n");

        if(line[0] == '.')
            line.insert(0, ".");

        send(sock, line.c_str(), line.size(), 0);
    }

    this->respondEnd(sock);
    file.close();
}

void Server::dele(string param, int sock)
{
    /*
        Funkce, která reprezentuje příkaz DELE.
        Tento příkaz označí mail, zadaný uživatelem, ke smazání.
        Email však bude smazán až ve stavu UPDATE.
    */

    char* znak;
    int number;
    string msg;

    number = strtol(param.c_str(), &znak, SOUSTAVA);

    if(*znak != '\0') //Kontrolo, zda uživatel zadal platný argument
    {
        msg = "zadan neciselny argument";
        this->respondERR(msg, sock);
        return;
    }

    if(number > this->msgNum || number < 1) //Kontrola, zda je argument v platném rozsahu
    {
        msg = "neexistujici zprava";
        this->respondERR(msg, sock);
        return;
    }

    //Kontrola, zda klient nezadal mail, který již byl ke smazání označen.
    if(find(this->delMark.begin(), this->delMark.end(), number) != this->delMark.end())
    {
        msg = "zprava cislo ";
        msg.append(to_string(number));
        msg.append(" jiz byla smazana");
        this->respondERR(msg, sock);
        return;
    }

    this->delMark.reserve(this->delMark.size() + 1);
    this->delMark.push_back(number);

    msg = "zprava cislo ";
    msg.append(to_string(number));
    msg.append(" smazana");
    this->respondOK(msg, sock);
}

void Server::rset(int sock)
{
    /*
        Funkce, která reprezentuje příkaz RSET.
        Tento příkaz všechny zprávy, které byli označené ke smazání, opět odoznačí.
    */

    string msg;

    this->delMark.clear();
    this->delMark.reserve(0);

    msg = to_string(this->msgNum);
    msg.append(" zprav v maildropu");
    this->respondOK(msg, sock);
}

bool Server::reset()
{
    /*
        Funkce, která slouží k restartování serveru po práci s naším programem.
        Funkce všechny nesmazené emaily opět přesune ze složky cur do složky new.
        Funkce také smaže všechny vytvořené pomocné soubory.
    */

    ifstream file;
    string line;
    string pom;
    string dir;

    pom = this->arg->getBinPath();
    pom.append("pom.pop");
    remove(pom.c_str());

    if(this->arg->getMailDirPath().size() == 0)
    {
        pom = this->arg->getBinPath();
        pom.append("dir.pop");
        file.open(pom.c_str());
        getline(file, line);
        this->arg->setMailDirPath(line);
        file.close();
    }

    if(!(this->mailDirControl()))
    {
        this->err.printErr(SPATNY_ARGUMENT);
        return false;
    }

    this->curToNew();
    remove(dir.c_str());

    return true;
}
