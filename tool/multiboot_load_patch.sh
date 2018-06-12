#!/bin/bash
#/**************************************************************************
# *
# *        Copyright (c) 2010 by Sunplus mMedia Inc., Ltd.
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

# to patch multiboot loading info to project image binary
# \param ${1} is the project image binary file
# \param ${2} is the multiboot load info file from 'multiboot_load_info.sh',
#  '-':<stdin>
# \param ${3} optional, is the fic binary to paste to ${1}
# results written to standard output

if [ "$#" != 2 ] && [ "$#" != 3 ] ; then
	echo "usage:${0} \`img' \`mload' [\`fic']" 1>&2
	exit 1
fi

img=${1}
mload=${2}

if [ ! -f "${img}" ] || [ ! -r "${img}" ] ; then
	echo "${0}:\`${img}':image can not open" 1>&2
	exit 1
fi

mloadfd=
if [ "${mload}" = "-" ] ; then
	mloadfd=0
else
	if [ ! -f "${mload}" ] || [ ! -r "${mload}" ] ; then
		echo "${0}:\`${mload}':multiload info can not open" 1>&2
		exit 1
	fi
	exec 6< "${mload}"
	mloadfd=6
fi

uname=/bin/uname
sed=/bin/sed
wc=/usr/bin/wc
cut=/usr/bin/cut
head=/usr/bin/head
tail=/usr/bin/tail
cat=/bin/cat
rm=/bin/rm
dirname=/usr/bin/dirname

read -u ${mloadfd} offset &&
read -u ${mloadfd} ficstart &&
read -u ${mloadfd} nstage

if [ -z "${offset}" ] || [ -z "${ficstart}" ] || [ -z "${nstage}" ] ; then
	echo "${0}:\`${mload}':multiload info invalid" 1>&2
	exit 1
fi

if [ "${nstage}" != 0 ] ; then
	if (( offset <= 0 )) || [ "${nstage}" -lt 2 ] ; then
		echo "${0}:\`${mload}':\`${offset}',\`${nstage}':invalid" 1>&2
		exit 1
	fi

	infosize=$(( ${nstage} * 24 + 28 ))
fi


set -o pipefail


if [ "$#" = 3 ] ; then
	fic=${3}
	if [ ! -f "${fic}" ] || [ ! -r "${fic}" ] ; then
		echo "${0}:\`${fic}':fic can not open" 1>&2
		exit 1
	fi
	# paste ${fic} to addr of "_fic_start" recorded at line 2 of ${mload}
	if (( ficstart <= 0 )) ; then
		echo "${0}:\`${mload}':L2:\`${ficstart}':_fic_start invalid" \
			1>&2
		exit 1
	fi
	ficsize=`$wc -c ${fic} | $cut -d ' ' -f 1`
	if [ $? != 0 ] || (( ficsize < 16 )) ; then
		ficsize=0
	fi
#	fictailoffset=$((ficstart - offset - infosize))
#	if [ "${fictailoffset}" -le 0 ] ; then
#		echo "${0}:\`${mload}':\`${ficstart}':fic addr invalid" 1>&2
#		exit 1
#	fi
fi

if [ "`$uname`" = Linux ] ; then
	toolpath_suffix=".linux"
else
	toolpath_suffix=
fi
toolpath=`$dirname ${0}` &&
toolpath=${toolpath}${toolpath_suffix} &&
t2b=${toolpath}/t2b
if [ $? != 0 ] || [ ! -x "${t2b}" ] ; then
	echo "${0}:\`${t2b}':invalid" 1>&2
	exit 1
fi
j=0
if [ "${nstage}" != 0 ] ; then
	echo "${0}:${img}:ofs${offset} nstage${nstage} size${infosize}" 1>&2

	$head -q -c $((offset)) ${img} &&
# t2b direct to stdout may convert \n to \r\n, workaround to output to tmpfile
	#$tail -q -n +3 ${mload} | ..
	{ echo "${nstage}" ;
	for (( i = infosize - ((${nstage}+1) * 4) ; i != 0 ; i -= 4 )) ; do
		read -u ${mloadfd} ofs
		if [ $? != 0 ] || [ -z "$ofs" ] ; then
			echo "${0}:${mload}:multiload infosize unfit" 1>&2
			exit 1
		fi
		echo "${ofs}"
	done ;
#	reserve data for multiboot checksum (data size = nstage * 4bytes)
	for (( i = 0 ; i < ${nstage} ; i ++ )) ; do
		echo "${j}"
	done ;
	}|${t2b} -b 4 -e l - ${img}.mload.bak &&
	$cat ${img}.mload.bak
	$rm -f ${img}.mload.bak
fi &&
if [ "$#" = 3 ] && [ "${ficsize}" != 0 ] ; then
# tail large file to pipe on cygwin has something wrong, dirty workaround
#	tail -q -c +$((${offset} + ${infosize} + 1)) ${img} | \
#	head -q -c ${fictailoffset} - &&
# tail large file consumes too much resource, using head than tail
#	tail -q -c +$((offset + infosize + 1)) ${img} > ${img}.bak &&
#	head -q -c ${fictailoffset} ${img}.bak &&
#	rm -f ${img}.bak &&

	if [ "${nstage}" != 0 ] ; then
		$head -q -c $((ficstart)) ${img} | \
		$tail -q -c +$((offset + infosize + 1)) -
	else
		$head -q -c $((ficstart)) ${img}
	fi &&
	$cat ${fic} &&
	$tail -q -c +$((ficstart + ficsize + 1)) ${img} &&
	echo "${0}:\`${img}':fic\`${fic}':paste ofs${ficstart} size${ficsize}" \
		1>&2
elif [ "${nstage}" != 0 ] ; then
	$tail -q -c +$((${offset} + ${infosize} + 1)) ${img}
else
	$cat ${img} # no multiload nor fic
fi
