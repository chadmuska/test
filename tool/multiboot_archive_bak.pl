#!/usr/bin/perl
#/**************************************************************************
# *
# *        Copyright (c) 2010-2016 by iCatch Technology Inc.
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
# *  Author: Anmin Deng
# *
# **************************************************************************/

# \brief extracting obj members written to ${bakpath}
# \param ${1} is the full path of original archive, in format like
# "${libpath}/${libname}", without ".stage*" even if supporting multi stage;
# ${libpath} must be relative path. absolute path not supported;
# \note ${1} must be in relative path. absolute path is not supported;
# \note ${bakpath} == `dirname ${1}`/_multiboot.bak/`basename ${1%.a}`;
# \note when archive extract success, there is a timestamp checking file
# "${bakpath}/_timestamp.bak" created for future buidling dependence checking.
# \note the same named objects from different paths in the archive is supported
# and are extracted to paths ${bakpath}/, ${bakpath}/2/, ${bakpath}/3/, etc.
# [LIMITATION] it is not allowed if any of the same named objects is staged

use strict;
use warnings;
use integer;

@ARGV == 2 or die "usage:${0} `archive' `prefix'\n";


# lib with no ".stage*" by tailing ".a", even if stage archive
my ($lib, $prefix) = @ARGV;
my $ar = ${prefix} . 'ar';

-f "${lib}" and -r "${lib}" and "${lib}" !~ /\.stage\d+\.a$/
	or die "${0}:`${lib}':archive can not open";


# \param $_[0] the archive file name to extract
# \note [LIMITATION] it is not allowed if objects with the same name
# from different paths in this archive *AND* any of these objects are staged
sub
archive_extract($)
{
	my ($l) = @_;

	unlink 'err.log';
	defined(my $members = `${ar} t ${l} 2> err.log`)
	and -z 'err.log'
		or die "$0:ar:`${l}':archive test failed";
	unlink 'err.log';
	my %mhash;
	my @mlist = split ' ', $members;
	if ("${l}" =~ /\.stage\d+\.a$/) {
	# not supporting multiple the same named staged objects in one archive
		my %mnohash;
		my %mstagehash;
		grep { /^(.*)\.stage\d+\.o$/ ?
			(!exists $mnohash{"$1.o"} && !$mstagehash{"$1.o"}++
				|| (die "$0:`${l}':`$_':stageobj same named"),
				++$mhash{$_}) :
			(!exists $mstagehash{$_}
				|| (die "$0:`${l}':`$_':stageobj same named"),
				++$mnohash{$_}, undef)
		} @mlist;
	} else {
		map { ++$mhash{$_} } @mlist;
	}
	undef @mlist;
	%mhash or return;

	# `ar' has no error code for `ar x archive obj'
	# if obj is not an entry in archive, but just print error messages
	# to standard error
	my $marg = join ' ', keys %mhash;
	defined(my $extracted = `$ar xvN 1 $l $marg 2> err.log`)
	and -z 'err.log'
		or (unlink keys %mhash),
		die "$0:ar:`$l':extracting failed";
	unlink 'err.log';
	my @exlist = split /\n/, $extracted;
	my %exhash;
	my $diff;
	map {
		substr $_, 0, 4, '';
		$exhash{$_} = undef;
		++$diff, print STDERR 'x - ', $_, "\n" if !exists $mhash{$_};
	} @exlist;
	map {
		++$diff, print STDERR 't - ', $_, "\n" if !exists $exhash{$_};
	} keys %mhash;
	$diff and
		die "$0:ar:`${l}':archive test/extract diff";

	while (my ($m, $n) = each %mhash) {
		$n > 1 or next;
		for my $i ( 2 .. $n ) {
			my $ex;
			(-d $i or mkdir $i) and
			(chdir $i) and
			defined($ex = `$ar xvN $i ../$l $m 2> err.log`)
			and -z 'err.log'
			and $ex eq "x - $m\n"
				or (unlink $m),
				die "$0:ar:`${l}':`$m':extract `N $i' failed";
			unlink 'err.log';
			chdir '..';
		}
	}

	1;
}

# \note we back up the objs in archives to "${bakpath}/${obj%noext}[.stage*].o"
# for non-system archives;
# where "${bakpath}" == "`dirname ${1}`/_multiboot.bak/`basename ${1%.a}`"
# the archive ${1} is always in the path "${bakpath}/../../";

my ($libdir, $libname, $lib0) = $lib =~ m#^(?:(.*)/)?(([^/]*?)\.a)$#;
(defined $libname) and (defined $lib0)
	or die "${0}:`$lib':bad lib path name";
$libdir = '.' if !defined $libdir; # empty string means root
my $bakpath = "$libdir/_multiboot.bak/$lib0";

-e "${bakpath}/_timestamp.bak" and
(stat "${bakpath}/_timestamp.bak")[9] >= (stat "${lib}")[9] # mtime
	and (print STDERR "${0}:`${bakpath}':up to date\n"), exit 0;

use File::Path qw(
	mkpath
	rmtree
);

(! -e $bakpath or rmtree ${bakpath}) and
mkpath ${bakpath} and chdir ${bakpath}
	or die "${0}:`${bakpath}':cd path failed";

-x "${ar}"
	or die "${0}:`${ar}':system path failed\n";

print STDERR "backup `${lib}' to `${bakpath}'\n";

# normal archive backup
my $extlib = "../../${libname}";

my $ok = archive_extract ${extlib};

if (!$ok) {
	print STDERR "${0}:`${extlib}':empty\n";
} else {

	# stage archives backup
	substr($extlib, -2) = '';  # removing tailing '.a'

	foreach my $l (glob "${extlib}.stage*.a") {
		archive_extract ${l};
	}

}

if (-e '_timestamp.bak') {
	utime undef, undef, '_timestamp.bak';
} else {
	open my $fh, '>', '_timestamp.bak'
		or die "${0}:`_timestamp.bak':$!";
	close $fh;
}
