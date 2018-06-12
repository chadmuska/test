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
# \file   c2b.sh
# \brief  to generate binary image file from text in C array
# \author Anmin Deng

# \param
# $0 `prefix' [-a] [-b n] [-e l|b] [`in' [`out']]
# ${1}, the tool chain prefix;
# options
#   -a, the output append at file `out' EOF;
#   -b n, n bytes per element, default '-b 1';
#   -e l|b, '-e l' little endian, '-e b' big endian, default '-e l';
#   `in', input text file in C array, default stdin, "-" means stdin;
#   `out', output binary result, default stdout, "-" means stdout;
# \return
# exit code 0: success; non-zero: failed
# result bin written to `out' or stdout


usage()
{
	echo "usage: $0 \`prefix' [-a] [-b n] [-e l|b] [\`in' [\`out']]" 1>&2
	exit 1
}

if [ $# = 0 ] ; then
	usage
fi

prefix=${1}
shift

label=_my_test_var

gcc=${prefix}gcc
objcopy=${prefix}objcopy
objdump=${prefix}objdump
if [ ! -x "${gcc}" ] || [ ! -x "${objcopy}" ] || [ ! -x "${objdump}" ] ; then
	echo "$0:\`prefix':gcc, objcopy, objdump not available" 1>&2
	exit 1
fi


append=
endian=l
bpe=1

until [ $# = 0 ] ; do
	case "x${1}" in
	x-a )
		append='yes'
		;;
	x-b )
		shift
		if [ $# = 0 ] ; then
			usage
		fi
		bpe=${1}
		;;
	x-e )
		shift
		if [ $# = 0 ] ; then
			usage
		fi
		endian=${1}
		;;
	x- | x* | * )
		if [ -z "${in}" ] ; then
			in=${1}
		elif [ -z "${out}" ] ; then
			out=${1}
		else
			usage
		fi
		;;
	esac

	shift
done

if [ -z "${out}" ] || [ "${out}" = "-" ] ; then
	result=c2b.$$.bak.bin
else
	result=${out}.$$.bak.bin
fi

if [ "${endian}" = l ] ; then
	E="-EL"
elif [ "${endian}" = b ] ; then
	E="-EB"
else
	echo "$0:\`${endian}':-e endian not supported" 1>&2
	exit 1
fi

if [ "${bpe}" = 1 ] ; then
	type="char"
elif [ "${bpe}" = 2 ] ; then
	type="short"
elif [ "${bpe}" = 4 ] ; then
	type="int"
else
	echo "$0:\`${bpe}':-b bpe not supported" 1>&2
	exit 1
fi


x=[[:xdigit:]]
s=[[:blank:]]

cat=/bin/cat
rm=/bin/rm
sed=/bin/sed
head=/usr/bin/head


set -o pipefail


( echo "unsigned ${type} ${label}[] = {" &&
$cat ${in} &&
echo '};' ) > ${result}.bak.c &&

${gcc} ${E} -G 0 -c ${result}.bak.c 1>&2 &&
${objcopy} --output-target=binary ${result}.bak.o ${result} 1>&2 &&
sz=0x`${objdump} -t ${result}.bak.o | \
$sed -n -e "s/^$x\{8\} g     O \.data\t\($x\{8\}\) ${label}$s\?$/\1/p"` &&

if [ -z "${out}" ] || [ "${out}" = "-" ] ; then
	out='(stdout)'
	$head -q -c $((sz)) ${result}
elif [ -z "${append}" ] ; then
	$head -q -c $((sz)) ${result} > ${out}
else
	$head -q -c $((sz)) ${result} >> ${out}
fi

if [ $? != 0 ] ; then
	$rm -f ${result}.bak.[co] ${result}
	exit 1
fi

$rm -f ${result}.bak.[co] ${result}
echo "$0:\`${out}': $((sz)) byte(s) written." 1>&2
