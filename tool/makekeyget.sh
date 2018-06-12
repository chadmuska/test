#!/bin/sh

s="[[:blank:]]"
w="[_[:alnum:]]"

sed=/bin/sed

if [ $# = 2 ]; then
	key=`$sed -n -e "s/\r//g" -e "s/${s}*\(\#.*\)\?$//" \
		-e "s/^${s}*$2${s}*:\?=${s}*//p" $1`
fi

echo "${key}"
