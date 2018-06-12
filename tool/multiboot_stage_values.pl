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
# \file   multiboot_stage_values.pl
# \brief  to generate the stage value list of the given multiboot stage label
# list from multiboot_api.h header
# \author Anmin Deng

use strict;
use warnings;
use integer;


# \param[in] options, must be provided before parameters:
#   -s, sorting in unique value (-l not supported);
#   -l, list of multiboot stage labels (list all if no label given);
#   -c, pre-processing only accepts C comments (not processing //...);
# \param[in] $ARGV[0], 'multiboot_api.h' header file name
# \param[in] $ARGV[1] .., the rest of the args, list of multiboot stage labels
# or labels derived from source file if leading with '@';
# file name '-' means stdin;
# \return exit code 0: OK; o/w failed;
# print out result list of stage values to stdout


# ==== parsing arguments ====

my  $opt_sort;
my  $opt_list;
our $opt_ansi;

while (@ARGV) {
	my $arg = $ARGV[0];
	substr($arg, 0, 1) eq '-' or last;
	$arg ne '-' or last;
	shift @ARGV;
	if ($arg eq '-s') {
		$opt_sort = 1;
	} elsif ($arg eq '-l') {
		$opt_list = 1;
	} elsif ($arg eq '-c') {
		$opt_ansi = 1;
	} else {
		die "$0:`$arg':option invalid";
	}
}

!($opt_list and $opt_sort) and @ARGV >= 1
or die "usage:$0 [options] `stageheader' [`stagelabel'|@`src' ..]\n",
	"\tlist all if no label given; file name '-' means <stdin>;\n",
	"\t[options] must be provided before parameters:\n",
	"\t-s: sorting in unique value (-l not supported)\n",
	"\t-l: list of multiboot stage labels\n",
	"\t-c: pre-processing only accepts C comments (not processing //...)";


# not supporting multiple stdin as input streams
my $hasstdin;

my $hdr = shift @ARGV;
my $fhdr;
if ($hdr eq '-') {
	$hasstdin = 1;
	open $fhdr, "<-" or die "$0:`$hdr':$!";
} else {
	open $fhdr, "<", $hdr or die "$0:`$hdr':$!";
}


# ==== functions ====
# \param[in] $_[1] file name
# \param[in] $_[0] file handle
# \return pre-processed line text, undef at eof or errors
# \bug errors when C or C++ comment in string or crossing #if .. #endif
sub
c_getline_pre_process($*)
{
	my ($f, $fh) = @_;

	defined(my $line = <$fh>) or return;

	# removing dos eol
	chomp $line; $line =~ tr/\r//d;
	rejoin: {
		# joining tailing '\' with the next line
		while (substr($line, -1) eq '\\') {
			defined(my $line2 = <$fh>)
				or die "$0:`$f':`$line':escape lost";
			chomp $line2; $line2 =~ tr/\r//d;
			substr($line, -1) = $line2;
		}
		# removing C++ comment, prior to C comment
		$line =~ s,\s*(//.*)?$,, unless $opt_ansi;
		# removing C comments in single line
		$line =~ s,/\*.*?\*/, ,g;
		# removing C comment in multiple lines
		if ($line =~ s,/\*.*$, ,) {
			while (my $line2 = <$fh>) {
				# removing dos eol
				chomp $line2; $line2 =~ tr/\r//d;
				$line2 =~ s,^.*?\*/,, or next;
				$line .= $line2;
				redo rejoin;
			}
			die "$0:`$f':`$line':comment lost";
		}
	}
	# removing tailing spaces
	$line =~ s,\s*$,,;

	return $line;
}


# ==== processing multiboot stage labels and values defined in header ====
my @multiboot_stage_def_list = ();
# hash of { label => value, .. }
my %multiboot_stage_def_hash = ();

while (defined(my $line = c_getline_pre_process $hdr, $fhdr)) {
	$line =~ /^\s*#\s*define\s+(MULTI_BOOT_STAGE_\w+)\s+(.*)$/ or next;
	push @multiboot_stage_def_list, $1;
	$multiboot_stage_def_hash{$1} = $2;
}

close $fhdr or die "$0:`$hdr':$!";


# ==== processing multiboot stage labels in requests ====
my @multiboot_stage_req_list;

foreach my $arg (@ARGV) {
	unless (substr($arg, 0, 1) eq '@') {
		push @multiboot_stage_req_list, $arg;
	} else {
		my $f = substr $arg, 1;
		my $ff;
		if ($f eq '-') {
			!defined $hasstdin or die "$0:args multiple stdin";
			$hasstdin = 1;
			open $ff, "<-" or die "$0:`$f':$!";
		} else {
			open $ff, "<", $f or die "$0:`$f':$!";
		}
		while (defined(my $line = c_getline_pre_process $f, $ff)) {
			$line =~ /^\s*#\s*if/ or next;
			$line =~ /\b(MULTI_BOOT_STAGE_\w+)\b/ or next;
			push @multiboot_stage_req_list, $1;
		}
		close $ff or die "$0:`$f':$!";
	}
}


# ==== generating results of multiboot stage labels or values on demands ====
my %seen = ();
my $reslist = @multiboot_stage_req_list ?
		\@multiboot_stage_req_list : \@multiboot_stage_def_list;

foreach my $label (@$reslist) {
	defined(my $val = $multiboot_stage_def_hash{$label})
		or die "$0:`$label':invalid stage label";
	if ($opt_sort) {
		$seen{$val} = undef;
	} elsif ($opt_list) {
		print $label, "\n";
	} else {
		print $val, "\n";
	}
}

if ($opt_sort) {
	my @stage_uniq_sort_val_list = sort { $a <=> $b } keys %seen;
	foreach my $stage (@stage_uniq_sort_val_list) {
		print $stage, "\n";
	}
}
