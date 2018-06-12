#!/bin/sh

s="[[:blank:]]"
w="[_[:alnum:]]"

sed=/bin/sed

if [ $# = 2 ]; then
	key=`$sed -n -e "s/\r//g" -e "s/\/\*.*\*\// /g" \
		-e "s/${s}*\(\/\/.*\)\?$//" \
		-e "s/^${s}*\#${s}*define${s}\+$2${s}\+//p" $1`
fi

echo "${key}"
