//============================================================================
// Jmeno        : user.cpp
// Autor        : xnahal01, xnahal01@stud.fit.vutbr.cz
// Popis        : POP3 server pro predmet ISA FIT VUT 2017/2018
// Datum        : 7.11.2017
//============================================================================


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "user.h"
#include "md5.h"

using namespace std;

User::User()
{
    /*
        Konstruktor, pro třídu User.
    */

    this->username = "";
    this->password = "";
    this->encryptedPassword = "";
}

string User::getUsername()
{
    /*
        Funkce, která vrátí uživatelské jméno.
    */

    return this->username;
}

string User::getPassword()
{
    /*
        Funkce, která vrátí heslo.
    */

    return this->password;
}

string User::getEncryptedPassword()
{
    /*
        Funkce, která vrátí zašifrované jméno.
    */

    return this->encryptedPassword;
}

void User::encryptPassword(string timeStamp)
{
    /*
        Funkce, která zašifruje uživatelovo heslo.
    */

    string str;

    str = timeStamp + this->password;

    this->encryptedPassword = md5(str);
}

void User::setPassword(string pass)
{
    /*
        Funkce, která nastaví uživatelské heslo.
    */

    this->password = pass;
}

void User::setUsername(string usr)
{
    /*
        Funkce, která nastaví uživatelské jméno.
    */

    this->username = usr;
}
