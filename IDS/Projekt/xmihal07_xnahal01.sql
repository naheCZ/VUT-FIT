-- Projekt do predmetu IDS
-- Model restaurace
-- Autori:  Roman Nahálka, xnahal01@stud.fit.vutbr.cz
--          Martin Mihál, xmihal07@stud.fit.vutbr.cz
-- 27.4.2017

--Odstraneni jiz vztvorenych tabulek
DROP TABLE stoly CASCADE CONSTRAINTS;
DROP TABLE zakaznik CASCADE CONSTRAINT;
DROP TABLE rezervace CASCADE CONSTRAINT;
DROP TABLE zamestnanec CASCADE CONSTRAINT;
DROP TABLE objednavka CASCADE CONSTRAINT;
DROP TABLE pokrm CASCADE CONSTRAINT;
DROP TABLE suroviny CASCADE CONSTRAINT;
DROP TABLE pocet_rezervace CASCADE CONSTRAINT;
DROP TABLE pocet_objednavka CASCADE CONSTRAINT;
DROP SEQUENCE rezervace_seq;
DROP SEQUENCE objednavka_seq;

-- Vytvoreni tabulky pro stoly
CREATE TABLE stoly(
  id_stolu NUMBER NOT NULL,
  pocet_mist NUMBER NOT NULL,
  mistnost NUMBER NOT NULL
  );
  
--Vytvoreni tabulky pro zakaznika
CREATE TABLE zakaznik(
  id_zakaznik NUMBER NOT NULL,
  jmeno VARCHAR(255) NOT NULL,
  prijmeni VARCHAR(255) NOT NULL,
  telefon VARCHAR(20) NOT NULL,
  email VARCHAR(255) NOT NULL,
  id_rezervace NUMBER NOT NULL
  );
  
--Vytvoreni tabulky pro rezervace
CREATE TABLE rezervace(
  id_rezervace NUMBER NOT NULL,
  cas VARCHAR(8) NOT NULL,
  datum DATE NOT NULL,
  nazev VARCHAR(255) NOT NULL,
  pocet_osob NUMBER NOT NULL,
  id_stolu NUMBER NOT NULL,
  id_zamestnance NUMBER NOT NULL
  );
  
--Vytvoreni tabulky pro zamestnance
CREATE TABLE zamestnanec(
  id_zamestnance NUMBER NOT NULL,
  titul VARCHAR(20),
  jmeno VARCHAR(255) NOT NULL,
  prijmeni VARCHAR(255) NOT NULL,
  adresa VARCHAR(255) NOT NULL,
  telefon VARCHAR(20) NOT NULL,
  pohlavi VARCHAR(5) NOT NULL,
  rodne_cislo VARCHAR(10) NOT NULL
  );
  
--Vytvoreni tabulky pro objednavky
CREATE TABLE objednavka(
  id_objednavky NUMBER NOT NULL,
  cas VARCHAR(8) NOT NULL,
  id_zamestnance NUMBER NOT NULL,
  id_stolu NUMBER NOT NULL
  );
  
--Vytvoreni tabulky pro jidlo a piti
CREATE TABLE pokrm(
  id_pokrmu NUMBER NOT NULL,
  typ VARCHAR(255) NOT NULL,
  cena NUMBER NOT NULL,
  popis VARCHAR(255) NOT NULL,
  id_suroviny NUMBER NOT NULL
  );
  
--Vytvoreni tabulky pro suroviny
CREATE TABLE suroviny(
  id_suroviny NUMBER NOT NULL,
  mnozstvi NUMBER NOT NULL,
  alergeny VARCHAR(255) NOT NULL
  );
  
CREATE TABLE pocet_rezervace(
  pocet NUMBER NOT NULL,
  id_rezervace NUMBER NOT NULL,
  id_pokrmu NUMBER NOT NULL
  );
  
CREATE TABLE pocet_objednavka(
  pocet NUMBER NOT NULL,
  id_objednavka NUMBER NOT NULL,
  id_pokrmu NUMBER NOT NULL
  );

