#!/bin/bash
#/**************************************************************************
# *
# *      Copyright (c) 2010-2012 by Sunplus mMedia Inc., Ltd.
# *
# *  This software is copyrighted by and is the property of Sunplus
# *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
# *  Inc., Ltd. This software may only be used in accordance with the
# *  corresponding license agreement. Any unauthorized use, duplication,
# *  distribution, or disclosure of this software is expressly forbidden.
# *
# *  This Copyright notice may not be removed or modified without prior
# *  written consent of Sunplus mMedia Inc., Ltd.
# *
# *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
# *  software without notice.
# *
# *  Sunplus mMedia Inc., Ltd.
# *  19-1, Innovation First Road, Science-Based Industrial Park,
# *  Hsin-Chu, Taiwan.
# *
# *  Author: Anmin Deng
# *
# **************************************************************************/

# to print multiboot loading info to stdout according to stage image sizes
# \param [options] must be provided before parameters
#    [-v], verbose;
#    [-b], reporting offset/size in bytes (default in pages);
#    [-k], reporting offset/size in Kbytes (default in pages);
#    [-x], reporting values in hexadecimal instead of in decimal;
# \param ${1} is the .elf file
# \param ${2} is the stagelist
# \param ${3}, the system building environment prefix path
# \param ${4}, optional, multiboot edges aligned to number of pages, default 1

# \note an intermediate file "${1}.sym.bak" will be created by
# "objdump -t ${1} > ${1}.sym.bak" for finding symbol addresses and future use.

usage()
{
	echo "usage:${0} [options] \`elf' \"\`stagelist'\" \`prefix' [\`npage']" 1>&2
	echo '[-v]:verbose; [-b]:in bytes; [-k]:in KB; [-x]:hexadecimal' 1>&2
	exit 1
}

prefix=`dirname $0` &&
symaddrfind=$prefix/sym_addr_find.sh
if [ $? != 0 ] || [ ! -e "$symaddrfind" ] ; then
	echo "$0:failed to tool" 1>&2
	exit 1
fi

scale=512

while [ $# != 0 ] ; do
	case "x${1}" in
	x-v )
		opt_verbose=1
		;;
	x-b )
		scale=1
		;;
	x-k )
		scale=1024
		;;
	x-x )
		opt_hex=1
		;;
	x-* )
		usage
		;;
	x* )
		break
		;;
	esac
	shift
done

if [ "$#" != 3 ] && [ "$#" != 4 ] ; then
	usage
fi

elf=${1}
stagelist=${2}
objdump=${3}objdump
if [ "$#" = 3 ] ; then
	npage=1
else
	npage=${4}
	if (( npage == 0 )) ; then
		echo "${0}:\`${npage}':bad npage" 1>&2
		exit 1
	fi
fi

if [ ! -f "${elf}" ] || [ ! -r "${elf}" ] ; then
	echo "${0}:\`${elf}':bad elf" 1>&2
	exit 1
fi

x=[[:xdigit:]]
s=[[:blank:]]
e=[[:space:]]

sed=/bin/sed
rm=/bin/rm

(( align = npage * 512 ))

# \param ${1} is the symbol string
# \param ${2} "up": to align up to page idx; "down": to align down to page idx;
# <pageidx>: to align up to page idx but not exceeding ${2} <pageidx>;
# N/A: no aligned address of symbol ${1}
find_symbol_addr()
{
	local sym=${1}
	local addr=`${symaddrfind} -o ${elf}.sym.bak ${sym}`
	# addr == 0 is supported
	if [ $? != 0 ] || [ -z "${addr}" ] || (( addr < 0 )) ; then
		echo "${0}:\`${elf}':\`${sym}':bad symbol" 1>&2
		return 1
	fi

	if [ -n "${2}" ] ; then
		if [ "${2}" = "down" ] ; then
			(( addr = addr / align * npage ))
		elif [ "${2}" = "up" ] ; then
			(( addr = ( addr + align - 1 ) / align * npage ))
		else
			(( addr = ( addr + align - 1 ) / align * npage ))
			if (( "${addr}" > "${2}" )) ; then
				addr=${2}
			fi
		fi
	fi
	printf '%#x\n' ${addr}
}

# main

if [ "${elf}.sym.bak" -ot "${elf}" ] ; then
	$rm -f ${elf}.sym.bak
	if [ ! -x "${objdump}" ] ; then
		echo "${0}:\`${objdump}':bad tool chain" 1>&2
		exit 1
	fi
	${objdump} -t ${elf} > ${elf}.sym.bak
	if [ $? != 0 ] ; then
		echo "${0}:objdump:\`${elf}':failed" 1>&2
		exit 1
	fi
fi

# not supporting multiload, only to use FIC
if [ -z "${stagelist}" ] ; then
	echo "0" &&
	find_symbol_addr _fic_start &&
	echo -e "0\n0"
	if [ $? != 0 ] ; then
		echo "${0}:fic only failed" 1>&2
		exit 1
	fi
	exit
