#!/bin/bash
if [ $# -gt 0 ] ; then
	prj=$(echo $1 | tr '[a-z]' '[A-Z]')
else
	prj=$(./helper.sh project)
fi

_$prj/download/frm.exe &
