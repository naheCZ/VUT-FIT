# Projekt: Prvni projekt pro predmet Typografie a publikovani
# Autor: Roman Nahálka
# Datum: 21.2.2017

FILE = proj2

all: $(FILE).tex
	latex -halt-on-error $(FILE).tex
	dvips -t a4 $(FILE).dvi
	ps2pdf $(FILE).ps