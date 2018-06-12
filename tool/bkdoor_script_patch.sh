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
# \file   bkdoor_script_patch.sh
# \brief  to patch dmem contents by the patching scripts streamed from <stdin>
# \author Anmin Deng

# \param
# ${1}, the dmem image;
# ${2}, the file of maskrom symbol list, derived from maskrom .map file;
# ${3}, the file of ram app symbol list, derived from ram app .map file;
# ${4}, optional, the patching script file, scripts from <stdin> if not given;
# \return
# exit code 0: success; non-zero: failed
# result direct patch to ${1}

# \note patching script text format
# in each text line any text after # to <EOL> is ignored as comments;
# empty lines are ignored;
# any lines not leading with '%<directive>' are appanded to previous line
# %bkdoor_fun  <bkdoor_romkey>
# %bkdoor_val  <bkdoor_romkey> [options...] <size> <val...>
# %bkdoor_addr <bkdoor_romkey> [options...] <size> <ramkey [+|- <ofs>] ...>
# %setval      <romkey> [options...] <size> <val...>
# %setaddr     <romkey> [options...] <size> <ramkey [+|-<ofs>] ...>
# %pokeval     <pokeaddr> [options...] <size> <val...>
# %pokeaddr    <pokeaddr> [options...] <size> <ramkey [+|-<ofs>] ...>
# where
# '%bkdoor_fun  <bkdoor_romkey>' is to enable bkdoor workaround function,
#   for example, script line '%bkdoor_fun my_fun' will patch rom variable
#   '_fp_bkdoor_wrapper_my_fun' by value of address of '_ram_hack_my_fun';
# '%bkdoor_val  <bkdoor_romkey> ... <val...>' is to patch contents of
#   _bkdoor_wrapper_<bkdoor_romkey> by <val...>;
# '%bkdoor_addr <bkdoor_romkey> ... <ramkey...>' is to patch contents of
#   _bkdoor_wrapper_<bkdoor_romkey> by values of addresses of <ramkey...>;
# '%setval      <romkey> ... <val...>' is to patch contents of
#   <romkey> by <val...>;
# '%setaddr     <romkey> ... <ramkey...>' is to patch contents of
#   <romkey> by values of addresses of <ramkey...>;
# '%pokeval     <pokeaddr> ... <val...>' is to patch contents at
#   DMEM <pokeaddr> by <val...>;
# '%pokeaddr    <pokeaddr> ... <ramkey...>' is to patch contents at
#   DMEM <pokeaddr> by values of addresses of <ramkey...>;
# [options...] are
#   "-o <offset>", patching the contents at the address of <romkey> + <offset>
#       '%pokeval' or '%pokeaddr' directive not supporting '-o' option;
#   "-b <n>", each item in list <val...> represents <n> bytes, default "-b 1",
#     while in cases of '%bkdoor_addr' and '%setaddr', -b <n> is not accepted
#     but forced to 4;
#   "-e l" or "-e b", little or big endian, default "-e l";
# <bkdoor_romkey>, <romkey>, the symbol key to patch, where
#   <bkdoor_romkey> will be added with '_bkdoor_wrapper_' prefix to key name,
#   only supporting global variables;
# <pokeaddr>, the DMEM address at which to patch;
# <size> is the size of patched key values in bytes,
#   in cases of <ramkey..> addresses as val, <size> must be multiple of 4;
# <val...>, the list of new key values, regarding to -b and -e options,
#   or, if <val...> is "@<file>", the new key values are in a "t2b" formatted
#   text file <file>;
# <ramkey [+|-<ofs>] ...>, the list of ram keys, the addresses of keys
#   resolved from ram app sumbol list +/-<ofs> (in bytes) as new key values,
#   each ram key [+|-<ofs>] for 4 bytes,
#   and the number of keys in <ramkey [+|-<ofs>] ...> * 4 must match <size>,
#   or, if <ramkey [+|-<ofs>] ...> is "@<file>", the list of ram keys and
#   +|-<ofs> are from <file>,
#   <ramkey> entries can be "NULL" or "0" but can not have +|-<ofs>;


# \brief to find symbol address value in .sym file, which was derived from
# .map file
# \param ${1}, the .sym file name
# \param ${2}, the key symbol name
# \return to print address value, in hex with prefix 0x, to <stdout>
sym_addr_find()
{
	local addr
	local sym=${1}
	local key=${2}
	addr=`$sed -n -e "s/^\(0x$x\{8\}\)\t${key}\$/\1/p" ${sym}`
	if [ $? != 0 ] || [ $((addr)) = 0 ] ; then
		echo "${0}:${sym}:\`${key}':sym failed to find" 1>&2
		exit 1
	fi
	echo "${addr}"
}

