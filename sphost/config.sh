#!/bin/bash
# configure project-specific settings of Sunplus SP5K platform
# It will
# 1. Call "make clean_resource" to clean old project resource
# 2. Call "sed" to change project-specific settings in
# makefile, dev_init.h
#
# SOP for adding new model (add a new project: zz6336n)
# 1. change config.sh
#	find old project (ex vc6335n) with similar hw and duplicate and change to zz6335n (at least two places)
# 2. search whole project for HW_VC6335N/PRJ_VC6335N (depend on your new project)
#   check if you need to add defined(PRJ_ZZ6336N) code
# 3. copy project/vc6336n.def to project/zz6335n.def and
#	make necessary change (folder name, project files, PRJ_LIB (libhost3a.a...) ...)
# 4. copy resource/vc7335n to resource/zz6335n
#	modify resource/zz633n/frm_user.ini: FirmwareName = VC7335N.BIN
#
# Adding illustration for updated new model SOP Usage
# 1.Project name ane hardware definition are fixed for one project.
# 2.HW_VER definition is to be used.
# 3.HW_VER represents different stage of HW,ex:A1,A2,B1,B2...
# It can be used for different adc key/gpio/hw related issue handling.
# 4.The latest version of HW kit is with no HW_VER definition.
#

export LC_ALL=C

s="[[:blank:]]"
w="[_[:alnum:]]"
W="[^_[:alnum:]]"

prefix=`dirname $0`
ckeyset=$prefix/../tool/ckeyset.sh
makekeyget=$prefix/../tool/makekeyget.sh
makekeyset=$prefix/../tool/makekeyset.sh

mkdir=/bin/mkdir
cp=/bin/cp
mv=/bin/mv
rm=/bin/rm
touch=/bin/touch
tr=/usr/bin/tr
diff=/usr/bin/diff
patch=/usr/bin/patch
make=/usr/bin/make

cfgfile_check_diff()
{
	$diff --version > /dev/null
	if [ $? != 0 ]; then
		echo "$0:$diff:tool invalid" 1>&2
		exit 1;
	fi
}

cfgfile_copy()
{
	local target=${1}
	local dest=$destdir/$target

# creating patch file
	$rm -f $dest.patch
# diff exit code: 0=the same; 1=different; 2,others=trouble
	if [ -f "$dest.base" ]; then
		$diff -duN $dest.base $target.tmpl > $dest.patch
		err=$?
		if [ $err != 0 ] && [ $err != 1 ] ; then
			echo "$0:$target:diff failed" 1>&2
			exit 1
		fi
	else
		echo "$0:$target:copying default"
		$mkdir -p `dirname $dest` &&
		$cp -fu $target.tmpl $dest.base &&
		$touch $dest.patch
		if [ $? != 0 ] ; then
			echo "$0:$target:copying base failed" 1>&2
			exit 1
		fi
	fi

	if [ -f "$dest" ]; then
		if [ -s "$dest.patch" ] ; then
			echo "$0:$dest:merging new version"
			if $patch --verbose -u -i $dest.patch -r $dest.rej -o $dest.merge $dest ; then
				$cp -f $target.tmpl $dest.base &&
				$mv -f $dest.merge $dest
			else
				echo "$0:$target.tmpl conflicts, use theirs" 1>&2
				echo "$0:backup mine in $dest.{bak,base.bak,merge,rej}" 1>&2
				$mv -f $dest $dest.bak &&
				$mv -f $dest.base $dest.base.bak &&
				$cp -f $target.tmpl $dest.base &&
				$cp -f $target.tmpl $dest
			fi
		fi
	else
		echo "$0:$dest:copying default"
		$mkdir -p `dirname $dest` &&
		$cp -f $target.tmpl $dest.base &&
		$cp -f $target.tmpl $dest
	fi
	if [ $? != 0 ] ; then
		echo "$0:$dest:failed" 1>&2
		exit 1
	fi
}

cfg_makekey_set()
{
	local target=${1}
	local key=${2}
	local val=${3}

	$makekeyset $destdir/$target $key $val
	if [ $? != 0 ] ; then
		echo "$0:$target:$key:$val:set failed" 1>&2
		exit 1
	fi
}

cfg_ckey_set()
{
	local target=${1}
	local key=${2}
	local val=${3}

	$ckeyset $destdir/$target $key $val
	if [ $? != 0 ] ; then
		echo "$0:$target:$key:$val:set failed" 1>&2
		exit 1
	fi
}


clean()
{
	echo "Clean Host..."
	$make clean > /dev/null
}

dram_config()
{
	local target=project/$1.def
	local dram2g

	if [ $# -lt 2 ] || [ "$2" != 2g ] ; then
		echo "$0:$1:dram parm-1g"
		dram2g=NO
	else
		echo "$0:$1:dram parm-2g"
		dram2g=YES
	fi

	cfg_makekey_set $target HW_DDR3_PARM_2G $dram2g
}

# /** \param $1: $prj */
platform_config () {
	if [ -z "$cfgchip" ] ; then
		platform=$($makekeyget $destdir/project/$1.def PLATFORM)
	elif [[ "$cfgchip" = [0-9]* ]] ; then
		platform=spca$cfgchip  # only given chip number, add prefix
	else
		platform=$cfgchip
	fi
	#oldchip=$($prefix/helper.sh platform)
	nandtime=0x13
	platform_path=$platform
	if [ ! -f $prefix/../$platform_path/tool_chain.def ] ; then
		echo "$0:$1:$platform_path:basefw not avaiable" 1>&2
		exit 1
	fi
	#if [ "$platform" != "$oldchip" ] ; then
	#	platform_changed=YES
	#fi
}
#hardware_config only to check the hw define
hardware_config () {
	case "$hw_with_ver" in
	EVB)
		;;
	DUAL2)
		;;
	SBC)
		;;
	V35EVB)
		;;
	JETDVR)
		;;
	PDVR)
		;;
	17DVR)
		;;
	19DVR)
		;;
	*)
		echo "$0:\`$hw_with_ver':unknown hw" 1>&2
		exit 1
		;;
	esac
}