--Trigger na kontrolu rodneho cisla
SET serveroutput ON;
CREATE OR REPLACE TRIGGER triggerRod
  BEFORE INSERT OR UPDATE OF rodne_cislo ON zamestnanec
  FOR EACH ROW
DECLARE
  rod zamestnanec.rodne_cislo%TYPE;
  rok NUMBER;
  mesic NUMBER;
  den NUMBER;
  koncovka NUMBER;
  delka NUMBER;
  cislo NUMBER;
  rc BOOLEAN := FALSE;
  ecp BOOLEAN := FALSE;
  
BEGIN
  rod := :NEW.rodne_cislo;
  delka := LENGTH(rod);
  cislo := TO_NUMBER(rod);
  
  IF (delka < 9) THEN
    RAISE_APPLICATION_ERROR(-20010, 'Rodne cislo je kratsi, nez 9 znaku!');
  END IF;
  
  IF (delka > 10) THEN
    RAISE_APPLICATION_ERROR(-20020, 'Rodne cislo je delsi, nez 10 znaku!');
  END IF;
  
  rok := TO_NUMBER(SUBSTR(rod, 1, 2));
  mesic := TO_NUMBER(SUBSTR(rod, 3, 2));
  
  IF (mesic > 50) THEN
    mesic := mesic - 50;
  END IF;
  
  IF (mesic > 20) THEN
    rc := TRUE;
    mesic := mesic - 20;
  END IF;
  
  den := TO_NUMBER(SUBSTR(rod, 5, 2));
  
  IF (den > 40) THEN
    ecp := TRUE;
    den := den - 40;
  END IF;
  
  IF (rc = TRUE AND ecp = TRUE) THEN
    RAISE_APPLICATION_ERROR(-20040, 'Nevalidni RC/ECP mesic +20 a zaroven den +40!');
  END IF; 
  
  IF (delka = 9) THEN
    koncovka := TO_NUMBER(SUBSTR(rod, 7, 3));
    IF (koncovka = 0) THEN
      RAISE_APPLICATION_ERROR(-20030, 'Koncovka rodneho cisla 000 je nepripustna!');
    END IF;  
  ELSE
    koncovka := TO_NUMBER(SUBSTR(rod, 7, 4));
  END IF;
  
  IF (mesic < 1 OR mesic > 12) THEN
    RAISE_APPLICATION_ERROR(-20050, 'Hodnota mesic musi byt 1-12!');
  END IF;
  
  IF (den < 1 OR den > 31) THEN
    RAISE_APPLICATION_ERROR(-20060, 'Hodnota den musi byt 1 - 31!');
  END IF;
  
  IF (delka = 10 AND MOD(cislo, 11) != 0) THEN
    RAISE_APPLICATION_ERROR(-20070, 'Rodne cislo neni delitne 11!');
  END IF;
  
  IF (ecp = TRUE) THEN
    IF (delka = 9 AND koncovka < 600) THEN
      RAISE_APPLICATION_ERROR(-20080, 'Nepripustna koncovka ECP!');
    END IF;
    IF (delka = 10 AND koncovka < 6000) THEN
      RAISE_APPLICATION_ERROR(-20090, 'Nepripustna koncovka ECP!');
    END IF;
  END IF; 
  
END triggerRod;
/
show errors 

--Trigger - automaticke generovani ID pro rezervace pomoci inkrementace
CREATE SEQUENCE rezervace_seq START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE OR REPLACE TRIGGER rezervace_id_trig
BEFORE INSERT ON rezervace
FOR EACH ROW
BEGIN
  SELECT rezervace_seq.NEXTVAL
  INTO: NEW.id_rezervace
  FROM dual;
END;
/