# \brief to process remaining script lines
# \note remaining text format in "${remain}" as:
# [options...] <size> <val..>, or,
# [options...] <size> <ramkey [+|-<ofs>]..>, or,
# [options...] <size> @<file>
# \var ${remain}, remaining text
process_script_remain_common()
{
	local arg1
	local arg2
	if [ -z "${remain}" ] ; then
		echo "${0}:L$((cmdln)):\`${cmd}':<size>.. expected" 1>&2
		exit 1
	fi

	args=
	arg1=${remain%% *}
	remain=${remain#${arg1}}
	remain=${remain## }
	while [[ "x${arg1}" = x-* ]] ; do
		if [ -z "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${arg1}':args.. expected" 1>&2
			exit 1
		fi
		arg2=${remain%% *}

		if [ -n "${pokeaddr}" ] && [ "${arg1}" = "-o" ] ; then
			echo "${0}:L$((cmdln)):poke not supporting \`-o'" 1>&2
			exit 1
		fi

		remain=${remain#${arg2}}
		remain=${remain## }
		args="${args} ${arg1} ${arg2}"

		if [ -z "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${arg2}':<size> expected" 1>&2
			exit 1
		fi

		arg1=${remain%% *}
		remain=${remain#${arg1}}
		remain=${remain## }
	done

	if [ -z "${remain}" ] ; then
		echo "${0}:L$((cmdln)):\`${args}':<val>.. expected" 1>&2
		exit 1
	fi
	keysz="${arg1}"
}

# \brief to process remaining script lines for addresses
# \note remaining text format in "${remain}" as:
# <ramkey [+|-<ofs>]..>, or, @<file>
process_script_remain_addrs()
{
	local ftext
	local appkey
	local ofs
	local addr

	# expanding @<file>, if presents
	if [[ "${remain}" = @* ]] ; then
		ftext=${remain#@}
		if [ ! -f "${ftest}" ] || [ ! -r "${ftest}" ] ; then
			echo "${0}:L$((cmdln)):\`${remain}':bad @<file>" 1>&2
			exit 1
		fi
		#removing comments, tailing spaces, empty lines
		remain=`$sed -e "s/$s*\(#.*\)\?\r\?\$//" -e '/^$/d' ${ftext}`
		if [ $? != 0 ] || [ -z "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${ftext}':bad file" 1>&2
			exit 1
		fi
	fi

	valist=
	while [ -n "${remain}" ] ; do
		appkey=${remain%% *} &&
		remain=${remain#${appkey}} &&
		remain=${remain## } &&

		if [ "${appkey}" = "NULL" ] || [ "${appkey}" = 0 ] ; then
			# supporting NULL pointers as patched addresses
			addr=0
		else
			addr=`sym_addr_find ${appsym} ${appkey}` &&
			if [ -n "${remain}" ] && [[ "${remain}" = [+-]* ]]
			then
				ofs=${remain%% *} &&
				remain=${remain#${ofs}} &&
				remain=${remain## } &&
				(( addr += ofs ))
			fi
		fi
		if [ $? != 0 ] ; then
			echo "${0}:L$((cmdln)):\`${appkey}..':key [+|-<ofs>]" \
					1>&2
			exit 1
		fi

		valist="${valist} ${addr}"
	done

	# addresses forced to be 4 bytes per entry
	args="${args} -b 4"
}

# \brief to process script line
# \param "$*", the script line to process
process_script_line()
{
	local args
	local keynm
	local keysz
	local valist
	local remain
	local directive
	local cmd="$*"
	local pokeaddr
	local vma
	local ofs
	local script
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

	#%bkdoor_fun  <bkdoor_romkey>
	x%bkdoor_fun  )
		if [ -n "${remain}" ] ; then
			echo "${0}:L$((cmdln)):\`${remain}':unexpected" 1>&2
			exit 1
		fi
		args="-b 4"
		keysz=4
		valist=`sym_addr_find ${appsym} _ram_hack_${keynm}` &&
		keynm="_fp_bkdoor_wrapper_${keynm}"
		;;

	#%bkdoor_addr <bkdoor_romkey> [options...] <size> <ramkey [+|-<ofs>]..>
	x%bkdoor_addr )
		keynm="_bkdoor_wrapper_${keynm}"
		process_script_remain_common &&
		process_script_remain_addrs
		;;

	#%setaddr     <romkey> [options...] <size> <ramkey [+|-<ofs>]..>
	x%setaddr     )
		process_script_remain_common &&
		process_script_remain_addrs
		;;

	#%pokeaddr    <pokeaddr> [options...] <size> <ramkey [+|-<ofs>]..>
	x%pokeaddr    )
		pokeaddr=${keynm}
		process_script_remain_common &&
		process_script_remain_addrs
		;;

	#%bkdoor_val  <bkdoor_romkey> [options...] <size> <val..>
	x%bkdoor_val  )
		keynm="_bkdoor_wrapper_${keynm}"
		process_script_remain_common &&
		valist=${remain}
		;;

	#%setval      <romkey> [options...] <size> <val..>
	x%setval      )
		process_script_remain_common &&
		valist=${remain}
		;;

	#%pokeval     <pokeaddr> [options...] <size> <val..>
	x%pokeval     )
		pokeaddr=${keynm}
		process_script_remain_common &&
		valist=${remain}
		;;

	x* | * )
		echo "${0}:L$((cmdln)):\`${cmd}':bad directive" 1>&2
		exit 1
		;;

	esac &&

	if [ -z "${pokeaddr}" ] ; then
#		echo "$keypatch $args $result $romsym $keynm $keysz $valist" \
#			1>&2 &&
		$keypatch $args $result $romsym $keynm $keysz $valist
	else
#vma=`sym_addr_find ${romsym} _dram_content_vma_start`
#if [ $? != 0 ] || [ $((vma)) = 0 ] ; then
#	echo "${0}:${romsym}:\`_dram_content_vma_start' invalid" 1>&2
#	exit 1
#fi
		vma=0xd0000800
		ofs=$(( pokeaddr - vma ))

		if (( ofs < 0 )) ; then
			echo "${0}:${pokeaddr}:${ofs}:offset invalid" 1>&2
			exit 1
		fi
		if [[ "${valist}" = @* ]] ; then
			# @<file>
			script=${valist#@}
			${t2bpatch} ${result} ${ofs} ${keysz} ${args} ${script}
		else
			echo ${valist} | \
			$sed -e "s/^$s\+//" -e "s/$s\+$//" -e "s/$s\+/\n/g" | \
			${t2bpatch} ${result} ${ofs} ${keysz} ${args}
		fi
	fi

	if [ $? != 0 ] ; then
		echo "${0}:L$((cmdln)):\`${cmd}':processing failed" 1>&2
		exit 1
	fi
}


# ===== main program ====================================================

if [ $# != 3 ] && [ $# != 4 ] ; then
	echo "${0}:\`dmem' \`romsym' \`ramsym' [\`script']" 1>&2
	exit 1
fi
if [ ! -f "${1}" ] || [ ! -r "${1}" ] || [ ! -w "${1}" ] \
|| [ ! -f "${2}" ] || [ ! -r "${2}" ] || [ ! -f "${3}" ] || [ ! -r "${3}" ]
then
	echo "${0}:$*:files invalid" 1>&2
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
appsym=${3}

x=[[:xdigit:]]
s=[[:blank:]]

# tool paths
#basename=/usr/bin/basename
dirname=/usr/bin/dirname
#cat=/bin/cat
#rm=/bin/rm
#mv=/bin/mv
#mkdir=/bin/mkdir
#touch=/bin/touch
sed=/bin/sed
#grep=/bin/grep
#csplit=/usr/bin/csplit

keypatch=`$dirname ${0}`/bkdoor_key_patch.sh &&
t2bpatch=`$dirname ${0}`/t2b_patch

if [ $? != 0 ] || [ ! -x "${keypatch}" ] || [ ! -x "${t2bpatch}" ] ; then
	echo "${0}:${keypatch},${t2bpatch}:external tools invalid" 1>&2
	exit 1
fi

set -o pipefail

#removing comments, tailing spaces, and then parsing
$sed -e "s/$s*\(#.*\)\?\r\?\$//" ${script} |
{
	scanln=0 &&
	cmdln=1 &&
	cmd= &&

	while read -r i ; do

		((++scanln)) &&

		if [ -z "${i}" ] ; then
			continue
		fi &&
		if [[ "${i}" != %* ]] ; then
			if [[ "${cmd}" != %* ]] ; then
				echo "${0}:L$((scanln)):\`${cmd} ${i}':no %" \
						1>&2
				exit 1
			fi &&
			cmd="${cmd} ${i}"
		else
			if [ -n "${cmd}" ] ; then
				process_script_line ${cmd}
			fi &&
			cmdln=$((scanln)) &&
			cmd="${i}"
		fi
	done &&
	if [ -n "${cmd}" ] ; then
		process_script_line ${cmd}
	fi
}
