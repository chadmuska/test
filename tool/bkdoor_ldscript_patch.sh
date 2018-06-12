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
# \file   bkdoor_ldscript_patch.sh
# \brief  to patch symbol assignments in building app linker script
# \author Anmin Deng

# \param
# ${1}, the result file of symbol list;
# ${2}, the file of symbol list, derived from maskrom .map file;
# ${3}, all the maskrom symbols, including static, derived from .elf;
# ${4}, optional, the patching script file, scripts from <stdin> if not given;
# \return
# exit code 0: success; non-zero: failed
# result direct to file ${1}

# \note patching script text format
# in each text line any text after # to <EOL> is ignored as comments;
# empty lines are ignored;
# any lines not recognised as the following formats are errors
# d  <symbol>
# a  <symbol>  <static_symbol>
# where
# 'd  <symbol>' is to delete <symbol> from maskrom symbol list,
#   where shadowing code with the same name on IMEM will be provided;
# 'a  <symbol> ..' is to add <symbol> to symbol list for app to link, where
#   the symbol value <static_symbol> is derived from static symbol of maskrom;
# <static_symbol> is in the form of "<file>::<symbol>", for examples,
#   'my_file1.c::my_func1', or 'my_file2.S::my_var2'


# \brief to find symbol address value in .all.sym file, which was derived from
# .elf file
# \param ${1}, the .all.sym file name
# \param ${2}, [<src>::]<symbol>
# \return to print address value, in hex with prefix 0x, to <stdout>
# sym_addr_find.sh


# \brief to delete key from romsym and write to ${result}.bak
# \param ${1}, the key name to delete ${keynm}
process_script_delete_key()
{
	local key=${1}

	echo "${0}:L$((cmdln)):\`$*':deleting" 1>&2

	$sed -i -e "/^0x$x\{8\}\t${key}\$/d" ${result}.bak
	if [ $? != 0 ] ; then
		echo "${0}:L$((cmdln)):\`${key}':delete failed" 1>&2
		exit 1
	fi
}

# \brief to delete key from romsym and write to ${result}.bak
# \param ${1}, the key name to add for linking app
# \param ${2-}, the static symbol name value as the value of key
process_script_add_key()
{
	local key=${1}
	shift
	local remain=$*
	local addr

	addr=`${sym_addr_find} ${allsym} ${remain}` &&
	echo -e "${addr}\t${key}" >> ${result}.bak
	if [ $? != 0 ] ; then
		echo "${0}:L$((cmdln)):${key}=${addr}:add failed" 1>&2
		exit 1
	fi
	echo "${0}:L$((cmdln)):\`${key}=${remain}[${addr}]':adding" 1>&2
}


# \brief to process script line
# \param "$*", the script line to process
process_script_line()
{
	local keynm
	local remain
	local directive
	local cmd="$*"
	# echo "${0}:L$((cmdln)):\`${cmd}': processing" 1>&2

	directive=${cmd%% *}
	remain=${cmd#${directive}}
	remain=${remain## }

	keynm=${remain%% *}
	remain=${remain#${keynm}}
	remain=${remain## }

	if [ -z "${keynm}" ] ; then
		echo "${0}:L$((cmdln)):\`${cmd}':no key" 1>&2
		exit 1
	fi

	case "x${directive}" in

	#d  <romkey>
	xd  )
		if [ -n "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${remain}':unexpected" 1>&2
			exit 1
		fi
		process_script_delete_key ${keynm}
		;;

	#a  <key> <static_symbol>
	xa )
		if [ -z "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${cmd}':needs symbol" 1>&2
			exit 1
		fi
		process_script_add_key ${keynm} ${remain}
		;;

	x* | * )
		echo "${0}:L$((cmdln)):\`${cmd}':bad directive" 1>&2
		exit 1
		;;

	esac

	if [ $? != 0 ] ; then
		echo "${0}:L$((cmdln)):\`${cmd}':processing failed" 1>&2
		exit 1
	fi
}


# ===== main program ====================================================

if [ $# != 3 ] && [ $# != 4 ] ; then
	echo "${0}:\`result' \`romsym' \`allsym' [\`script']" 1>&2
	exit 1
fi

if [ ! -f "${2}" ] || [ ! -r "${2}" ] || [ ! -f "${3}" ] || [ ! -r "${3}" ] 
then
	echo "${0}:\`${2}',\`${3}':files invalid" 1>&2
	exit 1
fi

if [ $# != 4 ] ; then
	script=  # <stdin>
else
	script=${4}
	if [ ! -f "${script}" ] || [ ! -r "${script}" ] ; then
		echo "${0}:${script}:files invalid" 1>&2
		exit 1
	fi
fi

result=${1}
romsym=${2}
allsym=${3}

x=[[:xdigit:]]
s=[[:blank:]]

# tool paths
#basename=/usr/bin/basename
dirname=/usr/bin/dirname
cat=/bin/cat
rm=/bin/rm
#cp=/bin/cp
mv=/bin/mv
#mkdir=/bin/mkdir
touch=/bin/touch
sed=/bin/sed
#grep=/bin/grep
head=/usr/bin/head
cut=/usr/bin/cut
od=/usr/bin/od
csplit=/usr/bin/csplit

$rm -f ${result} ${result}.bak &&
$cat ${romsym} > ${result}.bak &&
$touch ${result} &&
$rm -f ${result}
if [ $? != 0 ] ; then
	echo "${0}:\`${result}':create result failed" 1>&2
	exit 1
fi

t2b=`$dirname ${0}`/t2b
if [ $? != 0 ] || [ ! -x "${t2b}" ] ; then
	echo "${0}:${t2b}:external tools invalid" 1>&2
	exit 1
fi

sym_addr_find=`$dirname ${0}`/sym_addr_find.sh
if [ $? != 0 ] || [ ! -x "${sym_addr_find}" ] ; then
	echo "${0}:${sym_addr_find}:external tools invalid" 1>&2
	exit 1
fi

set -o pipefail

#removing comments, tailing spaces, and then parsing
$sed -e "s/$s*\(#.*\)\?\r\?\$//" ${script} |
{
	cmdln=0
	i=
	while read -r i ; do
		((++cmdln))
		if [ -z "${i}" ] ; then
			continue
		fi
		process_script_line ${i}
	done
} &&
$mv -f ${result}.bak ${result}
if [ $? != 0 ] ; then
	echo "${0}:\`${result}':processing result failed" 1>&2
	exit 1
fi
