#!/bin/bash
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
# \file   app_archive_install.sh
# \brief  to install members in archives to app lib path
#     and to delete the used members in archives for future building app use
# \author Anmin Deng

# \param
# ${1}, the archive member list;
# ${2}, the sp5k lib path;
# ${3}, the install destination path;
# ${4}, the system prefix, to find system tools and to check if system archive;
# ${5}..., CFLAGS for locating correct system libs
# \return
# exit code 0: success; non-zero: failed

## checking arguments

if [ $# -lt 4 ] ; then
	echo "${0}:usage:\`mlist' \`libdir' \`destdir' \`prefix' \`CFLAGS..'" 1>&2
	exit 1
fi

mlist=${1}
libdir=${2}
destdir=${3}
gcc=${4}gcc
ar=${4}ar
prefix=${4%/*}
prefix=${prefix%/*}/
shift 4
CFLAGS=$*

if [ ! -f "${mlist}" ] || [ ! -r "${mlist}" ] ; then
	echo "${0}:\`${mlist}':member list can not open" 1>&2
	exit 1
fi
if [ ! -d "${libdir}" ] ; then
	echo "${0}: \`${libdir}': sp5k lib path failed" 1>&2
	exit 1
fi
if [ ! -x "${gcc}" ] || [ ! -x "${ar}" ] || [ ! -d "${prefix}" ] ; then
	echo "${0}: \`${prefix}': system path failed" 1>&2
	exit 1
fi

s=[[:blank:]]

sed=/bin/sed
rm=/bin/rm
cp=/bin/cp

set -o pipefail

#removing comments, tailing spaces, empty lines, and then parsing
$sed -e "s/$s*\(#.*\)\?\$//" -e "/^$/d" ${mlist} |
{ while read -r i ; do

	case "x${i}" in

	# objs in system archives --->
	x\$stdlibs\$/*.a\(*.o\) )
		libbase=${i#\$stdlibs\$/}
		libbase=${libbase%(*.o)}
		lib=`${gcc} ${CFLAGS} --print-file-name=${libbase} | \
			$sed -e 's/\r//g'`
		if [ $? != 0 ] ; then
			echo "$0:\`${i}':\`${libbase}':gcc --print-file-name" \
					1>&2
			exit 1
		fi
		libbase=${libbase%.a}
		obj=${i##*.a(}
		obj=${obj%)}

		srclib=${lib}
		baklibdir=${libdir}/${libbase}
		destlib=
		;;

	# objs in system archives --->
	x${prefix}*.a\(*.o\) )
		lib=${i%(*.o)}
		libbase=${lib##*/}
		libbase=${libbase%.a}
		obj=${i##*.a(}
		obj=${obj%)}

		srclib=${lib}
		baklibdir=${libdir}/${libbase}
		destlib=
		;;

	x${libdir}/_multiboot.bak/*/*.o )
		libbase=${i#${libdir}/_multiboot.bak/}
		libbase=${libbase%/*.o}
		obj=${i##*/}

		srclib=${libdir}/${libbase}.a
		baklibdir=
		destlib=${destdir}/${libbase}.a
		;;

	# normal objs in sp5k archives ---->
	x*.a\(*.o\) )
		lib=${i%(*.o)}
		libbase=${lib##*/}
		libbase=${libbase%.a}
		obj=${i##*.a(}
		obj=${obj%)}

		srclib=${libdir}/${libbase}.a
		baklibdir=
		destlib=${destdir}/${libbase}.a
		;;

	x*.o )
		srclib=
		;;

	x* | * )
		echo "$0: \`${mlist}': \`${i}': invalid entry" 1>&2
		exit 1
		;;

	esac

	if [ -z "${srclib}" ] ; then
		continue
	fi
	if [ -n "${baklibdir}" ] ; then
		( mkdir -p ${baklibdir}/ && cd ${baklibdir}/ &&
		if [ "${obj}" -ot "${srclib}" ] ; then
			extracted=`${ar} xv ${srclib} ${obj} 2> err.bak \
				| $sed -e 's/\r//g' -e 's/^x - //'`
			# GCC-3 naming libc.a(*.o)
			# GCC-4 naming libc.a(lib_a-*.o)
			if [ $? != 0 ] || [ -s err.bak ] ; then
				if [ "${libbase}" = "libc" ] ; then
					# GCC-4 no _toupper, _tolower
					if [ "${obj}" = "_toupper.o" ] \
					|| [ "${obj}" = "_tolower.o" ] ; then
						$rm -f err.bak
						continue
					fi
					obj=lib_a-${obj}
					extracted=`${ar} xv ${srclib} ${obj} \
						2> err.bak | $sed -e 's/\r//g' \
							-e 's/^x - //'`
					if [ $? != 0 ] || [ -s err.bak ] ; then
						echo "$0:\`${srclib}':\`${obj}':failed" 1>&2
						exit 1
					fi
					if [ "${extracted}" != "${obj}" ] ; then
						echo "$0:\`${srclib}':\`${obj}':duplicated" 1>&2
						exit 1
					fi
				else
					echo "$0:\`${srclib}':\`${obj}':failed" 1>&2
					exit 1
				fi
			elif [ "${extracted}" != "${obj}" ] ; then
				echo "$0:\`${srclib}':\`${obj}':duplicated" 1>&2
				exit 1
			fi
		fi &&
		$rm -f err.bak )
	fi
	if [ -n "${destlib}" ] ; then
		if [ ! -f "${destlib}" ] ; then
			$cp -fu ${srclib} ${destlib}
		fi &&
		${ar} d ${destlib} ${obj} >& err.bak
		if [ $? != 0 ] || [ -s err.bak ] ; then
			echo "$0:\`${destlib}':\`${obj}':failed" 1>&2
			exit 1
		fi
		$rm -f err.bak
	fi

done }

if [ $? != 0 ] ; then
	echo "${0}: \`${mlist}': reading failed" 1>&2
	exit 1
fi