fi

stagearray=(${stagelist})
nstage=${#stagearray[*]}
stagemask=0

if [ -n "${opt_verbose}" ] ; then
	echo -n "multiload ${nstage} stages:" 1>&2
fi

for stage in ${stagelist} ; do
	(( stagemask |= 1 << (stage - 1) ))
	if [ -n "${opt_verbose}" ] ; then
		echo -n " ${stage}" 1>&2
	fi
done

if [ -n "${opt_verbose}" ] ; then
	printf ': stagemask %#.8x\n' ${stagemask} 1>&2
fi

etext=`find_symbol_addr _etext down` &&
erodata=`find_symbol_addr _erodata down` &&
edata=`find_symbol_addr _edata down`
if [ $? != 0 ] || [ -z "${etext}" ] || (( "${etext}" <= 0 )) \
|| [ -z "${erodata}" ] || (( "${erodata}" <= 0 )) \
|| [ -z "${edata}" ] || (( "${edata}" <= 0 )) ; then
	echo "${0}:\`${elf}':_etext,_erodata,_edata:bad symbol" 1>&2
	exit 1
fi

if (( "${etext}" > "${edata}" )) ; then
	etext=${edata}
fi
if (( "${erodata}" > "${edata}" )) ; then
	erodata=${edata}
fi

find_symbol_addr _multiboot_load_info &&
find_symbol_addr _fic_start &&
echo ${nstage} &&
printf '%#x\n' ${stagemask} &&
echo "${edata}" &&
esdata=`find_symbol_addr _bssStart up` && echo "${esdata}" &&
ebkdoor=`find_symbol_addr _multiboot_start down` && echo "${ebkdoor}" &&
echo "${etext}" &&
echo "${erodata}" &&
bss=`find_symbol_addr _bssStart` && echo "${bss}"
if [ $? != 0 ] ; then
	echo "${0}:bad beginning" 1>&2
	exit 1
fi

if [ -n "${opt_verbose}" ] ; then
	if [ -z "${opt_hex}" ] ; then
		v='%9u'
	else
		v='%9x'
	fi

	printf "sdata@${v} sz${v}\n" \
		$((edata*512/scale)) $(((esdata - edata)*512/scale)) 1>&2
	echo 'stg   text@       sz  rodata@       sz    data@       sz     bss@       sz' 1>&2

	pretext=${ebkdoor}
	prerodata=${etext}
	predata=${erodata}
	prebss=${bss}
fi

declare -a etext_addr_list

for stage in ${stagelist% [0-9]*} ; do
	etext_addr_list[${#etext_addr_list[*]}]=`find_symbol_addr \
		_etext.stage${stage}` &&
	curtext=`find_symbol_addr _etext.stage${stage} ${etext}` &&
	echo "${curtext}" &&
	currodata=`find_symbol_addr _erodata.stage${stage} ${erodata}` &&
	echo "${currodata}" &&
	curdata=`find_symbol_addr _edata.stage${stage} ${edata}` &&
	echo "${curdata}" &&
	curbss=`find_symbol_addr _ebss.stage${stage}` &&
	echo "${curbss}"
	if [ $? != 0 ] ; then
		echo "${0}:bad stage \`${stage}'" 1>&2
		exit 1
	fi

	if [ -n "${opt_verbose}" ] ; then
		printf "%2u${v}${v}${v}${v}${v}${v}${v}${v}\n" ${stage} \
		$((pretext*512/scale)) $(((curtext-pretext)*512/scale)) \
		$((prerodata*512/scale)) $(((currodata-prerodata)*512/scale)) \
		$((predata*512/scale)) $(((curdata-predata)*512/scale)) \
		$((prebss/scale)) $(((curbss-prebss)/scale)) 1>&2

		pretext=${curtext}
		prerodata=${currodata}
		predata=${curdata}
		prebss=${curbss}
	fi
done

echo "${etext}" &&
echo "${erodata}" &&
echo "${edata}" &&
ebss=`find_symbol_addr _bssEnd` && echo "${ebss}"
if [ $? != 0 ] ; then
	echo "${0}:bad last stage" 1>&2
	exit 1
fi

if [ -n "${opt_verbose}" ] ; then
	printf "32${v}${v}${v}${v}${v}${v}${v}${v}\n" \
	$((pretext*512/scale)) $(((etext-pretext)*512/scale)) \
	$((prerodata*512/scale)) $(((erodata-prerodata)*512/scale)) \
	$((predata*512/scale)) $(((edata-predata)*512/scale)) \
	$((prebss/scale)) $(((ebss-prebss)/scale)) 1>&2
	echo -n 'etext addr' 1>&2
fi

for p in ${etext_addr_list[*]} ; do
	echo "${p}"
	if [ -n "${opt_verbose}" ] ; then
		echo -n " ${p}" 1>&2
	fi
done
if [ -n "${opt_verbose}" ] ; then
	echo 1>&2
fi
