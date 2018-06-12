#!/bin/bash

#/**************************************************************************
# *
# *         Copyright (c) 2013 by iCatch Technology Inc.
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
# \file   addr2func.sh
# \brief  to guess the function addresses and names of the addresses listed
# in arguments.
# \author Anmin Deng

# \param a list of hexadecimal numbers (without leading '0x') as addresses
# \note stardard input: symbol table of functions,
# in each text line of format
# "<addr> <func>", where <addr> is in hexadecimal number without '0x'

S=[^[:blank:]]

rm=/bin/rm
sed=/bin/sed
mktemp=/bin/mktemp
sort=/usr/bin/sort

set -o pipefail

declare -a addrlist

addr=
next=
func=
rest=

findaddr()
{
	local -i i="${1%%	*}"
	local -i p="${1#*	}"
	local err=0
	local buf
	while true ; do
		if (( p < next )) ; then
			printf '%02u %lx in %lx:%s\n' \
					${i} ${p} "${addr}" "${func:--}"
			return
		fi
		if [ "${err}" != 0 ] ; then
			# already EOF but still not found
			# can never happen since "next" is max (-1) at err!=0
			return ${err}
		fi

		addr=${next}
		if [ -n "${rest}" ] ; then
			func="${rest}"
		fi

		read -r buf
		err=$?
		if [ "${err}" != 0 ] ; then
			next=0xffffffff
		else
			if [ -z "${buf}" ] ; then
				return 1
			fi
			next=${buf%% *}
			rest="${buf#${next}}"
			rest="${rest# }"
			if [ -z "${next}" ] || [ -z "${rest}" ] ; then
				return 1
			fi
			next=0x${next}
		fi
	done
}

tmp=`$mktemp` &&
{ declare -i argc=0
for i in "$@" ; do
	i=${i#0x} &&
	i=0x${i} &&
	printf '%u\t%u\n' ${argc} "${i}" &&
	(( ++argc ))
	if [ $? != 0 ] ; then
		exit 1
	fi
done } | $sort -k 2n -s > ${tmp} &&
readarray -t addrlist < ${tmp}
if [ $? != 0 ] ; then
	if [ -n "${tmp}" ] ; then
		$rm -f ${tmp}
	fi
	exit 1
fi

$rm -f ${tmp} &&
for i in "${addrlist[@]}" ; do
	findaddr "${i}"
	if [ $? != 0 ] ; then
		exit 1
	fi
done
