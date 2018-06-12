#!/bin/bash

if [ $# = 0 ] ; then
	echo "usage:$0 \`addr_in_hex' ..." 1>&2
	exit
fi

prefix=`dirname $0` &&
cfgfile=$prefix/lastprj.def &&
makekeyget=$prefix/../tool/makekeyget.sh &&
prj=$($makekeyget $cfgfile PRJ) &&
platform=$($makekeyget $cfgfile PLATFORM)
if [ $? != 0 ] || [ -z "$prj" ] || [ -z "$platform" ] ; then
	echo "$0:failed to get prj and platform" 1>&2
	exit 1
fi

w=[_[:alnum:]]
x=[[:xdigit:]]

set -o pipefail

rm=/bin/rm
sed=/bin/sed
sort=/usr/bin/sort
uname=/bin/uname

elf=$prefix/_$prj/$platform.elf
if [ ! -f "$elf" ] || [ ! -r "$elf" ] ; then
	echo "$0:\`$elf':can not open" 1>&2
	exit 1
fi

if [ "`$uname`" = Linux ] ; then
	toolpathexe_suffix=".linux"
else
	toolpathexe_suffix=
fi
addr2func=$prefix/../tool${toolpathexe_suffix}/addr2func
#addr2func=$prefix/../tool/addr2func.sh
if [ ! -f "$addr2func" ] || [ ! -x "$addr2func" ] ; then
	echo "$0:\`$addr2func':invalid" 1>&2
	exit 1
fi

text=${elf/%.elf/.text}
if [ "$text" -ot "$elf" ] ; then
	objdump=$($makekeyget $cfgfile PREFIX)objdump
	if [ $? != 0 ] || [ ! -f "$objdump" ] || [ ! -x "$objdump" ] ; then
		echo "$0:\`$objdump':invalid" 1>&2
		exit 1
	fi

	$rm -f $text &&
	$objdump -t $elf | \
	$sed -n -e "s/^\($x\{8\}\) .\{6\}F \.\(text\|rom\|except\)\(\.$w\+\)\?\t$x\{8\} \($w\+\)/\1 \4/p" \
		-e "s/^\($x\{8\}\) .\{7\} \(\*ABS\*\|\.\(text\|rom\|except\)\(\.$w\+\)\?\)\t$x\{8\} \(\(_etext\|_erom\|_eexcept\)\(_$w\+\)\?\)/\1 \5/p" | \
	$sort > $text
	if [ $? != 0 ] ; then
		echo "$0:\`$text':failed" 1>&2
		exit 1
	fi
fi

$addr2func $@ < $text | $sort
