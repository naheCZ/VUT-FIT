Program: P�enos Soubor�
Autor: Roman Nah�lka, xnahal01@stud.fit.vutbr.cz
Datum vytvo�en�: 21.4.2016

Popis: 	Program umo��uje p�enos soubor� mezi klientem a serverem. Pro funk�nost programu se nejd��ve na c�lov�m po��ta�i
	spust� server a na druh�m m�st� program client. Podle zadan�ch argument� programu bude klient na server soubor
	nahr�vat nebo ze serveru soubor stahovat. Klient mus� zad�vat soubory po jednom, server je v�ak schopen obslou�it
	n�kolik klient� z�rove�.

Spu�ten�: Klient: client -h jmeno_ciloveho_pocitace -p cislo_portu -[d|u] nazev_souboru
          Server: server -p cislo_portu

Vstup: 	��slo portu mus� b�t v rozmez� 1024 - 65534. Jm�no c�lov�ho po��ta�e mus� b�t skute�n� adresa serveru. A n�zev souboru
	existuj�c� soubor nach�zej�c� se bu� na serveru nebo u klienta, podle toho, kterou operaci chce u�ivatel prov�d�t.
	V p��pad� �patn�ch vstupn�ch dat se vyp�e chyba na standartn� chybov� v�stup serveru nebo klienta a klient ukon�� svou
	�innost.

V�stup:	Program p�enese soubor mezi c�lov�m a klientov�m po��ta�em. V p��pad� chyby se na standartn� chybov� v�stup vytiskne
	pat�i�n� chybov� hl�ka. Server na standartn� v�stup tiskne informace o p�ijet� po�adavku na nahr�n� nebo st�hnut�
	souboru a informuje tak� o dokon�en� p�enosu. 