-- Trigger na automatické pridanie primárneho klúča v tabuľke objednávok
CREATE SEQUENCE objednavka_seq START WITH 1 INCREMENT BY 1 NOCYCLE;
CREATE OR REPLACE TRIGGER objednavka_id_trig BEFORE
  INSERT ON objednavka
  FOR EACH ROW
    BEGIN
      SELECT objednavka_seq.NEXTVAL
      INTO : NEW.id_objednavky
      FROM dual;
    END;
/

--Procedúra, ktorej vstupným parametrom je id zamestanca a výstup je počet obslúžených ľudí týmto zamestancom
SET serveroutput ON;
CREATE OR REPLACE PROCEDURE pocet_obsluzenych_zakaznikov (id_zamestnanca NUMBER) IS
  CURSOR a_cur is SELECT * FROM rezervace;
  b_cur a_cur%ROWTYPE;
  obsluzeni_zakaznici NUMBER;
  BEGIN
    obsluzeni_zakaznici := 0;
    OPEN a_cur;
    LOOP
      FETCH a_cur into b_cur;
      EXIT WHEN a_cur%NOTFOUND;
      IF b_cur.id_zamestnance = id_zamestnanca THEN
        obsluzeni_zakaznici := obsluzeni_zakaznici + 1;
      END IF;
    END LOOP;
    DBMS_OUTPUT.PUT_LINE ('Tento zamestnanec obsluzil ' ||obsluzeni_zakaznici||' zakaznikov');
    CLOSE a_cur;
  EXCEPTION
    WHEN NO_DATA_FOUND THEN
      DBMS_OUTPUT.PUT_LINE('Zamestnanec s ' ||id_zamestnanca|| ' neexistuje!');
    WHEN OTHERS THEN
      RAISE_APPLICATION_ERROR(-20100, 'Chyba v procedure!');  
  END;
/

--Procedura, jejiž vstupním parametrem je jméno zákazníka a výstup je počet rezervací provedených tímto zákazníkem
SET serveroutput ON;
CREATE OR REPLACE PROCEDURE pocet_rezervaci (jmeno IN VARCHAR, prijmeni IN VARCHAR)
IS
  CURSOR a_cur IS SELECT * FROM zakaznik NATURAL JOIN rezervace;
  b_cur a_cur%ROWTYPE;
  pocet NUMBER;
BEGIN
  pocet := 0;
  OPEN a_cur;
  LOOP
    FETCH a_cur INTO b_cur;
    EXIT WHEN a_cur%NOTFOUND;
    IF (b_cur.jmeno = jmeno AND b_cur.prijmeni = prijmeni) THEN
      pocet := pocet + 1;
    END IF;
  END LOOP;
  DBMS_OUTPUT.PUT_LINE('Tento zakaznik provedl ' ||pocet|| ' rezervaci.');
  CLOSE a_cur;
EXCEPTION
  WHEN NO_DATA_FOUND THEN
    DBMS_OUTPUT.PUT_LINE('Zakaznik se jmenem ' ||jmeno|| ' ' ||prijmeni|| ' nexestije!');
  WHEN OTHERS THEN
    RAISE_APPLICATION_ERROR(-20110, 'Chyba v procedure!');  
END;
/

--Primarni klice
ALTER TABLE stoly ADD CONSTRAINT PK_stoly PRIMARY KEY (id_stolu);
ALTER TABLE zakaznik ADD CONSTRAINT PK_zakaznik PRIMARY KEY (id_zakaznik);
ALTER TABLE rezervace ADD CONSTRAINT PK_rezervace PRIMARY KEY (id_rezervace);
ALTER TABLE zamestnanec ADD CONSTRAINT PK_zamestnanec PRIMARY KEY (id_zamestnance);
ALTER TABLE objednavka ADD CONSTRAINT PK_objednavka PRIMARY KEY (id_objednavky);
ALTER TABLE pokrm ADD CONSTRAINT PK_pokrm PRIMARY KEY (id_pokrmu);
ALTER TABLE suroviny ADD CONSTRAINT PK_suroviny PRIMARY KEY (id_suroviny);

