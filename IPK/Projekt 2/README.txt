Program: Pøenos Souborù
Autor: Roman Nahálka, xnahal01@stud.fit.vutbr.cz
Datum vytvoøení: 21.4.2016

Popis: 	Program umožòuje pøenos souborù mezi klientem a serverem. Pro funkènost programu se nejdøíve na cílovém poèítaèi
	spustí server a na druhém místì program client. Podle zadaných argumentù programu bude klient na server soubor
	nahrávat nebo ze serveru soubor stahovat. Klient musí zadávat soubory po jednom, server je však schopen obsloužit
	nìkolik klientù zároveò.

Spuštení: Klient: client -h jmeno_ciloveho_pocitace -p cislo_portu -[d|u] nazev_souboru
          Server: server -p cislo_portu

Vstup: 	Èíslo portu musí být v rozmezí 1024 - 65534. Jméno cílového poèítaèe musí být skuteèná adresa serveru. A název souboru
	existující soubor nacházející se buï na serveru nebo u klienta, podle toho, kterou operaci chce uživatel provádìt.
	V pøípadì špatných vstupních dat se vypíše chyba na standartní chybový výstup serveru nebo klienta a klient ukonèí svou
	èinnost.

Výstup:	Program pøenese soubor mezi cílovým a klientovým poèítaèem. V pøípadì chyby se na standartní chybový výstup vytiskne
	patøièný chybová hláška. Server na standartní výstup tiskne informace o pøijetí požadavku na nahrání nebo stáhnutí
	souboru a informuje také o dokonèení pøenosu. 