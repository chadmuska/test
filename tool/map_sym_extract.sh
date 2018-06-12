#!/bin/bash
#/**************************************************************************
# *
# *         Copyright (c) 2011 by iCatch Technology Inc.
# *
# *  This software is copyrighted by and is the property of iCatch
# *  Technology Inc. All rights are reserved by iCatch Technology Inc.
# *  This software may only be used in accordance with the corresponding
# *  license agreement. Any unauthorized use, duplication, distribution,
# *  or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice MUST not be removed or modified without prior
# *  written consent of iCatch Technology Inc.
# *
# *  iCatch Technology Inc. reserves the right to modify this software
# *  without notice.
# *
# *  iCatch Technology Inc.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# **************************************************************************/
# \file   map_sym_extract.sh
# \brief  to print map file extern symbols and addresses to stdout
# \author Anmin Deng

# \param
# ${1} is the map file name, eg, "spca7002.map";
# ${2}, optional, is the obj file name, eg, "maskrom_app_cb.o".
#      if provided, only prints extern symbols of this obj in map;
#      if not provided, prints all the extern symbols in map
# \return
# exit code 0: success; non-zero: failed
# printing to stdout
# [0xaddr]\t[symbol]\n
# [0xaddr]\t[symbol]\n
# ...

## checking arguments

if [ $# != 1 ] && [ $# != 2 ] ; then
	echo "${0}: usage: \`mapfile' [\`objfile']" 1>&2
	exit 1
fi

map=${1}

if [ ! -f "${map}" ] || [ ! -r "${map}" ] ; then
	echo "${0}: \`${map}': map can not open" 1>&2
	exit 1
fi

if [ $# = 2 ] ; then
	obj=${2}
fi

x=[[:xdigit:]]

set -o pipefail

rm=/bin/rm
sed=/bin/sed
csplit=/usr/bin/csplit

# to cut the "because" and "memory" and "LOAD" parts in map file
$rm -f ${map}.sym*.bak ${map}.l.sym*.bak ${map}.o.sym*.bak &&
$sed -e 's/\r//g' ${map} |
$csplit -q -f "" -b ${map}.sym%02u.bak - '%^Linker script and memory map$%2' &&
$csplit -q -f "" -b ${map}.l.sym%02u.bak ${map}.sym00.bak '%^LOAD %' &&
$sed -e '/^LOAD .*/d' ${map}.l.sym00.bak |
(
if [ -n "${obj}" ] ; then
	# only extracting symbols of ${obj} in ${map}
	$csplit -q -f "" -b ${map}.o.sym%02u.bak - "/${obj}$/1" '{*}' &&
	$rm -f ${map}.o.sym00.bak &&
	for m in ${map}.o.sym*.bak ; do
		$sed -n -e "/^ \{16\}0x\(00000000\)\?$x\{8\} \{16\}[^ .=]*\( = .*\)\?$/!q" -e "s/^ \{16\}0x\(00000000\)\?\($x\{8\}\) \{16\}\([^ .=]*\)\( = .*\)\?$/0x\2\t\3/p" ${m}
		if [ $? != 0 ] ; then
			echo "${0}: \`${map}': \`${obj}': \`${m}': failed" 1>&2
			exit 1
		fi
		$rm -f ${m}
	done
else

	# extracting all symbols in ${map}
	$sed -n -e \
	"s/^ \{16\}0x\(00000000\)\?\($x\{8\}\) \{16\}\([^ .=]*\)\( = .*\)\?$/0x\2\t\3/p" -
fi
) &&
$rm -f ${map}.sym00.bak ${map}.l.sym00.bak

if [ $? != 0 ] ; then
	echo "${0}: \`${map}': extracting failed" 1>&2
	exit 1
fi
