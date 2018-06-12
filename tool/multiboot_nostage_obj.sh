#!/bin/sh
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

# to find the obj stage file with "nostage%" to use at multi boot ${2} stage
# ${1} is the multi boot stage value, eg, 5
# ${2} is a "nostage%" obj file name, as "*.nostage+([0-9]).o",
# eg, myobj.nostage5.o
# result is the obj stage file name output to stdout,
# eg, <NULL string> or myobj.stage5.o
# \note when we want to build stage ${1} and to find a ${2} "nostage%" obj,
# if ${1} >= "nostage%", the stage obj ${2} of stage ${1} is what we need;
# if ${1} < "nostage%", the stage obj ${2} is ignored to build stage ${1}.

# \example if we have myobj.nostage5.o as ${2}
# % ${0} 5 myobj.nostage5.o  ---> will print "myobj.stage5.o" to stdout;
# % ${0} 3 myobj.nostage5.o  ---> will print nothing to stdout;
# % ${0} 8 myobj.nostage5.o  ---> will print "myobj.stage8.o" to stdout;

if [ "$#" != 2 ] ; then
	echo "${0}: usage: \`stage_val' \`nostage-obj'" 1>&2
	exit 1
fi

stage=${1}
obj=${2}

sed=/bin/sed

nostage=`echo "${obj}" | $sed -n -e "s/^.\+\.nostage\([0-9]\+\)\.o$/\1/p"`

if [ -n "${nostage}" ] && [ "${stage}" -ge "${nostage}" ] ; then
	echo "${obj}" | \
	$sed -ne "s/^\(.\+\)\.nostage[0-9]\+\.o$/\1\.stage${stage}\.o/p"
fi
