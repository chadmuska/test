#/**************************************************************************
# *
# *         Copyright (c) 2014 by iCatch Technology Inc.
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
# \file   multiboot_stages.pm
# \brief  modules to use in multiboot tools
# \author Anmin Deng

use strict;
use warnings;
use integer;

package multiboot_stages;
require Exporter;

our @ISA = 'Exporter';
our @EXPORT_OK = qw(
	path_stage_find
	%nostage_obj_hash nostage_obj_hash_make
);


# ==== to find the specific staged item in path ====
# \param $_[0] stage
# \param $_[1] path0
# \param $_[2] ext
# \param $_[3] noret, optional, return value if path has no stage item exists
# \return
# $_[3] if $_[1] path has no stage item exists;
# undef path has stage items but stage < nostage;
# o/w the staged item name;
sub
path_stage_find($$$;$)
{
	my ($stage, $path0, $ext, $noret) = @_;

	my @slist = sort { $a <=> $b }
	map { /\.stage(\d+)\Q$ext\E$/; } glob "$path0.stage*$ext"
		or return $noret;

	# binary searching in sorted list
	my $onstage;
	for (my ($imin, $imax) = (0, $#slist);;) {
		my $imid = ($imin + $imax) / 2;
		$onstage = $slist[$imid];
		last if ($stage == $onstage);
		if ($stage > $onstage) {
			last if $imid == $imax;
			$imin = $imid + 1;
			if ((my $nextstage = $slist[$imin]) >= $stage) {
				$onstage = $nextstage if $nextstage == $stage;
				last;
			}
		} else {
			$imin != $imax and ($imax = $imid - 1) >= $imin or
			return undef;
			#die "$0:`$path0.stage$onstage$ext':stage$stage least";
		}
	}

	return "$path0.stage$onstage$ext";
}


# ==== to create nostaged obj list/hash to lookup ====

# shadow of @nostage_obj_list for easier lookup:
# hash of { obj => nostage, .. }, where nostage can be undef for normal obj
our %nostage_obj_hash = ();

# \param $_[0] slist, nostage obj list;
# \param $_[1] opt_check, defined to check if nostage obj list item exists;
sub
nostage_obj_hash_make($$)
{
	my ($slist, $opt_check) = @_;
	my @nostage_obj_list  = split ' ', $slist;

	foreach my $sobj (@nostage_obj_list) {
		if (my ($obj0, $nostage0, $ext) =
		$sobj =~ /^(.*?)(?:\.nostage(\d+))?(\.[ao])$/) {
			!$opt_check or my @o = glob "$obj0.stage*$ext"
				or die "$0:`$sobj':stage obj not exists";

			$nostage_obj_hash{"$obj0$ext"} = $nostage0;
		} elsif (my ($libfull, $path0, $lib0, $o0, $nostage) =
		$sobj =~ m#^((.*)/([^/]*?)\.a)\((.*?)(?:\.nostage(\d+))?\.o\)$#)
		{
		# $path0/_multiboot.bak/$lib0/*/$o0.stage*.o not supported
			!$opt_check or
			my @o = glob "$path0/_multiboot.bak/$lib0/$o0.stage*.o"
				or die "$0:`$sobj':stageobj not found";

			$nostage_obj_hash{"$libfull($o0.o)"} = $nostage;
		} else {
			die "$0:`$sobj':invalid stage obj";
		}
	}
}


1;
