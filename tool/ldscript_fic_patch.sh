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

# to patch linker script to support FIC (file in code)
# \param "-cachelinesize=CLS", optional, must be at the beginning of parameters
# if provided, to specify target chip cache line size in number of bytes,
# default 16 bytes;
# \param ${1} the linker script to be patched
# \param ${2} optional, the fic binary, removing fic from ${1} if not provided
# results linker script directly written back to ${1}

if [ $# != 0 ] && [[ "${1}" = -cachelinesize=* ]] ; then
	cachelinesize=${1#-cachelinesize=}
	shift
else
	cachelinesize=16
fi

if [ "$#" != 1 ] && [ "$#" != 2 ] ; then
	echo "${0}: usage:[-cachelinesize=\`CLS'] \`ldscript' [ \`fic' ]" 1>&2
	exit 1
fi

ldscript=${1}

if [ ! -f "${ldscript}" ] || [ ! -r "${ldscript}" ] || [ ! -w "${ldscript}" ]
then
	echo "${0}: \`${ldscript}': can not open" 1>&2
	exit 1
fi

wc=/usr/bin/wc
cut=/usr/bin/cut
sed=/bin/sed

set -o pipefail

if [ "$#" = 2 ] ; then
	fic=${2}
	if [ ! -f "${fic}" ] || [ ! -s "${fic}" ] ; then
		ficsize=${cachelinesize}  # minimun fic header
	else
		ficsize=`$wc -c ${fic} | $cut -d ' ' -f 1`
		if [ $? != 0 ] || (( ficsize < cachelinesize )) ; then
			echo "${0}: \`${fic}': size error" 1>&2
			exit 1
		fi
	fi
else
	ficsize=${cachelinesize}  # minimun fic header
fi


w=[_[:alnum:]]

$sed -i -e "s/\b\(_fic_start$w* = \. ; \. += \)[0-9]\+\(.*\)$/\1${ficsize}\2/" \
		${ldscript}
