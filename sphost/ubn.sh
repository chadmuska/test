#!/bin/bash

echo USB boot..

#Utility path definition
usbboot=C:/Progra~1/Sunplus/UsbBoot/usbboot.exe
prefix=`dirname $0`

pushd $prefix/_17DVR/download/
#Normal Boot
#F/W bin file download
$usbboot 0xa0000000 ./17DVR.BIN
fmtime=`stat -c %y ./17DVR.BIN`
popd
echo
echo "F/W compiled at $fmtime"
cutime=`date "+%F %T.%N %z"`
echo "Current time is $cutime"
