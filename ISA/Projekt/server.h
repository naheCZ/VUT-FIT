//============================================================================
// Jmeno        : server.h
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 17.11.2017
//============================================================================

#ifndef SERVER_H
#define SERVER_H

#include <string.h>
#include <mutex>
#include <thread>
#include <vector>
#include "arguments.h"
#include "error.h"

#define BUFFER 512
#define CAS 600

typedef enum commands
{
    LIST,
    STAT,
    RETR,
    DELE,
    RSET,
    NOOP,
    UIDL,
    QUIT,
    UNKNOWN
} commandNumber;

class Server
{
    public:
        Server(Arguments* arg);
        Arguments* getArguments();
        void setArguments(Arguments arg);
        bool connection();
        bool reset();

    private:
        Arguments* arg;
        Error err;
        thread thr;
        User* usr;
        mutex mtx;
        vector <string> mails;
        int msgNum;
        int bytes;
        vector <int> delMark;
        void authorization(int sock);
        void transaction(int sock);
        void update(int sock);
        string getTimeStamp();
        bool respondOK(string msg, int sock);
        bool respondERR(string msg, int sock);
        void respondEnd(int sock);
        void quit(int sock);
        commandNumber stringToCommand(string com);
        void listFunc(int sock);
        void listFunc(string param, int sock);
        void statFunc(int sock);
        void retr(string param, int sock);
        void dele(string param, int sock);
        void rset(int sock);
        void uidl(int sock);
        void uidl(string param, int sock);
        bool mailDirControl();
        void curToNew();
        void newToCur();
};

#endif
