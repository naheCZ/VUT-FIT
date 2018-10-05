# Projekt: Treti projekt pro predmet Typografie a publikovani
# Autor: Roman Nahálka
# Datum: 27.3.2017

FILE = proj3

all: $(FILE).tex
	latex -halt-on-error $(FILE).tex
	latex -halt-on-error $(FILE).tex
	dvips -t a4 $(FILE).dvi
	ps2pdf -sPAPERSIZE=a4 $(FILE).ps