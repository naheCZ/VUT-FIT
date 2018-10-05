#Prvni projekt do predmetu IOS
#Autor: Roman Nahalka, xnahal01@stud.fit.vutbr.cz

#!/bin/sh
export LC_ALL=C

function bezI #Funkce, ktera se spusti, pokud uzivatel nespustil skript s prepinacem -i
{
	ND=$(find ${DIR} -type d | wc -l | tr -d ' ') #Pocet slozek	
	NF=$(find ${DIR} -type f | wc -l | tr -d ' ') #Pocet souboru
	AF=$(( $NF / $ND )) #Prumerny pocet 
	DD=0	
	i=0
	pokracovat=1
	while [ $pokracovat -eq 1 ]
	do
		pom=$(find ${DIR} -mindepth ${i} -type d | wc -l)
		if [ $DD -lt $i ]
		then		
			DD=${i}
		fi
		if [ $pom -eq 0 ]
		then
			pokracovat=0
		fi
		i=$(( $i + 1))
	done
	LF=$(find ${DIR} -type f -ls | sort -nr -k7 | awk 'NR==1{print $7}')
	
	if [ $NF -eq 0 ]
	then
		AS="N/A"
	else	
		AS=$(find ${DIR} -type f -ls | awk '{ a+= $7 }; END {print a}')
		AS=$(( $AS / $NF ))
	fi
	
	if [ $NF -eq 0 ]
	then #Nemame zadne soubor, median bude nula
		MS="N/A"
	else #Mame soubory, vypocitama median
		del=$(( $NF % 2))		
		if [ $del -eq 0 ]
		then #Mame sudy pocet souboru	
			pom=$(( $NF / 2 ))
			MS1=$(find ${DIR} -type f -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			MS2=$(find ${DIR} -type f -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( MS1 + MS2 ))
			MS=$(( $pom / 2 ))
		else #Mame lichy pocet souboru		
			pom=$(( $NF + 1 ))
			pom=$(( $pom / 2 ))
			MS=$(find ${DIR} -type f -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
		fi
	fi

	ext_number=$(find ${DIR} -type f -name '?*.*' | sed 's|.*\.||' | sort -u | wc -l) #Zjistime si, kolil mame celkem pripon
	
	i=1
	while [ $i -le $ext_number ] #Pro kazdou priponu v cyklu zjistime potrebne informace
	do		
		el[$i]=$(find ${DIR} -type f -name '?*.*' | sed 's|.*\.||' | sort -u | sed -n ${i}p)
		next[$i]=$(find ${DIR} -type f -name "?*.${el[$i]}" | wc -l | tr -d ' ')		
		lext[$i]=$(find ${DIR} -type f -name "?*.${el[$i]}" -ls | sort -nr -k7 | awk 'NR==1{print $7}')
		aext[$i]=$(find ${DIR} -type f -name "?*.${el[$i]}" -ls | awk '{ a+= $7 }; END {print a}')
		aext[$i]=$(( ${aext[$i]} / ${next[$i]} ))
		if [ $((${next[$i]}%2)) -eq 0 ]
		then
			pom=$(( ${next[$i]} / 2 ))
			pom1=$(find ${DIR} -type f -name "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			pom2=$(find ${DIR} -type f -name "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( pom1 + pom2 ))
			mext[$i]=$(( $pom / 2 ))
		else
			pom=$(( ${next[$i]} + 1 ))
			pom=$(( $pom / 2 ))			
			mext[$i]=$(find ${DIR} -type f -name "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
		fi
		i=$(( $i + 1 ))
	done
			
	report	
}

function funkceI #Funkce, ktera se spusti, byl-li skript spusten s prepinacem -i
{
	ND=$(find ${DIR} -type d ! -regex $FILE_ERE | wc -l | tr -d ' ')	
	NF=$(find ${DIR} -type f ! -regex $FILE_ERE | wc -l | tr -d ' ')
	AF=$(( $NF / $ND ))
	#DD=$(find ${DIR} -type d -not -path "*/$FILE_ERE*" -printf '%d\n' | sort -n | tail -1)
	#DD=$(( $DD + 1 ))
	DD=0	
	i=0
	pokracovat=1
	while [ $pokracovat -eq 1 ]
	do
		pom=$(find ${DIR} -mindepth ${i} -type d ! -regex $FILE_ERE | wc -l)
		if [ $DD -lt $i ]
		then		
			DD=${i}
		fi
		if [ $pom -eq 0 ]
		then
			pokracovat=0
		fi
		i=$(( $i + 1))
	done	
	LF=$(find ${DIR} -type f ! -regex $FILE_ERE -ls | sort -nr -k7 | awk 'NR==1{print $7}')
	if [ $NF -eq 0 ]
	then
		AS="N/A"
	else	
		AS=$(find ${DIR} -type f ! -regex $FILE_ERE -ls | awk '{ a+= $7 }; END {print a}')
		AS=$(( $AS / $NF ))
	fi

	if [ $NF -eq 0 ]
	then
		MS="N/A"
	else
		if [ $((nf%2)) -eq 0 ]
		then
			pom=$(( $NF / 2 ))
			MS1=$(find ${DIR} -type f ! -regex $FILE_ERE -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			MS2=$(find ${DIR} -type f ! -regex $FILE_ERE -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( MS1 + MS2 ))
			MS=$(( $pom / 2 ))
		else
			pom=$(( $NF + 1 ))
			pom=$(( $pom / 2 ))
			MS=$(find ${DIR} -type f ! -regex $FILE_ERE -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
		fi
	fi

	ext_number=$(find ${DIR} -type f ! -regex $FILE_ERE -name '?*.*' | sed 's|.*\.||' | sort -u | wc -l) #Zjistime si, kolil mame celkem pripon
	
	i=1
	while [ $i -le $ext_number ] #Pro kazdou priponu v cyklu zjistime potrebne informace
	do		
		el[$i]=$(find ${DIR} -type f ! -regex $FILE_ERE -name '?*.*' | sed 's|.*\.||' | sort -u | sed -n ${i}p)
		next[$i]=$(find ${DIR} -type f ! -regex $FILE_ERE -name "?*.${el[$i]}" | wc -l | tr -d ' ')		
		lext[$i]=$(find ${DIR} -type f ! -regex $FILE_ERE -name "?*.${el[$i]}" -ls | sort -nr -k7 | awk 'NR==1{print $7}')
		aext[$i]=$(find ${DIR} -type f ! -regex $FILE_ERE -name "?*.${el[$i]}" -ls | awk '{ a+= $7 }; END {print a}')
		aext[$i]=$(( ${aext[$i]} / ${next[$i]} ))
		if [ $((${next[$i]}%2)) -eq 0 ]
		then
			pom=$(( ${next[$i]} / 2 ))
			pom1=$(find ${DIR} -type f -name ! -regex $FILE_ERE "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			pom2=$(find ${DIR} -type f -name ! -regex $FILE_ERE "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
			pom=$(( pom1 + pom2 ))
			mext[$i]=$(( $pom / 2 ))
		else
			pom=$(( ${next[$i]} + 1 ))
			pom=$(( $pom / 2 ))			
			mext[$i]=$(find ${DIR} -type f ! -regex $FILE_ERE -name "?*.${el[$i]}" -ls | awk '{print $7}' | sort -n | sed -n ${pom}p)
		fi
		i=$(( $i + 1 ))
	done

	report
}

function report #Funkce, ktera vypise vsechna potrebna data
{
	echo "Root directory: $DIR"
	echo "Directories: $ND"
	echo "Max depth: $DD"
	echo "Average no. of files: $AF"
	echo "All files: $NF"
	echo "  Largest file: $LF"
	echo "  Average file size: $AS"
	echo "  File size median: $MS"
	printf "File extensions: "	
	
	i=1
	while [ $i -le $ext_number ]
	do
		printf "%s" ${el[$i]}
		if [ $i -eq $ext_number ]
		then
			printf "\n"
		else
			printf ","
		fi
		i=$(( $i + 1 ))
	done

	i=1
	while [ $i -le $ext_number ]
	do
		echo "Files .${el[$i]}: ${next[$i]}"
		echo "  Largest file .${el[$i]}: ${lext[$i]}"
		echo "  Average file size .${el[$i]}: ${aext[$i]}"
		echo "  File size median .${el[$i]}: ${mext[$i]}"
		i=$(( $i + 1 ))
	done
	
}

DIR=""
FILE_ERE=""
EXIT_CODE=0

iParametr=false

#Zjistime a ulozime si zadane argumenty
while getopts ":i:" opt
do
	case "$opt" in
		i)	iParametr=true;;
		\?)	echo "Spatny prepinac: -$OPTARG" >&2; 
			exit 2;;
		:)	echo "Prepinac -$OPTARG vyzaduje argument!" >&2
			exit 2;;
	esac
done

FILE_ERE=$2
shift $(($OPTIND-1))
if [ $# -eq 1 ]
then
	DIR=$1;

elif [ $# -eq 0 ]
then
	DIR="$PWD"
else
	echo "Zadal jsi prilis mnoho argumentu!" >&2
	exit 2
fi

if [ -z "$DIR" ] #Jestlize nebyl zadan adresar, pouzijeme ten aktualni
then
	DIR="$PWD"
fi

#Zjistime, jestli zadany adresar existuje
if [ ! -d "$DIR" ]
then
	echo "Zadany adresar neexistuje!" >&2
	exit 2
fi

if $iParametr
then
	funkceI
else
	bezI
fi

exit 0
