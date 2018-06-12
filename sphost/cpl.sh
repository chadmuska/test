#!/bin/bash


#
# Change the following by yourself
#
PLATFORM=spca6350
#SRC_FLAG_I="install ram=1 ram4g=1 FPGA=YES DSP=YES HAPS62=YES -j"
#SRC_FLAG_C="clean   ram=1 ram4g=1 FPGA=YES DSP=YES HAPS62=YES -j"
SRC_FLAG_I="install -j"
SRC_FLAG_C="clean   -j"
HOST_FLAG_I="RES=NO -j"
HOST_FLAG_C="clean  -j"
HOST_FLAG_I_RES="   -j"
#PRE_CMD="touch customization/sp5k_job.c"
#POST_CMD="cp -f _EVB/download/EVB.bin /cygdrive/r/sphost/download"
#POST_CMD="./down.exe _EVBHOST/download/EVBHOST.bin"
#POST_CMD="ls -la _EVB/download/EVB.bin"
#POST_CMD="./ubm.sh"

warning_prompt=1
host_warning_prompt=0

#
# You don't have to change the following
#
SRC_PATH=../${PLATFORM}/src
SEARCH_PATH="middleware middleware/drivers api"


#
# Here starts the program
#
sp="[[:blank:]]"
w="[[:alnum:]]"

# check the library is empty or not
if [[ -z $1 ]]; then
	exit 1
fi

# Warning dump support.
if [[ ${warning_prompt} != 0 || ${host_warning_prompt} != 0 ]];then
	root=`pwd`
	warnlog=$root/_icatch_warning_summary.txt

	if [[ -a ${warnlog} ]];then
		rm ${warnlog}
	fi
fi

# Manual flags by command line
MFLAGS=""

# Modules to be compiled.
modules=""
# Separate flags and modules at first.
while [[ ! -z $1 ]]
do
	case $1 in
		'-c' | '-C')
			MAKE_CLEAN="1"
			;;
		*=*)
			MFLAGS="${MFLAGS} $1"
			;;
		*)
			modules="${modules} $1"
			;;
	esac
	shift
done

for module in ${modules}
do
	#clear the target at first
	target=""
	echo ${module}
	case ${module} in
		'host')
			break	
			;;
		'src' | 'all')
			target=${SRC_PATH}
			;;
		'os')
			target=${SRC_PATH}/os
			;;
		'hal')
			target=${SRC_PATH}/hal
			;;
		'middleware')
			target=${SRC_PATH}/middleware
			;;
		'api')
			target=${SRC_PATH}/api
			;;
		'5kmodesw' | '5kgfx' | '5kglobal' | '5kpip' | '5kfs' | '5kslide' | '5kdisp' | '5kcec')
			suffix=`echo ${module} | sed -e "s/5k//"`
			target=${SRC_PATH}/api/$suffix
			;;
		'dq')
			target=${SRC_PATH}/middleware/libdq
			;;			
		*)
			for search in ${SEARCH_PATH}
			do
				target_path=${SRC_PATH}/${search}/${module}
				if [[ -f ${target_path}/Makefile ]]; then
					target="${target} ${target_path}"
				fi
			done
			;;
	esac
	
	if [[ ! -z ${target} ]]; then
		for t in ${target}
		do
			pushd ${t}
			echo ""
			echo "[Compile] Enter ${t} "
			if [[ ! -z ${MAKE_CLEAN} ]]; then
				echo "make ${SRC_FLAG_C} ${MFLAGS}"
				make ${SRC_FLAG_C} ${MFLAGS}
				if [[ $? != 0 ]]; then
				exit 1
				fi
			fi
	
			echo "make ${SRC_FLAG_I} ${MFLAGS}"
			if [[ ${warning_prompt} != 0 ]];then
				make ${SRC_FLAG_I} ${MFLAGS} 2>&1 | tee -a ${warnlog}
				if [[ ${PIPESTATUS[0]} != 0 ]]; then
					exit 1
				fi
			else
				make ${SRC_FLAG_I} ${MFLAGS}
				if [[ $? != 0 ]];then
					exit 1
				fi
			fi
			popd
		done
	else
		echo "[Compile] Cannot found - $1 -"
		exit 1
	fi
	
	shift

done

if [[ ! -z ${PRE_CMD} ]]; then
	echo ${PRE_CMD}
	${PRE_CMD}
	if [[ $? != 0 ]]; then
		exit 1
	fi
fi

if [[ ! -z ${MAKE_CLEAN} ]]; then
	echo "make ${HOST_FLAG_C}"
	make ${HOST_FLAG_C}
	if [[ $? != 0 ]]; then
	exit 1
	fi
	HOST_FLAG=${HOST_FLAG_I_RES}
else
	HOST_FLAG=${HOST_FLAG_I}
fi

echo "make ${HOST_FLAG}"
if [[ ${host_warning_prompt} != 0 ]]; then
	make ${HOST_FLAG} 2>&1 | tee -a ${warnlog}
	if [[ ${PIPESTATUS[0]} != 0 ]]; then
		exit 1
	fi
else
	make ${HOST_FLAG}
	if [[ $? != 0 ]]; then
		exit 1
	fi
fi



# Exec POST_CMD if any
if [[ ! -z ${POST_CMD} ]]; then
	echo ${POST_CMD}
	${POST_CMD}
fi

if [[ ${warning_prompt} != 0 || ${host_warning_prompt} != 0 ]]; then
	echo
	echo "Warnings..."
	if [[ -f ${warnlog} ]]; then
		grep 'warning' ${warnlog} | grep -v '\[-Wcpp\]' 
	fi
fi
