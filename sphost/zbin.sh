#!/bin/bash
prj=$(./helper.sh project)
prjname=$(./helper.sh prjname)
platform=$(./helper.sh platform)
base_ver=$(./helper.sh base_version)
host_ver=$(./helper.sh host_version)
host_subver=$(./helper.sh host_subversion)
host_rev=$(./helper.sh host_revision)
datestr=`date +%m%d`
zipname="$prjname"_"$base_ver"_V"$host_ver"."$host_subver"."$host_rev"_"$datestr"B

#dirname=$datestr"_Release_for_VCA370z("$host_ver"."$host_subver")"
#mkdir $dirname

case $1 in
[0-9])
	mx=$1
	;;
*)
	mx=9
	;;
esac

echo archive $zipname ...
../tool/7za.exe a -mx$mx -t7z -r "../$zipname.7z" _$prj/download -xr!*.BRN -xr!*.bak -xr!*.scc -x!temp/* -x!?IMG -xr!*.bin_dram -xr!.svn

echo archive $prj elf.7z ...
../tool/7za.exe a -mx$mx -t7z "../$zipname elf.7z" _$prj/$platform.elf _$prj/$platform.map
