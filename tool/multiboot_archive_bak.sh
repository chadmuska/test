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

# \brief extracting obj members written to ${bakpath}
# \param ${1} is the full path of original archive, in format like
# "${libpath}/${libname}", without ".stage*" even if supporting multi stage;
# ${libpath} must be relative path. absolute path not supported;
# \note ${1} must be in relative path. absolute path is not supported;
# \note ${bakpath} == `dirname ${1}`/_multiboot.bak/`basename ${1%.a}`;
# \note when archive extract success, there is a timestamp checking file
# "${bakpath}/_timestamp.bak" created for future buidling dependence checking.

if [ $# != 2 ] ; then
	echo "${0}: usage: \`archive' \`prefix'" 1>&2
	exit 1
fi


lib=${1}    # no ".stage*" by tailing ".a", even if stage archive
ar=${2}ar

if [ ! -f "${lib}" ] || [ ! -r "${lib}" ] || [ "${lib}" == *.stage*.a ] ; then
	echo "${0}: \`${lib}' archive can not open" 1>&2
	exit 1
fi

dirname=/usr/bin/dirname
mkdir=/bin/mkdir
rm=/bin/rm
sed=/bin/sed
touch=/bin/touch
ls=/bin/ls
diff=/usr/bin/diff

set -o pipefail


# \param ${1} the archive file name to extract
archive_extract()
{
	local members
	local extracted
	local l=${1}

	$rm -f err.log
	if [ "${l}" == *.stage*.a ] ; then
		members=`${ar} t ${l} 2> err.log | \
			| $sed -n -e 's/\r//g' -e '/^.\+\.stage[0-9]\+\.o$/p'`
	else
		members=`${ar} t ${l} 2> err.log | $sed -e 's/\r//g'`
	fi
	if [ $? != 0 ] || [ -s err.log ] ; then
		echo "$0: ar: \`${l}': archive test failed" 1>&2
		exit 1
	fi
	$rm -f err.log

	if [ -z "${members}" ] ; then
		return 1
	fi

	# `ar' has no error code for `ar x archive obj'
	# if obj is not an entry in archive, but just print error messages
	# to standard error

	extracted=`${ar} xv ${l} ${members} 2> err.log \
		| $sed -e 's/\r//g' -e 's/^x - //'`
	if [ $? != 0 ] || [ -s err.log ] ; then
		$rm -f ${members}
		echo "$0: ar: \`${l}': extracting failed" 1>&2
		exit 1
	fi
	$rm -f err.log

	if [ "${members}" != "${extracted}" ] ; then
		echo "$0: \`${l}': members differ" 1>&2
		$rm -f ${members} members.log
		echo "${members}" > members.log
		$diff members.log - <<< "${extracted}" 1>&2
		exit 1
	fi
}

# \note we back up the objs in archives to "${bakpath}/${obj%noext}[.stage*].o"
# for non-system archives;
# where "${bakpath}" == "`dirname ${1}`/_multiboot.bak/`basename ${1%.a}`"
# the archive ${1} is always in the path "${bakpath}/../../";

libname=${lib##*/}
bakpath=`$dirname ${lib}`/_multiboot.bak/${libname%.a}
if [ $? != 0 ] ; then
	echo "${0}:\`${bakpath}':lib backup path failed" 1>&2
	exit 1
fi

if [ ! "${bakpath}/_timestamp.bak" -ot "${lib}" ] ; then
	echo "${0}: \`${bakpath}' is up to date" 1>&2
	exit 0
fi

$rm -fr ${bakpath}

$mkdir -p ${bakpath} &&
cd ${bakpath}
if [ $? != 0 ] ; then
	echo "${0}: \`${bakpath}': path failed" 1>&2
	exit 1
fi

if [ ! -x "${ar}" ] ; then
	echo "${0}: \`${ar}': system path failed" 1>&2
	exit 1
fi

echo "backup \`${lib}' to \`${bakpath}'" 1>&2

# normal archive backup
extlib=../../${libname}

archive_extract ${extlib}

if [ $? != 0 ] ; then
	echo "${0}: \`${extlib}': empty" 1>&2
else

	# stage archives backup
	stagelibs=`$ls ${extlib%.a}.stage*.a 2> /dev/null`

	for l in ${stagelibs} ; do
		archive_extract ${l}
	done

fi

$touch _timestamp.bak
