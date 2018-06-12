#!/bin/bash

#Utility path definition
usb_boot=C:/Progra~1/Sunplus/UsbBoot/usbboot.exe
mode=$1
if [ "$mode" = 0 ] ; then 
	tar_script=ubm.sh
else
	tar_script=ubn.sh
fi

#Check parameter
if [ $# = 0 ] ; then 
	echo Usage: $0 \<ProjectName\> [ParameterFile] 1>&2
	exit 1
fi

prj=$2
if [ -z "$3" ] ; then
	dpfile="-I ./frm_user.ini"
else
	dpfile=$3
fi

prefix=$(dirname $0) &&
makekeyget=$prefix/../tool/makekeyget.sh &&
prjname=$($makekeyget _$prj/project/$prj.def PRJ_NAME)
if [ $? != 0 ] || [ -z "$prjname" ] ; then
	echo "$0:bad prj" 1>&2
	exit 1
fi

#Target script file generation
cat > ${tar_script}.bak <<-EOF &&
#!/bin/bash

echo USB boot..

#Utility path definition
usbboot=$usb_boot
prefix=\`dirname \$0\`

pushd \$prefix/_$prj/download/
EOF
if [ $mode = 0 ] ; then 
	cat >> ${tar_script}.bak <<-EOF
	#Dram parameter download
	./SetPara.exe $dpfile
	#Customized register set
	./SetReg.exe -I ./SetReg.ini
	EOF
else
	cat >> ${tar_script}.bak <<-EOF
	#Normal Boot
	EOF
fi &&
cat >> ${tar_script}.bak <<-EOF &&
#F/W bin file download
\$usbboot 0xa0000000 ./$prjname.BIN
fmtime=\`stat -c %y ./$prjname.BIN\`
popd
echo
echo "F/W compiled at \$fmtime"
cutime=\`date "+%F %T.%N %z"\`
echo "Current time is \$cutime"
EOF

mv ${tar_script}.bak $tar_script &&

#Set script file access mode
chmod a+x $tar_script
