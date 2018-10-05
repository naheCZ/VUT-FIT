//============================================================================
// Jmeno        : arguments.h
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 17.11.2017
//============================================================================

#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string.h>
#include "user.h"

#define MIN_PARAMETRU 3
#define MAX_PARAMETRU 5
#define SOUSTAVA 10

using namespace std;

class Arguments
{
    public:
        Arguments(User* usr);
        bool parseArguments(int argc, char** argv);
        void printHelp();
        int getPortNumber();
        string getFilePath();
        bool getC();
        string getMailDirPath();
        void setMailDirPath(string path);
        bool getReset();
        bool getHelp();
        bool checkValidity();
        User* getUsr();
        string getBinPath();

    private:
        User* usr;
        bool help;
        string filePath;
        bool c;
        int portNumber;
        string mailDirPath;
        string binPath;
        bool reset;
        bool checkAuthFile();
        bool checkPort();
};

#endif