--Reference
ALTER TABLE pokrm ADD CONSTRAINT FK_suroviny FOREIGN KEY (id_suroviny) REFERENCES suroviny;
ALTER TABLE rezervace ADD CONSTRAINT FK_stoly_rezervace FOREIGN KEY (id_stolu) REFERENCES stoly;
ALTER TABLE zakaznik ADD CONSTRAINT FK_rezervace FOREIGN KEY (id_rezervace) REFERENCES rezervace;
ALTER TABLE rezervace ADD CONSTRAINT FK_zamestnanec_rezervace FOREIGN KEY (id_zamestnance) REFERENCES zamestnanec;
ALTER TABLE objednavka ADD CONSTRAINT FK_zamestnanec_objednavka FOREIGN KEY (id_zamestnance) REFERENCES zamestnanec;
ALTER TABLE objednavka ADD CONSTRAINT FK_stoly_objednavka FOREIGN KEY (id_stolu) REFERENCES stoly;
ALTER TABLE pocet_rezervace ADD CONSTRAINT FK_pocet_rezervace_rezervace FOREIGN KEY (id_rezervace) REFERENCES rezervace;
ALTER TABLE pocet_rezervace ADD CONSTRAINT FK_pocet_rezervace_pokrm FOREIGN KEY (id_pokrmu) REFERENCES pokrm;
ALTER TABLE pocet_objednavka ADD CONSTRAINT FK_pocet_objednavka_objednavka FOREIGN KEY (id_objednavka) REFERENCES objednavka;
ALTER TABLE pocet_objednavka ADD CONSTRAINT FK_pocet_objednavka_pokrm FOREIGN KEY (id_pokrmu) REFERENCES pokrm;

--Vlozeni 3 polozek do tabulky stoly
INSERT INTO stoly(id_stolu, pocet_mist, mistnost) VALUES('001', '3', '5');
INSERT INTO stoly(id_stolu, pocet_mist, mistnost) VALUES('002', '5', '7');
INSERT INTO stoly(id_stolu, pocet_mist, mistnost) VALUES('003', '4', '10');

--Vlozeni 3 polozek do tabulky zamestnanec
INSERT INTO zamestnanec(id_zamestnance, titul, jmeno, prijmeni, adresa, telefon, pohlavi, rodne_cislo) VALUES('001', 'Bc', 'Roman', 'Nahálka', 'Bezručova 3, Boskovice', '+420739116669', 'muž', '9404054242');
INSERT INTO zamestnanec(id_zamestnance, titul, jmeno, prijmeni, adresa, telefon, pohlavi, rodne_cislo) VALUES('002', 'Bc', 'Martin', 'Mihál', 'Slovensko', '+420739116669', 'muž', '9061152045');
INSERT INTO zamestnanec(id_zamestnance, titul, jmeno, prijmeni, adresa, telefon, pohlavi, rodne_cislo) VALUES('003', null, 'Petr', 'Čech', 'Fulham Rd, Fulham, London SW6 1HS', '+44111222333', 'muž', '9051045509');

--Vlozeni 3 polozek do tabulky rezervace
INSERT INTO rezervace(cas, datum, nazev, pocet_osob, id_stolu, id_zamestnance) VALUES('13:37:00', TO_DATE('2017-03-23', 'yyyy-mm-dd'), 'Nahálka', '2', '001', '001');
INSERT INTO rezervace(cas, datum, nazev, pocet_osob, id_stolu, id_zamestnance) VALUES('15:00:00', TO_DATE('2017-03-29', 'yyyy-mm-dd'), 'Mihál', '4', '002', '003'); 
INSERT INTO rezervace(cas, datum, nazev, pocet_osob, id_stolu, id_zamestnance) VALUES('18:00:00', TO_DATE('2017-04-01', 'yyyy-mm-dd'), 'FIT', '7', '003', '002');

