/*
 * Soubor:  proj2.c
 * Datum:   2016/05/02
 * Autor:   Roman Nahalka, xnahal01@stud.fit.vubr.cz
 * Projekt: Roller Coaster, projekt c. 2 pro predmet IOS
 */

//Knihovny
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>

//definice promennych
#define POCET_ARGUMENTU 5 //Pevny pocet argumentu
#define SOUBOR "proj2.out" //Soubor, do ktereho se bude ukladat vystup
#define SOUSTAVA 10 //Ciselna soustava pro funkci strtol
#define MAX_CAS 5001 //Maximalni mozny cas uspani procesu

typedef enum chyba //Vycet chyb
{
    SPATNY_POCET_ARGUMENTU,
    SPATNY_ARGUMENT,
    NELZE_OTEVRIT_SOUBOR,
    SEMAFOR_CHYBA_VYTVARENI,
    PAMET_CHYBA,
    KLIC_CHYBA,
    PROCES_CHYBA
} cislo_chyby;

typedef struct parametry //Struktura, ve ktere budou ulozeny vsechny argumenty
{
    int pasazeri; //P
    int kapacita; //C
    int novyPasazer; //PT
    int dobaPrujezdu; //RT
} type_parametry;

typedef struct semafory //Struktura, ve ktere budou olozeny vsechny semafory
{
   sem_t mutex; //Zapis do pameti
   sem_t mutex1; //Cekani na dokonceni vsech child procesu
   sem_t nalodovani; //Vozik je pripraven na nalodovani
   sem_t vylodovani; //Vozik je pripraven na vylodovani
   sem_t vsichniNalodeni; //Vsichni se uz nalodili
   sem_t vsichniVylodeni; //Vsichni se uy vylodili
   sem_t hotovo; //Proces car skoncil a vsechny procesy se mohou ukoncit
} type_semafory;

typedef struct pamet //Struktura sdilene pameti
{
   int vylodeno; //Pocet vylodenych pasazeru
   int nalodeno; //Pocet nalodenych pasazeru
   int akce; //Citac akci
   int pasazer; //Citac pasazeru
} type_pamet;

//Deklarace funkci
int kontrola_argumentu(int argc, char *argv[], type_parametry *par);
void car(type_parametry *param, type_pamet *pam, type_semafory *sem, FILE *soubor);
void passanger(type_parametry *param, type_pamet *pam, type_semafory *sem, FILE *soubor);
void uvolneni(type_semafory *sem, type_pamet *pam, int shmID, int semID);
void error(cislo_chyby n);

