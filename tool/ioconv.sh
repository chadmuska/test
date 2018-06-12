#!/bin/bash

#/**************************************************************************
# *
# *         Copyright (c) 2013 by iCatch Technology Inc.
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
# \file   ioconv.sh
# \brief  to convert IO description text file to C include header file
# for building host usage.
# \author Anmin Deng, Edward Kuo, Patrick Chen, Paul May

# history
#20090722 0.05 add path support
#20091023 0.06 add pocu gpio
#20091029 0.07 add CFG_ macro for NO_USE gpio group
#20091127 0.08 use sp5kGpioWrite for output
#20100330 0.09 fix no output bug for VCC400Z
#20100508 0.10 add chip info & pull support
#20100525 0.11 add pull mask define
#20100909 0.12 fix typo & duplicated pull set condition
#20100924 0.13 add option to generate pure SP5K output
#20101222 0.14 add 5330 & DS support
#20110124 0.15 add 5330 SAR & OIS gpio support
#20110407 0.16 fix: DS is available for 5330 only
#20110630 0.17 fix: 5330 GPIO 6/7 pull dir
#                          fix: io/ion array is too small
#                          add 6330 support and minor generalization
#20110909 0.18 fix: POCU polarity support
#20120103 0.19 add option to specify output filename
#                          add option to return version
#20120420 0.20 add check for chip name
#20120820 0.21 must add chip name to avoid using 5310B unintentionally
#20130522 0.22 add 6350, serious refactor
#20130719 0.23 6350 update
#20130719 0.24 add direction limitation & rgb group, 6350 update
#20130809 0.25 exit with err for unknown io group
#20130822 0.26 support fn
#20131102 0.27 porting to shell script
#              "-o=<result>" writes to own path instead of <iofile> path
#20140411 0.28 output ds mask
#20141020 0.29 to support and warn the last line of IO file without EOL

# \param (in any order)
# <iofile>, (non-"-"-leading) specifies input IO description text file name;
# -i=<gpioInFuncName> optional, specifies the string of GPIO In function name;
# -o=<result> optional, specifies the result output filename;
# -ver to print version string and exit;
# o/w print usage messages and exit;

# \return
# exit code 0: success; non-zero: failed
# \note if no "-o=<result>" option specified, using default
# `dirname ${iofile}`/app_Gpio_`basename ${iofile%.io}`.h

VERSION=29

ver()
{
	echo "ioconv v$(( VERSION / 100 )).$(( VERSION % 100 ))"
}

usage()
{
	ver
	echo "  usage: ioconv.sh \`iofile' [-i=\`gpifun'] [-o=\`resfile']"
	echo "  usage: ioconv.sh -ver : print version string"
	exit 1
}


# ------ parsing parameters ------

iofile=
res=
gpifun=
while [ $# != 0 ] ; do
	case "x${1}" in
	x-ver )
		ver
		exit 0
		;;

	x-i=* )
		if [ -n "${gpifun}" ] ; then
			echo "ioconv:\`${gpifun}':multiple gpifun, ignored" 1>&2
		fi
		gpifun=${1#-i=}
		;;

	x-o=* )
		if [ -n "${res}" ] ; then
			echo "ioconv:\`${res}':multiple resfile, ignored" 1>&2
		fi
		res=${1#-o=}
		;;

	x-* )
		echo "ioconv:\`${1}':option not recognzied" 1>&2
		usage
		;;

	x* )
		if [ -n "${iofile}" ] ; then
			echo "ioconv:\`${iofile}':multiple iofile, ignored" 1>&2
		fi
		iofile=${1}
		;;

	esac

	shift
done


# ------ variables ------

if [ -z "${iofile}" ] ; then
	usage
fi
if [ ! -f "${iofile}" ] || [ ! -r "${iofile}" ] ; then
	echo "ioconv:\`${iofile}':iofile open failed" 1>&2
	exit 1
fi

d=[[:digit:]]
#x=[[:xdigit:]]
s=[[:blank:]]
e=[[:space:]]
S=[^[:space:]]
W=[^_[:alnum:]]

# tool paths
#uname=/bin/uname
basename=/usr/bin/basename
dirname=/usr/bin/dirname
#cat=/bin/cat
rm=/bin/rm
#cp=/bin/cp
mv=/bin/mv
mkdir=/bin/mkdir
touch=/bin/touch
sed=/bin/sed
#grep=/bin/grep
mktemp=/bin/mktemp
false=/bin/false
#head=/usr/bin/head
#cut=/usr/bin/cut
#od=/usr/bin/od
#csplit=/usr/bin/csplit
seq=/usr/bin/seq

set -o pipefail
shopt -s extglob

iopath=`$dirname ${iofile}` &&
iobase=`$basename ${iofile%.io}`
if [ $? != 0 ] || [ -z "${iopath}" ] || [ -z "${iobase}" ] ; then
	echo "ioconv:\`${iofile}':iofile path failed" 1>&2
	exit 1
