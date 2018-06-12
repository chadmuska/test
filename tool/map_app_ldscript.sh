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
# \file   map_app_ldscript.sh
# \brief  to patch app linker script with attaching extern symbols
#      of maskrom and print to stdout
# \author Anmin Deng

# \param
# ${1} is the linker script template, eg, "spca7002app.ld.tmpl";
# ${2} is the extern symbol file extracted from map file by
#     "map_sym_obj_extract.sh", eg, "spca7002.sym";
# \return
# exit code 0: success; non-zero: failed
# result linker script printing to stdout

## checking arguments

if [ $# != 2 ] ; then
	echo "${0}:usage:\`ldscript_tmpl' \`symfile'" 1>&2
	exit 1
fi

tmpl=${1}
sym=${2}

if [ ! -f "${tmpl}" ] || [ ! -r "${tmpl}" ] ; then
	echo "${0}:\`${tmpl}':linker script tmpl can not open" 1>&2
	exit 1
fi
if [ ! -f "${sym}" ] || [ ! -r "${sym}" ] ; then
	echo "${0}:\`${sym}':extern symbol file can not open" 1>&2
	exit 1
fi

x=[[:xdigit:]]

sed=/bin/sed

set -o pipefail

ebasekey='\$_eimage_base\$'
ebaseval=`$sed -n -e "s/^\(0x$x\{8\}\)\t_eimage_base$/\1/p" ${sym}`
if [ $? != 0 ] || [ -z "$ebaseval" ] ; then
	echo "${0}:\`${sym}':_eimage_base not found" 1>&2
	exit 1
fi

# copying the rest of linker script and patching $_eimage_base$
$sed -e "s/${ebasekey}/${ebaseval}/g" ${tmpl}

if [ $? != 0 ] ; then
	echo "${0}:\`${tmpl}':\`${sym}': failed" 1>&2
	exit 1
fi
