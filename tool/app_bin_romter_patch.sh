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
# \file   app_bin_romter_patch.sh
# \brief  to stitch and to split maskrom and app build bin files and to patch
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

basename=/usr/bin/basename
dirname=/usr/bin/dirname
rm=/bin/rm
touch=/bin/touch
sed=/bin/sed
head=/usr/bin/head
tail=/usr/bin/tail
mv=/bin/mv
wc=/usr/bin/wc
cut=/usr/bin/cut
cp=/bin/cp
cat=/bin/cat

resnoext=`$basename ${1}` &&
resnoext=`$dirname ${1}`/${resnoext%%.*} &&
keypatch=`$dirname ${0}`/bkdoor_key_patch.sh &&
scriptpatch=`$dirname ${0}`/bkdoor_script_patch.sh

if [ $? != 0 ] || [ ! -x "${keypatch}" ] || [ ! -x "${scriptpatch}" ] ; then
	echo "${0}:${keypatch} ${scriptpatch}:invalid" 1>&2
	exit 1
fi

$rm -f ${result} ${resnoext}.* ${result}.bak &&
$touch ${result} ${result}.bak
if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${appbin}:${result}:create failed" 1>&2
	exit 1
fi

# ----- stitching maskrom bin and app bin ------------------------------
# maskrom bin: irom, dmem1, drom, otp; where dmem1 needs app cb patching
# app     bin: romter, imem, dmem2;
load=`$sed -n -e "s/^\(0x$x\{8\}\)\t_romter_load\$/\1/p" ${appsym}` &&
eload=`$sed -n -e "s/^\(0x$x\{8\}\)\t_eromter_load\$/\1/p" ${appsym}` &&
imem=`$sed -n -e "s/^\(0x$x\{8\}\)\t_text_app\$/\1/p" ${appsym}` &&
eimem=`$sed -n -e "s/^\(0x$x\{8\}\)\t_etext_app\$/\1/p" ${appsym}` &&
imemsz=$((eimem - imem))