int main(int argc, char *argv[]) //Hlavni funkce programu
{
    type_parametry param; //Vytvoreni struktury s parametry
    type_semafory *sem; //Vytvoreni struktury se semafory
    type_pamet *pam; //Vytvoreni struktury se spolecnymi promennymi
    int shmID, semID; //ID sdilene pameti
    pid_t pasPID, carPID; //PID procesu
    key_t klic1, klic2; //Klice pro sdilenou pamet
    FILE *soubor; //Soubor

    if(kontrola_argumentu(argc, argv, &param) == 0) //Zkontrolujeme zadana parametry
        return 1;

    //Nastaveni sdilene pameti
    klic1 = ftok("proj2", 1);
    if(klic1 == -1)
    {
        error(KLIC_CHYBA);
        return 2;
    }

    //Sdilena pamet promennych
    shmID = shmget(klic1, sizeof(type_pamet), IPC_CREAT | 0666);
    if(shmID < 0)
    {
        error(PAMET_CHYBA);
        return 2;
    }

    pam = (type_pamet*)shmat(shmID, NULL, 0);
    if(pam == (void *) -1)
    {
        uvolneni(NULL, pam, shmID, 0);
        error(PAMET_CHYBA);
        return 2;
    }

    pam->nalodeno = 0;
    pam->vylodeno = 0;
    pam->akce = 1;
    pam->pasazer = 1;

    klic2 = ftok("proj2", 2);
    if(klic2 == -1)
    {
        uvolneni(NULL, pam, shmID, 0);
        error(KLIC_CHYBA);
        return 2;
    }

    //Sdilena pamet semaforu
    semID = shmget(klic2, sizeof(type_semafory), IPC_CREAT | 0666);
    if(semID < 0)
    {
        uvolneni(NULL, pam, shmID, 0);
        error(PAMET_CHYBA);
        return 2;
    }

    sem = (type_semafory*)shmat(semID, NULL, 0);
    if(sem == (void*)-1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(PAMET_CHYBA);
        return 2;
    }

    //Inicializace jednotlivych semaforu
    if(sem_init(&sem->mutex, 1, 1) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }
    if(sem_init(&sem->mutex1, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }
    if(sem_init(&sem->nalodovani, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }
    if(sem_init(&sem->vylodovani, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }
    if(sem_init(&sem->vsichniNalodeni, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }
    if(sem_init(&sem->vsichniVylodeni, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }

    if(sem_init(&sem->hotovo, 1, 0) == -1)
    {
        uvolneni(sem, pam, shmID, semID);
        error(SEMAFOR_CHYBA_VYTVARENI);
        return 2;
    }

    soubor = fopen(SOUBOR, "w"); //Otevreme soubor
    if(SOUBOR == NULL)
    {
        uvolneni(sem, pam, shmID, semID);
        error(NELZE_OTEVRIT_SOUBOR);
        return 1;
    }

    srand(time(NULL)); //Aby jsme generovali nahodna cisla

    carPID = fork(); //Vytvorime proces car
    if(carPID == 0)
    {
        car(&param, pam, sem, soubor);
        return 0;
    }
    else if(carPID < 0)
    {
        fclose(soubor);
        uvolneni(sem, pam, shmID, semID);
        error(PROCES_CHYBA);
        return 2;
    }

    //Vytvareni procesu pro vsechny pasazery
    for(int i = 0; i < param.pasazeri; i++)
    {
        if(param.novyPasazer > 0)
            usleep(rand() % (param.novyPasazer * 1000));
        pasPID = fork();
        if(pasPID == 0)
        {
            passanger(&param, pam, sem, soubor);
            return 0;
        }
        else if(pasPID < 0)
        {
            error(PROCES_CHYBA);
            fclose(soubor);
            break;
        }
    }

    sem_wait(&sem->mutex1); //Cekame dokud vsechny procesy neskonci
    fclose(soubor); //Uzavreme soubor
    uvolneni(sem, pam, shmID, semID); //Uvolnime pamet

    return 0;
 }

int kontrola_argumentu(int argc, char *argv[], type_parametry *par)
{
    /*
        Funkce zkontroluje, jestli byli vsechny argumenty zadany ve spravnem formatu.
        Funkce dale zadane argumenty ulozi do struktury par, aby jsme s nimi mohli nadale pracovat.
        V pripade chybi fraci funkce hodnotu 0, v pripade uspechu bude navratova hodnota 1.
    */

    char *znak;
    int argument;

    if (argc != POCET_ARGUMENTU)
    {
        error(SPATNY_POCET_ARGUMENTU);
        return 0;
    }

    for(int i=1; i<POCET_ARGUMENTU; i++) //Cyklus na projizdeni argumentu
    {
        argument = strtol(argv[i], &znak, SOUSTAVA); //V cyklu, aby jsme to nemuseli pro kazdy argument psat zvlast
        if(*znak != '\0') //Kontrola, ze bylo zadana ciste jen cislo
        {
            error(SPATNY_ARGUMENT);
            return 0;
        }
        else //Nacteni jedtnotlivych argumentu
        {
            if(i == 1)
                par->pasazeri = argument;
            else if (i == 2)
                par->kapacita = argument;
            else if (i == 3)
                par->novyPasazer = argument;
            else if (i == 4)
                par->dobaPrujezdu = argument;
        }
    }

    if(par->pasazeri <=0 || par->kapacita <=0 || par->kapacita >= par->pasazeri || (par->pasazeri % par->kapacita)) //Kontrola hodnou prvnich dvou argumentu
    {
        error(SPATNY_ARGUMENT);
        return 0;
    }

    if(par->novyPasazer < 0 || par->novyPasazer >= MAX_CAS || par->dobaPrujezdu < 0 || par->dobaPrujezdu >= MAX_CAS) //Kontrola hodnot zbyvajich argumentu
    {
        error(SPATNY_ARGUMENT);
        return 0;
    }

    return 1;
}

void car(type_parametry *param, type_pamet *pam, type_semafory *sem, FILE *soubor)
{
    int pocetCyklu = param->pasazeri / param->kapacita;

    srand(time(NULL));
    sem_wait(&sem->mutex);
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: C 1: started\n", pam->akce);
    pam->akce++;

    for(int i = 0; i < pocetCyklu; i++)
    {
        sem_post(&sem->nalodovani);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: C 1: load\n", pam->akce);
        pam->akce++;
        sem_post(&sem->mutex);
        sem_wait(&sem->vsichniNalodeni);
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: C 1: run\n", pam->akce);
        pam->akce++;
        sem_post(&sem->mutex);
        if(param->dobaPrujezdu > 0)
            usleep(rand() % (param->dobaPrujezdu * 1000));
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: C 1: unload\n", pam->akce);
        pam->akce++;
        sem_post(&sem->mutex);
        sem_post(&sem->vylodovani);
        sem_wait(&sem->vsichniVylodeni);
    }
    sem_post(&sem->hotovo);
    sem_wait(&sem->mutex);
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: C 1: finished\n", pam->akce);
    pam->akce++;
    sem_post(&sem->mutex);
    sem_post(&sem->mutex1);
}

void passanger(type_parametry *param, type_pamet *pam, type_semafory *sem, FILE *soubor)
{
    int pasazer;

    sem_wait(&sem->mutex);
    pasazer = pam->pasazer;
    pam->pasazer++;

    srand(time(NULL));
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: P %d: started\n", pam->akce, pasazer);
    pam->akce++;
    sem_post(&sem->mutex);
    sem_wait(&sem->nalodovani);
    sem_wait(&sem->mutex);
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: P %d: board\n", pam->akce, pasazer);
    pam->akce++;
    pam->nalodeno++;
    sem_post(&sem->mutex);
    if(pam->nalodeno < param->kapacita)
    {
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: P %d: board order %d\n", pam->akce, pasazer, pam->nalodeno);
        pam->akce++;
        sem_post(&sem->mutex);
        sem_post(&sem->nalodovani);
    }
    else
    {
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: P %d: board last\n", pam->akce, pasazer);
        pam->akce++;
        pam->nalodeno = 0;
        sem_post(&sem->mutex);
        sem_post(&sem->vsichniNalodeni);
    }
    sem_wait(&sem->vylodovani);
    sem_wait(&sem->mutex);
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: P %d: unboard\n", pam->akce, pasazer);
    pam->akce++;
    pam->vylodeno++;
    sem_post(&sem->mutex);
    if(pam->vylodeno < param->kapacita)
    {
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: P %d: unboard order %d\n", pam->akce, pasazer, pam->vylodeno);
        pam->akce++;
        sem_post(&sem->mutex);
        sem_post(&sem->vylodovani);
    }
    else
    {
        sem_wait(&sem->mutex);
        setbuf(soubor, NULL);
        fprintf(soubor, "%d: P %d: unboard last\n", pam->akce, pasazer);
        pam->akce++;
        pam->vylodeno = 0;
        sem_post(&sem->mutex);
        sem_post(&sem->vsichniVylodeni);
    }

    sem_wait(&sem->hotovo);
    sem_wait(&sem->mutex);
    setbuf(soubor, NULL);
    fprintf(soubor, "%d: P %d: finished\n", pam->akce, pasazer);
    pam->akce++;
    sem_post(&sem->mutex);
    sem_post(&sem->hotovo);
}

void uvolneni(type_semafory *sem, type_pamet *pam, int shmID, int semID)
{
    shmdt(sem);
    shmctl(semID, IPC_RMID, NULL);
    shmdt(pam);
    shmctl(shmID, IPC_RMID, NULL);
}

void error(cislo_chyby n)
{
    const char *chybova_hlaseni[] = //Text odpovodijaci chybovemu hlaseni
    {
       "Zadal jsi spatny pocet argumentu!\n",
       "Spatne jsi zadal jeden z argumentu!\n",
       "Nepodarilo se otevrit soubor!\n",
       "Nastala chyba pri vytvareni semaforu!\n",
       "Chyba pri vytvareni spolecne pameti!\n",
       "Nastala chyba pri ziskani klice!\n",
       "Chyba pri vytvareni procesu!\n"
    };

    fprintf(stderr, "%s", chybova_hlaseni[n]);
}
