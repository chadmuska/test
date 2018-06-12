#!/bin/sh

if [ $# -lt 2 ] ; then
	echo "$0: usage FILE KEY [VALUE]"
	exit 1
fi

s="[[:blank:]]"
w="[_[:alnum:]]"
W="[^_[:alnum:]]"

grep=/bin/grep
sed=/bin/sed

if [ -n "$3" ] ; then
#$val inserting a heading space
	val=" $3"
fi

if ! $grep -q -m 1 "^$s*\#$s*\(define\|undef\)$s\+$2\($W\+.*\)\?$" $1
then
	echo "$0:\`$1': key \`$2' not found"
	exit 1
fi

if [ -z "$val" ] ; then
	re="undef\3"
else
	re="define\3$val"
fi
$sed -i -e "s/\(^$s*\#$s*\)\(define\|undef\)\($s\+$2\)\($W\+.*\)\?$/\1$re/" $1
