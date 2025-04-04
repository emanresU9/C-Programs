#!/bin/bash

many=$1
oFile="$2"

chars=("A" "B" "C" "D" "E" "F" "G" "H" "I" "J" "K" "L" "M" "N" "O" "P" "Q" "R" "S" "T" "U" "V" "W" "X" "Y" "Z")

echo -n "" > "${oFile}"

i=0
while [ $i -lt $many ]; do
    echo -n ${chars[ $(($RANDOM % 26)) ]} >> "${oFile}"
    ((i++))
    if [ $(($i % 50)) -eq 0 ]; then
	echo "" >> "${oFile}"
    fi
done
echo "" >> "${oFile}"

exit 0
