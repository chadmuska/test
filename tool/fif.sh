#!/bin/bash
#/**************************************************************************
# *
# *        Copyright (c) 2010-2012 by iCatch Technology Inc.
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
# *  Author: Anmin Deng
# *
# **************************************************************************/

# \brief to compose files specified by arguments into one FIF (Files In File)
# binary, or to list, verify, or extract individual files from an FIF binary.
# \param "-cachelinesize=CLS", optional, must be at the beginning of parameters
# if provided, to specify target chip cache line size in number of bytes,
# default 16 bytes;
# \param "-l", optional, to list entries in FIF ${1} if given;
# \param "-v[<path>]", optional, to verify entries in FIF ${1} and
# compare with files in <path> if given;
# \param "-x[<path>]", optional, to extract FIF ${1} to <path> if given;
# \param "-c[<rev>]", to compose files ${2}.. into FIF ${1};
# all the files ${2}.. names must be in DOS 8.3 format;
# <rev> is FIF revision, default rev=0;
# rev=0 supports indivdual entry files to be < 64KB;
# rev=1 supports indivdual entry files to be >= 64KB;
# if any entry file size >= 64KB detected,
# force to FIF1 regardless of -c option;
# option -l|-v|-x|-c is after -cachelinesize=CLS (if provided) and
# before FIF ${1};
# \param ${1} is the result FIF binary file name;
# \param ${2}.. remains are the files to be composed;

export LANG=C
export LC_ALL=C

usage()
{
	echo "${0}:options" 1>&2
	echo "  [-cachelinesize=\`CLS'] -l \`fif' : list" 1>&2
	echo "  [-cachelinesize=\`CLS'] -v[\`path'] \`fif' : verify" 1>&2
	echo "  [-cachelinesize=\`CLS'] -x[\`path'] \`fif' : extract" 1>&2
	echo "  [-cachelinesize=\`CLS'] -c[\`rev'] \`fif' \`files'... : compose" 1>&2
}

uname=/bin/uname
dirname=/usr/bin/dirname
sed=/bin/sed
sort=/usr/bin/sort
wc=/usr/bin/wc
cut=/usr/bin/cut
rm=/bin/rm
touch=/bin/touch
head=/usr/bin/head
tail=/usr/bin/tail
mkdir=/bin/mkdir
cat=/bin/cat
od=/usr/bin/od
cmp=/usr/bin/cmp

set -o pipefail

fifhdrsize=16
fifentrysize=16

