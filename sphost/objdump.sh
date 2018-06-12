#!/bin/sh

prefix=`dirname $0` &&
cfgfile=$prefix/lastprj.def &&
makekeyget=$prefix/../tool/makekeyget.sh &&
tool_prefix=$($makekeyget $cfgfile PREFIX) &&
objdump=${tool_prefix}objdump

if [ $? != 0 ] || [ -z "$tool_prefix" ] || [ ! -x "$objdump" ] ; then
	echo "$0:failed to get prj and prefix" 1>&2
	exit 1
fi


$objdump $@
