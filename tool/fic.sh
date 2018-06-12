#!/bin/bash
#/**************************************************************************
# *
# *        Copyright (c) 2010-2016 by iCatch Technology Inc.
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

# to compose files specified by arguments into one FIC (Files In Code) binary,
# or to extract individual files from given FIC binary.
# \param "-v", to print version;
# \param "-cachelinesize=CLS", optional, must be at the beginning of parameters
# if provided, to specify target chip cache line size in number of bytes,
# default 16 bytes;
# \param "-l", optional, to list entries in FIC ${1} if given;
# \param "-t[<path>]", optional, to test entries in FIC ${1} and
# compare with files in <path> if given;
# \param "-x[<path>]", optional, to extract FIC ${1} to <path> if given;
# option -l|-t|-x is after -cachelinesize=CLS (if provided) and before FIC ${1};
# to compose files ${2}.. into FIC ${1} if -l, -x, or -t not given;
# \param ${1} is the result FIC binary file name;
# \param ${2}.. remains are the files to be merged;

ver="0.4"

# history
# 0.4 20170203
#   workaround fixing file sorting order different at different locale.
# 0.3 20160909
#   fixing file contents can be not aligned if cachelinesize > 32.
# 0.2 supporting test options -l, -t, -x.
# 0.1 supporting different cachelinesize than 16 as an option.
# 0.0 initial release.

export LANG=C
export LC_ALL=C

