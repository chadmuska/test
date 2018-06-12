#!/usr/bin/perl

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
# \file   multiboot_lscript_input.pl
# \brief  to generate the input list for linking multiboot project
# \author Anmin Deng

use strict;
use warnings;
use integer;


# \param[in] options must be provided before parameters:
#   -q, to suppress printing to stdout;
#   -k, to check if the result items exist;
# \param[in] $ARGV[0], stage;
# \param[in] $ARGV[1], the original input list to be fed to the linker,
# eg 'obj1.o .. libx.a .. -lx ..', note the quote;
# \param[in] $ARGV[2], optional, the stage obj list,
# eg 'obj1.o obj2.nostage9.o ..', note the quote;
# \return exit code 0: OK; o/w failed;
# print out result input list to stdout unless option -q


# ==== parsing arguments ====

our $opt_quiet;
our $opt_check;

while (@ARGV) {
	my $arg = $ARGV[0];
	substr($arg, 0, 1) eq '-' or last;
	shift @ARGV;
	if ($arg eq '-q') {
		$opt_quiet = 1;
	} elsif ($arg eq '-k') {
		$opt_check = 1;
	} else {
		die "$0:`$arg':option invalid";
	}
}

(@ARGV == 2 or @ARGV == 3)
or die "usage:$0 [options] `stage' 'origlist' [`stageobjlist']\n",
	"\t-q: to suppress printing to stdout\n",
	"\t-k: verify if the result items exist";


my ($stage, $olist, $slist) = @ARGV;

$stage >= 1 and $stage <= 32 or die "$0:`$stage':stage [1..32] invalid";

my @orig_input_list = split ' ', $olist;


use FindBin qw($Bin);
use lib "$Bin";

if (@ARGV == 3) {
	use multiboot_stages qw(%nostage_obj_hash nostage_obj_hash_make);
	nostage_obj_hash_make $slist, $opt_check;
}


# ==== sub routines ====

# \param $_[0] stage
# \param $_[1] lib0
sub
archive_stage_find($@)
{
	my ($stage, $lib0) = @_;

	use multiboot_stages 'path_stage_find';

	path_stage_find $stage, $lib0, '.a', "$lib0.a";
}

# \param $_[0] stage
# \param $_[1] item (normal/staged obj or archive)
# \param $_[2] hash of staged objs
sub
input_stage_find($$\%)
{
	my ($stage, $i, $sobjhash) = @_;
	my $stageitem;

	if (substr($i, 0, 2) eq '-l') {
		return $i;
	}
	elsif (my ($obj0, $nostage0) = $i =~ /^(.*?)(?:\.nostage(\d+))?\.o$/) {
		$stageitem = "$obj0.stage$stage.o";
		my $key0 = "$obj0.o";
		if ($nostage0) {
			exists $sobjhash->{$key0}
			and $nostage0 == $sobjhash->{$key0}
			and $nostage0 <= $stage
				or return undef;
				# or die "$0:`$i':nostage lost";
		} elsif (exists $sobjhash->{$key0}) {
			my $nostage = $sobjhash->{$key0};
			!defined $nostage or $stage >= $nostage
				or return undef;
				# or die "$0:`$i':stage $nostage least";
		} else {
			$stageitem = $i;
		}
	} elsif (my ($lib0) = $i =~ /^(.*)\.a$/) {
		defined($stageitem = archive_stage_find $stage, $lib0)
			or return undef;
	} else {
		die "$0:`$i':invalid input";
	}

	-e $stageitem and -f $stageitem and -r $stageitem
		or die "$0:`$stageitem':input stage $stage not exists"
		if $opt_check;

	return $stageitem;
}


# ====== main program ======
#

foreach my $i (@orig_input_list) {
	my $stageitem = input_stage_find $stage, $i, %nostage_obj_hash;
	print $stageitem, "\n" if !defined $opt_quiet and defined $stageitem;
}
