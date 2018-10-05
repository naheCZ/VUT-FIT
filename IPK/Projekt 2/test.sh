#!/bin/bash
export LC_ALL=C

SS="Slozka_server"

# Ukazka - nahrani a stazeni par souboru
echo "Nahraji soubor text.txt na server a stahnu z neho soubor textik.txt"
echo "Server se spusti ve slozce Slozka_server a klient v nadrazenem adresari"
mkdir -p $SS
make 1>/dev/null
cp server $SS
cp textik.txt $SS
rm textik.txt
cd $SS
./server -p 6666&
cd ..
sleep 0.5
./client -h localhost -p 6666 -u text.txt
./client -h localhost -p 6666 -d textik.txt
sleep 0.5
killall server

