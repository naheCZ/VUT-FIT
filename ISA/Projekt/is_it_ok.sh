#!/usr/bin/env sh

GRAY='\033[1;30m'
RED='\033[1;31m'
GREEN='\033[1;32m'
NC='\033[0m'

myLogin=`whoami`

printf "${GRAY}Jméno archivu\t\t\t\t\t\t\t\t  "
if [ -f $myLogin".tar" ]; then
	
	printf "${GREEN}[ OK ]\n"
else

	printf "${RED}[FAIL]\n"
fi

printf "${GRAY}Extrakce archivu\t\t\t\t\t\t\t  "
tar -xvf $myLogin".tar" 1>/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
	
	printf "${GREEN}[ OK ]\n"
else

	printf "${RED}[FAIL]\n"
fi

printf "${GRAY}Dokumentace\t\t\t\t\t\t\t\t  "
if [ -f "manual.pdf" ]; then
	
	printf "${GREEN}[ OK ]\n"
else

	printf "${RED}[FAIL]\n"
fi

printf "${GRAY}README\t\t\t\t\t\t\t\t\t  "
if [ -f "README" ]; then
	
	printf "${GREEN}[ OK ]\n"
else

	printf "${RED}[FAIL]\n"
fi

printf "${GRAY}make\t\t\t\t\t\t\t\t\t  "
make 1>/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
	
	printf "${GREEN}[ OK ]\n${NC}"
else

	printf "${RED}[FAIL]\n${NC}"
fi

#exit 0

rm -rf "isa-pop3-server-tests/"
git clone "https://github.com/atepr/isa-pop3-server-tests.git" 1>/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
	
	cd "isa-pop3-server-tests/" && "./make-and-test.sh"
	cd ".."
else
	printf "${RED}Test selhal!\n${NC}"
fi

#Než spustíš cizí skript, měl by sis ho projít.
#Mohlo by v něm být něco nepěkného, třeba:
#rm -rf ~/*

exit 0