software_config() {
# hw_facetrack
	hw_facetrack=NO

# hw_dist
	hw_dist=NO
}

configure () {
	local prj=${1}
	hw=`$tr '[a-z]' '[A-Z]' <<< "${2}"`      # ucase
	hw_ver=`$tr '[a-z]' '[A-Z]' <<< "${3}"`  # ucase
	hw_with_ver=$hw
	if [ -n "$hw_ver" ] ; then
		hw_with_ver=${hw}_$hw_ver
	fi

	#echo prj=$prj
	#echo hw=$hw
	#echo hw_ver=$hw_ver
	#echo hw_with_ver=$hw_with_ver

	platform_config $prj
	hardware_config $hw $prj
	software_config $hw $prj

#
	target=host.def
	echo Update $target
	$makekeyset $target HW_DIST $hw_dist
	$makekeyset $target HW_FACETRACK $hw_facetrack

#
	target=project/$prj.def
	if [ -n "$cfgchip" ] ; then
		echo Update $target
		cfg_makekey_set $target PLATFORM $platform
	fi

	$prefix/ubgen.sh 0 $prj
	$prefix/ubgen.sh 1 $prj

#
	$make -C $prefix PRJ=$prj ${fpga_yes} lastprj.def
	hw_ver_old=$($prefix/helper.sh hardware_version)
	if [ -n "$hw_ver" ] ; then
		echo "HW_VER := $hw_ver" > last_hwver_$prj.def
	else
		$rm -f last_hwver_$prj.def
	fi
	hw_ver=$($prefix/helper.sh hardware_version)

	if [ -n "$cfgchip" ] || [ "$hw_ver_old" != "$hw_ver" ] ; then
		clean $clean_host
	fi

}

# after branch, remove all extended project like PL/RO/MU...

main ()
{
	cfgname=$1
	cfgver=${cfgname##*_}
	if [ -z "$cfgver" ] ; then
		cfgname=${cfgname%_*}
	elif [ "$cfgver" = "$cfgname" ] ; then
		cfgver=
	else
		cfgname=${cfgname%_*}
	fi

	if [ "$cfgname" = dual2 ] || [ "$cfgname" = evb ] || [ "$cfgname" = sbc ]|| [ "$cfgname" = jetdvr ]|| [ "$cfgname" = pdvr ]|| [ "$cfgname" = 17dvr ]|| [ "$cfgname" = 19dvr ];  then
		cfgchip=$3
	else
		cfgchip=$2
	fi

	prj=`$tr '[a-z]' '[A-Z]' <<< "${cfgname}"`     # ucase
	destdir=$prefix/_$prj

# force update iCatchTek.xls to current system time, mark by JQ
#touch ./resource/$prj/MENU/iCatchTek.xls

	case "$cfgname" in
	evb)
		cfgname=v35evb # for using _EVB_DSC13003_HW_
		;;
	sbc)
		cfgname=sbc # for using _SBC_HW_
		;;
	dual2)
		cfgname=v35evb # for using _EVB_DSC13003_HW_
		;;
	jetdvr)
		cfgname=jetdvr # for using _JETDVR_HW_
		;;
	pdvr)
		cfgname=pdvr # for using _PDVR_HW_
		;;
	17dvr)
		cfgname=17dvr # for using _17DVR_HW_
		;;
	19dvr)
		cfgname=19dvr # for using _19DVR_HW_
		;;
	*)
		echo "Usage: $0 [project [platform]]"
		echo "  Available projects:"
		echo "  evb   : EVB (DSC13003A) "
		echo "  sbc   : SBC "
		echo "  dual2   : EVB (DSC13003A) "
		echo "  jetdvr   : JETDVR "
		echo "  pdvr   : PDVR "
		echo "  17dvr   : 17DVR "
		echo "  19dvr   : 19DVR "
		echo
		exit
		;;
	esac

	cfgfile_copy sensor.def
	cfgfile_copy multiboot.def
	cfgfile_copy project/$prj.def
	cfgfile_copy include/customization/macro_expand.h
	cfgfile_copy include/customization/app_init.h
	cfgfile_copy include/customization/dev_init.h
	cfgfile_copy include/customization/project_def.h

	configure $prj $cfgname $cfgver

	if [ "$prj" = DUAL2 ] || [ "$prj" = EVB ] || [ "$prj" = SBC ]|| [ "$cfgname" = jetdvr ]|| [ "$cfgname" = pdvr ]|| [ "$cfgname" = 17dvr ]|| [ "$prj" = 19DVR ]; then
		echo "$prj: dram parm-check"
		dramtype=$($tr '[A-Z]' '[a-z]' <<< "$2")
		dram_config $prj $dramtype
	fi

	echo "Current project: $($prefix/helper.sh project) [ $($prefix/helper.sh hardware) $($prefix/helper.sh hardware_version) $($prefix/helper.sh platform) ]"
}

cfgfile_check_diff

main $*
