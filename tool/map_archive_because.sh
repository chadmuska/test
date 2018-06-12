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

# \brief
# to find the because chain of specific archive(obj) in map file

# \param
# -u, optional, should go first if used,
# to print unique because chain in multiple archive(obj) cases;
# \param
# ${1} is the map file name, eg, "*.map";
# \param
# ${2} is the obj in archive, "*.a", "*.o", or "*.a(*.o)", supporting regexp;
# for multiboot stage archives and objects, "xxx.stage*.a" and "yyy.stage*.o",
# just "xxx.a" and "yyy.o" are accepted;
# for new tool chain GCC-4.6.x standard libs naming, eg "libc.a(lib_a-xxx.o)",
# also accepts "libc.a(xxx.o)" or just "xxx.o";
# \return
# exit code 0: ok;
# exit code 1: archive(obj) not found in map file;
# o/w: error;
# result texts printed to stdout,

## checking arguments

if [ $# = 0 ] ; then
	echo "usage:${0} [-u] \`mapfile' \`obj'" 1>&2
	exit 2
fi
if [ "${1}" = "-u" ] ; then
	unique=yes
	shift
else
	unique=
fi

if [ $# != 2 ] ; then
	echo "usage:${0} [-u] \`mapfile' \`obj'" 1>&2
	exit 2
fi

map=${1}
argobj=${2}

if [ ! -f "${map}" ] || [ ! -r "${map}" ] ; then
	echo "${0}:\`${map}':map can not open" 1>&2
	exit 2
fi

s=[[:blank:]]
S=[^[:blank:]]

case "x${argobj}" in

x*.a\(*.o\) )
	patarchive="${argobj%.a(*.o)}"
	patobj="${argobj##*.a(}"
	patobj="${patobj%.o)}"
	;;

x*.a )
	patarchive="${argobj%.a}"
	patobj="$S\+"
	;;

x*.o )
	patarchive="$S\+"
	patobj="${argobj%.o}"
	;;

x* | * )
	echo "${0}:\`${argobj}':not regexp in \`*.a', \`*.o', \`*.a(*.o)'" 1>&2
	exit 2
	;;

esac

# tool paths
cat=/bin/cat
rm=/bin/rm
mv=/bin/mv
sed=/bin/sed
#cut=/usr/bin/cut
#head=/usr/bin/head
#grep=/bin/grep
csplit=/usr/bin/csplit
#wc=/usr/bin/wc
mktemp=/bin/mktemp

set -o pipefail
shopt -s extglob

# finding "because" part: .a(.o) was loaded because... in map file
## format:
## <SOF>
## ^Archive member included because of file (symbol)$
## ^$ #### empty line
## #### >> ld -u <sym> or linker script EXTERN(<sym>) give empty "YY.a(yy.o) "
## ^XX1.a(xx1.o)$
## ^                              [[YY1.a(yy1.o)|yy1.o] ](symbol1)$
##or (depending on string length of "XX1.a(xx1.o)"
## ^XX1.a(xx1.o)                  [[YY1.a(yy1.o)|yy1.o] ](symbol1)$
## ...
## ^XXn.a(xxn.o)$
## ^                              [[YYn.a(yyn.o)|yyn.o] ](symboln)$
##or
## ^XXn.a(xxn.o)                  [[YYn.a(yyn.o)|yyn.o] ](symboln)$
## ^$ #### empty line
## #### >>>>>> optional section if compiled without "-fno-common"
## ^Allocating common symbols$
## ^Common symbol       size              file$
## ^$ #### empty line
## ^<symbol>            <size>            <file.o>$
## ...
## ^$ #### empty line
## #### <<<<<< optional section if compiled without "-fno-common"
## #### >>>>>> GCC-4 only
## ^Discarded input sections$
## ^$ #### empty line
## ...
## ^$ #### empty line
## #### <<<<<< GCC-4 only
## ^Memory Configuration$
## ^$ #### empty line
## ...
## ^$ #### empty line

B=${map}.b0.bak

if [ ! -f "${B}" ] || [ ! -r "${B}" ] || [ "${B}" -ot "${map}" ] ; then
	$rm -f ${map}.{a0,b0,b1}.bak &&
	$sed -e 's/\r//g' ${map} | \
	$csplit -q -f "" -b ${map}.a%1u.bak - \
		'%^Archive member included because of file (symbol)$%2'
	if [ $? != 0 ] ; then
		$rm -f ${map}.a0.bak
		echo "${0}:\`${map}':finding because failed" 1>&2
		exit 2
	fi
	# splitting at empty line after 'because part'
	$csplit -q -f "" -b ${map}.b%1u.bak ${map}.a0.bak '/^$/'
	# not to reject map file without an empty line after 'because part',
	# possibly a result of failure build.
	if [ $? = 0 ] ; then
		$rm -f ${map}.{a0,b1}.bak
	else
		$mv ${map}.a0.bak ${B}
		if [ $? != 0 ] ; then
			echo "${0}:\`${map}':I/O failed" 1>&2
			exit 2
		fi
	fi

