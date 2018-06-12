#!/bin/sh
# release batch for vq projects
#
# It will
# 1. build basefw if no argument is specified
# 2. for each project
#	a. call config.sh to configure
#	b. build project
#	c. call ./zbin.sh to zip specified projects downloadable firmware and build card isp (if necessary)
# 3. call ./zvc83.sh & ../tool/zlib.bat to zip host code & environment (../5210/toolchain.def, ../5210/inc, ../5210/lib, & ../tool)
# 4. call zbase.bat to zip basefw code if no argument is specified
# ##5. call mail_vbs.sh to generate mailing VBSes.

init() {
	# You can chang directly change project here
	VQ_PRJ="vsc400w vsc540z vsd540z"
	#BQ_PRJ="bqc403s bqc503z bqc513s"
	PROJECTS="$VQ_PRJ ""$BQ_PRJ"

	for PROJECT in $PROJECTS
	do
		platform=$(./helper.sh project_platform $PROJECT)
		case "$platform" in
		spca5210)
			LIB5210=YES
			;;
		spca5310)
			LIB5310=YES
			;;
		esac
	done

	HOSTPATH=$(pwd)
	HOSTPATH=${HOSTPATH##/*/}
	# get base_version
	BASEVER=$(./helper.sh base_version)
	parabuild="-j 2" # faster build
}

buildbasefw () {
	echo ======================
	echo  build basefw
	echo ======================
	if [ -d ../$1/src ] ; then
		make install $parabuild -C ../$1/src SENSOR_BUILD=YES
		if [ $? -ne 0 ] ; then
			echo Error building basefw $1
			exit 1
		fi
	fi
}

buildbasefw_all() {
	if [ -n "$LIB5210" ] ; then 
		buildbasefw 5210 
	fi
	if [ -n "$LIB5310" ] ; then 
		buildbasefw spca5310 
	fi
}

buildhost () {
	echo ======================
	echo  build host $1
	echo ======================
	rm -rf _5210
	./config.sh $1 
	make clean
	make $parabuild
	if [ $? -ne 0 ] ; then
		echo Error building $1
		exit 1
	fi
	
	echo ======================
	echo  zip $1 download
	echo ======================
	./zbin.sh
}

buildhost_all () {
	for PROJECT in $PROJECTS
	do
		buildhost $PROJECT
	done  
}

ziphost() {
	echo ===============
	echo  zip host code
	echo ===============
	./zvc83.sh
}

ziplib() {
	echo ===============
	echo  zip lib
	echo ===============

	cd ../tool
	if [ "$LIB5210" = "YES" ] ;	then
		./zlib.bat 5210 
		mv ../lib5210.7z ../lib5210_$BASEVER.7z 
	fi
	if [ "$LIB5310" = "YES" ] ;	then
		./zlib.bat spca5310 
		mv ../libspca5310.7z ../lib5310_$BASEVER.7z 
	fi
	cd ../$HOSTPATH
}

zipbase() {
	echo ===============
	echo  zip base code
	echo ===============
	cd ../tool
	if [ "$LIB5210" = "YES" ] ;	then 
		./zbase.bat 5210
		mv ../base5210.7z ../base5210_$BASEVER.7z 
	fi
	if [ "$LIB5310" = "YES" ] ;	then 
		./zbase.bat spca5310
		mv ../basespca5310.7z ../base5310_$BASEVER.7z 
	fi
	cd ../$HOSTPATH
}

###############################
# main entry
init
#buildbasefw_all
buildhost_all
ziphost  
#ziplib
#zipbase

