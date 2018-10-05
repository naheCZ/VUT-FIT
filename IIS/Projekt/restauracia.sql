SET NAMES utf8;
SET foreign_key_checks = 0;
SET time_zone = 'SYSTEM';
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

DROP TABLE IF EXISTS stoly cascade;
DROP TABLE IF EXISTS zakaznik cascade;
DROP TABLE IF EXISTS rezervace cascade;
DROP TABLE IF EXISTS zamestnanec cascade;
DROP TABLE IF EXISTS objednavka cascade;
DROP TABLE IF EXISTS order_meals cascade;
DROP TABLE IF EXISTS pokrm cascade;
DROP TABLE IF EXISTS suroviny cascade;
DROP TABLE IF EXISTS users cascade;

CREATE TABLE stoly(
  id_stolu SERIAL PRIMARY KEY,
  pocet_mist INT NOT NULL,
  mistnost INT NOT NULL
  );


CREATE TABLE zakaznik(
  id_zakaznik SERIAL PRIMARY KEY,
  jmeno varchar(30) NOT NULL,
  prijmeni varchar(30) NOT NULL,
  telefon varchar(30) NOT NULL,
  email varchar(30) NOT NULL,
  user_id int NOT NULL
  );


CREATE TABLE rezervace(
  id_rezervace SERIAL PRIMARY KEY,
  datum TIMESTAMP NOT NULL,
  nazev varchar(30) NOT NULL,
  pocet_osob int NOT NULL,
  id_stolu int NOT NULL,
  user_id int NOT NULL
  );


CREATE TABLE zamestnanec(
  id_zamestnance SERIAL PRIMARY KEY,
  titul VARCHAR(30),
  jmeno VARCHAR(30) NOT NULL,
  prijmeni VARCHAR(30) NOT NULL,
  adresa VARCHAR(30) NOT NULL,
  telefon VARCHAR(30) NOT NULL,
  pohlavi VARCHAR(30) NOT NULL,
  rodne_cislo VARCHAR(30) NOT NULL,
  user_id int NOT NULL
  );


CREATE TABLE objednavka(
  id_objednavky SERIAL PRIMARY KEY,
  cas TIMESTAMP NOT NULL,
  id_zamestnance integer NOT NULL,
  id_stolu int NOT NULL
  );


CREATE TABLE order_meals(
  id_objednavky int NOT NULL,
  id_pokrmu int NOT NULL
);


CREATE TABLE pokrm(
  id_pokrmu   SERIAL PRIMARY KEY,
  typ         VARCHAR(30) NOT NULL,
  cena        INT NOT NULL,
  popis       VARCHAR(30) NOT NULL
);


CREATE TABLE suroviny(
  id_suroviny SERIAL PRIMARY KEY,
  mnozstvi int NOT NULL,
  alergeny VARCHAR(30) NOT NULL,
  id_pokrmu int NOT NULL
  );


CREATE TABLE users(
  user_id SERIAL PRIMARY KEY,
  username VARCHAR(30) NOT NULL,
  password VARCHAR(30) NOT NULL,
  privileges SMALLINT NOT NULL
);


ALTER TABLE zakaznik ADD CONSTRAINT FK_users_zakaznik FOREIGN KEY (user_id) REFERENCES users;
ALTER TABLE rezervace ADD CONSTRAINT FK_stoly_rezervace FOREIGN KEY (id_stolu) REFERENCES stoly;
ALTER TABLE rezervace ADD CONSTRAINT FK_users_rezervace FOREIGN KEY (user_id) REFERENCES users;
ALTER TABLE zamestnanec ADD CONSTRAINT FK_users_zamestnanec FOREIGN KEY (user_id) REFERENCES users;
ALTER TABLE objednavka ADD CONSTRAINT FK_zamestnanec_objednavka FOREIGN KEY (id_zamestnance) REFERENCES zamestnanec;
ALTER TABLE objednavka ADD CONSTRAINT FK_stoly_objednavka FOREIGN KEY (id_stolu) REFERENCES stoly;
ALTER TABLE order_meals ADD CONSTRAINT FK_objednavka_order_meals FOREIGN KEY (id_objednavky) REFERENCES objednavka;
ALTER TABLE order_meals ADD CONSTRAINT FK_pokrm_order_meals FOREIGN KEY (id_pokrmu) REFERENCES pokrm;
ALTER TABLE suroviny ADD CONSTRAINT FK_pokrm_suroviny FOREIGN KEY (id_pokrmu) REFERENCES pokrm;

ALTER TABLE users ADD CONSTRAINT unique_username UNIQUE (username);
ALTER TABLE objednavka ALTER COLUMN cas SET DEFAULT now();