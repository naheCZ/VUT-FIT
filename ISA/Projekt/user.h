//============================================================================
// Jmeno        : user.h
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 7.11.2017
//============================================================================

#ifndef USER_H
#define USER_H

#include <string.h>

using namespace std;

class User
{
    private:
        string username;
        string password;
        string encryptedPassword;

    public:
        User();
        string getUsername();
        void setUsername(string usr);
        string getPassword();
        void setPassword(string pass);
        void encryptPassword(string timeStamp);
        string getEncryptedPassword();
};

#endif
