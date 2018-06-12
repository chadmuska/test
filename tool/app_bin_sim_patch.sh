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
# \file   app_bin_sim_patch.sh
# \brief  to stitch maskrom and app build bin together and to patch
#     app cb pointer values and fic for simulated romter
# \author Anmin Deng

# \param
# ${1}, the result bin;
# ${2}, the app bin;
# ${3}, the app symbols;
# ${4}, the maskrom bin;
# ${5}, the maskrom symbols, except app cb;
# ${6}, the maskrom app cb symbols;
# ${7}, the fic;
# ${8}, the fic on dmem;
# ${9}, the back door patching scripts text file;
# \return
# exit code 0: success; non-zero: failed
# result bin written to ${1}


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


if [ "$#" != 9 ] ; then
	echo "${0}:usage:\`result' \`appbin' \`appsym' \`rombin' \`romsym' \`romcbsym' \`fic' \`ficdmem' \`bkscript'" 1>&2
	exit 1
fi

if [ ! -f "${2}" ] || [ ! -r "${2}" ] \
|| [ ! -f "${3}" ] || [ ! -r "${3}" ] \
|| [ ! -f "${4}" ] || [ ! -r "${4}" ] \
|| [ ! -f "${5}" ] || [ ! -r "${5}" ] \
|| [ ! -f "${6}" ] || [ ! -r "${6}" ] ; then
	echo "${0}:$@:files invalid" 1>&2
	exit 1
fi

result=${1}
appbin=${2}
appsym=${3}
rombin=${4}
romsym=${5}
romcbsym=${6}
fic=${7}
ficdmem=${8}
bkscript=${9}

x=[[:xdigit:]]

set -o pipefail

dirname=/usr/bin/dirname
sed=/bin/sed
rm=/bin/rm
cp=/bin/cp
head=/usr/bin/head
tail=/usr/bin/tail
mv=/bin/mv
wc=/usr/bin/wc
cut=/usr/bin/cut
cat=/bin/cat

t2bpatch=`$dirname ${0}`/t2b_patch

if [ $? != 0 ] || [ ! -x "${t2bpatch}" ] ; then
	echo "${0}:${t2bpatch}:invalid" 1>&2
	exit 1
fi

_rom=`sym_addr_find ${romsym} _rom` &&
lma=`sym_addr_find ${romsym} _rom _dram_content_lma_start` &&
vma=`sym_addr_find ${romsym} _rom _dram_content_vma_start`

if [ $? != 0 ] || [ $((_rom)) = 0 ] || [ $((lma)) = 0 ] || [ $((vma)) = 0 ]
then
	echo "${0}:${romsym}:invalid" 1>&2
	exit 1
fi

echo "${0}:${romsym}:_rom[${_rom}] lma[${lma}] vma[${vma}]" 1>&2

$rm -f ${result} ${result}.bak &&
$cp -f ${rombin} ${result}
if [ $? != 0 ] ; then
	echo "${0}:${rombin}:create failed" 1>&2
	exit 1
fi

# ----- patching _rom_app backdoor ram code ----------------------------
$head -q -c $(( lma - _rom )) ${result} > ${result}.bak &&
$head -q -c 1024 ${appbin} >> ${result}.bak &&
$tail -q -c +$((lma - _rom + 1024 + 1)) ${result} >> ${result}.bak &&
$rm -f ${result} &&
$mv -f ${result}.bak ${result}

