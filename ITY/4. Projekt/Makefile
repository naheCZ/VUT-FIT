# Projekt: Ctvrty projekt pro predmet Typografie a publikovani
# Autor: Roman Nahálka
# Datum: 13.4.2017

FILE = proj4

all: $(FILE).tex
	latex -halt-on-error $(FILE).tex
	bibtex $(FILE).aux
	latex -halt-on-error $(FILE).tex
	latex -halt-on-error $(FILE).tex
	dvips -t a4 $(FILE).dvi
	ps2pdf -sPAPERSIZE=a4 $(FILE).ps