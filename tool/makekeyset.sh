#!/bin/sh

s="[[:blank:]]"
w="[_[:alnum:]]"
c="[^[:blank:]#]"

grep=/bin/grep
sed=/bin/sed

if [ $# -lt 2 ] ; then
	echo "$0: usage FILE KEY [VALUE]"
	exit 1
fi

if ! $grep -q -m 1 "^$s*$2$s*:\?=$s*.*$" $1 ; then
	if ! $grep -q -m 1 "^$s*$2$s*?\?=$s*.*$" $1 ; then
		echo "${0}: \`$1': key \`$2' not found"
		exit 1
	else
		$sed -i -e "s/\(^$s*$2$s*?\?=$s*\)\($s*$c\+\)*\($s*#.*\)\?$/\1$3\3/" $1
	fi
else
	$sed -i -e "s/\(^$s*$2$s*:\?=$s*\)\($s*$c\+\)*\($s*#.*\)\?$/\1$3\3/" $1
fi
