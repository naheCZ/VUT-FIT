#Prvni projekt do predmetu IOS
#Autor: Roman Nahalka, xnahal01@stud.fit.vutbr.cz

#!/bin/sh
export LC_ALL=C

function bezI ()
{
	ND=$(find ${DIR} -type d | wc -l)	
	NF=$(find ${DIR} -type f | wc -l)
	AF=$(( $NF / $ND ))
	DD=$(find ${DIR} -type d -printf '%d\n' | sort -n | tail -1)
	DD=$(( $DD + 1 ))
	LF=$(find ${DIR} -type f -printf '%s\n' | sort -n | tail -1)
	
	if [ $NF -eq 0 ]
	then
		AS="N/A"
	else	
		AS=$(find ${DIR} -type f -printf '%s + ' | dc -e0 -f- -ep)
		AS=$(( $AS / $NF ))
	fi
	
	if [ $NF -eq 0 ]
	then
		MS="N/A"
	else
		if [ $((nf%2)) -eq 0 ]
		then
			pom=$(( $NF / 2 ))
			MS1=$(find ${DIR} -type f -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			MS2=$(find ${DIR} -type f -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( MS1 + MS2 ))
			MS=$(( $pom / 2 ))
		else
			pom=$(( $NF + 1 ))
			pom=$(( $pom / 2 ))
			MS=$(find -type f -printf '%s\n' | sort -n | sed -n ${pom}p)
		fi
	fi

	ext_number=$(find ${DIR} -type f -name '*.*' | sed 's|.*\.||' | sort -u | wc -l)
	
	i=1
	while [ $i -le $ext_number ]
	do		
		el[$i]=$(find ${DIR} -type f -name '*.*' | sed 's|.*\.||' | sort -u | sed -n ${i}p)
		next[$i]=$(find ${DIR} -type f -name "*.${el[$i]}" | wc -l)		
		lext[$i]=$(find ${DIR} -type f -name "*.${el[$i]}" -printf '%s\n' | sort -n | tail -1)
		aext[$i]=$(find ${DIR} -type f -name "*.${el[$i]}" -printf '%s + ' | dc -e0 -f- -ep)
		aext[$i]=$(( ${aext[$i]} / ${next[$i]} ))
		if [ $((${next[$i]}%2)) -eq 0 ]
		then
			pom=$(( ${next[$i]} / 2 ))
			pom1=$(find ${DIR} -type f -name "*.${el[$i]}" -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			pom2=$(find ${DIR} -type f -name "*.${el[$i]}" -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( pom1 + pom2 ))
			mext[$i]=$(( $pom / 2 ))
		else
			pom=$(( ${next[$i]} + 1 ))
			pom=$(( $pom / 2 ))			
			mext[$i]=$(find ${DIR} -type f -name "*.${el[$i]}" -printf '%s\n' | sort -n | sed -n ${pom}p)
		fi
		i=$(( $i + 1 ))
	done
			
	report	
}

function funkceI ()
{
	ND=$(find ${DIR} -type d -not -path "*/$FILE_ERE*" | wc -l)	
	NF=$(find ${DIR} -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*"  | wc -l)
	AF=$(( $NF / $ND ))
	DD=$(find ${DIR} -type d -not -path "*/$FILE_ERE*" -printf '%d\n' | sort -n | tail -1)
	DD=$(( $DD + 1 ))
	LF=$(find ${DIR} -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*" -printf '%s\n' | sort -n | tail -1)
	if [ $NF -eq 0 ]
	then
		AS="N/A"
	else	
		AS=$(find ${DIR} -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*" -printf '%s + ' | dc -e0 -f- -ep)
		AS=$(( $AS / $NF ))
	fi

	if [ $NF -eq 0 ]
	then
		MS="N/A"
	else
		if [ $((nf%2)) -eq 0 ]
		then
			pom=$(( $NF / 2 ))
			MS1=$(find ${DIR} -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*" -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( $pom + 1 ))
			MS2=$(find ${DIR} -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*" -printf '%s\n' | sort -n | sed -n ${pom}p)
			pom=$(( MS1 + MS2 ))
			MS=$(( $pom / 2 ))
		else
			pom=$(( $NF + 1 ))
			pom=$(( $pom / 2 ))
			MS=$(find -type f ! -iname "$FILE_ERE" -not -path "*/$FILE_ERE*" -printf '%s\n' | sort -n | sed -n ${pom}p)
		fi
	fi

	report
}

function report ()
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