fi

# if no resfile specified, using default
# `dirname ${iofile}`/app_Gpio_`basename ${iofile%.io}`.h
if [ -z "${res}" ] ; then
	res=${iopath}/app_Gpio_${iobase}.h
fi

# testing if result file can be written
$rm -f ${res} &&
resbak=`$mktemp ${res}-XXXXXX.bak` &&
iobak=`$mktemp ${res%.h}.io-XXXXXX.bak`
if [ $? != 0 ] ; then
	echo "ioconv:\`${iofile}':\`${res}':intermediate file failed" 1>&2
	error
fi

headertag=_APP_GPIO_${iobase^^}_H       #upper cases
headertag=${headertag//$W/_} #replacing bad C name characters to "_"


# ------ functions ------

error()
{
	if [ -n "${resbak}" ] ; then
		$rm -f ${resbak}
	fi
	if [ -n "${iobak}" ] ; then
		$rm -f ${iobak}
	fi
	exit 1
}

platform_info_get()
{
	local -ru line="${1}" # upper cases
	local p
	p=`$sed -e "s,^;$s*/\*$s*\($S\+\)$s*\*/$s*$,\1," <<< "${line}"` &&
	case "x${p}" in
	xSPCA6350* ) platform=SPCA6350 ;;
	xSPCA6330* ) platform=SPCA6330 ;;
	xSPCA5330* ) platform=SPCA5330 ;;
	xSPCA5310B* ) platform=SPCA5310B ;;
	xSPCA5310* ) platform=SPCA5310 ;;
	* ) $false ;;
	esac
	if [ $? != 0 ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):\`${line}':platform" 1>&2
		error
	fi
#	echo "${platform}"
}

findgrp()
{
	local name
	for i in ${grplist} ; do
		eval name=\${grpdef_${i}_name}
		if [ "${name^^}" = ${grp} ] ; then
			grp=${i}
			if [ "${grp}" = FML ] && [ "${pin}" -ge 32 ] ; then
				grp=FMH
			elif [ "${grp}" = DUMMY ] ; then
				pin=0
			fi
			return 0
		fi
	done
	echo "ioconv:\`${iofile}':L$((lineno)):\`${grp}':bad grp" 1>&2
	error
}

finddir()
{
	if [[ "${dir}" = \!* ]] ; then
		inv=!
		dir=${dir:1}
	fi
	if [ -z "${dir}" ] || [[ "${dir}" != IO && "${dir}" != IN && \
	"${dir}" != OUT && "${dir}" != FN ]] \
	|| [[ -n "${inv}" && "${dir}" = FN ]] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):\`${inv}${dir}':bad dir" 1>&2
		error
	fi
}

dir_check()
{
	local -i grpisro=$((grpdef_${grp}_attr & GRP_ATTR_READONLY))
	local -i pinmask
	if [ "${pin}" -ge 32 ] ; then
		pinmask=$(( 1 << (pin - 32) ))
	else
		pinmask=$(( 1 << pin ))
	fi
	# group is readonly
	if (( grpisro != 0 )) && [[ "${dir}" = OUT || "${dir}" = IO ]]
	then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}:readonly" 1>&2
		error
	fi
	# pin input not supported
	if [[ "${dir}" = IN  || "${dir}" = IO ]] && \
	(( (pindef_${platform}_${grp}_dir_noin & pinmask) != 0 )) ; then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:input not supported" 1>&2
		error
	fi
	# pin output not supported
	if [[ "${dir}" = OUT || "${dir}" = IO ]] && \
	(( (pindef_${platform}_${grp}_dir_noout & pinmask) != 0 )) ; then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:output not supported" 1>&2
		error
	fi
}

pull_check_set()
{
# PU=<p1>[,<p2>]
# PD=<p1>[,<p2>]
# where <p1> and <p2> are 0 or 1, <p2> is used at power saving mode if given
	local norm=
	local min=
	local pullstr
	local -i pinmask
	pullstr=`$sed -ne "s/^.*\(P[UD]=\($d\+\(,$d\+\)\?\)\?\).*$/\1/p" \
			<<< "${line}"`
	if [ $? != 0 ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):sed pullset failed" 1>&2
		error
	fi
	if [ -z "${pullstr}" ] ; then
		return 0  # not setting pull
	fi

	if [ "${pin}" -ge 32 ] ; then
		pinmask=$(( 1 << (pin - 32) ))
	else
		pinmask=$(( 1 << pin ))
	fi

	if (( (pindef_${platform}_${grp}_pull_avail & pinmask) == 0 ))
	then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:pull not supported" 1>&2
		error
	fi
	if [[ "${pullstr}" = PD* ]] ; then
		if (( (pindef_${platform}_${grp}_pull_lvl & pinmask) != 0 ))
		then
			echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:supporting PU" 1>&2
			error
		fi
	else # PU
		if (( (pindef_${platform}_${grp}_pull_lvl & pinmask) == 0 ))
		then
			echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:supporting PD" 1>&2
			error
		fi
	fi

	pullstr="${pullstr:3}"
	if [ -z "${pullstr}" ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):pull empty" 1>&2
		error
	fi

	norm=${pullstr%%,*}
	min=${pullstr#${norm}}
	min=${min#,}

	if [ -n "${min}" ] ; then
	# user specifies min pull
		if (( (pull_${grp}_mask_min & pinmask) != 0 )) ; then
			echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:multiple min pulls" 1>&2
			error
		fi
		(( pull_${grp}_mask_min |= pinmask ))
		(( pull_${grp}_en_min &= ~pinmask ))
		if (( min != 0 )) ; then
			(( pull_${grp}_en_min |= pinmask ))
		fi
	fi

	# user specifies normal pull
	if (( (pull_${grp}_mask_norm & pinmask) != 0 )) ; then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:multiple normal pulls" 1>&2
		error
	fi
	(( pull_${grp}_mask_norm |= pinmask ))
	(( pull_${grp}_en_norm &= ~pinmask ))
	if (( norm != 0 )) ; then
		(( pull_${grp}_en_norm |= pinmask ))
	fi
}

ds_check_set()
{
# DS=<p1>[,<p2>]
# where <p1> and <p2> are 0-3, <p2> is used at power saving mode if given
	local norm=
	local min=
	local dsshift
	local dsgrp
	local dsstr
	local -i pinmask
	dsstr=`$sed -ne "s/^.*\(DS=\($d\+\(,$d\+\)\?\)\?\).*$/\1/p" \
			<<< "${line}"`
	if [ $? != 0 ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):sed DS= failed" 1>&2
		error
	fi
	if [ -z "${dsstr}" ] ; then
		return 0  # not setting ds
	fi

	if [ "${pin}" -ge 32 ] ; then
		pinmask=$(( 1 << (pin - 32) ))
		(( dsshift = (pin - 32) * 2 ))
	else
		pinmask=$(( 1 << pin ))
		(( dsshift = pin * 2 ))
	fi

	if (( (pindef_${platform}_${grp}_ds_avail & pinmask) == 0 ))
	then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:ds not supported" 1>&2
		error
	fi

	dsstr="${dsstr:3}"
	if [ -z "${dsstr}" ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):ds empty" 1>&2
		error
	fi

	norm=${dsstr%%,*}
	min=${dsstr#${norm}}
	min=${min#,}

	if (( dsshift < 32 )) ; then
		dsgrp=0
	else
		dsgrp=1
		(( dsshift -= 32 ))
	fi

	if [ -n "${min}" ] ; then
	# user specifies min ds
		if (( (ds_${grp}_mask_min & pinmask) != 0 )) ; then
			echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:multiple min ds" 1>&2
			error
		fi
		if (( min > 3 )) ; then
			echo "ioconv:\`${iofile}':L$((lineno)):\`${min}':bad ds" 1>&2
			error
		fi
		(( ds_${grp}_mask_min |= pinmask ))
		(( ds_${grp}_min_${dsgrp} &= ~(3<<dsshift) ))
		if (( min != 0 )) ; then
			(( ds_${grp}_min_${dsgrp} |= (min<<dsshift) ))
		fi
	fi

	# user specifies normal ds
	if (( (ds_${grp}_mask_norm & pinmask) != 0 )) ; then
		echo "ioconv:\`${iofile}':L$((lineno)):${platform}.${grp}.${pin}:multiple normal ds" 1>&2
		error
	fi
	if (( norm > 3 )) ; then
		echo "ioconv:\`${iofile}':L$((lineno)):\`${norm}':bad ds" 1>&2
		error
	fi
	(( ds_${grp}_mask_norm |= pinmask ))
	(( ds_${grp}_norm_${dsgrp} &= ~(3<<dsshift) ))
	if (( norm != 0 )) ; then
		(( ds_${grp}_norm_${dsgrp} |= (norm<<dsshift) ))
	fi
}

output_pin_macro()
{
	local nameUC
	if [ "${grp}" = DUMMY ] ; then
		printf '#define GRP_%-27s\tSP5K_GPIO_GRP_NO_USE\n' \
				"${id}" >> ${resbak}
	else
		eval nameUC=\${grpdef_${grp}_nameUC}
		printf '#define GRP_%-27s\tSP5K_GPIO_GRP_%s\n' \
				"${id}" "${nameUC:-${grp}}" >> ${resbak}
	fi
}

output_pin_io_macro()
{
	local pol
	local name
	if [ "${dir}" = IN ] || [ "${dir}" = IO ] ; then
		if [ "${grp}" = DUMMY ] ; then
			printf '#define IN_%-28s\t0\n' "${id}" >> ${resbak}
		elif [ -z "${gpifun}" ] ; then
			eval name=\${grpdef_${grp}_name}
			printf "#define IN_%-28s\t(${inv}VALI_${name}GPIO(bit_${pin}))\n" "${id}" >> ${resbak}
		else
			printf "#define IN_%-28s\t(${inv}${gpifun}(GRP_${id}, 1<<CFG_${id}))\n" "${id}" >> ${resbak}
		fi
	fi &&
	if [ "${dir}" = OUT ] || [ "${dir}" = IO ] ; then
		if [ "${grp}" = DUMMY ] ; then
			printf "#define OUT_%-27s (const UINT32)(${inv}dummyio)\n" "${id}" >> ${resbak} &&
			printf '#define SET_%-27s\n' "${id}(en)" >> ${resbak}
		else
			if [ -z "${gpifun}" ] ; then
				eval name=\${grpdef_${grp}_name}
				printf "#define OUT_%-27s (const UINT32)(${inv}VALO_${name}GPIO(bit_${pin}))\n" "${id}" >> ${resbak}
			fi &&
			printf "#define SET_%-27s sp5kGpioWrite(GRP_${id}, 1<<CFG_${id}, (${inv}en)<<CFG_${id})\n" "${id}(en)" >> ${resbak}
		fi
	fi &&
	if [ "${dir}" != FN ] ; then
		if [ -z "${inv}" ] ; then
			pol=HIGH
		else
			pol=LOW
		fi
		printf '#define POL_%-27s\tSP5K_GPIO_ACTIVE_%s\n' \
				"${id}" "${pol}" >> ${resbak}
	fi
}

output_pin_cfg_macro()
{
	local -i ion
	if [ "${grp}" != DUMMY ] && [ "${dir}" != FN ] ; then
		ion=$(( ion_${grp} ))
		eval io_${ion}_${grp}_id="${id}"
		eval io_${ion}_${grp}_inv="${inv}"
		eval io_${ion}_${grp}_dir="${dir}"
		eval io_${ion}_${grp}_ini="${ini}"
		(( ++ion_${grp} ))
	fi
	printf "#define CFG_%-27s PIN_${pin}\n\n" "${id}" >> ${resbak}
}

gpio_cfg_layout()
{
	local type=${1}
	local hasprint=

	echo "#define SPCA_${nameUC}_GPIO_${type} (\\" >> ${resbak} &&
	for i in ${ionlist} ; do
		local iodir
		eval iodir=\${io_${i}_${grp}_dir}
		if [[ "${type}" != INIT_* ]] \
		|| [ "${iodir}" = OUT ] || [ "${iodir}" = IO ] ; then
			local ioid
			eval ioid=\${io_${i}_${grp}_id}
			if [ -n "${hasprint}" ] ; then
				echo '| \' >> ${resbak}
			fi &&
			if [ "${type}" = CFGIO ] ; then
				local dir
				if [ "${iodir}" = OUT ] || [ "${iodir}" = IO ]
				then
					dir=OUT
				else
					dir=IN
				fi
				echo -ne "\t\t\tGPIO_CFG_${dir}(CFG_${ioid})" \
						>> ${resbak}
			elif [ "${type}" = INIT_VAL ] ; then
				local val
				if (( io_${i}_${grp}_ini != 0 )) ; then
					val=HI
				else
					val=LO
				fi
				echo -ne "\t\t\tGPIO_PIN_VAL_${val}(CFG_${ioid})" \
						>> ${resbak}
			else
				echo -ne "\t\t\tGPIO_MASK(CFG_${ioid})" \
						>> ${resbak}
			fi &&
			hasprint=yes
		fi
		if [ $? != 0 ] ; then
			echo "ioconv:\`${iofile}':io_${i}_${grp}:gpio cfg \`${type}'" 1>&2
			error
		fi
	done &&
	if [ "${type}" = INIT_VAL ] ; then
		if [ -z "${hasprint}" ] ; then
			echo -ne '\t\t\t0' >> ${resbak}
		fi &&
		echo -e " )\n" >> ${resbak}
	else
		if [ -n "${hasprint}" ] ; then
			echo '| \' >> ${resbak}
		fi &&
		echo -e "\t\t\t(MASK_NOUSE_${nameUC}) )\n" >> ${resbak}
	fi
}

# line easy format: id dir grp pin ini
io_parseline()
{
	local -ra lineitems=(${1})
	id="${lineitems[0]}"
	dir="${lineitems[1]}"
	grp="${lineitems[2]}"
	pin="${lineitems[3]}"
	ini="${lineitems[4]}"
#	echo ${line}:${id}-${dir}-${grp}-${pin}-${ini}=
}

line_parsing()
{
	local line="$1"
	local id=
	local grp=
	local pin=
	local inv=
	local dir=
	local ini=

	if [ -z "${line}" ] ; then
		echo >> ${resbak} # empty line
	elif [[ "${line}" = \;* ]] ; then
		if (( lineno == 1 )) ; then
			platform_info_get "${line}"
		fi &&
		echo "${line:1}" >> ${resbak} # direct include
	else
		line="${line^^}" && # upper cases
		io_parseline "${line}" &&
		finddir &&
		findgrp &&
# checking dir supporting
		dir_check &&
# check pull set support in format PU|PD=0[,1],
# PU or PD must match chip definition, former value is normal pull set
# and the latter one is minimum power pull set
		pull_check_set &&
# check ds set support in format DS=0[,1],
# former value is normal DS and the latter one is minimum power DS, DS=0~3
		ds_check_set &&
		output_pin_macro &&
		output_pin_io_macro &&
		output_pin_cfg_macro
	fi
	if [ $? != 0 ] ; then
		echo "ioconv:\`${iofile}':L$((lineno)):\`${line}'" 1>&2
		error
	fi
}

io_conv()
{
	local iofile=${1}
	local resbak=${2}

# arithmatics of (( <undef_var> )) is value 0 and exit code as false
# such as pindef_${platform}_${grp}_pull_avail is 0 if undef.
	local grplist="DUMMY GEN TG DISP LMI FML FMH POCU USB SAR OIS CEC RGB"

	# ---- assigning platform GPIO group attribute definitions ----
	local GRP_ATTR_READONLY=0x1

	# ---- assigning platform GPIO group definitions ----
	local grpdef_DUMMY_name=dummy
	local grpdef_GEN_name=Gen
	local grpdef_TG_name=Tg
	local grpdef_DISP_name=Disp
	local grpdef_LMI_name=Lmi
	local grpdef_FML_name=Fm
	local grpdef_FMH_name=Fm
	local grpdef_POCU_name=Pocu
	local grpdef_USB_name=Usb
	local grpdef_SAR_name=Sar
	local grpdef_OIS_name=Ois
	local grpdef_CEC_name=Cec
	local grpdef_RGB_name=Rgb
	local grpdef_TG_nameUC=TGL # the only ${grp} != ${grpdef_${grp}_nameUC}
	local grpdef_POCU_attr=$((GRP_ATTR_READONLY))
	local grpdef_USB_attr=$((GRP_ATTR_READONLY))

	# ---- assigning platform GPIO pin definitions ----
	# SPCA5310A // SPCA5310A & B not support POCU / SAR / OIS group
	local pindef_SPCA5310_GEN_max=18
	local pindef_SPCA5310_GEN_pull_avail=0x0003ffff
	local pindef_SPCA5310_GEN_pull_lvl=0x000300c7
	local pindef_SPCA5310_GEN_pull_en=0x00031f38
	local pindef_SPCA5310_TG_max=16
	local pindef_SPCA5310_TG_pull_avail=0x0000ff7f
	local pindef_SPCA5310_TG_pull_lvl=0x001e3c00
	local pindef_SPCA5310_TG_pull_en=0x0018ff60
	local pindef_SPCA5310_DISP_max=32
	local pindef_SPCA5310_DISP_pull_avail=0xffffffff
	local pindef_SPCA5310_DISP_pull_lvl=0x0000e000
	local pindef_SPCA5310_DISP_pull_en=0xffffffff
	local pindef_SPCA5310_LMI_max=12
	local pindef_SPCA5310_LMI_pull_avail=0x00000fff
	local pindef_SPCA5310_LMI_pull_lvl=0x00000000
	local pindef_SPCA5310_LMI_pull_en=0x00000fff
	local pindef_SPCA5310_FML_max=32
	local pindef_SPCA5310_FML_pull_avail=0xffffffff
	local pindef_SPCA5310_FML_pull_lvl=0x00001fcf
	local pindef_SPCA5310_FML_pull_en=0x0000781f
	local pindef_SPCA5310_FMH_max=20
	local pindef_SPCA5310_FMH_pull_avail=0x000fffff
	local pindef_SPCA5310_FMH_pull_lvl=0x00014000
	local pindef_SPCA5310_FMH_pull_en=0x00013fff
	local pindef_SPCA5310_POCU_max=4

	#SPCA5310B
	# the only differences between SPCA5310A and B are FML_pull_lvl
	local pindef_SPCA5310B_GEN_max=18
	local pindef_SPCA5310B_GEN_pull_avail=0x0003ffff
	local pindef_SPCA5310B_GEN_pull_lvl=0x000300c7
	local pindef_SPCA5310B_GEN_pull_en=0x00031f38
	local pindef_SPCA5310B_TG_max=16
	local pindef_SPCA5310B_TG_pull_avail=0x0000ff7f
	local pindef_SPCA5310B_TG_pull_lvl=0x001e3c00
	local pindef_SPCA5310B_TG_pull_en=0x0018ff60
	local pindef_SPCA5310B_DISP_max=32
	local pindef_SPCA5310B_DISP_pull_avail=0xffffffff
	local pindef_SPCA5310B_DISP_pull_lvl=0x0000e000
	local pindef_SPCA5310B_DISP_pull_en=0xffffffff
	local pindef_SPCA5310B_LMI_max=12
	local pindef_SPCA5310B_LMI_pull_avail=0x00000fff
	local pindef_SPCA5310B_LMI_pull_lvl=0x00000000
	local pindef_SPCA5310B_LMI_pull_en=0x00000fff
	local pindef_SPCA5310B_FML_max=32
	local pindef_SPCA5310B_FML_pull_avail=0xffffffff
	local pindef_SPCA5310B_FML_pull_lvl=0x20001fcf
	local pindef_SPCA5310B_FML_pull_en=0x0000781f
	local pindef_SPCA5310B_FMH_max=20
	local pindef_SPCA5310B_FMH_pull_avail=0x000fffff
	local pindef_SPCA5310B_FMH_pull_lvl=0x00014000
	local pindef_SPCA5310B_FMH_pull_en=0x00013fff
	local pindef_SPCA5310B_POCU_max=4

	#SPCA5330
	local pindef_SPCA5330_GEN_max=18
	local pindef_SPCA5330_GEN_pull_avail=0x0003ffff
	local pindef_SPCA5330_GEN_pull_lvl=0x00030007
	local pindef_SPCA5330_GEN_pull_en=0x00031f38
	local pindef_SPCA5330_GEN_ds_avail=0x0003ffff
	local pindef_SPCA5330_TG_max=16
	local pindef_SPCA5330_TG_pull_avail=0x001fff7f
	local pindef_SPCA5330_TG_pull_lvl=0x001e3c00
	local pindef_SPCA5330_TG_pull_en=0x0000fb60
	local pindef_SPCA5330_TG_ds_avail=0x001fff7f
	local pindef_SPCA5330_DISP_max=18
	local pindef_SPCA5330_DISP_pull_avail=0xffffffff
	local pindef_SPCA5330_DISP_pull_lvl=0x0000e000
	local pindef_SPCA5330_DISP_pull_en=0xffffffff
	local pindef_SPCA5330_DISP_ds_avail=0x0003ffff
	local pindef_SPCA5330_LMI_max=16
	local pindef_SPCA5330_LMI_pull_avail=0x0000ffff
	local pindef_SPCA5330_LMI_pull_lvl=0x00000000
	local pindef_SPCA5330_LMI_pull_en=0x0000ffff
	local pindef_SPCA5330_LMI_ds_avail=0x0000ffff
	local pindef_SPCA5330_FML_max=32
	local pindef_SPCA5330_FML_pull_avail=0xffffffff
	local pindef_SPCA5330_FML_pull_lvl=0x20001fcf
	local pindef_SPCA5330_FML_pull_en=0x0000781f
	local pindef_SPCA5330_FML_ds_avail=0xffffffff
	local pindef_SPCA5330_FMH_max=20
	local pindef_SPCA5330_FMH_pull_avail=0x000fffff
	local pindef_SPCA5330_FMH_pull_lvl=0x00014000
	local pindef_SPCA5330_FMH_pull_en=0x00017fff
	local pindef_SPCA5330_FMH_ds_avail=0x000fffff
	local pindef_SPCA5330_POCU_max=4
	local pindef_SPCA5330_USB_max=1
	local pindef_SPCA5330_SAR_max=4
	local pindef_SPCA5330_SAR_pull_avail=0x000000f0
	local pindef_SPCA5330_SAR_pull_lvl=0x00000000
	local pindef_SPCA5330_SAR_pull_en=0x00000000
	local pindef_SPCA5330_OIS_max=4
	local pindef_SPCA5330_OIS_pull_avail=0x0000000f
	local pindef_SPCA5330_OIS_pull_lvl=0x00000000
	local pindef_SPCA5330_OIS_pull_en=0x00000000

	#SPCA6330
	local pindef_SPCA6330_GEN_max=18
	local pindef_SPCA6330_GEN_pull_avail=0x0003ffff
	local pindef_SPCA6330_GEN_pull_lvl=0x00030007
	local pindef_SPCA6330_GEN_pull_en=0x00031f38
	local pindef_SPCA6330_GEN_ds_avail=0x0003ffff
	local pindef_SPCA6330_TG_max=16
	local pindef_SPCA6330_TG_pull_avail=0x001fff7f
	local pindef_SPCA6330_TG_pull_lvl=0x001e3c00
	local pindef_SPCA6330_TG_pull_en=0x0000fb60
	local pindef_SPCA6330_TG_ds_avail=0x001fff7f
	local pindef_SPCA6330_DISP_max=18
	local pindef_SPCA6330_DISP_pull_avail=0xffffffff
	local pindef_SPCA6330_DISP_pull_lvl=0x0000e000
	local pindef_SPCA6330_DISP_pull_en=0xffffffff
	local pindef_SPCA6330_DISP_ds_avail=0x0003ffff
	local pindef_SPCA6330_LMI_max=16
	local pindef_SPCA6330_LMI_pull_avail=0x0000ffff
	local pindef_SPCA6330_LMI_pull_lvl=0x00000000
	local pindef_SPCA6330_LMI_pull_en=0x0000ffff
	local pindef_SPCA6330_LMI_ds_avail=0x0000ffff
	local pindef_SPCA6330_FML_max=32
	local pindef_SPCA6330_FML_pull_avail=0xffffffff
	local pindef_SPCA6330_FML_pull_lvl=0x20001fcf
	local pindef_SPCA6330_FML_pull_en=0x0000781f
	local pindef_SPCA6330_FML_ds_avail=0xffffffff
	local pindef_SPCA6330_FMH_max=20
	local pindef_SPCA6330_FMH_pull_avail=0x000fffff
	local pindef_SPCA6330_FMH_pull_lvl=0x00014000
	local pindef_SPCA6330_FMH_pull_en=0x00017fff
	local pindef_SPCA6330_FMH_ds_avail=0x000fffff
	local pindef_SPCA6330_POCU_max=4
	local pindef_SPCA6330_USB_max=1
	local pindef_SPCA6330_SAR_max=4
	local pindef_SPCA6330_SAR_pull_avail=0x000000f0
	local pindef_SPCA6330_SAR_pull_lvl=0x00000000
	local pindef_SPCA6330_SAR_pull_en=0x00000000 # sar4~7
	local pindef_SPCA6330_OIS_max=4
	local pindef_SPCA6330_OIS_pull_avail=0x0000000f
	local pindef_SPCA6330_OIS_pull_lvl=0x00000000
	local pindef_SPCA6330_OIS_pull_en=0x00000000 # ois0~3
	local pindef_SPCA6330_CEC_max=1

	#SPCA6350 // 6350 remove OIS group
	local pindef_SPCA6350_GEN_max=18
	local pindef_SPCA6350_GEN_pull_avail=0x0003ffff
	local pindef_SPCA6350_GEN_pull_lvl=0x000302c7
	local pindef_SPCA6350_GEN_pull_en=0x0003ff00
	local pindef_SPCA6350_GEN_ds_avail=0x0003ffff
	local pindef_SPCA6350_TG_max=16
	local pindef_SPCA6350_TG_pull_avail=0x000fffff
	local pindef_SPCA6350_TG_pull_lvl=0x000ffc0f
	local pindef_SPCA6350_TG_pull_en=0x000f0370
	local pindef_SPCA6350_TG_ds_avail=0x000fffff
	local pindef_SPCA6350_DISP_max=18
	local pindef_SPCA6350_DISP_pull_avail=0x0003e200
	local pindef_SPCA6350_DISP_pull_lvl=0x0002e000
	local pindef_SPCA6350_DISP_pull_en=0x00000200
	local pindef_SPCA6350_DISP_ds_avail=0x0003e201 #DISP0 stands for 0~7
	local pindef_SPCA6350_DISP_dir_noout=0x00000000
	local pindef_SPCA6350_DISP_dir_noin=0x00001dff #DISP 0~8,10~12 out only
	local pindef_SPCA6350_LMI_max=22
	local pindef_SPCA6350_LMI_pull_avail=0x003fffff
	local pindef_SPCA6350_LMI_pull_lvl=0x0000ff00
	local pindef_SPCA6350_LMI_pull_en=0x000000ff
	local pindef_SPCA6350_LMI_ds_avail=0x003fffff
	local pindef_SPCA6350_FML_max=32
	local pindef_SPCA6350_FML_pull_avail=0xffffffff
	local pindef_SPCA6350_FML_pull_lvl=0xff801fef
	local pindef_SPCA6350_FML_pull_en=0x0001781f
	local pindef_SPCA6350_FML_ds_avail=0xffffffff
	local pindef_SPCA6350_FMH_max=20
	local pindef_SPCA6350_FMH_pull_avail=0x000fffff
	local pindef_SPCA6350_FMH_pull_lvl=0x0003fe00
	local pindef_SPCA6350_FMH_pull_en=0x000001ff
	local pindef_SPCA6350_FMH_ds_avail=0x000fffff
	local pindef_SPCA6350_POCU_max=5
	local pindef_SPCA6350_USB_max=1
	local pindef_SPCA6350_SAR_max=10
	local pindef_SPCA6350_SAR_pull_avail=0x000003df
	local pindef_SPCA6350_SAR_pull_lvl=0x000003ff
	local pindef_SPCA6350_SAR_pull_en=0x000003ff
	local pindef_SPCA6350_CEC_max=1
	local pindef_SPCA6350_OIS_max=0 # obsolete
	local pindef_SPCA6350_RGB_max=26
	local pindef_SPCA6350_RGB_pull_avail=0x03ffffff
	local pindef_SPCA6350_RGB_pull_lvl=0x00000000
	local pindef_SPCA6350_RGB_pull_en=0x00000000
	local pindef_SPCA6350_RGB_dir_noout=0x00000003
	local pindef_SPCA6350_RGB_dir_noin=0x00000003 # RGB0~1 cannot set IO

	local platform=SPCA5310 #very old spca5310 .io has no ";/* SPCAxxxx */"

#space-starting lines as comments,
#removing tailing spaces (also DOS->UNIX text), and then parsing

# [NOTE!] piping sed to "while" results in ineffective variable changes in loop
# using intermediate bak file for sed instead
	$sed -e "s/^$e.*$//" -e "s/$e*$//" ${iofile} > ${iobak} &&
	{ local -i lineno=0

	while read -r line ; do
		((++lineno))
		line_parsing "${line}"
	done
	if [ -n "${line}" ] ; then
		((++lineno))
		echo "ioconv:\`${iofile}':L$((lineno)):no EOL" 1>&2
		line_parsing "${line}"
	fi
	} < "${iobak}" &&
	$rm "${iobak}" &&
	iobak= &&

	echo -e '\n/* GPIO cfg */' >> ${resbak} &&

	# excluding grp "DUMMY"
	for grp in ${grplist#* } ; do
		local nameUC
		local ionlist
		eval nameUC=\${grpdef_${grp}_nameUC}
		nameUC=${nameUC:-${grp}}
		ionlist="`$seq 0 $(( ion_${grp} - 1 ))`" &&
		gpio_cfg_layout CFGIO &&
		gpio_cfg_layout MASK &&
		gpio_cfg_layout INIT_VAL &&
		gpio_cfg_layout INIT_MASK
		if [ $? != 0 ] ; then
			echo "ioconv:\`${iofile}':\`${grp}' gpio cfg" 1>&2
			error
		fi
	done &&

	echo -e '\n/* GPIO pull */' >> ${resbak} &&
	for grp in ${grplist#* } ; do
		local nameUC
		local -i en
		local -i mask
		if (( pindef_${platform}_${grp}_pull_avail == 0 )) ; then
			continue
		fi
		eval nameUC=\${grpdef_${grp}_nameUC}
		nameUC=${nameUC:-${grp}}
		en=$(( pindef_${platform}_${grp}_pull_en ))
		mask=$(( pindef_${platform}_${grp}_pull_avail \
			& pull_${grp}_mask_min ))
		en=$(( (en & ~mask) | pull_${grp}_en_min ))
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" \
			"${nameUC}_PULL_MIN" $(( en )) >> ${resbak} &&
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" \
			"${nameUC}_PULL_MASK_MIN" $(( mask )) >> ${resbak}
		if [ $? != 0 ] ; then
			echo "ioconv:\`${iofile}':\`${grp}' gpio pull min" 1>&2
			error
		fi

		mask=$(( pindef_${platform}_${grp}_pull_avail \
			& pull_${grp}_mask_norm ))
		en=$(( (en & ~mask) | pull_${grp}_en_norm ))
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" \
			"${nameUC}_PULL" $(( en )) >> ${resbak} &&
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" \
			"${nameUC}_PULL_MASK" $(( mask )) >> ${resbak}
		if [ $? != 0 ] ; then
			echo "ioconv:\`${iofile}':\`${grp}' gpio pull" 1>&2
			error
		fi
	done &&
	echo -e '\n' >> ${resbak} &&

	echo -e '\n/* GPIO driving strength */' >> ${resbak} &&
	for grp in ${grplist#* } ; do
		local nameUC
		local mask;
		if (( pindef_${platform}_${grp}_ds_avail == 0 )) ; then
			continue
		fi
		eval nameUC=\${grpdef_${grp}_nameUC}
		nameUC=${nameUC:-${grp}}
		mask=$(( pindef_${platform}_${grp}_ds_avail \
			& ds_${grp}_mask_min ))
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS0_MIN" \
				$(( ds_${grp}_min_0 )) >> ${resbak} &&
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS1_MIN" \
				$(( ds_${grp}_min_1 )) >> ${resbak} &&
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS_MASK_MIN" \
				$(( mask )) >> ${resbak}

		mask=$(( pindef_${platform}_${grp}_ds_avail \
			& ds_${grp}_mask_norm ))
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS0" \
				$(( ds_${grp}_norm_0 )) >> ${resbak} &&
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS1" \
				$(( ds_${grp}_norm_1 )) >> ${resbak}
		printf "#define SP5K_GPIO_%-26s 0x%08X\n" "${nameUC}_DS_MASK" \
				$(( mask )) >> ${resbak}
		if [ $? != 0 ] ; then
			echo "ioconv:\`${iofile}':\`${grp}' gpio ds" 1>&2
			error
		fi
	done &&
	echo -e '\n' >> ${resbak}
}


# ------ main ------

cat >> ${resbak} <<-EOF &&
	#ifndef ${headertag}
	#define ${headertag}

	#ifdef __cplusplus
	extern "C" {
	#endif

	EOF

io_conv ${iofile} ${resbak} &&

cat >> ${resbak} <<-EOF &&

	#ifdef __cplusplus
	}
	#endif

	#endif /* ${headertag} */
	EOF

$mv -f ${resbak} ${res}

if [ $? != 0 ] ; then
	echo "ioconv:\`${iofile}'>\`${res}':failed" 1>&2
	error
fi
