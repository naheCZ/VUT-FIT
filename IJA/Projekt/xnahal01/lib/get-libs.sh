#!/bin/sh

# skrip pre stiahnutie obrázkov potrebných pre projekt z IJA

# authors:  Roman Nahalka (xnahal01)
#           Adam Zivcak (xzivca03)
# date: 2nd May 2017

wget -q http://www.stud.fit.vutbr.cz/~xnahal01/images.zip

unzip images.zip > /dev/null

rm images.zip