# 1st -e '..' changing msdos path '\' mark in paths to unix '/' mark.
# pattern matching ${prefix} later in our tool chain, where '\' not supported
# \note 2nd regexp string with \" mark, there are variables need to expand

# 2nd -e ".." processing: single line because
## ^XX.a(xx.o)                    [[YY.a(yy.o)|yy.o] ](symbol)$
# splitted into 2 lines
	$sed -i -e 's,\\,/,g' \
		-e "s/^\($S\+\)$s\+\(\(.\+\(\.o\|\.a(.\+\.o)\) \)\?(.\+)\)$/\1\n                              \2/" ${B}
	if [ $? != 0 ] ; then
		$rm -f ${B}
		echo "${0}:\`${B}':because intermediate failed" 1>&2
		exit 2
	fi
fi

# \brief to print because chain starting from line of map because section
# \param ${1} is the line number of map because section to start tracing
map_because_trace()
{
	local ln=${1}
	local obj
	local t
	while true ; do
		$sed -n -e "$((ln+1))p" ${B} &&
		obj=`$sed -n -e "$((ln+1))s,^$s\+\(\($S\+\) \)\?(.\+)$,\2,p" \
				${B}`
		if [ $? != 0 ] ; then
			echo "${0}:\`${B}':L${ln}:finding failed" 1>&2
			return 2
		elif [ -z "${obj}" ] ; then
			break
		fi
		obj=`$sed -e 's,\.stage[0-9]\+\.,.stage[0-9]\\\+.,g' \
				-e 's/\./\\\./g' <<< "${obj}"` &&
		ln=`$sed -n -e "\,^${obj}$,=" ${B}`
		if [ $? != 0 ] || [ -z "${obj}" ] ; then
			echo "${0}:\`${B}':L${ln}:step failed" 1>&2
			return 2
		elif [ -z "${ln}" ] ; then
			break
		elif [ -n "${unique}" ] ; then
			t=`$sed -n -e "\,^\($s\+${obj} (.\+)\|${obj}\)$,=" \
					"${tmpout}"`
			if [ $? != 0 ] ; then
				echo "${0}:\`${tmpout}':unique failed" 1>&2
				return 2
			elif [ -n "${t}" ] ; then
				break
			fi
		fi
	done
}

lnlist=(`$sed -n -e "\,^$S*\(${patarchive}\(\.stage[0-9]\+\)\?\.a(${patobj}\(\.stage[0-9]\+\)\?\.o)\|_multiboot\.bak/${patarchive}/${patobj}\(\.stage[0-9]\+\)\?\.o\)$,=" ${B}`)
lnlist=${lnlist[*]}
if [ $? != 0 ] || [ -z "${lnlist}" ] ; then
	# try again with new tool chain GCC-4.6.x standard libs naming
	lnlist=(`$sed -n -e "\,^$S*\(${patarchive}\.a(lib_a-${patobj}\.o)\|_multiboot\.bak/${patarchive}/lib_a-${patobj}\.o\)$,=" ${B}`)
	lnlist=${lnlist[*]}
	if [ $? != 0 ] || [ -z "${lnlist}" ] ; then
		echo "${0}:\`${argobj}':not found" 1>&2
		exit 1
	fi
fi

if [ -n "${unique}" ] ; then
	tmpout=`$mktemp bczchain.XXXXXX.bak` &&
	tmpout0=`$mktemp bczchain.XXXXXX.bak`
	if [ $? != 0 ] ; then
		$rm -f "${tmpout}" "${tmpout0}"
		echo "${0}:\`bczchain.*.bak':create failed" 1>&2
		exit 2
	fi
fi

while [ -n "${lnlist}" ] ; do
	ln=${lnlist%% *}
	if [ -n "${unique}" ] ; then
		{ $sed -n -e "${ln}p" ${B} &&
		map_because_trace ${ln} ; } >> "${tmpout0}"
		if [ $? != 0 ] ; then
			$rm -f "${tmpout}" "${tmpout0}"
			exit 2
		fi
		$cat "${tmpout0}" &&
		$cat "${tmpout0}" >> "${tmpout}" &&
		$cat /dev/null > "${tmpout0}"
		if [ $? != 0 ] ; then
			$rm -f "${tmpout}" "${tmpout0}"
			echo "${0}:\`bczchain.*.bak':process failed" 1>&2
			exit 2
		fi
	else
		$sed -n -e "${ln}p" ${B} &&
		map_because_trace ${ln}
		if [ $? != 0 ] ; then
			exit 2
		fi
	fi
	lnlist=${lnlist##${ln}?( )}
done

if [ -n "${unique}" ] ; then
	$rm -f "${tmpout}" "${tmpout0}"
fi