if [ $? != 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${rombin}:${result}:patching .rom.app failed" 1>&2
	exit 1
fi

# ----- patching maskrom app cb pointers with app cb address values ------

$rm -f ${result}.bak &&
{ while read -r i ; do

	# finding app cb pointer key in maskrom map
	cbkey=${i:11} &&
	cbromaddr=${i:0:10}
	if [ $? != 0 ] || [ -z "${cbkey}" ] || [ $((cbromaddr)) = 0 ] ; then
		$rm -f ${result} ${result}.bak
		echo "${0}:${romcbsym}:${i}:entry invalid" 1>&2
		exit 1
	fi

	# finding app cb definition in app map
	cbappaddr=`sym_addr_find ${appsym} ${cbkey}`
	if [ $? != 0 ] ; then
		$rm -f ${result} ${result}.bak
		echo "${0}:${appsym}:${cbkey}:failed" 1>&2
		exit 1
	fi
	if [ $((cbappaddr)) = 0 ] ; then
		echo "${0}:${appsym}:${cbkey}:undefined" 1>&2
		continue
	fi

	# patching maskrom cb pointer content with app definition address value

	offset=$(( cbromaddr - vma + lma - _rom ))

	echo "${0}:${cbkey}:ofs${offset} [${cbromaddr}]=${cbappaddr}" 1>&2

	echo "${cbappaddr}" | \
	${t2bpatch} ${result} ${offset} 4 -b 4 -e l
	if [ $? != 0 ] ; then
		$rm -f ${result} ${result}.bak
		echo "${0}:${result}:${cbkey}:patch failed" 1>&2
		exit 1
	fi

done } < ${romcbsym}

if [ $? != 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${result}:${romcbsym}:patch failed" 1>&2
	exit 1
fi

# ----- back door patching scripts ---------------------------------------
echo "${0}:${resnoext}.DME:${bkscript}:scripts not supported on EVBSIM" 1>&2


# ----- patching fic -----------------------------------------------------
ficstart=`sym_addr_find ${romsym} _fic_start` &&
if [[ -f "${fic}" ]] && [[ -r "${fic}" ]] ; then
	ficsize=`$wc -c ${fic} | $cut -d ' ' -f 1`
	if [ $? != 0 ] || [ $((ficstart)) = 0 ] || (( ficsize > 32 * 1024 ))
	then
		$rm -f ${result} ${result}.bak
		echo "${0}:${romsym}:_fic_start/ficsize\`${ficsize}' fail" 1>&2
		exit 1
	fi

	echo "${0}:${romsym}:ficstart [${ficstart}] ficsize \`${ficsize}'" 1>&2

	$rm -f ${result}.bak &&
	$head -q -c $((ficstart - _rom)) ${result} > ${result}.bak &&
	$cat ${fic} >> ${result}.bak
else
	$rm -f ${result}.bak &&
	$head -q -c $((ficstart - _rom)) ${result} > ${result}.bak
fi &&
$head -q -c $((32 * 1024 - ficsize)) /dev/zero >> ${result}.bak &&
$tail -q -c +$((ficstart - _rom + 32 * 1024 + 1)) ${result} >> ${result}.bak &&
$rm -f ${result} &&
$mv -f ${result}.bak ${result}

if [ $? != 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${result}:${fic}:paste failed" 1>&2
	exit 1
fi

# ----- attaching app bin and patching fic dmem --------------------------

ebase=`sym_addr_find ${romsym} _eimage_base`
if [ $? != 0 ] || [ $((ebase)) = 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${romsym}:_eimage_base invalid" 1>&2
	exit 1
fi
echo "${0}:${romsym}:ebase [${ebase}]" 1>&2

ficstart=`sym_addr_find ${appsym} _fic_start_dmem`
if [ $? != 0 ] || [ $((ficstart)) = 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${appsym}:_fic_start_dmem:\`${ficstart}' failed" 1>&2
	exit 1
fi

ficsize=`$wc -c ${ficdmem} | $cut -d ' ' -f 1`
# ignoring errors, treated as empty fic dmem
if [ $? != 0 ] ; then
	ficsize=0
fi

$rm -f ${result}.bak &&
$head -q -c $((ebase - vma + lma - _rom)) ${result} > ${result}.bak &&
if (( ficsize >= 16 )) ; then
	# valid ficdmem
	echo "${0}:${appsym}:ficstartdmem[${ficstart}] sz \`${ficsize}'" 1>&2

	offset=$(( ficstart - ebase + 1024 ))
	if [ $? != 0 ] || (( offset <= 0 )) ; then
		$rm -f ${result} ${result}.bak
		echo "${0}:${appsym}:ficdmem:offset\`${offset}' failed" 1>&2
		exit 1
	fi

	echo "${0}:ficstartdmem:ofs${offset}" 1>&2

	$head -q -c ${offset} ${appbin} |
	$tail -q -c +$((1024 + 1)) - >> ${result}.bak &&
	$cat ${ficdmem} >> ${result}.bak &&
	$tail -q -c +$((offset + ficsize + 1)) ${appbin} >> ${result}.bak
else
	$tail -q -c +$((1024 + 1)) ${appbin} >> ${result}.bak
fi &&
$rm -f ${result} &&
$mv -f ${result}.bak ${result}

if [ $? != 0 ] ; then
	$rm -f ${result} ${result}.bak
	echo "${0}:${result}:${appbin}:attach failed" 1>&2
	exit 1
fi