--Vlozeni 3 polozek do tabulky zakaznik
INSERT INTO zakaznik(id_zakaznik, jmeno, prijmeni, telefon, email, id_rezervace) VALUES('001', 'Roman', 'Nahálka', '+420739116669', 'xnahal01@stud.fit.vutbr.cz', '001');
INSERT INTO zakaznik(id_zakaznik, jmeno, prijmeni, telefon, email, id_rezervace) VALUES('002', 'Martin', 'Mihál', '+421721964325', 'xmihal07@stud.fit.vutbr.cz', '002'); 
INSERT INTO zakaznik(id_zakaznik, jmeno, prijmeni, telefon, email, id_rezervace) VALUES('003', 'Jan', 'Novák', '+420666777888', 'novak@gmail.com', '003');

--Vlozeni 3 polozek do tabulky objednavka
INSERT INTO objednavka(cas, id_zamestnance, id_stolu) VALUES('13:50:05', '003', '001');
INSERT INTO objednavka(cas, id_zamestnance, id_stolu) VALUES('13:50:10', '001', '002'); 
INSERT INTO objednavka(cas, id_zamestnance, id_stolu) VALUES('13:50:15', '002', '003');

--Vlozeni 3 polozek do tabulky suroviny
INSERT INTO suroviny(id_suroviny, mnozstvi, alergeny) VALUES('001', '25', 'Lepek');
INSERT INTO suroviny(id_suroviny, mnozstvi, alergeny) VALUES('002', '10', 'Vejce');  
INSERT INTO suroviny(id_suroviny, mnozstvi, alergeny) VALUES('003', '11', 'Sezam');

--Vlozeni 3 polozek do tabulky jidlo a piti
INSERT INTO pokrm(id_pokrmu, typ, cena, popis, id_suroviny) VALUES('001', 'Pivo', '25', 'Gambrinus', '001');
INSERT INTO pokrm(id_pokrmu, typ, cena, popis, id_suroviny) VALUES('002', 'Jidlo', '95', 'Guláš', '002');  
INSERT INTO pokrm(id_pokrmu, typ, cena, popis, id_suroviny) VALUES('003', 'Nealko', '27', 'Kofola', '003');

--Vlozeni 3 polozek do tabulky pocet pokrmu u rezervace
INSERT INTO pocet_rezervace(pocet, id_rezervace, id_pokrmu) VALUES('2', '001', '001');
INSERT INTO pocet_rezervace(pocet, id_rezervace, id_pokrmu) VALUES('4', '002', '002'); 
INSERT INTO pocet_rezervace(pocet, id_rezervace, id_pokrmu) VALUES('7', '003', '003');

--Vlozeni 3 polozek do tabulky pocet pokrmu u objednavky
INSERT INTO pocet_objednavka(pocet, id_objednavka, id_pokrmu) VALUES('2', '001', '001');
INSERT INTO pocet_objednavka(pocet, id_objednavka, id_pokrmu) VALUES('4', '002', '002'); 
INSERT INTO pocet_objednavka(pocet, id_objednavka, id_pokrmu) VALUES('7', '003', '003');

--Výpis zákazníkov a ich rezervácií
SELECT zakaznik.jmeno, zakaznik.prijmeni, rezervace.id_rezervace
FROM zakaznik
INNER JOIN rezervace
ON rezervace.id_rezervace = zakaznik.id_rezervace;

--Výpis jedla a jeho surovín
SELECT pokrm.id_pokrmu, pokrm.popis, suroviny.id_suroviny
FROM pokrm
INNER JOIN suroviny
ON suroviny.id_suroviny = pokrm.id_suroviny;

--Výpis objednávok, zamestnanca, ktorý objednávku vytvoril a stôl
SELECT zamestnanec.jmeno, objednavka.id_objednavky, stoly.id_stolu
FROM zamestnanec
INNER JOIN objednavka
ON objednavka.id_zamestnance = zamestnanec.id_zamestnance
INNER JOIN stoly
ON stoly.id_stolu = objednavka.id_stolu;

