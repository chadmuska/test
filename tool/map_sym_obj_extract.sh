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
# \file   map_sym_obj_extract.sh
# \brief  to split all the extern symbols and addresses in map file
#    to 2 specific output files,
#    one with the symbols in only the specific obj,
#    the other with all the symbols except in the specific obj
# \author Anmin Deng

# \param
# ${1} is the map file name, eg, "spca7002.map";
# ${2} is the obj file name, eg, "maskrom_app_cb.o";
# ${3} output file name, with the symbols in only ${2} obj;
# ${4} output file name, with the symbols not in ${2} obj;
# \return
# exit code 0: success; non-zero: failed
# output format
# [0xaddr]\t[symbol]\n
# [0xaddr]\t[symbol]\n
# ...

## checking arguments

if [ $# != 4 ] ; then
	echo "${0}: usage: \`map' \`obj' \`inc' \`exc'" 1>&2
	exit 1
fi

map=${1}
obj=${2}
inc=${3}
exc=${4}

if [ ! -f "${map}" ] || [ ! -r "${map}" ] ; then
	echo "${0}: \`${map}': map can not open" 1>&2
	exit 1
fi

x=[[:xdigit:]]

set -o pipefail

rm=/bin/rm
sed=/bin/sed
csplit=/usr/bin/csplit
touch=/bin/touch

# to cut the "because" and "memory" and "LOAD" parts in map file
$rm -f ${map}.sym*.bak ${map}.l.sym*.bak ${map}.o.sym*.bak &&
$sed -e 's/\r//g' ${map} |
$csplit -q -f "" -b ${map}.sym%02u.bak - '%^Linker script and memory map$%2' &&
$csplit -q -f "" -b ${map}.l.sym%02u.bak ${map}.sym00.bak '%^LOAD %' &&
$sed -e '/^LOAD .*/d' ${map}.l.sym00.bak |
$csplit -q -f "" -b ${map}.o.sym%02u.bak - "/${obj}$/1" '{*}' &&
$rm -f ${map}.sym00.bak ${map}.l.sym00.bak

if [ $? != 0 ] ; then
	echo "${0}: \`${map}': \`${obj}': extracting failed" 1>&2
	exit 1
fi

$rm -f ${inc} ${exc} &&
$touch ${inc} ${exc}
if [ $? != 0 ] ; then
	echo "${0}: \`${inc}' \`${ext}': creating failed" 1>&2
	exit 1
fi

m=${map}.o.sym00.bak
$sed -n -e "s/^ \{16\}0x\(00000000\)\?\($x\{8\}\) \{16\}\([^ .=]*\)\( = .*\)\?$/0x\2\t\3/p" \
		${m} >> ${exc}
if [ $? != 0 ] ; then
	echo "${0}:\`${map}':\`${obj}':\`${exc}':\`${m}':failed" 1>&2
	exit 1
fi
$rm -f ${m}

for m in ${map}.o.sym*.bak ; do
	$sed -n -e "/^ \{16\}0x\(00000000\)\?$x\{8\} \{16\}[^ .=]*\( = .*\)\?$/!q" \
	-e "s/^ \{16\}0x\(00000000\)\?\($x\{8\}\) \{16\}\([^ .=]*\)\( = .*\)\?$/0x\2\t\3/p" \
		${m} >> ${inc}
	if [ $? != 0 ] ; then
		echo "${0}: \`${map}':\`${obj}':\`${inc}':\`${m}':failed" 1>&2
		exit 1
	fi

	c=`$sed -n -e "/^ \{16\}0x\(00000000\)\?$x\{8\} \{16\}[^ .=]*\( = .*\)\?$/!{
		=
		q
	}" ${m}`
	if [ $? != 0 ] ; then
		echo "${0}: \`${map}':\`${obj}':\`${exc}':\`${m}':failed" 1>&2
		exit 1
	fi
	if [ -n "${c}" ] ; then
		$sed -n -e "${c},\$s/^ \{16\}0x\(00000000\)\?\($x\{8\}\) \{16\}\([^ .=]*\)\( = .*\)\?$/0x\2\t\3/p" ${m} >> ${exc}
		if [ $? != 0 ] ; then
			echo "${0}: \`${map}':\`${obj}':\`${exc}':\`${m}'" 1>&2
			exit 1
		fi
	fi
	$rm -f ${m}
done