usage()
{
	echo "${0}:options" 1>&2
	echo "  -v : version" 1>&2
	echo "  [-cachelinesize=\`CLS'] -l \`fic' : list" 1>&2
	echo "  [-cachelinesize=\`CLS'] -t[\`path'] \`fic' : test" 1>&2
	echo "  [-cachelinesize=\`CLS'] -x[\`path'] \`fic' : extract" 1>&2
	echo "  [-cachelinesize=\`CLS'] \`fic' \`files'... : compose" 1>&2
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

if [ $# != 0 ] && [[ "${1}" = -cachelinesize=* ]] ; then
	cachelinesize=${1#-cachelinesize=}
	shift
else
	cachelinesize=16
fi

if [ $# != 0 ] && [ "${1}" = -v ] ; then
	echo "${0}:ver ${ver}" 1>&2
	exit 0
fi

if [ "${cachelinesize}" -lt 32 ] ; then
	fichdrsize=${cachelinesize}
else
	fichdrsize=32
fi
ficentrysize=32

# ------- to test or extract -------
if [ $# != 0 ] && [[ "${1}" = "-l" || "${1}" = -t* || "${1}" = -x* ]] ; then
	if [ $# != 2 ] ; then
		usage
		exit 1
	fi

	fic=${2}
	if [ ! -f "${fic}" ] || [ ! -r "${fic}" ] ; then
		echo "${0}:\`${fic}':invalid FIC" 1>&2
		exit 1
	fi
	if [ ! -s "${fic}" ] ; then
		echo "${0}:\`${fic}':zero size" 1>&2
		exit 0
	fi

	if [[ "${1}" = -t* ]] ; then
		xpath=${1#-t}
	elif [[ "${1}" = -x* ]] ; then
		xpath=${1#-x}
	fi
	if [ -z "${xpath}" ] ; then
		xpath="."
	fi

	ficsize=`$wc -c ${fic} | $cut -d ' ' -f 1`
	if [ $? != 0 ] || [ "${ficsize}" -lt "${fichdrsize}" ] ; then
		echo "${0}:\`${fic}':ficsize \`${ficsize}'" 1>&2
		exit 1
	fi

	sumsize=`$od -j0 -N4 -t u4 -An ${fic}` &&
	nfiles=`$od -j4 -N4 -t u4 -An ${fic}`
	if [ $? != 0 ] || (( sumsize != ficsize )) ; then
		echo "${0}:\`${fic}':header \`$((sumsize))' \`$((nfiles))'" 1>&2
		exit 1
	fi
	echo "\`${fic}':ficsize=${ficsize} [header:sumsize=$((sumsize)) nfiles=$((nfiles))]" 1>&2

	err=0
	ficofs=$(( (fichdrsize + nfiles * ficentrysize + cachelinesize - 1) \
					& -cachelinesize ))
	i=0
	while (( i != nfiles )) ; do
		entryofs=$(( fichdrsize + i * ficentrysize )) &&
		fofs=`$od -j$((entryofs)) -N4 -t u4 -An ${fic}` &&
		fsize=`$od -j$((entryofs + 4)) -N4 -t u4 -An ${fic}` &&
# tail large file consumes too much resource, using head than tail
		fname=`$head -q -c $((entryofs + 32)) ${fic} | \
			$tail -q -c +$((entryofs + 8 + 1)) -` &&
		fname=${fname/\\/\/}
		if [ $? != 0 ] ; then
			echo "${0}:\`${fic}':file[${i}]\`${fname}':entry failed" 1>&2
			exit 1
		fi

# seeking large file not supported, ignoring "large" bit
		largebit=$(( fofs & 0x80000000 ))
		if (( largebit != 0 )) ; then
			(( fofs &= 0x7fffffff ))
		elif (( fsize > 65535 )) ; then
			echo "${0}:\`${fic}':file[${i}]\`${fname}':too large \`$((fsize))'" 1>&2
			exit 1
		fi
		echo "file[${i}]\`${fname}':ofs=$((fofs)) size=$((fsize))" 1>&2

		if (( fofs > 0x0fffffff )) || (( fsize > 0x0fffffff )) \
		|| (( ficofs != fofs )) || (( fofs + fsize > ficsize )) ; then
			echo "${0}:\`${fic}':file[${i}]\`${fname}'@ofs${ficofs}:size failed" 1>&2
			exit 1
		fi

		# ------- to compare -------
		if [[ "${1}" = -t* ]] ; then
			fullname=${xpath}/${fname}
			echo " t file[${i}] with \`${fullname}'" 1>&2
			if [ ! -f "${fullname}" ] || [ ! -r "${fullname}" ]
			then
				echo "${0}:\`${fullname}':can not open" 1>&2
				(( ++err ))
			else
				$head -q -c $((fofs + fsize)) ${fic} | \
				$tail -q -c +$((fofs + 1)) - | \
				$cmp -b - ${fullname} 1>&2
				if [ $? != 0 ] ; then
					echo "${0}:\`${fic}':file[${i}]\`fullname':compare failed" 1>&2
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
				$head -q -c $((fofs + fsize)) ${fic} | \
				$tail -q -c +$((fofs + 1)) - > ${fullname}
				if [ $? != 0 ] ; then
					echo "${0}:\`${fic}':file[${i}]\`fullname':extract failed" 1>&2
					(( ++err ))
				fi
			fi
		fi

		(( ficofs += (fsize + cachelinesize - 1) & -cachelinesize ))
		(( ++i ))
	done

	if (( err != 0 )) ; then
		echo "${0}:\`${fic}':failed" 1>&2
		exit 1
	fi
	if (( ficofs != ficsize )) ; then
		echo "${0}:\`${fic}':size mismatches" 1>&2
		exit 1
	fi

	exit 0
fi

if [ $# -lt 2 ] ; then
	usage
	exit 1
fi

# ------- to create -------
fic=${1}
ficbak=${fic}.bak

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

shift 1
nfiles=0
sumsize=0
files=`echo $* | $sed -e "s/^$s\+//" -e "s/$s\+/\n/g" | $sort -u` &&
for f in ${files} ; do
	if [ ! -f "${f}" ] || [ ! -r "${f}" ] ; then
		echo "${0}:\`${f}':bin can not open" 1>&2
		exit 1
	fi &&
	if [[ "${f}" != */A/RO_RES/* ]] ; then
		echo "${0}:\`${f}':bin not in A:\\RO_RES\\" 1>&2
		exit 1
	fi &&
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1` &&
	if (( "${fsize}" >= 0x10000000 )) ; then
		echo "${0}:\`${f}':size \`${fsize}' too big" 1>&2
		exit 1
	fi &&
	ff=${f#*/A/RO_RES/} &&
	ff=${ff//\//\\} &&
	fnamelen=${#ff} &&
	if [ ${fnamelen} -ge 24 ] ; then
		echo "${0}:\`${ff}':file name too long" 1>&2
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
	echo "${0}:fic entries error" 1>&2
	exit 1
fi

if [ ${nfiles} = 0 ] ; then
	echo "${0}:no fic" 1>&2
	exit 0
fi

hdrentryend=$(( fichdrsize + nfiles * ficentrysize ))
foffset=$(( ( hdrentryend + cachelinesize - 1 ) & -cachelinesize ))
hdrentryfill=$(( foffset - hdrentryend ))
(( sumsize += foffset ))


# FIC header
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
echo -e "${sumsize}\n${nfiles}" | $t2b -q -b 4 - ${ficbak} &&
fillsize=$(( fichdrsize - 8 )) &&
$head -c ${fillsize} /dev/zero >> ${ficbak}
if [ $? != 0 ] ; then
	echo "${0}:\`${ficbak}':fic header error" 1>&2
	exit 1
fi

# FIC entries
for f in ${files} ; do
	ff=${f#*/A/RO_RES/}
	ff=${ff//\//\\}
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1`
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':file size error" 1>&2
		exit 1
	fi
	poffset=${foffset}
	if [ "${fsize}" -gt 65535 ] ; then
		# seek on large file not supported, set "large" bit
		(( poffset += 0x80000000 ))
	fi
	fnamelen=${#ff}
	fillsize=$(( 24 - fnamelen ))
# t2b direct to stdout may convert \n to \r\n, workaround to output to file
	echo -e "${poffset}\n${fsize}" | $t2b -q -a -b 4 - ${ficbak} &&
	echo -n "${ff}" >> ${ficbak} &&
	$head -c ${fillsize} /dev/zero >> ${ficbak}
	if [ $? != 0 ] ; then
		echo "${0}:\`${ff}':fic entry error" 1>&2
		exit 1
	fi
	(( foffset += ( fsize + cachelinesize - 1 ) & -cachelinesize ))
done

$head -c ${hdrentryfill} /dev/zero >> ${ficbak}
if [ $? != 0 ] ; then
	echo "${0}:\`${ficbak}':fic entry end aligning error" 1>&2
	exit 1
fi

# FIC contains
for f in ${files} ; do
	fsize=`$wc -c ${f} | $cut -d ' ' -f 1` &&
	fillsize=$((((fsize + cachelinesize - 1) & -cachelinesize) - fsize)) &&
	$cat ${f} >> ${ficbak} &&
	$head -c ${fillsize} /dev/zero >> ${ficbak}
	if [ $? != 0 ] ; then
		echo "${0}:\`${f}':fic contains error" 1>&2
		exit 1
	fi
	echo " + \`${f}'" 1>&2
done

mv ${ficbak} ${fic}
if [ $? != 0 ] ; then
	echo "${0}:\`${fic}':fic mv failed" 1>&2
	exit 1
fi
