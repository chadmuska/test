#!/bin/sh
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
# \file   bkdoor_key_patch.sh
# \brief  to patch dmem contents by the given variable key value
# \author Anmin Deng

# \param
# "-o <offset>", patching the contents at the address of symbol key + <offset>
# "-b <n>", each param ${5},[${6},...] represents <n> bytes, default "-b 1";
# "-e l" or "-e b", little or big endian, if -b multiple bytes, default "-e l";
# "-..." options must go before the following params
# ${1}, the dmem image;
# ${2}, the file of maskrom symbol list, derived from .map file, except app cb;
# ${3}, the symbol key to patch, only supporting global variables;
# ${4}, the size of key value, in bytes;
# ${5},[${6},...], the list of new key values, regarding to -b and -e options,
# or, if ${5} is "@<file>", the new key values are in a "t2b" formatted
# text file <file>, "@-" means new key values in stdin;
# \return
# exit code 0: success; non-zero: failed
# result direct patch to ${1}


# \brief to find symbol address value in .sym file, which was derived from
# .map file
# \param ${1}, the .sym file name
# \param ${2}, the key symbol name
# \return to print address value, in hex with prefix 0x, to <stdout>
sym_addr_find()
{
	local addr
	local sym=${1}
	local key=${2}
	addr=`$sed -n -e "s/^\(0x$x\{8\}\)\t${key}\$/\1/p" ${sym}`
	if [ $? != 0 ] || [ $((addr)) = 0 ] ; then
		echo "${0}:${sym}:\`${key}':sym failed to find" 1>&2
		exit 1
	fi
	echo "${addr}"
}


ofsarg=0
t2bargs=
t2bargb=1
while (( $# >= 5 )) ; do
	if [ "$1" = "-o" ] ; then
		ofsarg=$2
		shift 2
	elif [ "$1" = "-b" ] ; then
		t2bargb=$2
		t2bargs="${t2bargs} $1 $2"
		shift 2
	elif [ "$1" = "-e" ] ; then
		t2bargs="${t2bargs} $1 $2"
		shift 2
	else
		break
	fi
done

if (( $# < 5 )) || [[ "${5}" = @* ]] && [ $# != 5 ] ; then
	echo "${0}:[-o <ofs>] [-b <n>] [-e l/b] \`dmem' \`romsym' \`key' \`size' {\`val...' or \`@<file>'}" 1>&2
	exit 1
fi

if [ ! -f "${1}" ] || [ ! -r "${1}" ] || [ ! -w "${1}" ] \
|| [ ! -f "${2}" ] || [ ! -r "${2}" ] ; then
	echo "${0}:$1 $2:files invalid" 1>&2
	exit 1
fi

result=${1}
romsym=${2}
keynm=${3}
keysz=${4}
shift 4

if [[ "${1}" = @* ]] ; then
	t2bfile=${1#@}
	if [ "${t2bfile}" != "-" ] ; then
		if [ -z "${t2bfile}" ] \
		|| [ ! -f "${t2bfile}" ] || [ ! -r "${t2bfile}" ] ; then
			echo "${0}:\`${1}':t2b text file invalid" 1>&2
			exit 1
		fi
	fi
fi

if [ -z "${t2bfile}" ] && (( t2bargb * $# != keysz )) ; then
	echo "${0}:arg list \`$*':key size \`${keysz}' not matched" 1>&2
	exit 1
fi

x=[[:xdigit:]]
s=[[:blank:]]

wc=/usr/bin/wc
cut=/usr/bin/cut
dirname=/usr/bin/dirname
sed=/bin/sed

set -o pipefail

ressz=`$wc -c ${result} | $cut -d ' ' -f 1`
if [ $? != 0 ] || [ $((ressz)) = 0 ] ; then
	echo "${0}:${result}:\`${ressz}' file size invalid" 1>&2
	exit 1
fi

t2bpatch=`$dirname ${0}`/t2b_patch

if [ $? != 0 ] || [ ! -x "${t2bpatch}" ] ; then
	echo "${0}:${t2bpatch}:invalid" 1>&2
	exit 1
fi

# ----- finding maskrom dmem map base address ---------------------------

#vma=`sym_addr_find ${romsym} _dram_content_vma_start`
#if [ $? != 0 ] || [ $((vma)) = 0 ] ; then
#	echo "${0}:${romsym}:\`_dram_content_vma_start' invalid" 1>&2
#	exit 1
#fi
vma=0xd0000800

#echo "${0}:${romsym}:dmem[${vma}-]" 1>&2

# ----- patching maskrom variable values --------------------------------

# finding key address defined in maskrom map
keyaddr=`sym_addr_find ${romsym} ${keynm}`
if [ $? != 0 ] || [ $((keyaddr)) = 0 ] ; then
	echo "${0}:${romsym}:${keynm}:failed" 1>&2
	exit 1
fi

# patching maskrom variable values with the rest of arg list

offset=$(( keyaddr - vma + ofsarg ))

if (( offset < 0 )) || (( offset + keysz > ressz )) ; then
	echo "${0}:${keynm}:${keyaddr}+${ofsarg}:offset invalid" 1>&2
	exit 1
fi

echo "${0}:${keynm}:ofs${offset} [${keyaddr}]=$*" 1>&2

if [ -z "${t2bfile}" ] ; then
	echo "$*" | \
	$sed -e "s/^$s\+//" -e "s/$s\+$//" -e "s/$s\+/\n/g" | \
	${t2bpatch} ${result} ${offset} ${keysz} ${t2bargs}
else
	${t2bpatch} ${result} ${offset} ${keysz} ${t2bargs} ${t2bfile}
fi

if [ $? != 0 ] ; then
	echo "${0}:${result}:${keynm}:patch failed" 1>&2
	exit 1
fi