if [ $? != 0 ] || [ $((imem)) = 0 ] || [ $((eimem)) = 0 ] \
|| [ $((imemsz)) = 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${appsym}:romter loader/imem invalid" 1>&2
	exit 1
fi

echo "${0}:${appsym}:load[${load}-${eload}] imem[${imem}-${eimem}]" 1>&2

_rom=`$sed -n -e "s/^\(0x$x\{8\}\)\t_rom\$/\1/p" ${romsym}` &&
lma=`$sed -n -e "s/^\(0x$x\{8\}\)\t_dram_content_lma_start\$/\1/p" ${romsym}` &&
vma=`$sed -n -e "s/^\(0x$x\{8\}\)\t_dram_content_vma_start\$/\1/p" ${romsym}` &&
ebase=`$sed -n -e "s/^\(0x$x\{8\}\)\t_eimage_base\$/\1/p" ${romsym}` &&
drom=`$sed -n -e "s/^\(0x$x\{8\}\)\t_rodata_base\$/\1/p" ${romsym}` &&
edrom=`$sed -n -e "s/^\(0x$x\{8\}\)\t_erodata_base\$/\1/p" ${romsym}`

if [ $? != 0 ] || [ $((_rom)) = 0 ] || [ $((lma)) = 0 ] || [ $((vma)) = 0 ] \
|| [ $((ebase)) = 0 ] || [ $((drom)) = 0 ] || [ $((edrom)) = 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${romsym}:irom/dmem invalid" 1>&2
	exit 1
fi

echo "${0}:${romsym}:irom[${_rom}-${lma}] dmem[${vma}-${ebase}] drom[${drom}-${edrom}]" 1>&2


# ----- splitting maskrom bin and app bin ------------------------------
# result  bin: romter, imem, irom, dmem1, dmem2, drom, otp
$head -q -c $((eload - load + imemsz)) ${appbin} > ${result} &&
$tail -q -c +$((eload - load + 1)) ${result} > ${resnoext}.IME.bak &&
$head -q -c $((lma - _rom)) ${rombin} > ${resnoext}.IRO &&
# tail large file and pipe to head on cygwin consumes too much resource,
# workaround by using head than pipe to tail
#tail -q -c +$((lma - _rom + 1)) ${rombin} | \
#head -q -c $((ebase - vma)) - > ${resnoext}.DME &&
$head -q -c $((lma - _rom + ebase - vma)) ${rombin} | \
$tail -q -c +$((lma - _rom + 1)) - > ${resnoext}.DME &&
$tail -q -c +$((eload - load + imemsz + 1)) ${appbin} >> ${resnoext}.DME &&
# tail large file and pipe to head on cygwin consumes too much resource,
# workaround by using head than pipe to tail
#tail -q -c +$((lma - _rom + ebase - vma + 1)) ${rombin} | \
#head -q -c $((edrom - drom)) - > ${resnoext}.DRO
$head -q -c $((lma - _rom + ebase - vma + edrom - drom)) ${rombin} | \
$tail -q -c +$((lma - _rom + ebase - vma + 1)) - > ${resnoext}.DRO

if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${result}:preliminary splitting failed" 1>&2
	exit 1
fi

# ----- shuffling imem bin into 8KB banks in reverse order -------------
imemszrem=$((imemsz % 8192)) &&
if [ $((imemszrem)) != 0 ] ; then
	((imemsz -= imemszrem)) #&&..if expression results 0, exit code nonzero
	$tail -q -c +$((imemsz + 1)) ${resnoext}.IME.bak > ${resnoext}.IME &&
	$head -q -c $((8192 - imemszrem)) /dev/zero >> ${resnoext}.IME
fi &&
while ((imemsz > 8192)) ; do
	((imemsz -= 8192)) &&
# tail large file and pipe to head on cygwin consumes too much resource,
# workaround by using head than pipe to tail
#	tail -q -c +$((imemsz + 1)) ${resnoext}.IME.bak | \
#	head -q -c 8192 - >> ${resnoext}.IME
	$head -q -c $((imemsz + 8192)) ${resnoext}.IME.bak | \
	$tail -q -c +$((imemsz + 1)) - >> ${resnoext}.IME
	if [ $? != 0 ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${resnoext}.IME:imem bank failed" 1>&2
		exit 1
	fi
done &&
if [ $((imemsz)) != 0 ] ; then
	$head -q -c 8192 ${resnoext}.IME.bak >> ${resnoext}.IME
fi &&
$rm -f ${resnoext}.IME.bak

if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${resnoext}.IME:imem shuffle failed" 1>&2
	exit 1
fi

# ----- patching fic dmem ------------------------------------------------
ficstart=`$sed -n -e "s/^\(0x$x\{8\}\)\t_fic_start_dmem\$/\1/p" ${appsym}`
if [ $? != 0 ] || [ $((ficstart)) = 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${appsym}:_fic_start_dmem:\`${ficstart}' failed" 1>&2
	exit 1
fi

ficsize=`$wc -c ${ficdmem} | $cut -d ' ' -f 1`
if [ $? = 0 ] && (( ficsize >= 16 )) ; then
	# valid ficdmem
	echo "${0}:${appsym}:ficstartdmem[${ficstart}] sz \`${ficsize}'" 1>&2

	offset=$(( ficstart - vma ))
	if [ $? != 0 ] || (( offset <= 0 )) ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${appsym}:ficdmem:offset\`${offset}' failed" 1>&2
		exit 1
	fi

	echo "${0}:ficstartdmem:ofs${offset}" 1>&2

	# to separate and dispose FICDMEM from DMEM image
	# FICDMEM is at the end of DMEM image
	$rm -f ${result}.bak &&
	$head -q -c ${offset} ${resnoext}.DME > ${result}.bak &&
	$rm -f ${resnoext}.DME &&
	$mv -f ${result}.bak ${resnoext}.DME
	if [ $? != 0 ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${resnoext}.DME:${ficdmem}:patch failed" 1>&2
		exit 1
	fi
else
	ficdmem=
fi

# ----- patching maskrom app cb pointers with app cb address values ------

$rm -f ${result}.bak &&
{ while read -r i ; do

	# finding app cb pointer key in maskrom map
	cbkey=${i:11}
	if [ $? != 0 ] || [ -z "${cbkey}" ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${romcbsym}:${i}:entry invalid" 1>&2
		exit 1
	fi

	# finding app cb definition in app map
	cbappaddr=`$sed -n -e "s/^\(0x$x\{8\}\)\t${cbkey}\$/\1/p" ${appsym}`
	if [ $? != 0 ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${appsym}:${cbkey}:failed" 1>&2
		exit 1
	fi
	if [ $((cbappaddr)) = 0 ] ; then
		echo "${0}:${appsym}:${cbkey}:undefined" 1>&2
		continue
	fi

	# patching maskrom cb pointer content with app definition address value

	${keypatch} -b 4 ${resnoext}.DME ${romcbsym} ${cbkey} 4 ${cbappaddr}

	if [ $? != 0 ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${resnoext}.DME:${cbkey}:patch failed" 1>&2
		exit 1
	fi

done } < ${romcbsym}

if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${resnoext}.DME:${romcbsym}:patch failed" 1>&2
	exit 1
fi

# ----- back door patching scripts ---------------------------------------
if [[ -f "${bkscript}" ]] && [[ -r "${bkscript}" ]] ; then
	${scriptpatch} ${resnoext}.DME ${romsym} ${appsym} < ${bkscript}

	if [ $? != 0 ] ; then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${resnoext}.DME:${bkscript}:bad scripts patch" 1>&2
		exit 1
	fi
fi

# ----- attaching fic ----------------------------------------------------
ficstart=`$sed -n -e "s/^\(0x$x\{8\}\)\t_fic_start\$/\1/p" ${romsym}` &&
if [[ -f "${fic}" ]] && [[ -r "${fic}" ]] ; then
	ficsize=`$wc -c ${fic} | $cut -d ' ' -f 1`
	if [ $? != 0 ] || [ $((ficstart)) = 0 ] || (( ficsize > 32 * 1024 ))
	then
		$rm -f ${result} ${resnoext}.* ${result}.bak
		echo "${0}:${romsym}:_fic_start/ficsize\`${ficsize}' fail" 1>&2
		exit 1
	fi

	echo "${0}:${romsym}:ficstart [${ficstart}] ficsize \`${ficsize}'" 1>&2

	$cp -f ${fic} ${resnoext}.OTP
fi &&
$head -q -c $((32 * 1024 - ficsize)) /dev/zero >> ${resnoext}.OTP

if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${resnoext}.OTP:${fic}:paste failed" 1>&2
	exit 1
fi

# ----- stitching maskrom bin and app bin ------------------------------
# result  bin: romter, imem, irom, dmem1, dmem2 (fic on dmem), drom, otp
$cat ${resnoext}.IRO  ${resnoext}.DME ${ficdmem} ${resnoext}.DRO \
		${resnoext}.OTP >> ${result}

if [ $? != 0 ] ; then
	$rm -f ${result} ${resnoext}.* ${result}.bak
	echo "${0}:${result}:stitching failed" 1>&2
	exit 1
fi