if [ $# != 0 ] && [[ "${1}" = -cachelinesize=* ]] ; then
	cachelinesize=${1#-cachelinesize=}
	shift
else
	cachelinesize=16
fi

# ------- to list, verify, or extract -------
if [ $# != 0 ] && [[ "${1}" = "-l" || "${1}" = -v* || "${1}" = -x* ]] ; then
	if [ $# != 2 ] ; then
		usage
		exit 1
	fi

	fif=${2}
	if [ ! -f "${fif}" ] || [ ! -r "${fif}" ] ; then
		echo "${0}:\`${fif}':invalid FIF" 1>&2
		exit 1
	fi

	if [[ "${1}" = -v* ]] ; then
		xpath=${1#-v}
	elif [[ "${1}" = -x* ]] ; then
		xpath=${1#-x}
	fi
	if [ -z "${xpath}" ] ; then
		xpath="."
	fi

	fifsize=`$wc -c ${fif} | $cut -d ' ' -f 1`
	if [ $? != 0 ] || [ "${fifsize}" -lt "${fifhdrsize}" ] ; then
		echo "${0}:\`${fif}':fifsize \`${fifsize}'" 1>&2
		exit 1
	fi

	chipid=`$od -j2 -N2 -t u2 -An ${fif}` &&
	sumsize=`$od -j6 -N2 -t u2 -An ${fif}` &&
	sig=`$head -q -c 12 ${fif} | $tail -q -c +9 -` &&
	nfiles=`$od -j12 -N4 -t u4 -An ${fif}`
	if [ $? != 0 ] || (( chipid != 5110 )) || [[ "${sig}" != FIF* ]] ; then
		echo "${0}:\`${fif}':header failed" 1>&2
		$od -N8 -t u2 ${fif} 1>&2
		$od -j8 -N8 -t x1 ${fif} 1>&2
		exit 1
	fi
	rev=${sig#FIF}
	if [ "${rev}" != 0 ] && [ "${rev}" != 1 ] ; then
		echo "${0}:\`${fif}':rev\`${rev}' not supported" 1>&2
		exit 1
	fi
	if (( sumsize + 8 != fifsize )) ; then
		echo "${0}:\`${fif}':warning:fifsize not matching sumsize" 1>&2
	fi
	echo "\`${fif}':fifsize=${fifsize} rev=${rev}"
	echo "[header:chipid=$((chipid)) sumsize=$((sumsize)) sig=\`${sig}' nfiles=$((nfiles))]" 1>&2

	err=0
	fifofs=$(( (fifhdrsize + nfiles * fifentrysize + cachelinesize - 1) \
					& -cachelinesize ))
	i=0
	while (( i != nfiles )) ; do
		entryofs=$(( fifhdrsize + i * fifentrysize )) &&
# tail large file consumes too much resource, using head than tail
# file name only support 8.3
		if [ "${rev}" = 0 ] ; then
			fname=`$head -q -c $((entryofs + 14)) ${fif} | \
				$tail -q -c +$((entryofs + 1)) -` &&
			fsize=`$od -j$((entryofs + 14)) -N2 -t u2 -An ${fif}`
		else
			fname=`$head -q -c $((entryofs + 12)) ${fif} | \
				$tail -q -c +$((entryofs + 1)) -` &&
			fsize=`$od -j$((entryofs + 12)) -N4 -t u4 -An ${fif}`
		fi
		if [ $? != 0 ] ; then
			echo "${0}:\`${fif}':file[${i}]\`${fname}':entry failed" 1>&2
			exit 1
		fi

		echo "file[${i}]\`${fname}':ofs=$((fifofs)) size=$((fsize))" 1>&2

		if (( fifofs + fsize > fifsize )) ; then
			echo "${0}:\`${fif}':file[${i}]\`${fname}'@ofs${fifofs}:size failed" 1>&2
			exit 1
		fi

		# ------- to compare -------
		if [[ "${1}" = -v* ]] ; then
			fullname=${xpath}/${fname}
			echo " v file[${i}] with \`${fullname}'" 1>&2
			if [ ! -f "${fullname}" ] || [ ! -r "${fullname}" ]
			then
				echo "${0}:\`${fullname}':can not open" 1>&2
				(( ++err ))
			else
				$head -q -c $((fifofs + fsize)) ${fif} | \
				$tail -q -c +$((fifofs + 1)) - | \
				$cmp -b - ${fullname} 1>&2
				if [ $? != 0 ] ; then
					echo "${0}:\`${fif}':file[${i}]\`fullname':compare failed" 1>&2
					(( ++err ))
				fi
			fi

		# ------- to extract -------
		elif [[ "${1}" = -x* ]] ; then
			fullname=${xpath}/${fname}
			echo " x file[${i}] to \`${fullname}'" 1>&2
			if [ -e "${fullname}" ] ; then
				echo "${0}:\`${fullname}':exists, stopped" 1>&2
				(( ++err ))
			else
				$mkdir -p `$dirname ${fullname}` &&
				$head -q -c $((fifofs + fsize)) ${fif} | \
				$tail -q -c +$((fifofs + 1)) - > ${fullname}
				if [ $? != 0 ] ; then
					echo "${0}:\`${fif}':file[${i}]\`fullname':extract failed" 1>&2
					(( ++err ))
				fi
			fi
		fi

		(( fifofs += (fsize + cachelinesize - 1) & -cachelinesize ))
		(( ++i ))
	done

	if (( err != 0 )) ; then
		echo "${0}:\`${fif}':failed" 1>&2
		exit 1
	fi
	if (( fifofs != fifsize )) ; then
		echo "${0}:\`${fif}':size mismatches" 1>&2
		exit 1
	fi

	exit 0
fi

if [ $# -lt 3 ] || [[ "${1}" != -c* ]] ; then
	usage
	exit 1
fi

rev=${1#-c}
# default creating rev0
if [ -z "${rev}" ] ; then
	rev=0
fi
# only supporting rev0 and rev1 for now
if [ "${rev}" != 0 ] && [ "${rev}" != 1 ] ; then
	echo "${0}:\`${1}':FIF rev not supported" 1>&2
	usage
	exit 1
fi

# ------- to create -------
fif=${2}
fifbak=${fif}.bak
echo "${0}:\`${fif}':creating rev${rev}" 1>&2

s=[[:blank:]]

if [ "`$uname`" = Linux ] ; then
	toolpath_suffix=".linux"
else
	toolpath_suffix=
fi
toolpath=`$dirname ${0}` &&
toolpath=${toolpath}${toolpath_suffix} &&
t2b=${toolpath}/t2b
if [ $? != 0 ] || [ ! -x "$t2b" ] ; then
	echo "${0}:\`${t2b}':invalid" 1>&2
	exit 1
fi

shift 2
nfiles=0
sumsize=0
files=`echo $* | $sed -e "s/^$s\+//" -e "s/$s\+/\n/g" | $sort -u` &&
for f in ${files} ; do
	if [ ! -f "${f}" ] || [ ! -r "${f}" ] ; then
		echo "${0}:\`${f}':bin can not open" 1>&2
		exit 1
	fi &&
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1` &&
	if [ "${rev}" = 0 ] && (( fsize >= 65536 )) ; then
		echo "${0}:\`${f}':size\`${fsize}' FIF0 unfit, force FIF1" 1>&2
		rev=1
	fi &&
	ff=${f##*/} &&
	ff=`$sed -e 's/.*/\U&/g' <<< "${ff}"` &&  # uppercases
	ffbase=${ff%%.*} &&
	ffext=${ff#${ffbase}} &&
	ffext=${ffext#.} &&
	if [ -z "${ffbase}" ] || [ "${#ffbase}" -gt 8 ] \
	|| [ "${#ffext}" -gt 3 ] ; then
		echo "${0}:\`${ff}':file name not 8.3" 1>&2
		exit 1
	fi &&
	(( ++nfiles )) &&
	(( sumsize += ( fsize + cachelinesize - 1 ) & -cachelinesize ))
	# zero size file may have problems when sumsize==0
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file error" 1>&2
		exit 1
	fi
done
if [ $? != 0 ] ; then
	echo "${0}:fif entries error" 1>&2
	exit 1
fi

if [ ${nfiles} = 0 ] ; then
	echo "${0}:no fif" 1>&2
	exit 0
fi

hdrentryend=$(( fifhdrsize + nfiles * fifentrysize ))
foffset=$(( ( hdrentryend + cachelinesize - 1 ) & -cachelinesize ))
hdrentryfill=$(( foffset - hdrentryend ))
(( sumsize += foffset - 8 ))
if (( sumsize >= 65536 )) ; then
	echo "${0}:warning:fif sumsize=$((sumsize)) too large" 1>&2
	sumsize=65535
fi


# FIF header
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
echo -e "0\n5110\n0\n$((sumsize))" | $t2b -q -b 2 - ${fifbak} &&
echo -n "FIF${rev}" >> ${fifbak} &&
echo "${nfiles}" | $t2b -q -a -b 4 - ${fifbak}
if [ $? != 0 ] ; then
	echo "${0}:\`${fifbak}':fif header error" 1>&2
	exit 1
fi

# FIF entries
for f in ${files} ; do
	ff=${f##*/} &&
	ff=`$sed -e 's/.*/\U&/g' <<< "${ff}"` &&  # uppercases
	ffbase=${ff%%.*} &&
	ffext=${ff#${ffbase}} &&
	ffext=${ffext#.} &&
	if [ -z "${ffext}" ] ; then
		ff=${ffbase}
	else
		ff=${ffbase}.${ffext}
	fi &&
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1`
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file error" 1>&2
		exit 1
	fi
	echo -n "${ff}" >> ${fifbak} &&
	fnamelen=${#ff} &&
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
	if [ "${rev}" = 0 ] ; then
		fillsize=$(( 14 - fnamelen )) &&
		$head -c ${fillsize} /dev/zero >> ${fifbak} &&
		echo "${fsize}" | $t2b -q -a -b 2 - ${fifbak}
	else
		fillsize=$(( 12 - fnamelen )) &&
		$head -c ${fillsize} /dev/zero >> ${fifbak} &&
		echo "${fsize}" | $t2b -q -a -b 4 - ${fifbak}
	fi
	if [ $? != 0 ] ; then
		echo "${0}:\`${ff}':fif entry error" 1>&2
		exit 1
	fi
done

$head -c ${hdrentryfill} /dev/zero >> ${fifbak}
if [ $? != 0 ] ; then
	echo "${0}:\`${fifbak}':fif entry end aligning error" 1>&2
	exit 1
fi

# FIF contains
for f in ${files} ; do
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1` &&
	fillsize=$((((fsize + cachelinesize - 1) & -cachelinesize) - fsize)) &&
	$cat ${f} >> ${fifbak} &&
	$head -c ${fillsize} /dev/zero >> ${fifbak}
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':fif contains error" 1>&2
		exit 1
	fi
	echo " + \`${f}'" 1>&2
done

mv ${fifbak} ${fif}
if [ $? != 0 ] ; then
	echo "${0}:\`${fif}':fif mv failed" 1>&2
	exit 1
fi
