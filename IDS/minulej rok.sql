DROP TABLE Ridic CASCADE CONSTRAINTS;
DROP TABLE Ucastnik CASCADE CONSTRAINTS;
DROP TABLE Nehoda CASCADE CONSTRAINTS;

CREATE TABLE Ridic
(
  IDridice NUMBER,
  jmeno VARCHAR2(50)
);
 
CREATE TABLE Ucastnik
(
  IDnehody NUMBER,
  IDridice NUMBER
);
 
CREATE TABLE Nehoda
(
  IDnehody NUMBER,
  misto VARCHAR(50)
);

INSERT INTO Ridic VALUES('1', 'Jan Novak');
INSERT INTO Ridic VALUES('2', 'Pavel Novy');
INSERT INTO Ridic VALUES('3', 'Roman Nahalka');
INSERT INTO Ridic VALUES('4', 'Pepa Bican');

INSERT INTO Ucastnik VALUES('4', '1');
INSERT INTO Ucastnik VALUES('2', '2');
INSERT INTO Ucastnik VALUES('3', '3');
INSERT INTO Ucastnik VALUES('4', '2');

INSERT INTO Nehoda VALUES('1', 'Boskovice');
INSERT INTO Nehoda VALUES('2', 'Blansko');
INSERT INTO Nehoda VALUES('3', 'Brno');
INSERT INTO Nehoda VALUES('4', 'Boskovice');

SELECT misto
FROM Nehoda NATURAL JOIN Ucastnik NATURAL JOIN Ridic
WHERE jmeno = 'Jan Novak' AND IDNehody IN (SELECT IDNehody
                                           FROM Nehoda NATURAL JOIN Ucastnik NATURAL JOIN Ridic
                                           WHERE jmeno = 'Pavel Novy');
                                           
SELECT Ridic.IDridice, Ridic.jmeno, COUNT(Ucastnik.IDnehody)
FROM Ridic 
LEFT OUTER JOIN Ucastnik ON Ridic.IDridice = Ucastnik.IDridice
GROUP BY Ridic.IDridice, Ridic.jmeno
ORDER BY COUNT(Ucastnik.IDnehody);