--Výpis typu pokrmu a počet pokrmov rovnakého typu
SELECT pokrm.typ, COUNT(pokrm.typ)
FROM pokrm
GROUP BY pokrm.typ;

--Výpis zamestnancov a počet objednávok, ktoré maju pridelené
SELECT zamestnanec.jmeno, zamestnanec.prijmeni, COUNT(objednavka.id_objednavky)
FROM zamestnanec
LEFT JOIN objednavka
ON objednavka.id_zamestnance = zamestnanec.id_zamestnance
GROUP BY zamestnanec.jmeno, zamestnanec.prijmeni;

--Výpis zamestnancov, ktorí nemajú pridelenú objednávku
SELECT *
FROM zamestnanec
WHERE NOT EXISTS
  (SELECT *
  FROM objednavka
  WHERE objednavka.id_zamestnance = zamestnanec.id_zamestnance
  );

--Výpis rezervacií, kde je rezervovaný stôl s dvoma alebo viac miestami
SELECT *
FROM rezervace
WHERE id_stolu IN
  (SELECT id_stolu
  FROM stoly
  WHERE pocet_mist >= '2'
  );

--Zavolani procedur
exec pocet_obsluzenych_zakaznikov(002);
exec pocet_rezervaci('Roman', 'Nahálka');

--EXPLAIN PLAN bez Indexu
EXPLAIN PLAN FOR
SELECT jmeno, AVG(pocet_osob)
FROM zakaznik NATURAL JOIN rezervace
GROUP BY pocet_osob, jmeno;
SELECT * FROM TABLE(dbms_xplan.display);

--Vytvoreni Indexu
CREATE INDEX myIndex ON zakaznik (jmeno);

--EXPLAIN PLAN znovu s Indexem
EXPLAIN PLAN FOR
SELECT /*+ INDEX(zakaznik myIndex)*/ jmeno, AVG(pocet_osob)
FROM ZAKAZNIK NATURAL JOIN rezervace
GROUP BY pocet_osob, jmeno;
SELECT * FROM TABLE(dbms_xplan.display);

--Vytvoření přístupu pro druhého studenta
GRANT ALL ON stoly TO xmihal07;
GRANT ALL ON zakaznik TO xmihal07;
GRANT ALL ON rezervace TO xmihal07;
GRANT ALL ON zamestnanec TO xmihal07;
GRANT ALL ON objednavka TO xmihal07;
GRANT ALL ON pokrm TO xmihal07;
GRANT ALL ON suroviny TO xmihal07;
GRANT ALL ON pocet_rezervace TO xmihal07;
GRANT ALL ON pocet_objednavka TO xmihal07;
GRANT EXECUTE ON pocet_obsluzenych_zakaznikov TO xmihal07;
GRANT EXECUTE ON pocet_rezervaci TO xmihal07;
COMMIT;

--Materializovany pohled
DROP MATERIALIZED VIEW viewZakaznik;

CREATE MATERIALIZED VIEW LOG ON zakaznik WITH PRIMARY KEY;
CREATE MATERIALIZED VIEW viewZakaznik
  CACHE
  BUILD IMMEDIATE
  REFRESH FAST ON COMMIT
  ENABLE QUERY REWRITE
  AS
    SELECT Z.id_zakaznik, Z.jmeno, Z.prijmeni, R.cas, R.datum, R.nazev
    FROM zakaznik Z, rezervace R
    WHERE Z.id_rezervace = R.id_rezervace;
GRANT ALL ON viewZakaznik TO xmihal07;

-- Ukážka materializovaného pohľadu viewZakaznik
SELECT * FROM viewZakaznik;
INSERT INTO zakaznik(id_zakaznik, jmeno, prijmeni, telefon, email, id_rezervace) VALUES('004', 'Martin', 'Mihál', '+421721964325', 'xmihal07@stud.fit.vutbr.cz', '002');
COMMIT;
SELECT * FROM viewZakaznik;