#!/usr/bin/perl
#line 2 "/usr/local/bin/par.pl"

eval 'exec /usr/bin/perl  -S $0 ${1+"$@"}'
    if 0; # not running under some shell
eval 'exec /usr/bin/perl  -S $0 ${1+"$@"}'
    if 0; # not running under some shell

package __par_pl;

# --- This script must not use any modules at compile time ---
# use strict;

#line 161

my ($par_temp, $progname, @tmpfile);
END { if ($ENV{PAR_CLEAN}) {
    require File::Temp;
    require File::Basename;
    require File::Spec;
    my $topdir = File::Basename::dirname($par_temp);
    outs(qq{Removing files in "$par_temp"});
    File::Find::finddepth(sub { ( -d ) ? rmdir : unlink }, $par_temp);
    rmdir $par_temp;
    # Don't remove topdir because this causes a race with other apps
    # that are trying to start.

    if (-d $par_temp && $^O ne 'MSWin32') {
        # Something went wrong unlinking the temporary directory.  This
        # typically happens on platforms that disallow unlinking shared
        # libraries and executables that are in use. Unlink with a background
        # shell command so the files are no longer in use by this process.
        # Don't do anything on Windows because our parent process will
        # take care of cleaning things up.

        my $tmp = new File::Temp(
            TEMPLATE => 'tmpXXXXX',
            DIR => File::Basename::dirname($topdir),
            SUFFIX => '.cmd',
            UNLINK => 0,
        );

        print $tmp "#!/bin/sh
x=1; while [ \$x -lt 10 ]; do
   rm -rf '$par_temp'
   if [ \! -d '$par_temp' ]; then
       break
   fi
   sleep 1
   x=`expr \$x + 1`
done
rm '" . $tmp->filename . "'
";
            chmod 0700,$tmp->filename;
        my $cmd = $tmp->filename . ' >/dev/null 2>&1 &';
        close $tmp;
        system($cmd);
        outs(qq(Spawned background process to perform cleanup: )
             . $tmp->filename);
    }
} }

BEGIN {
    Internals::PAR::BOOT() if defined &Internals::PAR::BOOT;

    eval {

_par_init_env();

if (exists $ENV{PAR_ARGV_0} and $ENV{PAR_ARGV_0} ) {
    @ARGV = map $ENV{"PAR_ARGV_$_"}, (1 .. $ENV{PAR_ARGC} - 1);
    $0 = $ENV{PAR_ARGV_0};
}
else {
    for (keys %ENV) {
        delete $ENV{$_} if /^PAR_ARGV_/;
    }
}

my $quiet = !$ENV{PAR_DEBUG};

# fix $progname if invoked from PATH
my %Config = (
    path_sep    => ($^O =~ /^MSWin/ ? ';' : ':'),
    _exe        => ($^O =~ /^(?:MSWin|OS2|cygwin)/ ? '.exe' : ''),
    _delim      => ($^O =~ /^MSWin|OS2/ ? '\\' : '/'),
);

_set_progname();
_set_par_temp();

# Magic string checking and extracting bundled modules {{{
my ($start_pos, $data_pos);
{
    local $SIG{__WARN__} = sub {};

    # Check file type, get start of data section {{{
    open _FH, '<', $progname or last;
    binmode(_FH);

    my $buf;
    seek _FH, -8, 2;
    read _FH, $buf, 8;
    last unless $buf eq "\nPAR.pm\n";

    seek _FH, -12, 2;
    read _FH, $buf, 4;
    seek _FH, -12 - unpack("N", $buf), 2;
    read _FH, $buf, 4;

    $data_pos = (tell _FH) - 4;
    # }}}

    # Extracting each file into memory {{{
    my %require_list;
    while ($buf eq "FILE") {
        read _FH, $buf, 4;
        read _FH, $buf, unpack("N", $buf);

        my $fullname = $buf;
        outs(qq(Unpacking file "$fullname"...));
        my $crc = ( $fullname =~ s|^([a-f\d]{8})/|| ) ? $1 : undef;
        my ($basename, $ext) = ($buf =~ m|(?:.*/)?(.*)(\..*)|);

        read _FH, $buf, 4;
        read _FH, $buf, unpack("N", $buf);

        if (defined($ext) and $ext !~ /\.(?:pm|pl|ix|al)$/i) {
            my ($out, $filename) = _tempfile($ext, $crc);
            if ($out) {
                binmode($out);
                print $out $buf;
                close $out;
                chmod 0755, $filename;
            }
            $PAR::Heavy::FullCache{$fullname} = $filename;
            $PAR::Heavy::FullCache{$filename} = $fullname;
        }
        elsif ( $fullname =~ m|^/?shlib/| and defined $ENV{PAR_TEMP} ) {
            # should be moved to _tempfile()
            my $filename = "$ENV{PAR_TEMP}/$basename$ext";
            outs("SHLIB: $filename\n");
            open my $out, '>', $filename or die $!;
            binmode($out);
            print $out $buf;
            close $out;
        }
        else {
            $require_list{$fullname} =
            $PAR::Heavy::ModuleCache{$fullname} = {
                buf => $buf,
                crc => $crc,
                name => $fullname,
            };
        }
        read _FH, $buf, 4;
    }
    # }}}

    local @INC = (sub {
        my ($self, $module) = @_;

        return if ref $module or !$module;

        my $filename = delete $require_list{$module} || do {
            my $key;
            foreach (keys %require_list) {
                next unless /\Q$module\E$/;
                $key = $_; last;
            }
            delete $require_list{$key} if defined($key);
        } or return;

        $INC{$module} = "/loader/$filename/$module";

        if ($ENV{PAR_CLEAN} and defined(&IO::File::new)) {
            my $fh = IO::File->new_tmpfile or die $!;
            binmode($fh);
            print $fh $filename->{buf};
            seek($fh, 0, 0);
            return $fh;
        }
        else {
            my ($out, $name) = _tempfile('.pm', $filename->{crc});
            if ($out) {
                binmode($out);
                print $out $filename->{buf};
                close $out;
            }
            open my $fh, '<', $name or die $!;
            binmode($fh);
            return $fh;
        }

        die "Bootstrapping failed: cannot find $module!\n";
    }, @INC);

    # Now load all bundled files {{{

    # initialize shared object processing
    require XSLoader;
    require PAR::Heavy;
    require Carp::Heavy;
    require Exporter::Heavy;
    PAR::Heavy::_init_dynaloader();

    # now let's try getting helper modules from within
    require IO::File;

    # load rest of the group in
    while (my $filename = (sort keys %require_list)[0]) {
        #local $INC{'Cwd.pm'} = __FILE__ if $^O ne 'MSWin32';
        unless ($INC{$filename} or $filename =~ /BSDPAN/) {
            # require modules, do other executable files
            if ($filename =~ /\.pmc?$/i) {
                require $filename;
            }
            else {
                # Skip ActiveState's sitecustomize.pl file:
                do $filename unless $filename =~ /sitecustomize\.pl$/;
            }
        }
        delete $require_list{$filename};
    }

    # }}}

    last unless $buf eq "PK\003\004";
    $start_pos = (tell _FH) - 4;
}
# }}}

# Argument processing {{{
my @par_args;
my ($out, $bundle, $logfh, $cache_name);

delete $ENV{PAR_APP_REUSE}; # sanitize (REUSE may be a security problem)

$quiet = 0 unless $ENV{PAR_DEBUG};
# Don't swallow arguments for compiled executables without --par-options
if (!$start_pos or ($ARGV[0] eq '--par-options' && shift)) {
    my %dist_cmd = qw(
        p   blib_to_par
        i   install_par
        u   uninstall_par
        s   sign_par
        v   verify_par
    );

    # if the app is invoked as "appname --par-options --reuse PROGRAM @PROG_ARGV",
    # use the app to run the given perl code instead of anything from the
    # app itself (but still set up the normal app environment and @INC)
    if (@ARGV and $ARGV[0] eq '--reuse') {
        shift @ARGV;
        $ENV{PAR_APP_REUSE} = shift @ARGV;
    }
    else { # normal parl behaviour

        my @add_to_inc;
        while (@ARGV) {
            $ARGV[0] =~ /^-([AIMOBLbqpiusTv])(.*)/ or last;

            if ($1 eq 'I') {
                push @add_to_inc, $2;
            }
            elsif ($1 eq 'M') {
                eval "use $2";
            }
            elsif ($1 eq 'A') {
                unshift @par_args, $2;
            }
            elsif ($1 eq 'O') {
                $out = $2;
            }
            elsif ($1 eq 'b') {
                $bundle = 'site';
            }
            elsif ($1 eq 'B') {
                $bundle = 'all';
            }
            elsif ($1 eq 'q') {
                $quiet = 1;
            }
            elsif ($1 eq 'L') {
                open $logfh, ">>", $2 or die "XXX: Cannot open log: $!";
            }
            elsif ($1 eq 'T') {
                $cache_name = $2;
            }

            shift(@ARGV);

            if (my $cmd = $dist_cmd{$1}) {
                delete $ENV{'PAR_TEMP'};
                init_inc();
                require PAR::Dist;
                &{"PAR::Dist::$cmd"}() unless @ARGV;
                &{"PAR::Dist::$cmd"}($_) for @ARGV;
                exit;
            }
        }

        unshift @INC, @add_to_inc;
    }
}

# XXX -- add --par-debug support!

# }}}

# Output mode (-O) handling {{{
if ($out) {
    {
        #local $INC{'Cwd.pm'} = __FILE__ if $^O ne 'MSWin32';
        require IO::File;
        require Archive::Zip;
    }

    my $par = shift(@ARGV);
    my $zip;


    if (defined $par) {
        open my $fh, '<', $par or die "Cannot find '$par': $!";
        binmode($fh);
        bless($fh, 'IO::File');

        $zip = Archive::Zip->new;
        ( $zip->readFromFileHandle($fh, $par) == Archive::Zip::AZ_OK() )
            or die "Read '$par' error: $!";
    }


    my %env = do {
        if ($zip and my $meta = $zip->contents('META.yml')) {
            $meta =~ s/.*^par:$//ms;
            $meta =~ s/^\S.*//ms;
            $meta =~ /^  ([^:]+): (.+)$/mg;
        }
    };

    # Open input and output files {{{
    local $/ = \4;

    if (defined $par) {
        open PAR, '<', $par or die "$!: $par";
        binmode(PAR);
        die "$par is not a PAR file" unless <PAR> eq "PK\003\004";
    }

    CreatePath($out) ;
    
    my $fh = IO::File->new(
        $out,
        IO::File::O_CREAT() | IO::File::O_WRONLY() | IO::File::O_TRUNC(),
        0777,
    ) or die $!;
    binmode($fh);

    $/ = (defined $data_pos) ? \$data_pos : undef;
    seek _FH, 0, 0;
    my $loader = scalar <_FH>;
    if (!$ENV{PAR_VERBATIM} and $loader =~ /^(?:#!|\@rem)/) {
        require PAR::Filter::PodStrip;
        PAR::Filter::PodStrip->new->apply(\$loader, $0)
    }
    foreach my $key (sort keys %env) {
        my $val = $env{$key} or next;
        $val = eval $val if $val =~ /^['"]/;
        my $magic = "__ENV_PAR_" . uc($key) . "__";
        my $set = "PAR_" . uc($key) . "=$val";
        $loader =~ s{$magic( +)}{
            $magic . $set . (' ' x (length($1) - length($set)))
        }eg;
    }
    $fh->print($loader);
    $/ = undef;
    # }}}

    # Write bundled modules {{{
    if ($bundle) {
        require PAR::Heavy;
        PAR::Heavy::_init_dynaloader();
        init_inc();

        require_modules();

        my @inc = sort {
            length($b) <=> length($a)
        } grep {
            !/BSDPAN/
        } grep {
            ($bundle ne 'site') or
            ($_ ne $Config::Config{archlibexp} and
             $_ ne $Config::Config{privlibexp});
        } @INC;

        # File exists test added to fix RT #41790:
        # Funny, non-existing entry in _<....auto/Compress/Raw/Zlib/autosplit.ix.
        # This is a band-aid fix with no deeper grasp of the issue.
        # Somebody please go through the pain of understanding what's happening,
        # I failed. -- Steffen
        my %files;
        /^_<(.+)$/ and -e $1 and $files{$1}++ for keys %::;
        $files{$_}++ for values %INC;

        my $lib_ext = $Config::Config{lib_ext};
        my %written;

        foreach (sort keys %files) {
            my ($name, $file);

            foreach my $dir (@inc) {
                if ($name = $PAR::Heavy::FullCache{$_}) {
                    $file = $_;
                    last;
                }
                elsif (/^(\Q$dir\E\/(.*[^Cc]))\Z/i) {
                    ($file, $name) = ($1, $2);
                    last;
                }
                elsif (m!^/loader/[^/]+/(.*[^Cc])\Z!) {
                    if (my $ref = $PAR::Heavy::ModuleCache{$1}) {
                        ($file, $name) = ($ref, $1);
                        last;
                    }
                    elsif (-f "$dir/$1") {
                        ($file, $name) = ("$dir/$1", $1);
                        last;
                    }
                }
            }

            next unless defined $name and not $written{$name}++;
            next if !ref($file) and $file =~ /\.\Q$lib_ext\E$/;
            outs( join "",
                qq(Packing "), ref $file ? $file->{name} : $file,
                qq("...)
            );

            my $content;
            if (ref($file)) {
                $content = $file->{buf};
            }
            else {
                open FILE, '<', $file or die "Can't open $file: $!";
                binmode(FILE);
                $content = <FILE>;
                close FILE;

                PAR::Filter::PodStrip->new->apply(\$content, $file)
                    if !$ENV{PAR_VERBATIM} and $name =~ /\.(?:pm|ix|al)$/i;

                PAR::Filter::PatchContent->new->apply(\$content, $file, $name);
            }

            outs(qq(Written as "$name"));
            $fh->print("FILE");
            $fh->print(pack('N', length($name) + 9));
            $fh->print(sprintf(
                "%08x/%s", Archive::Zip::computeCRC32($content), $name
            ));
            $fh->print(pack('N', length($content)));
            $fh->print($content);
        }
    }
    # }}}

    # Now write out the PAR and magic strings {{{
    $zip->writeToFileHandle($fh) if $zip;

    $cache_name = substr $cache_name, 0, 40;
    if (!$cache_name and my $mtime = (stat($out))[9]) {
        my $ctx = eval { require Digest::SHA; Digest::SHA->new(1) }
            || eval { require Digest::SHA1; Digest::SHA1->new }
            || eval { require Digest::MD5; Digest::MD5->new };

        # Workaround for bug in Digest::SHA 5.38 and 5.39
        my $sha_version = eval { $Digest::SHA::VERSION } || 0;
        if ($sha_version eq '5.38' or $sha_version eq '5.39') {
            $ctx->addfile($out, "b") if ($ctx);
        }
        else {
            if ($ctx and open(my $fh, "<$out")) {
                binmode($fh);
                $ctx->addfile($fh);
                close($fh);
            }
        }

        $cache_name = $ctx ? $ctx->hexdigest : $mtime;
    }
    $cache_name .= "\0" x (41 - length $cache_name);
    $cache_name .= "CACHE";
    $fh->print($cache_name);
    $fh->print(pack('N', $fh->tell - length($loader)));
    $fh->print("\nPAR.pm\n");
    $fh->close;
    chmod 0755, $out;
    # }}}

    exit;
}
# }}}

# Prepare $progname into PAR file cache {{{
{
    last unless defined $start_pos;

    _fix_progname();

    # Now load the PAR file and put it into PAR::LibCache {{{
    require PAR;
    PAR::Heavy::_init_dynaloader();


    {
        #local $INC{'Cwd.pm'} = __FILE__ if $^O ne 'MSWin32';
        require File::Find;
        require Archive::Zip;
    }
    my $zip = Archive::Zip->new;
    my $fh = IO::File->new;
    $fh->fdopen(fileno(_FH), 'r') or die "$!: $@";
    $zip->readFromFileHandle($fh, $progname) == Archive::Zip::AZ_OK() or die "$!: $@";

    push @PAR::LibCache, $zip;
    $PAR::LibCache{$progname} = $zip;

    $quiet = !$ENV{PAR_DEBUG};
    outs(qq(\$ENV{PAR_TEMP} = "$ENV{PAR_TEMP}"));

    if (defined $ENV{PAR_TEMP}) { # should be set at this point!
        foreach my $member ( $zip->members ) {
            next if $member->isDirectory;
            my $member_name = $member->fileName;
            next unless $member_name =~ m{
                ^
                /?shlib/
                (?:$Config::Config{version}/)?
                (?:$Config::Config{archname}/)?
                ([^/]+)
                $
            }x;
            my $extract_name = $1;
            my $dest_name = File::Spec->catfile($ENV{PAR_TEMP}, $extract_name);
            if (-f $dest_name && -s _ == $member->uncompressedSize()) {
                outs(qq(Skipping "$member_name" since it already exists at "$dest_name"));
            } else {
                outs(qq(Extracting "$member_name" to "$dest_name"));
                $member->extractToFileNamed($dest_name);
                chmod(0555, $dest_name) if $^O eq "hpux";
            }
        }
    }
    # }}}
}
# }}}

# If there's no main.pl to run, show usage {{{
unless ($PAR::LibCache{$progname}) {
    die << "." unless @ARGV;
Usage: $0 [ -Alib.par ] [ -Idir ] [ -Mmodule ] [ src.par ] [ program.pl ]
       $0 [ -B|-b ] [-Ooutfile] src.par
.
    $ENV{PAR_PROGNAME} = $progname = $0 = shift(@ARGV);
}
# }}}

sub CreatePath {
    my ($name) = @_;
    
    require File::Basename;
    my ($basename, $path, $ext) = File::Basename::fileparse($name, ('\..*'));
    
    require File::Path;
    
    File::Path::mkpath($path) unless(-e $path); # mkpath dies with error
}

sub require_modules {
    #local $INC{'Cwd.pm'} = __FILE__ if $^O ne 'MSWin32';

    require lib;
    require DynaLoader;
    require integer;
    require strict;
    require warnings;
    require vars;
    require Carp;
    require Carp::Heavy;
    require Errno;
    require Exporter::Heavy;
    require Exporter;
    require Fcntl;
    require File::Temp;
    require File::Spec;
    require XSLoader;
    require Config;
    require IO::Handle;
    require IO::File;
    require Compress::Zlib;
    require Archive::Zip;
    require PAR;
    require PAR::Heavy;
    require PAR::Dist;
    require PAR::Filter::PodStrip;
    require PAR::Filter::PatchContent;
    require attributes;
    eval { require Cwd };
    eval { require Win32 };
    eval { require Scalar::Util };
    eval { require Archive::Unzip::Burst };
    eval { require Tie::Hash::NamedCapture };
    eval { require PerlIO; require PerlIO::scalar };
}

# The C version of this code appears in myldr/mktmpdir.c
# This code also lives in PAR::SetupTemp as set_par_temp_env!
sub _set_par_temp {
    if (defined $ENV{PAR_TEMP} and $ENV{PAR_TEMP} =~ /(.+)/) {
        $par_temp = $1;
        return;
    }

    foreach my $path (
        (map $ENV{$_}, qw( PAR_TMPDIR TMPDIR TEMPDIR TEMP TMP )),
        qw( C:\\TEMP /tmp . )
    ) {
        next unless defined $path and -d $path and -w $path;
        my $username;
        my $pwuid;
        # does not work everywhere:
        eval {($pwuid) = getpwuid($>) if defined $>;};

        if ( defined(&Win32::LoginName) ) {
            $username = &Win32::LoginName;
        }
        elsif (defined $pwuid) {
            $username = $pwuid;
        }
        else {
            $username = $ENV{USERNAME} || $ENV{USER} || 'SYSTEM';
        }
        $username =~ s/\W/_/g;

        my $stmpdir = "$path$Config{_delim}par-".unpack("H*", $username);
        mkdir $stmpdir, 0755;
        if (!$ENV{PAR_CLEAN} and my $mtime = (stat($progname))[9]) {
            open (my $fh, "<". $progname);
            seek $fh, -18, 2;
            sysread $fh, my $buf, 6;
            if ($buf eq "\0CACHE") {
                seek $fh, -58, 2;
                sysread $fh, $buf, 41;
                $buf =~ s/\0//g;
                $stmpdir .= "$Config{_delim}cache-" . $buf;
            }
            else {
                my $ctx = eval { require Digest::SHA; Digest::SHA->new(1) }
                    || eval { require Digest::SHA1; Digest::SHA1->new }
                    || eval { require Digest::MD5; Digest::MD5->new };

                # Workaround for bug in Digest::SHA 5.38 and 5.39
                my $sha_version = eval { $Digest::SHA::VERSION } || 0;
                if ($sha_version eq '5.38' or $sha_version eq '5.39') {
                    $ctx->addfile($progname, "b") if ($ctx);
                }
                else {
                    if ($ctx and open(my $fh, "<$progname")) {
                        binmode($fh);
                        $ctx->addfile($fh);
                        close($fh);
                    }
                }

                $stmpdir .= "$Config{_delim}cache-" . ( $ctx ? $ctx->hexdigest : $mtime );
            }
            close($fh);
        }
        else {
            $ENV{PAR_CLEAN} = 1;
            $stmpdir .= "$Config{_delim}temp-$$";
        }

        $ENV{PAR_TEMP} = $stmpdir;
        mkdir $stmpdir, 0755;
        last;
    }

    $par_temp = $1 if $ENV{PAR_TEMP} and $ENV{PAR_TEMP} =~ /(.+)/;
}

sub _tempfile {
    my ($ext, $crc) = @_;
    my ($fh, $filename);

    $filename = "$par_temp/$crc$ext";

    if ($ENV{PAR_CLEAN}) {
        unlink $filename if -e $filename;
        push @tmpfile, $filename;
    }
    else {
        return (undef, $filename) if (-r $filename);
    }

    open $fh, '>', $filename or die $!;
    binmode($fh);
    return($fh, $filename);
}

# same code lives in PAR::SetupProgname::set_progname
sub _set_progname {
    if (defined $ENV{PAR_PROGNAME} and $ENV{PAR_PROGNAME} =~ /(.+)/) {
        $progname = $1;
    }

    $progname ||= $0;

    if ($ENV{PAR_TEMP} and index($progname, $ENV{PAR_TEMP}) >= 0) {
        $progname = substr($progname, rindex($progname, $Config{_delim}) + 1);
    }

    if (!$ENV{PAR_PROGNAME} or index($progname, $Config{_delim}) >= 0) {
        if (open my $fh, '<', $progname) {
            return if -s $fh;
        }
        if (-s "$progname$Config{_exe}") {
            $progname .= $Config{_exe};
            return;
        }
    }

    foreach my $dir (split /\Q$Config{path_sep}\E/, $ENV{PATH}) {
        next if exists $ENV{PAR_TEMP} and $dir eq $ENV{PAR_TEMP};
        $dir =~ s/\Q$Config{_delim}\E$//;
        (($progname = "$dir$Config{_delim}$progname$Config{_exe}"), last)
            if -s "$dir$Config{_delim}$progname$Config{_exe}";
        (($progname = "$dir$Config{_delim}$progname"), last)
            if -s "$dir$Config{_delim}$progname";
    }
}

sub _fix_progname {
    $0 = $progname ||= $ENV{PAR_PROGNAME};
    if (index($progname, $Config{_delim}) < 0) {
        $progname = ".$Config{_delim}$progname";
    }

    # XXX - hack to make PWD work
    my $pwd = (defined &Cwd::getcwd) ? Cwd::getcwd()
                : ((defined &Win32::GetCwd) ? Win32::GetCwd() : `pwd`);
    chomp($pwd);
    $progname =~ s/^(?=\.\.?\Q$Config{_delim}\E)/$pwd$Config{_delim}/;

    $ENV{PAR_PROGNAME} = $progname;
}

sub _par_init_env {
    if ( $ENV{PAR_INITIALIZED}++ == 1 ) {
        return;
    } else {
        $ENV{PAR_INITIALIZED} = 2;
    }

    for (qw( SPAWNED TEMP CLEAN DEBUG CACHE PROGNAME ARGC ARGV_0 ) ) {
        delete $ENV{'PAR_'.$_};
    }
    for (qw/ TMPDIR TEMP CLEAN DEBUG /) {
        $ENV{'PAR_'.$_} = $ENV{'PAR_GLOBAL_'.$_} if exists $ENV{'PAR_GLOBAL_'.$_};
    }

    my $par_clean = "__ENV_PAR_CLEAN__               ";

    if ($ENV{PAR_TEMP}) {
        delete $ENV{PAR_CLEAN};
    }
    elsif (!exists $ENV{PAR_GLOBAL_CLEAN}) {
        my $value = substr($par_clean, 12 + length("CLEAN"));
        $ENV{PAR_CLEAN} = $1 if $value =~ /^PAR_CLEAN=(\S+)/;
    }
}

sub outs {
    return if $quiet;
    if ($logfh) {
        print $logfh "@_\n";
    }
    else {
        print "@_\n";
    }
}

sub init_inc {
    require Config;
    push @INC, grep defined, map $Config::Config{$_}, qw(
        archlibexp privlibexp sitearchexp sitelibexp
        vendorarchexp vendorlibexp
    );
}

########################################################################
# The main package for script execution

package main;

require PAR;
unshift @INC, \&PAR::find_par;
PAR->import(@par_args);

die qq(par.pl: Can't open perl script "$progname": No such file or directory\n)
    unless -e $progname;

do $progname;
CORE::exit($1) if ($@ =~/^_TK_EXIT_\((\d+)\)/);
die $@ if $@;

};

$::__ERROR = $@ if $@;
}

CORE::exit($1) if ($::__ERROR =~/^_TK_EXIT_\((\d+)\)/);
die $::__ERROR if $::__ERROR;

1;

#line 1014

__END__
FILE   5f3711a6/AutoLoader.pm  e#line 1 "/usr/lib/perl5/5.14/AutoLoader.pm"
package AutoLoader;

use strict;
use 5.006_001;

our($VERSION, $AUTOLOAD);

my $is_dosish;
my $is_epoc;
my $is_vms;
my $is_macos;

BEGIN {
    $is_dosish = $^O eq 'cygwin' || $^O eq 'dos' || $^O eq 'os2' || $^O eq 'MSWin32' || $^O eq 'NetWare';
    $is_epoc = $^O eq 'epoc';
    $is_vms = $^O eq 'VMS';
    $is_macos = $^O eq 'MacOS';
    $VERSION = '5.71';
}

AUTOLOAD {
    my $sub = $AUTOLOAD;
    my $filename = AutoLoader::find_filename( $sub );

    my $save = $@;
    local $!; # Do not munge the value. 
    eval { local $SIG{__DIE__}; require $filename };
    if ($@) {
	if (substr($sub,-9) eq '::DESTROY') {
	    no strict 'refs';
	    *$sub = sub {};
	    $@ = undef;
	} elsif ($@ =~ /^Can't locate/) {
	    # The load might just have failed because the filename was too
	    # long for some old SVR3 systems which treat long names as errors.
	    # If we can successfully truncate a long name then it's worth a go.
	    # There is a slight risk that we could pick up the wrong file here
	    # but autosplit should have warned about that when splitting.
	    if ($filename =~ s/(\w{12,})\.al$/substr($1,0,11).".al"/e){
		eval { local $SIG{__DIE__}; require $filename };
	    }
	}
	if ($@){
	    $@ =~ s/ at .*\n//;
	    my $error = $@;
	    require Carp;
	    Carp::croak($error);
	}
    }
    $@ = $save;
    goto &$sub;
}

sub find_filename {
    my $sub = shift;
    my $filename;
    # Braces used to preserve $1 et al.
    {
	# Try to find the autoloaded file from the package-qualified
	# name of the sub. e.g., if the sub needed is
	# Getopt::Long::GetOptions(), then $INC{Getopt/Long.pm} is
	# something like '/usr/lib/perl5/Getopt/Long.pm', and the
	# autoload file is '/usr/lib/perl5/auto/Getopt/Long/GetOptions.al'.
	#
	# However, if @INC is a relative path, this might not work.  If,
	# for example, @INC = ('lib'), then $INC{Getopt/Long.pm} is
	# 'lib/Getopt/Long.pm', and we want to require
	# 'auto/Getopt/Long/GetOptions.al' (without the leading 'lib').
	# In this case, we simple prepend the 'auto/' and let the
	# C<require> take care of the searching for us.

	my ($pkg,$func) = ($sub =~ /(.*)::([^:]+)$/);
	$pkg =~ s#::#/#g;
	if (defined($filename = $INC{"$pkg.pm"})) {
	    if ($is_macos) {
		$pkg =~ tr#/#:#;
		$filename = undef
		  unless $filename =~ s#^(.*)$pkg\.pm\z#$1auto:$pkg:$func.al#s;
	    } else {
		$filename = undef
		  unless $filename =~ s#^(.*)$pkg\.pm\z#$1auto/$pkg/$func.al#s;
	    }

	    # if the file exists, then make sure that it is a
	    # a fully anchored path (i.e either '/usr/lib/auto/foo/bar.al',
	    # or './lib/auto/foo/bar.al'.  This avoids C<require> searching
	    # (and failing) to find the 'lib/auto/foo/bar.al' because it
	    # looked for 'lib/lib/auto/foo/bar.al', given @INC = ('lib').

	    if (defined $filename and -r $filename) {
		unless ($filename =~ m|^/|s) {
		    if ($is_dosish) {
			unless ($filename =~ m{^([a-z]:)?[\\/]}is) {
			    if ($^O ne 'NetWare') {
				$filename = "./$filename";
			    } else {
				$filename = "$filename";
			    }
			}
		    }
		    elsif ($is_epoc) {
			unless ($filename =~ m{^([a-z?]:)?[\\/]}is) {
			     $filename = "./$filename";
			}
		    }
		    elsif ($is_vms) {
			# XXX todo by VMSmiths
			$filename = "./$filename";
		    }
		    elsif (!$is_macos) {
			$filename = "./$filename";
		    }
		}
	    }
	    else {
		$filename = undef;
	    }
	}
	unless (defined $filename) {
	    # let C<require> do the searching
	    $filename = "auto/$sub.al";
	    $filename =~ s#::#/#g;
	}
    }
    return $filename;
}

sub import {
    my $pkg = shift;
    my $callpkg = caller;

    #
    # Export symbols, but not by accident of inheritance.
    #

    if ($pkg eq 'AutoLoader') {
	if ( @_ and $_[0] =~ /^&?AUTOLOAD$/ ) {
	    no strict 'refs';
	    *{ $callpkg . '::AUTOLOAD' } = \&AUTOLOAD;
	}
    }

    #
    # Try to find the autosplit index file.  Eg., if the call package
    # is POSIX, then $INC{POSIX.pm} is something like
    # '/usr/local/lib/perl5/POSIX.pm', and the autosplit index file is in
    # '/usr/local/lib/perl5/auto/POSIX/autosplit.ix', so we require that.
    #
    # However, if @INC is a relative path, this might not work.  If,
    # for example, @INC = ('lib'), then
    # $INC{POSIX.pm} is 'lib/POSIX.pm', and we want to require
    # 'auto/POSIX/autosplit.ix' (without the leading 'lib').
    #

    (my $calldir = $callpkg) =~ s#::#/#g;
    my $path = $INC{$calldir . '.pm'};
    if (defined($path)) {
	# Try absolute path name, but only eval it if the
        # transformation from module path to autosplit.ix path
        # succeeded!
	my $replaced_okay;
	if ($is_macos) {
	    (my $malldir = $calldir) =~ tr#/#:#;
	    $replaced_okay = ($path =~ s#^(.*)$malldir\.pm\z#$1auto:$malldir:autosplit.ix#s);
	} else {
	    $replaced_okay = ($path =~ s#^(.*)$calldir\.pm\z#$1auto/$calldir/autosplit.ix#);
	}

	eval { require $path; } if $replaced_okay;
	# If that failed, try relative path with normal @INC searching.
	if (!$replaced_okay or $@) {
	    $path ="auto/$calldir/autosplit.ix";
	    eval { require $path; };
	}
	if ($@) {
	    my $error = $@;
	    require Carp;
	    Carp::carp($error);
	}
    } 
}

sub unimport {
    my $callpkg = caller;

    no strict 'refs';

    for my $exported (qw( AUTOLOAD )) {
	my $symname = $callpkg . '::' . $exported;
	undef *{ $symname } if \&{ $symname } == \&{ $exported };
	*{ $symname } = \&{ $symname };
    }
}

1;

__END__

#line 430
FILE   b0419cec/Carp.pm  '�#line 1 "/usr/lib/perl5/5.14/Carp.pm"
package Carp;

use strict;
use warnings;

our $VERSION = '1.20';

our $MaxEvalLen = 0;
our $Verbose    = 0;
our $CarpLevel  = 0;
our $MaxArgLen  = 64;    # How much of each argument to print. 0 = all.
our $MaxArgNums = 8;     # How many arguments to print. 0 = all.

require Exporter;
our @ISA       = ('Exporter');
our @EXPORT    = qw(confess croak carp);
our @EXPORT_OK = qw(cluck verbose longmess shortmess);
our @EXPORT_FAIL = qw(verbose);    # hook to enable verbose mode

# The members of %Internal are packages that are internal to perl.
# Carp will not report errors from within these packages if it
# can.  The members of %CarpInternal are internal to Perl's warning
# system.  Carp will not report errors from within these packages
# either, and will not report calls *to* these packages for carp and
# croak.  They replace $CarpLevel, which is deprecated.    The
# $Max(EvalLen|(Arg(Len|Nums)) variables are used to specify how the eval
# text and function arguments should be formatted when printed.

our %CarpInternal;
our %Internal;

# disable these by default, so they can live w/o require Carp
$CarpInternal{Carp}++;
$CarpInternal{warnings}++;
$Internal{Exporter}++;
$Internal{'Exporter::Heavy'}++;

# if the caller specifies verbose usage ("perl -MCarp=verbose script.pl")
# then the following method will be called by the Exporter which knows
# to do this thanks to @EXPORT_FAIL, above.  $_[1] will contain the word
# 'verbose'.

sub export_fail { shift; $Verbose = shift if $_[0] eq 'verbose'; @_ }

sub _cgc {
    no strict 'refs';
    return \&{"CORE::GLOBAL::caller"} if defined &{"CORE::GLOBAL::caller"};
    return;
}

sub longmess {
    # Icky backwards compatibility wrapper. :-(
    #
    # The story is that the original implementation hard-coded the
    # number of call levels to go back, so calls to longmess were off
    # by one.  Other code began calling longmess and expecting this
    # behaviour, so the replacement has to emulate that behaviour.
    my $cgc = _cgc();
    my $call_pack = $cgc ? $cgc->() : caller();
    if ( $Internal{$call_pack} or $CarpInternal{$call_pack} ) {
        return longmess_heavy(@_);
    }
    else {
        local $CarpLevel = $CarpLevel + 1;
        return longmess_heavy(@_);
    }
}

our @CARP_NOT;

sub shortmess {
    my $cgc = _cgc();

    # Icky backwards compatibility wrapper. :-(
    local @CARP_NOT = $cgc ? $cgc->() : caller();
    shortmess_heavy(@_);
}

sub croak   { die shortmess @_ }
sub confess { die longmess @_ }
sub carp    { warn shortmess @_ }
sub cluck   { warn longmess @_ }

sub caller_info {
    my $i = shift(@_) + 1;
    my %call_info;
    my $cgc = _cgc();
    {
        package DB;
        @DB::args = \$i;    # A sentinel, which no-one else has the address of
        @call_info{
            qw(pack file line sub has_args wantarray evaltext is_require) }
            = $cgc ? $cgc->($i) : caller($i);
    }

    unless ( defined $call_info{pack} ) {
        return ();
    }

    my $sub_name = Carp::get_subname( \%call_info );
    if ( $call_info{has_args} ) {
        my @args;
        if (   @DB::args == 1
            && ref $DB::args[0] eq ref \$i
            && $DB::args[0] == \$i ) {
            @DB::args = ();    # Don't let anyone see the address of $i
            local $@;
            my $where = eval {
                my $func    = $cgc or return '';
                my $gv      = B::svref_2object($func)->GV;
                my $package = $gv->STASH->NAME;
                my $subname = $gv->NAME;
                return unless defined $package && defined $subname;

                # returning CORE::GLOBAL::caller isn't useful for tracing the cause:
                return if $package eq 'CORE::GLOBAL' && $subname eq 'caller';
                " in &${package}::$subname";
            } // '';
            @args
                = "** Incomplete caller override detected$where; \@DB::args were not set **";
        }
        else {
            @args = map { Carp::format_arg($_) } @DB::args;
        }
        if ( $MaxArgNums and @args > $MaxArgNums )
        {    # More than we want to show?
            $#args = $MaxArgNums;
            push @args, '...';
        }

        # Push the args onto the subroutine
        $sub_name .= '(' . join( ', ', @args ) . ')';
    }
    $call_info{sub_name} = $sub_name;
    return wantarray() ? %call_info : \%call_info;
}

# Transform an argument to a function into a string.
sub format_arg {
    my $arg = shift;
    if ( ref($arg) ) {
        $arg = defined($overload::VERSION) ? overload::StrVal($arg) : "$arg";
    }
    if ( defined($arg) ) {
        $arg =~ s/'/\\'/g;
        $arg = str_len_trim( $arg, $MaxArgLen );

        # Quote it?
        $arg = "'$arg'" unless $arg =~ /^-?[0-9.]+\z/;
    }                                    # 0-9, not \d, as \d will try to
    else {                               # load Unicode tables
        $arg = 'undef';
    }

    # The following handling of "control chars" is direct from
    # the original code - it is broken on Unicode though.
    # Suggestions?
    utf8::is_utf8($arg)
        or $arg =~ s/([[:cntrl:]]|[[:^ascii:]])/sprintf("\\x{%x}",ord($1))/eg;
    return $arg;
}

# Takes an inheritance cache and a package and returns
# an anon hash of known inheritances and anon array of
# inheritances which consequences have not been figured
# for.
sub get_status {
    my $cache = shift;
    my $pkg   = shift;
    $cache->{$pkg} ||= [ { $pkg => $pkg }, [ trusts_directly($pkg) ] ];
    return @{ $cache->{$pkg} };
}

# Takes the info from caller() and figures out the name of
# the sub/require/eval
sub get_subname {
    my $info = shift;
    if ( defined( $info->{evaltext} ) ) {
        my $eval = $info->{evaltext};
        if ( $info->{is_require} ) {
            return "require $eval";
        }
        else {
            $eval =~ s/([\\\'])/\\$1/g;
            return "eval '" . str_len_trim( $eval, $MaxEvalLen ) . "'";
        }
    }

    return ( $info->{sub} eq '(eval)' ) ? 'eval {...}' : $info->{sub};
}

# Figures out what call (from the point of view of the caller)
# the long error backtrace should start at.
sub long_error_loc {
    my $i;
    my $lvl = $CarpLevel;
    {
        ++$i;
        my $cgc = _cgc();
        my $pkg = $cgc ? $cgc->($i) : caller($i);
        unless ( defined($pkg) ) {

            # This *shouldn't* happen.
            if (%Internal) {
                local %Internal;
                $i = long_error_loc();
                last;
            }
            else {

                # OK, now I am irritated.
                return 2;
            }
        }
        redo if $CarpInternal{$pkg};
        redo unless 0 > --$lvl;
        redo if $Internal{$pkg};
    }
    return $i - 1;
}

sub longmess_heavy {
    return @_ if ref( $_[0] );    # don't break references as exceptions
    my $i = long_error_loc();
    return ret_backtrace( $i, @_ );
}

# Returns a full stack backtrace starting from where it is
# told.
sub ret_backtrace {
    my ( $i, @error ) = @_;
    my $mess;
    my $err = join '', @error;
    $i++;

    my $tid_msg = '';
    if ( defined &threads::tid ) {
        my $tid = threads->tid;
        $tid_msg = " thread $tid" if $tid;
    }

    my %i = caller_info($i);
    $mess = "$err at $i{file} line $i{line}$tid_msg\n";

    while ( my %i = caller_info( ++$i ) ) {
        $mess .= "\t$i{sub_name} called at $i{file} line $i{line}$tid_msg\n";
    }

    return $mess;
}

sub ret_summary {
    my ( $i, @error ) = @_;
    my $err = join '', @error;
    $i++;

    my $tid_msg = '';
    if ( defined &threads::tid ) {
        my $tid = threads->tid;
        $tid_msg = " thread $tid" if $tid;
    }

    my %i = caller_info($i);
    return "$err at $i{file} line $i{line}$tid_msg\n";
}

sub short_error_loc {
    # You have to create your (hash)ref out here, rather than defaulting it
    # inside trusts *on a lexical*, as you want it to persist across calls.
    # (You can default it on $_[2], but that gets messy)
    my $cache = {};
    my $i     = 1;
    my $lvl   = $CarpLevel;
    {
        my $cgc = _cgc();
        my $called = $cgc ? $cgc->($i) : caller($i);
        $i++;
        my $caller = $cgc ? $cgc->($i) : caller($i);

        return 0 unless defined($caller);    # What happened?
        redo if $Internal{$caller};
        redo if $CarpInternal{$caller};
        redo if $CarpInternal{$called};
        redo if trusts( $called, $caller, $cache );
        redo if trusts( $caller, $called, $cache );
        redo unless 0 > --$lvl;
    }
    return $i - 1;
}

sub shortmess_heavy {
    return longmess_heavy(@_) if $Verbose;
    return @_ if ref( $_[0] );    # don't break references as exceptions
    my $i = short_error_loc();
    if ($i) {
        ret_summary( $i, @_ );
    }
    else {
        longmess_heavy(@_);
    }
}

# If a string is too long, trims it with ...
sub str_len_trim {
    my $str = shift;
    my $max = shift || 0;
    if ( 2 < $max and $max < length($str) ) {
        substr( $str, $max - 3 ) = '...';
    }
    return $str;
}

# Takes two packages and an optional cache.  Says whether the
# first inherits from the second.
#
# Recursive versions of this have to work to avoid certain
# possible endless loops, and when following long chains of
# inheritance are less efficient.
sub trusts {
    my $child  = shift;
    my $parent = shift;
    my $cache  = shift;
    my ( $known, $partial ) = get_status( $cache, $child );

    # Figure out consequences until we have an answer
    while ( @$partial and not exists $known->{$parent} ) {
        my $anc = shift @$partial;
        next if exists $known->{$anc};
        $known->{$anc}++;
        my ( $anc_knows, $anc_partial ) = get_status( $cache, $anc );
        my @found = keys %$anc_knows;
        @$known{@found} = ();
        push @$partial, @$anc_partial;
    }
    return exists $known->{$parent};
}

# Takes a package and gives a list of those trusted directly
sub trusts_directly {
    my $class = shift;
    no strict 'refs';
    no warnings 'once';
    return @{"$class\::CARP_NOT"}
        ? @{"$class\::CARP_NOT"}
        : @{"$class\::ISA"};
}

1;

__END__

#line 579
FILE   c1416a3d/Carp/Heavy.pm  M#line 1 "/usr/lib/perl5/5.14/Carp/Heavy.pm"
package Carp;

# On one line so MakeMaker will see it.
use Carp;  our $VERSION = $Carp::VERSION;

1;

# Most of the machinery of Carp used to be there.
# It has been moved in Carp.pm now, but this placeholder remains for
# the benefit of modules that like to preload Carp::Heavy directly.
FILE   1ff24f2a/Compress/Zlib.pm  8�#line 1 "/usr/lib/perl5/5.14/Compress/Zlib.pm"

package Compress::Zlib;

require 5.004 ;
require Exporter;
use Carp ;
use IO::Handle ;
use Scalar::Util qw(dualvar);

use IO::Compress::Base::Common 2.033 ;
use Compress::Raw::Zlib 2.033 ;
use IO::Compress::Gzip 2.033 ;
use IO::Uncompress::Gunzip 2.033 ;

use strict ;
use warnings ;
use bytes ;
our ($VERSION, $XS_VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

$VERSION = '2.033';
$XS_VERSION = $VERSION; 
$VERSION = eval $VERSION;

@ISA = qw(Exporter);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
        deflateInit inflateInit

        compress uncompress

        gzopen $gzerrno
    );

push @EXPORT, @Compress::Raw::Zlib::EXPORT ;

@EXPORT_OK = qw(memGunzip memGzip zlib_version);
%EXPORT_TAGS = (
    ALL         => \@EXPORT
);

BEGIN
{
    *zlib_version = \&Compress::Raw::Zlib::zlib_version;
}

use constant FLAG_APPEND             => 1 ;
use constant FLAG_CRC                => 2 ;
use constant FLAG_ADLER              => 4 ;
use constant FLAG_CONSUME_INPUT      => 8 ;

our (@my_z_errmsg);

@my_z_errmsg = (
    "need dictionary",     # Z_NEED_DICT     2
    "stream end",          # Z_STREAM_END    1
    "",                    # Z_OK            0
    "file error",          # Z_ERRNO        (-1)
    "stream error",        # Z_STREAM_ERROR (-2)
    "data error",          # Z_DATA_ERROR   (-3)
    "insufficient memory", # Z_MEM_ERROR    (-4)
    "buffer error",        # Z_BUF_ERROR    (-5)
    "incompatible version",# Z_VERSION_ERROR(-6)
    );


sub _set_gzerr
{
    my $value = shift ;

    if ($value == 0) {
        $Compress::Zlib::gzerrno = 0 ;
    }
    elsif ($value == Z_ERRNO() || $value > 2) {
        $Compress::Zlib::gzerrno = $! ;
    }
    else {
        $Compress::Zlib::gzerrno = dualvar($value+0, $my_z_errmsg[2 - $value]);
    }

    return $value ;
}

sub _set_gzerr_undef
{
    _set_gzerr(@_);
    return undef;
}
sub _save_gzerr
{
    my $gz = shift ;
    my $test_eof = shift ;

    my $value = $gz->errorNo() || 0 ;

    if ($test_eof) {
        #my $gz = $self->[0] ;
        # gzread uses Z_STREAM_END to denote a successful end
        $value = Z_STREAM_END() if $gz->eof() && $value == 0 ;
    }

    _set_gzerr($value) ;
}

sub gzopen($$)
{
    my ($file, $mode) = @_ ;

    my $gz ;
    my %defOpts = (Level    => Z_DEFAULT_COMPRESSION(),
                   Strategy => Z_DEFAULT_STRATEGY(),
                  );

    my $writing ;
    $writing = ! ($mode =~ /r/i) ;
    $writing = ($mode =~ /[wa]/i) ;

    $defOpts{Level}    = $1               if $mode =~ /(\d)/;
    $defOpts{Strategy} = Z_FILTERED()     if $mode =~ /f/i;
    $defOpts{Strategy} = Z_HUFFMAN_ONLY() if $mode =~ /h/i;
    $defOpts{Append}   = 1                if $mode =~ /a/i;

    my $infDef = $writing ? 'deflate' : 'inflate';
    my @params = () ;

    croak "gzopen: file parameter is not a filehandle or filename"
        unless isaFilehandle $file || isaFilename $file  || 
               (ref $file && ref $file eq 'SCALAR');

    return undef unless $mode =~ /[rwa]/i ;

    _set_gzerr(0) ;

    if ($writing) {
        $gz = new IO::Compress::Gzip($file, Minimal => 1, AutoClose => 1, 
                                     %defOpts) 
            or $Compress::Zlib::gzerrno = $IO::Compress::Gzip::GzipError;
    }
    else {
        $gz = new IO::Uncompress::Gunzip($file, 
                                         Transparent => 1,
                                         Append => 0, 
                                         AutoClose => 1, 
                                         MultiStream => 1,
                                         Strict => 0) 
            or $Compress::Zlib::gzerrno = $IO::Uncompress::Gunzip::GunzipError;
    }

    return undef
        if ! defined $gz ;

    bless [$gz, $infDef], 'Compress::Zlib::gzFile';
}

sub Compress::Zlib::gzFile::gzread
{
    my $self = shift ;

    return _set_gzerr(Z_STREAM_ERROR())
        if $self->[1] ne 'inflate';

    my $len = defined $_[1] ? $_[1] : 4096 ; 

    if ($self->gzeof() || $len == 0) {
        # Zap the output buffer to match ver 1 behaviour.
        $_[0] = "" ;
        return 0 ;
    }

    my $gz = $self->[0] ;
    my $status = $gz->read($_[0], $len) ; 
    _save_gzerr($gz, 1);
    return $status ;
}

sub Compress::Zlib::gzFile::gzreadline
{
    my $self = shift ;

    my $gz = $self->[0] ;
    {
        # Maintain backward compatibility with 1.x behaviour
        # It didn't support $/, so this can't either.
        local $/ = "\n" ;
        $_[0] = $gz->getline() ; 
    }
    _save_gzerr($gz, 1);
    return defined $_[0] ? length $_[0] : 0 ;
}

sub Compress::Zlib::gzFile::gzwrite
{
    my $self = shift ;
    my $gz = $self->[0] ;

    return _set_gzerr(Z_STREAM_ERROR())
        if $self->[1] ne 'deflate';

    $] >= 5.008 and (utf8::downgrade($_[0], 1) 
        or croak "Wide character in gzwrite");

    my $status = $gz->write($_[0]) ;
    _save_gzerr($gz);
    return $status ;
}

sub Compress::Zlib::gzFile::gztell
{
    my $self = shift ;
    my $gz = $self->[0] ;
    my $status = $gz->tell() ;
    _save_gzerr($gz);
    return $status ;
}

sub Compress::Zlib::gzFile::gzseek
{
    my $self   = shift ;
    my $offset = shift ;
    my $whence = shift ;

    my $gz = $self->[0] ;
    my $status ;
    eval { $status = $gz->seek($offset, $whence) ; };
    if ($@)
    {
        my $error = $@;
        $error =~ s/^.*: /gzseek: /;
        $error =~ s/ at .* line \d+\s*$//;
        croak $error;
    }
    _save_gzerr($gz);
    return $status ;
}

sub Compress::Zlib::gzFile::gzflush
{
    my $self = shift ;
    my $f    = shift ;

    my $gz = $self->[0] ;
    my $status = $gz->flush($f) ;
    my $err = _save_gzerr($gz);
    return $status ? 0 : $err;
}

sub Compress::Zlib::gzFile::gzclose
{
    my $self = shift ;
    my $gz = $self->[0] ;

    my $status = $gz->close() ;
    my $err = _save_gzerr($gz);
    return $status ? 0 : $err;
}

sub Compress::Zlib::gzFile::gzeof
{
    my $self = shift ;
    my $gz = $self->[0] ;

    return 0
        if $self->[1] ne 'inflate';

    my $status = $gz->eof() ;
    _save_gzerr($gz);
    return $status ;
}

sub Compress::Zlib::gzFile::gzsetparams
{
    my $self = shift ;
    croak "Usage: Compress::Zlib::gzFile::gzsetparams(file, level, strategy)"
        unless @_ eq 2 ;

    my $gz = $self->[0] ;
    my $level = shift ;
    my $strategy = shift;

    return _set_gzerr(Z_STREAM_ERROR())
        if $self->[1] ne 'deflate';
 
    my $status = *$gz->{Compress}->deflateParams(-Level   => $level, 
                                                -Strategy => $strategy);
    _save_gzerr($gz);
    return $status ;
}

sub Compress::Zlib::gzFile::gzerror
{
    my $self = shift ;
    my $gz = $self->[0] ;
    
    return $Compress::Zlib::gzerrno ;
}


sub compress($;$)
{
    my ($x, $output, $err, $in) =('', '', '', '') ;

    if (ref $_[0] ) {
        $in = $_[0] ;
        croak "not a scalar reference" unless ref $in eq 'SCALAR' ;
    }
    else {
        $in = \$_[0] ;
    }

    $] >= 5.008 and (utf8::downgrade($$in, 1) 
        or croak "Wide character in compress");

    my $level = (@_ == 2 ? $_[1] : Z_DEFAULT_COMPRESSION() );

    $x = new Compress::Raw::Zlib::Deflate -AppendOutput => 1, -Level => $level
            or return undef ;

    $err = $x->deflate($in, $output) ;
    return undef unless $err == Z_OK() ;

    $err = $x->flush($output) ;
    return undef unless $err == Z_OK() ;
    
    return $output ;

}

sub uncompress($)
{
    my ($x, $output, $err, $in) =('', '', '', '') ;

    if (ref $_[0] ) {
        $in = $_[0] ;
        croak "not a scalar reference" unless ref $in eq 'SCALAR' ;
    }
    else {
        $in = \$_[0] ;
    }

    $] >= 5.008 and (utf8::downgrade($$in, 1) 
        or croak "Wide character in uncompress");

    $x = new Compress::Raw::Zlib::Inflate -ConsumeInput => 0 or return undef ;
 
    $err = $x->inflate($in, $output) ;
    return undef unless $err == Z_STREAM_END() ;
 
    return $output ;
}


 
sub deflateInit(@)
{
    my ($got) = ParseParameters(0,
                {
                'Bufsize'       => [1, 1, Parse_unsigned, 4096],
                'Level'         => [1, 1, Parse_signed,   Z_DEFAULT_COMPRESSION()],
                'Method'        => [1, 1, Parse_unsigned, Z_DEFLATED()],
                'WindowBits'    => [1, 1, Parse_signed,   MAX_WBITS()],
                'MemLevel'      => [1, 1, Parse_unsigned, MAX_MEM_LEVEL()],
                'Strategy'      => [1, 1, Parse_unsigned, Z_DEFAULT_STRATEGY()],
                'Dictionary'    => [1, 1, Parse_any,      ""],
                }, @_ ) ;

    croak "Compress::Zlib::deflateInit: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $obj ;
 
    my $status = 0 ;
    ($obj, $status) = 
      Compress::Raw::Zlib::_deflateInit(0,
                $got->value('Level'), 
                $got->value('Method'), 
                $got->value('WindowBits'), 
                $got->value('MemLevel'), 
                $got->value('Strategy'), 
                $got->value('Bufsize'),
                $got->value('Dictionary')) ;

    my $x = ($status == Z_OK() ? bless $obj, "Zlib::OldDeflate"  : undef) ;
    return wantarray ? ($x, $status) : $x ;
}
 
sub inflateInit(@)
{
    my ($got) = ParseParameters(0,
                {
                'Bufsize'       => [1, 1, Parse_unsigned, 4096],
                'WindowBits'    => [1, 1, Parse_signed,   MAX_WBITS()],
                'Dictionary'    => [1, 1, Parse_any,      ""],
                }, @_) ;


    croak "Compress::Zlib::inflateInit: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $status = 0 ;
    my $obj ;
    ($obj, $status) = Compress::Raw::Zlib::_inflateInit(FLAG_CONSUME_INPUT,
                                $got->value('WindowBits'), 
                                $got->value('Bufsize'), 
                                $got->value('Dictionary')) ;

    my $x = ($status == Z_OK() ? bless $obj, "Zlib::OldInflate"  : undef) ;

    wantarray ? ($x, $status) : $x ;
}

package Zlib::OldDeflate ;

our (@ISA);
@ISA = qw(Compress::Raw::Zlib::deflateStream);


sub deflate
{
    my $self = shift ;
    my $output ;

    my $status = $self->SUPER::deflate($_[0], $output) ;
    wantarray ? ($output, $status) : $output ;
}

sub flush
{
    my $self = shift ;
    my $output ;
    my $flag = shift || Compress::Zlib::Z_FINISH();
    my $status = $self->SUPER::flush($output, $flag) ;
    
    wantarray ? ($output, $status) : $output ;
}

package Zlib::OldInflate ;

our (@ISA);
@ISA = qw(Compress::Raw::Zlib::inflateStream);

sub inflate
{
    my $self = shift ;
    my $output ;
    my $status = $self->SUPER::inflate($_[0], $output) ;
    wantarray ? ($output, $status) : $output ;
}

package Compress::Zlib ;

use IO::Compress::Gzip::Constants 2.033 ;

sub memGzip($)
{
  my $out;

  # if the deflation buffer isn't a reference, make it one
  my $string = (ref $_[0] ? $_[0] : \$_[0]) ;

  $] >= 5.008 and (utf8::downgrade($$string, 1) 
      or croak "Wide character in memGzip");

  _set_gzerr(0);
  if ( ! IO::Compress::Gzip::gzip($string, \$out, Minimal => 1) )
  {
      $Compress::Zlib::gzerrno = $IO::Compress::Gzip::GzipError;
      return undef ;
  }

  return $out;
}

sub _removeGzipHeader($)
{
    my $string = shift ;

    return Z_DATA_ERROR() 
        if length($$string) < GZIP_MIN_HEADER_SIZE ;

    my ($magic1, $magic2, $method, $flags, $time, $xflags, $oscode) = 
        unpack ('CCCCVCC', $$string);

    return Z_DATA_ERROR()
        unless $magic1 == GZIP_ID1 and $magic2 == GZIP_ID2 and
           $method == Z_DEFLATED() and !($flags & GZIP_FLG_RESERVED) ;
    substr($$string, 0, GZIP_MIN_HEADER_SIZE) = '' ;

    # skip extra field
    if ($flags & GZIP_FLG_FEXTRA)
    {
        return Z_DATA_ERROR()
            if length($$string) < GZIP_FEXTRA_HEADER_SIZE ;

        my ($extra_len) = unpack ('v', $$string);
        $extra_len += GZIP_FEXTRA_HEADER_SIZE;
        return Z_DATA_ERROR()
            if length($$string) < $extra_len ;

        substr($$string, 0, $extra_len) = '';
    }

    # skip orig name
    if ($flags & GZIP_FLG_FNAME)
    {
        my $name_end = index ($$string, GZIP_NULL_BYTE);
        return Z_DATA_ERROR()
           if $name_end == -1 ;
        substr($$string, 0, $name_end + 1) =  '';
    }

    # skip comment
    if ($flags & GZIP_FLG_FCOMMENT)
    {
        my $comment_end = index ($$string, GZIP_NULL_BYTE);
        return Z_DATA_ERROR()
            if $comment_end == -1 ;
        substr($$string, 0, $comment_end + 1) = '';
    }

    # skip header crc
    if ($flags & GZIP_FLG_FHCRC)
    {
        return Z_DATA_ERROR()
            if length ($$string) < GZIP_FHCRC_SIZE ;
        substr($$string, 0, GZIP_FHCRC_SIZE) = '';
    }
    
    return Z_OK();
}

sub _ret_gun_error
{
    $Compress::Zlib::gzerrno = $IO::Uncompress::Gunzip::GunzipError;
    return undef;
}


sub memGunzip($)
{
    # if the buffer isn't a reference, make it one
    my $string = (ref $_[0] ? $_[0] : \$_[0]);
 
    $] >= 5.008 and (utf8::downgrade($$string, 1) 
        or croak "Wide character in memGunzip");

    _set_gzerr(0);

    my $status = _removeGzipHeader($string) ;
    $status == Z_OK() 
        or return _set_gzerr_undef($status);
     
    my $bufsize = length $$string > 4096 ? length $$string : 4096 ;
    my $x = new Compress::Raw::Zlib::Inflate({-WindowBits => - MAX_WBITS(),
                         -Bufsize => $bufsize}) 

              or return _ret_gun_error();

    my $output = "" ;
    $status = $x->inflate($string, $output);
    
    if ( $status == Z_OK() )
    {
        _set_gzerr(Z_DATA_ERROR());
        return undef;
    }

    return _ret_gun_error()
        if ($status != Z_STREAM_END());

    if (length $$string >= 8)
    {
        my ($crc, $len) = unpack ("VV", substr($$string, 0, 8));
        substr($$string, 0, 8) = '';
        return _set_gzerr_undef(Z_DATA_ERROR())
            unless $len == length($output) and
                   $crc == crc32($output);
    }
    else
    {
        $$string = '';
    }

    return $output;   
}

# Autoload methods go after __END__, and are processed by the autosplit program.

1;
__END__


#line 1484
FILE   f786ecc3/Digest/base.pm  �#line 1 "/usr/lib/perl5/5.14/Digest/base.pm"
package Digest::base;

use strict;
use vars qw($VERSION);
$VERSION = "1.16";

# subclass is supposed to implement at least these
sub new;
sub clone;
sub add;
sub digest;

sub reset {
    my $self = shift;
    $self->new(@_);  # ugly
}

sub addfile {
    my ($self, $handle) = @_;

    my $n;
    my $buf = "";

    while (($n = read($handle, $buf, 4*1024))) {
        $self->add($buf);
    }
    unless (defined $n) {
	require Carp;
	Carp::croak("Read failed: $!");
    }

    $self;
}

sub add_bits {
    my $self = shift;
    my $bits;
    my $nbits;
    if (@_ == 1) {
	my $arg = shift;
	$bits = pack("B*", $arg);
	$nbits = length($arg);
    }
    else {
	($bits, $nbits) = @_;
    }
    if (($nbits % 8) != 0) {
	require Carp;
	Carp::croak("Number of bits must be multiple of 8 for this algorithm");
    }
    return $self->add(substr($bits, 0, $nbits/8));
}

sub hexdigest {
    my $self = shift;
    return unpack("H*", $self->digest(@_));
}

sub b64digest {
    my $self = shift;
    require MIME::Base64;
    my $b64 = MIME::Base64::encode($self->digest(@_), "");
    $b64 =~ s/=+$//;
    return $b64;
}

1;

__END__

#line 101FILE   ee423060/Exporter.pm  	�#line 1 "/usr/lib/perl5/5.14/Exporter.pm"
package Exporter;

require 5.006;

# Be lean.
#use strict;
#no strict 'refs';

our $Debug = 0;
our $ExportLevel = 0;
our $Verbose ||= 0;
our $VERSION = '5.64_03';
our (%Cache);

sub as_heavy {
  require Exporter::Heavy;
  # Unfortunately, this does not work if the caller is aliased as *name = \&foo
  # Thus the need to create a lot of identical subroutines
  my $c = (caller(1))[3];
  $c =~ s/.*:://;
  \&{"Exporter::Heavy::heavy_$c"};
}

sub export {
  goto &{as_heavy()};
}

sub import {
  my $pkg = shift;
  my $callpkg = caller($ExportLevel);

  if ($pkg eq "Exporter" and @_ and $_[0] eq "import") {
    *{$callpkg."::import"} = \&import;
    return;
  }

  # We *need* to treat @{"$pkg\::EXPORT_FAIL"} since Carp uses it :-(
  my $exports = \@{"$pkg\::EXPORT"};
  # But, avoid creating things if they don't exist, which saves a couple of
  # hundred bytes per package processed.
  my $fail = ${$pkg . '::'}{EXPORT_FAIL} && \@{"$pkg\::EXPORT_FAIL"};
  return export $pkg, $callpkg, @_
    if $Verbose or $Debug or $fail && @$fail > 1;
  my $export_cache = ($Cache{$pkg} ||= {});
  my $args = @_ or @_ = @$exports;

  local $_;
  if ($args and not %$export_cache) {
    s/^&//, $export_cache->{$_} = 1
      foreach (@$exports, @{"$pkg\::EXPORT_OK"});
  }
  my $heavy;
  # Try very hard not to use {} and hence have to  enter scope on the foreach
  # We bomb out of the loop with last as soon as heavy is set.
  if ($args or $fail) {
    ($heavy = (/\W/ or $args and not exists $export_cache->{$_}
               or $fail and @$fail and $_ eq $fail->[0])) and last
                 foreach (@_);
  } else {
    ($heavy = /\W/) and last
      foreach (@_);
  }
  return export $pkg, $callpkg, ($args ? @_ : ()) if $heavy;
  local $SIG{__WARN__} = 
	sub {require Carp; &Carp::carp} if not $SIG{__WARN__};
  # shortcut for the common case of no type character
  *{"$callpkg\::$_"} = \&{"$pkg\::$_"} foreach @_;
}

# Default methods

sub export_fail {
    my $self = shift;
    @_;
}

# Unfortunately, caller(1)[3] "does not work" if the caller is aliased as
# *name = \&foo.  Thus the need to create a lot of identical subroutines
# Otherwise we could have aliased them to export().

sub export_to_level {
  goto &{as_heavy()};
}

sub export_tags {
  goto &{as_heavy()};
}

sub export_ok_tags {
  goto &{as_heavy()};
}

sub require_version {
  goto &{as_heavy()};
}

1;
__END__

#line 585



FILE   7281097a/Exporter/Heavy.pm  �#line 1 "/usr/lib/perl5/5.14/Exporter/Heavy.pm"
package Exporter::Heavy;

use strict;
no strict 'refs';

# On one line so MakeMaker will see it.
require Exporter;  our $VERSION = $Exporter::VERSION;

#line 22

#
# We go to a lot of trouble not to 'require Carp' at file scope,
#  because Carp requires Exporter, and something has to give.
#

sub _rebuild_cache {
    my ($pkg, $exports, $cache) = @_;
    s/^&// foreach @$exports;
    @{$cache}{@$exports} = (1) x @$exports;
    my $ok = \@{"${pkg}::EXPORT_OK"};
    if (@$ok) {
	s/^&// foreach @$ok;
	@{$cache}{@$ok} = (1) x @$ok;
    }
}

sub heavy_export {

    # First make import warnings look like they're coming from the "use".
    local $SIG{__WARN__} = sub {
	my $text = shift;
	if ($text =~ s/ at \S*Exporter\S*.pm line \d+.*\n//) {
	    require Carp;
	    local $Carp::CarpLevel = 1;	# ignore package calling us too.
	    Carp::carp($text);
	}
	else {
	    warn $text;
	}
    };
    local $SIG{__DIE__} = sub {
	require Carp;
	local $Carp::CarpLevel = 1;	# ignore package calling us too.
	Carp::croak("$_[0]Illegal null symbol in \@${1}::EXPORT")
	    if $_[0] =~ /^Unable to create sub named "(.*?)::"/;
    };

    my($pkg, $callpkg, @imports) = @_;
    my($type, $sym, $cache_is_current, $oops);
    my($exports, $export_cache) = (\@{"${pkg}::EXPORT"},
                                   $Exporter::Cache{$pkg} ||= {});

    if (@imports) {
	if (!%$export_cache) {
	    _rebuild_cache ($pkg, $exports, $export_cache);
	    $cache_is_current = 1;
	}

	if (grep m{^[/!:]}, @imports) {
	    my $tagsref = \%{"${pkg}::EXPORT_TAGS"};
	    my $tagdata;
	    my %imports;
	    my($remove, $spec, @names, @allexports);
	    # negated first item implies starting with default set:
	    unshift @imports, ':DEFAULT' if $imports[0] =~ m/^!/;
	    foreach $spec (@imports){
		$remove = $spec =~ s/^!//;

		if ($spec =~ s/^://){
		    if ($spec eq 'DEFAULT'){
			@names = @$exports;
		    }
		    elsif ($tagdata = $tagsref->{$spec}) {
			@names = @$tagdata;
		    }
		    else {
			warn qq["$spec" is not defined in %${pkg}::EXPORT_TAGS];
			++$oops;
			next;
		    }
		}
		elsif ($spec =~ m:^/(.*)/$:){
		    my $patn = $1;
		    @allexports = keys %$export_cache unless @allexports; # only do keys once
		    @names = grep(/$patn/, @allexports); # not anchored by default
		}
		else {
		    @names = ($spec); # is a normal symbol name
		}

		warn "Import ".($remove ? "del":"add").": @names "
		    if $Exporter::Verbose;

		if ($remove) {
		   foreach $sym (@names) { delete $imports{$sym} } 
		}
		else {
		    @imports{@names} = (1) x @names;
		}
	    }
	    @imports = keys %imports;
	}

        my @carp;
	foreach $sym (@imports) {
	    if (!$export_cache->{$sym}) {
		if ($sym =~ m/^\d/) {
		    $pkg->VERSION($sym); # inherit from UNIVERSAL
		    # If the version number was the only thing specified
		    # then we should act as if nothing was specified:
		    if (@imports == 1) {
			@imports = @$exports;
			last;
		    }
		    # We need a way to emulate 'use Foo ()' but still
		    # allow an easy version check: "use Foo 1.23, ''";
		    if (@imports == 2 and !$imports[1]) {
			@imports = ();
			last;
		    }
		} elsif ($sym !~ s/^&// || !$export_cache->{$sym}) {
		    # Last chance - see if they've updated EXPORT_OK since we
		    # cached it.

		    unless ($cache_is_current) {
			%$export_cache = ();
			_rebuild_cache ($pkg, $exports, $export_cache);
			$cache_is_current = 1;
		    }

		    if (!$export_cache->{$sym}) {
			# accumulate the non-exports
			push @carp,
			  qq["$sym" is not exported by the $pkg module\n];
			$oops++;
		    }
		}
	    }
	}
	if ($oops) {
	    require Carp;
	    Carp::croak("@{carp}Can't continue after import errors");
	}
    }
    else {
	@imports = @$exports;
    }

    my($fail, $fail_cache) = (\@{"${pkg}::EXPORT_FAIL"},
                              $Exporter::FailCache{$pkg} ||= {});

    if (@$fail) {
	if (!%$fail_cache) {
	    # Build cache of symbols. Optimise the lookup by adding
	    # barewords twice... both with and without a leading &.
	    # (Technique could be applied to $export_cache at cost of memory)
	    my @expanded = map { /^\w/ ? ($_, '&'.$_) : $_ } @$fail;
	    warn "${pkg}::EXPORT_FAIL cached: @expanded" if $Exporter::Verbose;
	    @{$fail_cache}{@expanded} = (1) x @expanded;
	}
	my @failed;
	foreach $sym (@imports) { push(@failed, $sym) if $fail_cache->{$sym} }
	if (@failed) {
	    @failed = $pkg->export_fail(@failed);
	    foreach $sym (@failed) {
                require Carp;
		Carp::carp(qq["$sym" is not implemented by the $pkg module ],
			"on this architecture");
	    }
	    if (@failed) {
		require Carp;
		Carp::croak("Can't continue after import errors");
	    }
	}
    }

    warn "Importing into $callpkg from $pkg: ",
		join(", ",sort @imports) if $Exporter::Verbose;

    foreach $sym (@imports) {
	# shortcut for the common case of no type character
	(*{"${callpkg}::$sym"} = \&{"${pkg}::$sym"}, next)
	    unless $sym =~ s/^(\W)//;
	$type = $1;
	no warnings 'once';
	*{"${callpkg}::$sym"} =
	    $type eq '&' ? \&{"${pkg}::$sym"} :
	    $type eq '$' ? \${"${pkg}::$sym"} :
	    $type eq '@' ? \@{"${pkg}::$sym"} :
	    $type eq '%' ? \%{"${pkg}::$sym"} :
	    $type eq '*' ?  *{"${pkg}::$sym"} :
	    do { require Carp; Carp::croak("Can't export symbol: $type$sym") };
    }
}

sub heavy_export_to_level
{
      my $pkg = shift;
      my $level = shift;
      (undef) = shift;			# XXX redundant arg
      my $callpkg = caller($level);
      $pkg->export($callpkg, @_);
}

# Utility functions

sub _push_tags {
    my($pkg, $var, $syms) = @_;
    my @nontag = ();
    my $export_tags = \%{"${pkg}::EXPORT_TAGS"};
    push(@{"${pkg}::$var"},
	map { $export_tags->{$_} ? @{$export_tags->{$_}} 
                                 : scalar(push(@nontag,$_),$_) }
		(@$syms) ? @$syms : keys %$export_tags);
    if (@nontag and $^W) {
	# This may change to a die one day
	require Carp;
	Carp::carp(join(", ", @nontag)." are not tags of $pkg");
    }
}

sub heavy_require_version {
    my($self, $wanted) = @_;
    my $pkg = ref $self || $self;
    return ${pkg}->VERSION($wanted);
}

sub heavy_export_tags {
  _push_tags((caller)[0], "EXPORT",    \@_);
}

sub heavy_export_ok_tags {
  _push_tags((caller)[0], "EXPORT_OK", \@_);
}

1;
FILE   a7511f5b/File/Basename.pm  �#line 1 "/usr/lib/perl5/5.14/File/Basename.pm"

#line 36


package File::Basename;

# File::Basename is used during the Perl build, when the re extension may
# not be available, but we only actually need it if running under tainting.
BEGIN {
  if (${^TAINT}) {
    require re;
    re->import('taint');
  }
}


use strict;
use 5.006;
use warnings;
our(@ISA, @EXPORT, $VERSION, $Fileparse_fstype, $Fileparse_igncase);
require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(fileparse fileparse_set_fstype basename dirname);
$VERSION = "2.82";

fileparse_set_fstype($^O);


#line 102


sub fileparse {
  my($fullname,@suffices) = @_;

  unless (defined $fullname) {
      require Carp;
      Carp::croak("fileparse(): need a valid pathname");
  }

  my $orig_type = '';
  my($type,$igncase) = ($Fileparse_fstype, $Fileparse_igncase);

  my($taint) = substr($fullname,0,0);  # Is $fullname tainted?

  if ($type eq "VMS" and $fullname =~ m{/} ) {
    # We're doing Unix emulation
    $orig_type = $type;
    $type = 'Unix';
  }

  my($dirpath, $basename);

  if (grep { $type eq $_ } qw(MSDOS DOS MSWin32 Epoc)) {
    ($dirpath,$basename) = ($fullname =~ /^((?:.*[:\\\/])?)(.*)/s);
    $dirpath .= '.\\' unless $dirpath =~ /[\\\/]\z/;
  }
  elsif ($type eq "OS2") {
    ($dirpath,$basename) = ($fullname =~ m#^((?:.*[:\\/])?)(.*)#s);
    $dirpath = './' unless $dirpath;	# Can't be 0
    $dirpath .= '/' unless $dirpath =~ m#[\\/]\z#;
  }
  elsif ($type eq "MacOS") {
    ($dirpath,$basename) = ($fullname =~ /^(.*:)?(.*)/s);
    $dirpath = ':' unless $dirpath;
  }
  elsif ($type eq "AmigaOS") {
    ($dirpath,$basename) = ($fullname =~ /(.*[:\/])?(.*)/s);
    $dirpath = './' unless $dirpath;
  }
  elsif ($type eq 'VMS' ) {
    ($dirpath,$basename) = ($fullname =~ /^(.*[:>\]])?(.*)/s);
    $dirpath ||= '';  # should always be defined
  }
  else { # Default to Unix semantics.
    ($dirpath,$basename) = ($fullname =~ m{^(.*/)?(.*)}s);
    if ($orig_type eq 'VMS' and $fullname =~ m{^(/[^/]+/000000(/|$))(.*)}) {
      # dev:[000000] is top of VMS tree, similar to Unix '/'
      # so strip it off and treat the rest as "normal"
      my $devspec  = $1;
      my $remainder = $3;
      ($dirpath,$basename) = ($remainder =~ m{^(.*/)?(.*)}s);
      $dirpath ||= '';  # should always be defined
      $dirpath = $devspec.$dirpath;
    }
    $dirpath = './' unless $dirpath;
  }
      

  my $tail   = '';
  my $suffix = '';
  if (@suffices) {
    foreach $suffix (@suffices) {
      my $pat = ($igncase ? '(?i)' : '') . "($suffix)\$";
      if ($basename =~ s/$pat//s) {
        $taint .= substr($suffix,0,0);
        $tail = $1 . $tail;
      }
    }
  }

  # Ensure taint is propagated from the path to its pieces.
  $tail .= $taint;
  wantarray ? ($basename .= $taint, $dirpath .= $taint, $tail)
            : ($basename .= $taint);
}



#line 212


sub basename {
  my($path) = shift;

  # From BSD basename(1)
  # The basename utility deletes any prefix ending with the last slash `/'
  # character present in string (after first stripping trailing slashes)
  _strip_trailing_sep($path);

  my($basename, $dirname, $suffix) = fileparse( $path, map("\Q$_\E",@_) );

  # From BSD basename(1)
  # The suffix is not stripped if it is identical to the remaining 
  # characters in string.
  if( length $suffix and !length $basename ) {
      $basename = $suffix;
  }
  
  # Ensure that basename '/' == '/'
  if( !length $basename ) {
      $basename = $dirname;
  }

  return $basename;
}



#line 281


sub dirname {
    my $path = shift;

    my($type) = $Fileparse_fstype;

    if( $type eq 'VMS' and $path =~ m{/} ) {
        # Parse as Unix
        local($File::Basename::Fileparse_fstype) = '';
        return dirname($path);
    }

    my($basename, $dirname) = fileparse($path);

    if ($type eq 'VMS') { 
        $dirname ||= $ENV{DEFAULT};
    }
    elsif ($type eq 'MacOS') {
	if( !length($basename) && $dirname !~ /^[^:]+:\z/) {
            _strip_trailing_sep($dirname);
	    ($basename,$dirname) = fileparse $dirname;
	}
	$dirname .= ":" unless $dirname =~ /:\z/;
    }
    elsif (grep { $type eq $_ } qw(MSDOS DOS MSWin32 OS2)) { 
        _strip_trailing_sep($dirname);
        unless( length($basename) ) {
	    ($basename,$dirname) = fileparse $dirname;
	    _strip_trailing_sep($dirname);
	}
    }
    elsif ($type eq 'AmigaOS') {
        if ( $dirname =~ /:\z/) { return $dirname }
        chop $dirname;
        $dirname =~ s{[^:/]+\z}{} unless length($basename);
    }
    else {
        _strip_trailing_sep($dirname);
        unless( length($basename) ) {
	    ($basename,$dirname) = fileparse $dirname;
	    _strip_trailing_sep($dirname);
	}
    }

    $dirname;
}


# Strip the trailing path separator.
sub _strip_trailing_sep  {
    my $type = $Fileparse_fstype;

    if ($type eq 'MacOS') {
        $_[0] =~ s/([^:]):\z/$1/s;
    }
    elsif (grep { $type eq $_ } qw(MSDOS DOS MSWin32 OS2)) { 
        $_[0] =~ s/([^:])[\\\/]*\z/$1/;
    }
    else {
        $_[0] =~ s{(.)/*\z}{$1}s;
    }
}


#line 369


BEGIN {

my @Ignore_Case = qw(MacOS VMS AmigaOS OS2 RISCOS MSWin32 MSDOS DOS Epoc);
my @Types = (@Ignore_Case, qw(Unix));

sub fileparse_set_fstype {
    my $old = $Fileparse_fstype;

    if (@_) {
        my $new_type = shift;

        $Fileparse_fstype = 'Unix';  # default
        foreach my $type (@Types) {
            $Fileparse_fstype = $type if $new_type =~ /^$type/i;
        }

        $Fileparse_igncase = 
          (grep $Fileparse_fstype eq $_, @Ignore_Case) ? 1 : 0;
    }

    return $old;
}

}


1;


#line 403FILE   1b61dce1/File/Copy.pm  /�#line 1 "/usr/lib/perl5/5.14/File/Copy.pm"
# File/Copy.pm. Written in 1994 by Aaron Sherman <ajs@ajs.com>. This
# source code has been placed in the public domain by the author.
# Please be kind and preserve the documentation.
#
# Additions copyright 1996 by Charles Bailey.  Permission is granted
# to distribute the revised code under the same terms as Perl itself.

package File::Copy;

use 5.006;
use strict;
use warnings;
use File::Spec;
use Config;
# During perl build, we need File::Copy but Scalar::Util might not be built yet
# And then we need these games to avoid loading overload, as that will
# confuse miniperl during the bootstrap of perl.
my $Scalar_Util_loaded = eval q{ require Scalar::Util; require overload; 1 };
our(@ISA, @EXPORT, @EXPORT_OK, $VERSION, $Too_Big, $Syscopy_is_copy);
sub copy;
sub syscopy;
sub cp;
sub mv;

$VERSION = '2.21';

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(copy move);
@EXPORT_OK = qw(cp mv);

$Too_Big = 1024 * 1024 * 2;

sub croak {
    require Carp;
    goto &Carp::croak;
}

sub carp {
    require Carp;
    goto &Carp::carp;
}

# Look up the feature settings on VMS using VMS::Feature when available.

my $use_vms_feature = 0;
BEGIN {
    if ($^O eq 'VMS') {
        if (eval { local $SIG{__DIE__}; require VMS::Feature; }) {
            $use_vms_feature = 1;
        }
    }
}

# Need to look up the UNIX report mode.  This may become a dynamic mode
# in the future.
sub _vms_unix_rpt {
    my $unix_rpt;
    if ($use_vms_feature) {
        $unix_rpt = VMS::Feature::current("filename_unix_report");
    } else {
        my $env_unix_rpt = $ENV{'DECC$FILENAME_UNIX_REPORT'} || '';
        $unix_rpt = $env_unix_rpt =~ /^[ET1]/i;
    }
    return $unix_rpt;
}

# Need to look up the EFS character set mode.  This may become a dynamic
# mode in the future.
sub _vms_efs {
    my $efs;
    if ($use_vms_feature) {
        $efs = VMS::Feature::current("efs_charset");
    } else {
        my $env_efs = $ENV{'DECC$EFS_CHARSET'} || '';
        $efs = $env_efs =~ /^[ET1]/i;
    }
    return $efs;
}


sub _catname {
    my($from, $to) = @_;
    if (not defined &basename) {
	require File::Basename;
	import  File::Basename 'basename';
    }

    return File::Spec->catfile($to, basename($from));
}

# _eq($from, $to) tells whether $from and $to are identical
sub _eq {
    my ($from, $to) = map {
        $Scalar_Util_loaded && Scalar::Util::blessed($_)
	    && overload::Method($_, q{""})
            ? "$_"
            : $_
    } (@_);
    return '' if ( (ref $from) xor (ref $to) );
    return $from == $to if ref $from;
    return $from eq $to;
}

sub copy {
    croak("Usage: copy(FROM, TO [, BUFFERSIZE]) ")
      unless(@_ == 2 || @_ == 3);

    my $from = shift;
    my $to = shift;

    my $size;
    if (@_) {
	$size = shift(@_) + 0;
	croak("Bad buffer size for copy: $size\n") unless ($size > 0);
    }

    my $from_a_handle = (ref($from)
			 ? (ref($from) eq 'GLOB'
			    || UNIVERSAL::isa($from, 'GLOB')
                            || UNIVERSAL::isa($from, 'IO::Handle'))
			 : (ref(\$from) eq 'GLOB'));
    my $to_a_handle =   (ref($to)
			 ? (ref($to) eq 'GLOB'
			    || UNIVERSAL::isa($to, 'GLOB')
                            || UNIVERSAL::isa($to, 'IO::Handle'))
			 : (ref(\$to) eq 'GLOB'));

    if (_eq($from, $to)) { # works for references, too
	carp("'$from' and '$to' are identical (not copied)");
        # The "copy" was a success as the source and destination contain
        # the same data.
        return 1;
    }

    if ((($Config{d_symlink} && $Config{d_readlink}) || $Config{d_link}) &&
	!($^O eq 'MSWin32' || $^O eq 'os2')) {
	my @fs = stat($from);
	if (@fs) {
	    my @ts = stat($to);
	    if (@ts && $fs[0] == $ts[0] && $fs[1] == $ts[1] && !-p $from) {
		carp("'$from' and '$to' are identical (not copied)");
                return 0;
	    }
	}
    }

    if (!$from_a_handle && !$to_a_handle && -d $to && ! -d $from) {
	$to = _catname($from, $to);
    }

    if (defined &syscopy && !$Syscopy_is_copy
	&& !$to_a_handle
	&& !($from_a_handle && $^O eq 'os2' )	# OS/2 cannot handle handles
	&& !($from_a_handle && $^O eq 'mpeix')	# and neither can MPE/iX.
	&& !($from_a_handle && $^O eq 'MSWin32')
	&& !($from_a_handle && $^O eq 'NetWare')
       )
    {
	my $copy_to = $to;

        if ($^O eq 'VMS' && -e $from) {

            if (! -d $to && ! -d $from) {

                my $vms_efs = _vms_efs();
                my $unix_rpt = _vms_unix_rpt();
                my $unix_mode = 0;
                my $from_unix = 0;
                $from_unix = 1 if ($from =~ /^\.\.?$/);
                my $from_vms = 0;
                $from_vms = 1 if ($from =~ m#[\[<\]]#);

                # Need to know if we are in Unix mode.
                if ($from_vms == $from_unix) {
                    $unix_mode = $unix_rpt;
                } else {
                    $unix_mode = $from_unix;
                }

                # VMS has sticky defaults on extensions, which means that
                # if there is a null extension on the destination file, it
                # will inherit the extension of the source file
                # So add a '.' for a null extension.

                # In unix_rpt mode, the trailing dot should not be added.

                if ($vms_efs) {
                    $copy_to = $to;
                } else {
                    $copy_to = VMS::Filespec::vmsify($to);
                }
                my ($vol, $dirs, $file) = File::Spec->splitpath($copy_to);
                $file = $file . '.'
                    unless (($file =~ /(?<!\^)\./) || $unix_rpt);
                $copy_to = File::Spec->catpath($vol, $dirs, $file);

                # Get rid of the old versions to be like UNIX
                1 while unlink $copy_to;
            }
        }

        return syscopy($from, $copy_to) || 0;
    }

    my $closefrom = 0;
    my $closeto = 0;
    my ($status, $r, $buf);
    local($\) = '';

    my $from_h;
    if ($from_a_handle) {
       $from_h = $from;
    } else {
       open $from_h, "<", $from or goto fail_open1;
       binmode $from_h or die "($!,$^E)";
       $closefrom = 1;
    }

    # Seems most logical to do this here, in case future changes would want to
    # make this croak for some reason.
    unless (defined $size) {
	$size = tied(*$from_h) ? 0 : -s $from_h || 0;
	$size = 1024 if ($size < 512);
	$size = $Too_Big if ($size > $Too_Big);
    }

    my $to_h;
    if ($to_a_handle) {
       $to_h = $to;
    } else {
	$to_h = \do { local *FH }; # XXX is this line obsolete?
	open $to_h, ">", $to or goto fail_open2;
	binmode $to_h or die "($!,$^E)";
	$closeto = 1;
    }

    $! = 0;
    for (;;) {
	my ($r, $w, $t);
       defined($r = sysread($from_h, $buf, $size))
	    or goto fail_inner;
	last unless $r;
	for ($w = 0; $w < $r; $w += $t) {
           $t = syswrite($to_h, $buf, $r - $w, $w)
		or goto fail_inner;
	}
    }

    close($to_h) || goto fail_open2 if $closeto;
    close($from_h) || goto fail_open1 if $closefrom;

    # Use this idiom to avoid uninitialized value warning.
    return 1;

    # All of these contortions try to preserve error messages...
  fail_inner:
    if ($closeto) {
	$status = $!;
	$! = 0;
       close $to_h;
	$! = $status unless $!;
    }
  fail_open2:
    if ($closefrom) {
	$status = $!;
	$! = 0;
       close $from_h;
	$! = $status unless $!;
    }
  fail_open1:
    return 0;
}

sub cp {
    my($from,$to) = @_;
    my(@fromstat) = stat $from;
    my(@tostat) = stat $to;
    my $perm;

    return 0 unless copy(@_) and @fromstat;

    if (@tostat) {
        $perm = $tostat[2];
    } else {
        $perm = $fromstat[2] & ~(umask || 0);
	@tostat = stat $to;
    }
    # Might be more robust to look for S_I* in Fcntl, but we're
    # trying to avoid dependence on any XS-containing modules,
    # since File::Copy is used during the Perl build.
    $perm &= 07777;
    if ($perm & 06000) {
	croak("Unable to check setuid/setgid permissions for $to: $!")
	    unless @tostat;

	if ($perm & 04000 and                     # setuid
	    $fromstat[4] != $tostat[4]) {         # owner must match
	    $perm &= ~06000;
	}

	if ($perm & 02000 && $> != 0) {           # if not root, setgid
	    my $ok = $fromstat[5] == $tostat[5];  # group must match
	    if ($ok) {                            # and we must be in group
                $ok = grep { $_ == $fromstat[5] } split /\s+/, $)
	    }
	    $perm &= ~06000 unless $ok;
	}
    }
    return 0 unless @tostat;
    return 1 if $perm == ($tostat[2] & 07777);
    return eval { chmod $perm, $to; } ? 1 : 0;
}

sub _move {
    croak("Usage: move(FROM, TO) ") unless @_ == 3;

    my($from,$to,$fallback) = @_;

    my($fromsz,$tosz1,$tomt1,$tosz2,$tomt2,$sts,$ossts);

    if (-d $to && ! -d $from) {
	$to = _catname($from, $to);
    }

    ($tosz1,$tomt1) = (stat($to))[7,9];
    $fromsz = -s $from;
    if ($^O eq 'os2' and defined $tosz1 and defined $fromsz) {
      # will not rename with overwrite
      unlink $to;
    }

    my $rename_to = $to;
    if (-$^O eq 'VMS' && -e $from) {

        if (! -d $to && ! -d $from) {

            my $vms_efs = _vms_efs();
            my $unix_rpt = _vms_unix_rpt();
            my $unix_mode = 0;
            my $from_unix = 0;
            $from_unix = 1 if ($from =~ /^\.\.?$/);
            my $from_vms = 0;
            $from_vms = 1 if ($from =~ m#[\[<\]]#);

            # Need to know if we are in Unix mode.
            if ($from_vms == $from_unix) {
                $unix_mode = $unix_rpt;
            } else {
                $unix_mode = $from_unix;
            }

            # VMS has sticky defaults on extensions, which means that
            # if there is a null extension on the destination file, it
            # will inherit the extension of the source file
            # So add a '.' for a null extension.

            # In unix_rpt mode, the trailing dot should not be added.

            if ($vms_efs) {
                $rename_to = $to;
            } else {
                $rename_to = VMS::Filespec::vmsify($to);
            }
            my ($vol, $dirs, $file) = File::Spec->splitpath($rename_to);
            $file = $file . '.'
                unless (($file =~ /(?<!\^)\./) || $unix_rpt);
            $rename_to = File::Spec->catpath($vol, $dirs, $file);

            # Get rid of the old versions to be like UNIX
            1 while unlink $rename_to;
        }
    }

    return 1 if rename $from, $rename_to;

    # Did rename return an error even though it succeeded, because $to
    # is on a remote NFS file system, and NFS lost the server's ack?
    return 1 if defined($fromsz) && !-e $from &&           # $from disappeared
                (($tosz2,$tomt2) = (stat($to))[7,9]) &&    # $to's there
                  ((!defined $tosz1) ||			   #  not before or
		   ($tosz1 != $tosz2 or $tomt1 != $tomt2)) &&  #   was changed
                $tosz2 == $fromsz;                         # it's all there

    ($tosz1,$tomt1) = (stat($to))[7,9];  # just in case rename did something

    {
        local $@;
        eval {
            local $SIG{__DIE__};
            $fallback->($from,$to) or die;
            my($atime, $mtime) = (stat($from))[8,9];
            utime($atime, $mtime, $to);
            unlink($from)   or die;
        };
        return 1 unless $@;
    }
    ($sts,$ossts) = ($! + 0, $^E + 0);

    ($tosz2,$tomt2) = ((stat($to))[7,9],0,0) if defined $tomt1;
    unlink($to) if !defined($tomt1) or $tomt1 != $tomt2 or $tosz1 != $tosz2;
    ($!,$^E) = ($sts,$ossts);
    return 0;
}

sub move { _move(@_,\&copy); }
sub mv   { _move(@_,\&cp);   }

# &syscopy is an XSUB under OS/2
unless (defined &syscopy) {
    if ($^O eq 'VMS') {
	*syscopy = \&rmscopy;
    } elsif ($^O eq 'mpeix') {
	*syscopy = sub {
	    return 0 unless @_ == 2;
	    # Use the MPE cp program in order to
	    # preserve MPE file attributes.
	    return system('/bin/cp', '-f', $_[0], $_[1]) == 0;
	};
    } elsif ($^O eq 'MSWin32' && defined &DynaLoader::boot_DynaLoader) {
	# Win32::CopyFile() fill only work if we can load Win32.xs
	*syscopy = sub {
	    return 0 unless @_ == 2;
	    return Win32::CopyFile(@_, 1);
	};
    } else {
	$Syscopy_is_copy = 1;
	*syscopy = \&copy;
    }
}

1;

__END__

#line 596

FILE   2bfbaaf0/File/Find.pm  T�#line 1 "/usr/lib/perl5/5.14/File/Find.pm"
package File::Find;
use 5.006;
use strict;
use warnings;
use warnings::register;
our $VERSION = '1.19';
require Exporter;
require Cwd;

#
# Modified to ensure sub-directory traversal order is not inverted by stack
# push and pops.  That is remains in the same order as in the directory file,
# or user pre-processing (EG:sorted).
#

#line 344

our @ISA = qw(Exporter);
our @EXPORT = qw(find finddepth);


use strict;
my $Is_VMS;
my $Is_Win32;

require File::Basename;
require File::Spec;

# Should ideally be my() not our() but local() currently
# refuses to operate on lexicals

our %SLnkSeen;
our ($wanted_callback, $avoid_nlink, $bydepth, $no_chdir, $follow,
    $follow_skip, $full_check, $untaint, $untaint_skip, $untaint_pat,
    $pre_process, $post_process, $dangling_symlinks);

sub contract_name {
    my ($cdir,$fn) = @_;

    return substr($cdir,0,rindex($cdir,'/')) if $fn eq $File::Find::current_dir;

    $cdir = substr($cdir,0,rindex($cdir,'/')+1);

    $fn =~ s|^\./||;

    my $abs_name= $cdir . $fn;

    if (substr($fn,0,3) eq '../') {
       1 while $abs_name =~ s!/[^/]*/\.\./+!/!;
    }

    return $abs_name;
}

sub PathCombine($$) {
    my ($Base,$Name) = @_;
    my $AbsName;

    if (substr($Name,0,1) eq '/') {
	$AbsName= $Name;
    }
    else {
	$AbsName= contract_name($Base,$Name);
    }

    # (simple) check for recursion
    my $newlen= length($AbsName);
    if ($newlen <= length($Base)) {
	if (($newlen == length($Base) || substr($Base,$newlen,1) eq '/')
	    && $AbsName eq substr($Base,0,$newlen))
	{
	    return undef;
	}
    }
    return $AbsName;
}

sub Follow_SymLink($) {
    my ($AbsName) = @_;

    my ($NewName,$DEV, $INO);
    ($DEV, $INO)= lstat $AbsName;

    while (-l _) {
	if ($SLnkSeen{$DEV, $INO}++) {
	    if ($follow_skip < 2) {
		die "$AbsName is encountered a second time";
	    }
	    else {
		return undef;
	    }
	}
	$NewName= PathCombine($AbsName, readlink($AbsName));
	unless(defined $NewName) {
	    if ($follow_skip < 2) {
		die "$AbsName is a recursive symbolic link";
	    }
	    else {
		return undef;
	    }
	}
	else {
	    $AbsName= $NewName;
	}
	($DEV, $INO) = lstat($AbsName);
	return undef unless defined $DEV;  #  dangling symbolic link
    }

    if ($full_check && defined $DEV && $SLnkSeen{$DEV, $INO}++) {
	if ( ($follow_skip < 1) || ((-d _) && ($follow_skip < 2)) ) {
	    die "$AbsName encountered a second time";
	}
	else {
	    return undef;
	}
    }

    return $AbsName;
}

our($dir, $name, $fullname, $prune);
sub _find_dir_symlnk($$$);
sub _find_dir($$$);

# check whether or not a scalar variable is tainted
# (code straight from the Camel, 3rd ed., page 561)
sub is_tainted_pp {
    my $arg = shift;
    my $nada = substr($arg, 0, 0); # zero-length
    local $@;
    eval { eval "# $nada" };
    return length($@) != 0;
}

sub _find_opt {
    my $wanted = shift;
    die "invalid top directory" unless defined $_[0];

    # This function must local()ize everything because callbacks may
    # call find() or finddepth()

    local %SLnkSeen;
    local ($wanted_callback, $avoid_nlink, $bydepth, $no_chdir, $follow,
	$follow_skip, $full_check, $untaint, $untaint_skip, $untaint_pat,
	$pre_process, $post_process, $dangling_symlinks);
    local($dir, $name, $fullname, $prune);
    local *_ = \my $a;

    my $cwd            = $wanted->{bydepth} ? Cwd::fastcwd() : Cwd::getcwd();
    if ($Is_VMS) {
	# VMS returns this by default in VMS format which just doesn't
	# work for the rest of this module.
	$cwd = VMS::Filespec::unixpath($cwd);

	# Apparently this is not expected to have a trailing space.
	# To attempt to make VMS/UNIX conversions mostly reversable,
	# a trailing slash is needed.  The run-time functions ignore the
	# resulting double slash, but it causes the perl tests to fail.
        $cwd =~ s#/\z##;

	# This comes up in upper case now, but should be lower.
	# In the future this could be exact case, no need to change.
    }
    my $cwd_untainted  = $cwd;
    my $check_t_cwd    = 1;
    $wanted_callback   = $wanted->{wanted};
    $bydepth           = $wanted->{bydepth};
    $pre_process       = $wanted->{preprocess};
    $post_process      = $wanted->{postprocess};
    $no_chdir          = $wanted->{no_chdir};
    $full_check        = $Is_Win32 ? 0 : $wanted->{follow};
    $follow            = $Is_Win32 ? 0 :
                             $full_check || $wanted->{follow_fast};
    $follow_skip       = $wanted->{follow_skip};
    $untaint           = $wanted->{untaint};
    $untaint_pat       = $wanted->{untaint_pattern};
    $untaint_skip      = $wanted->{untaint_skip};
    $dangling_symlinks = $wanted->{dangling_symlinks};

    # for compatibility reasons (find.pl, find2perl)
    local our ($topdir, $topdev, $topino, $topmode, $topnlink);

    # a symbolic link to a directory doesn't increase the link count
    $avoid_nlink      = $follow || $File::Find::dont_use_nlink;

    my ($abs_dir, $Is_Dir);

    Proc_Top_Item:
    foreach my $TOP (@_) {
	my $top_item = $TOP;

	($topdev,$topino,$topmode,$topnlink) = $follow ? stat $top_item : lstat $top_item;

	if ($Is_Win32) {
	    $top_item =~ s|[/\\]\z||
	      unless $top_item =~ m{^(?:\w:)?[/\\]$};
	}
	else {
	    $top_item =~ s|/\z|| unless $top_item eq '/';
	}

	$Is_Dir= 0;

	if ($follow) {

	    if (substr($top_item,0,1) eq '/') {
		$abs_dir = $top_item;
	    }
	    elsif ($top_item eq $File::Find::current_dir) {
		$abs_dir = $cwd;
	    }
	    else {  # care about any  ../
		$top_item =~ s/\.dir\z//i if $Is_VMS;
		$abs_dir = contract_name("$cwd/",$top_item);
	    }
	    $abs_dir= Follow_SymLink($abs_dir);
	    unless (defined $abs_dir) {
		if ($dangling_symlinks) {
		    if (ref $dangling_symlinks eq 'CODE') {
			$dangling_symlinks->($top_item, $cwd);
		    } else {
			warnings::warnif "$top_item is a dangling symbolic link\n";
		    }
		}
		next Proc_Top_Item;
	    }

	    if (-d _) {
		$top_item =~ s/\.dir\z//i if $Is_VMS;
		_find_dir_symlnk($wanted, $abs_dir, $top_item);
		$Is_Dir= 1;
	    }
	}
	else { # no follow
	    $topdir = $top_item;
	    unless (defined $topnlink) {
		warnings::warnif "Can't stat $top_item: $!\n";
		next Proc_Top_Item;
	    }
	    if (-d _) {
		$top_item =~ s/\.dir\z//i if $Is_VMS;
		_find_dir($wanted, $top_item, $topnlink);
		$Is_Dir= 1;
	    }
	    else {
		$abs_dir= $top_item;
	    }
	}

	unless ($Is_Dir) {
	    unless (($_,$dir) = File::Basename::fileparse($abs_dir)) {
		($dir,$_) = ('./', $top_item);
	    }

	    $abs_dir = $dir;
	    if (( $untaint ) && (is_tainted($dir) )) {
		( $abs_dir ) = $dir =~ m|$untaint_pat|;
		unless (defined $abs_dir) {
		    if ($untaint_skip == 0) {
			die "directory $dir is still tainted";
		    }
		    else {
			next Proc_Top_Item;
		    }
		}
	    }

	    unless ($no_chdir || chdir $abs_dir) {
		warnings::warnif "Couldn't chdir $abs_dir: $!\n";
		next Proc_Top_Item;
	    }

	    $name = $abs_dir . $_; # $File::Find::name
	    $_ = $name if $no_chdir;

	    { $wanted_callback->() }; # protect against wild "next"

	}

	unless ( $no_chdir ) {
	    if ( ($check_t_cwd) && (($untaint) && (is_tainted($cwd) )) ) {
		( $cwd_untainted ) = $cwd =~ m|$untaint_pat|;
		unless (defined $cwd_untainted) {
		    die "insecure cwd in find(depth)";
		}
		$check_t_cwd = 0;
	    }
	    unless (chdir $cwd_untainted) {
		die "Can't cd to $cwd: $!\n";
	    }
	}
    }
}

# API:
#  $wanted
#  $p_dir :  "parent directory"
#  $nlink :  what came back from the stat
# preconditions:
#  chdir (if not no_chdir) to dir

sub _find_dir($$$) {
    my ($wanted, $p_dir, $nlink) = @_;
    my ($CdLvl,$Level) = (0,0);
    my @Stack;
    my @filenames;
    my ($subcount,$sub_nlink);
    my $SE= [];
    my $dir_name= $p_dir;
    my $dir_pref;
    my $dir_rel = $File::Find::current_dir;
    my $tainted = 0;
    my $no_nlink;

    if ($Is_Win32) {
	$dir_pref
	  = ($p_dir =~ m{^(?:\w:[/\\]?|[/\\])$} ? $p_dir : "$p_dir/" );
    } elsif ($Is_VMS) {

	#	VMS is returning trailing .dir on directories
	#	and trailing . on files and symbolic links
	#	in UNIX syntax.
	#

	$p_dir =~ s/\.(dir)?$//i unless $p_dir eq '.';

	$dir_pref = ($p_dir =~ m/[\]>]+$/ ? $p_dir : "$p_dir/" );
    }
    else {
	$dir_pref= ( $p_dir eq '/' ? '/' : "$p_dir/" );
    }

    local ($dir, $name, $prune, *DIR);

    unless ( $no_chdir || ($p_dir eq $File::Find::current_dir)) {
	my $udir = $p_dir;
	if (( $untaint ) && (is_tainted($p_dir) )) {
	    ( $udir ) = $p_dir =~ m|$untaint_pat|;
	    unless (defined $udir) {
		if ($untaint_skip == 0) {
		    die "directory $p_dir is still tainted";
		}
		else {
		    return;
		}
	    }
	}
	unless (chdir ($Is_VMS && $udir !~ /[\/\[<]+/ ? "./$udir" : $udir)) {
	    warnings::warnif "Can't cd to $udir: $!\n";
	    return;
	}
    }

    # push the starting directory
    push @Stack,[$CdLvl,$p_dir,$dir_rel,-1]  if  $bydepth;

    while (defined $SE) {
	unless ($bydepth) {
	    $dir= $p_dir; # $File::Find::dir
	    $name= $dir_name; # $File::Find::name
	    $_= ($no_chdir ? $dir_name : $dir_rel ); # $_
	    # prune may happen here
	    $prune= 0;
	    { $wanted_callback->() };	# protect against wild "next"
	    next if $prune;
	}

	# change to that directory
	unless ($no_chdir || ($dir_rel eq $File::Find::current_dir)) {
	    my $udir= $dir_rel;
	    if ( ($untaint) && (($tainted) || ($tainted = is_tainted($dir_rel) )) ) {
		( $udir ) = $dir_rel =~ m|$untaint_pat|;
		unless (defined $udir) {
		    if ($untaint_skip == 0) {
			die "directory (" . ($p_dir ne '/' ? $p_dir : '') . "/) $dir_rel is still tainted";
		    } else { # $untaint_skip == 1
			next;
		    }
		}
	    }
	    unless (chdir ($Is_VMS && $udir !~ /[\/\[<]+/ ? "./$udir" : $udir)) {
		warnings::warnif "Can't cd to (" .
		    ($p_dir ne '/' ? $p_dir : '') . "/) $udir: $!\n";
		next;
	    }
	    $CdLvl++;
	}

	$dir= $dir_name; # $File::Find::dir

	# Get the list of files in the current directory.
	unless (opendir DIR, ($no_chdir ? $dir_name : $File::Find::current_dir)) {
	    warnings::warnif "Can't opendir($dir_name): $!\n";
	    next;
	}
	@filenames = readdir DIR;
	closedir(DIR);
	@filenames = $pre_process->(@filenames) if $pre_process;
	push @Stack,[$CdLvl,$dir_name,"",-2]   if $post_process;

	# default: use whatever was specified
        # (if $nlink >= 2, and $avoid_nlink == 0, this will switch back)
        $no_nlink = $avoid_nlink;
        # if dir has wrong nlink count, force switch to slower stat method
        $no_nlink = 1 if ($nlink < 2);

	if ($nlink == 2 && !$no_nlink) {
	    # This dir has no subdirectories.
	    for my $FN (@filenames) {
		if ($Is_VMS) {
		# Big hammer here - Compensate for VMS trailing . and .dir
		# No win situation until this is changed, but this
		# will handle the majority of the cases with breaking the fewest

		    $FN =~ s/\.dir\z//i;
		    $FN =~ s#\.$## if ($FN ne '.');
		}
		next if $FN =~ $File::Find::skip_pattern;
		
		$name = $dir_pref . $FN; # $File::Find::name
		$_ = ($no_chdir ? $name : $FN); # $_
		{ $wanted_callback->() }; # protect against wild "next"
	    }

	}
	else {
	    # This dir has subdirectories.
	    $subcount = $nlink - 2;

	    # HACK: insert directories at this position. so as to preserve
	    # the user pre-processed ordering of files.
	    # EG: directory traversal is in user sorted order, not at random.
            my $stack_top = @Stack;

	    for my $FN (@filenames) {
		next if $FN =~ $File::Find::skip_pattern;
		if ($subcount > 0 || $no_nlink) {
		    # Seen all the subdirs?
		    # check for directoriness.
		    # stat is faster for a file in the current directory
		    $sub_nlink = (lstat ($no_chdir ? $dir_pref . $FN : $FN))[3];

		    if (-d _) {
			--$subcount;
			$FN =~ s/\.dir\z//i if $Is_VMS;
			# HACK: replace push to preserve dir traversal order
			#push @Stack,[$CdLvl,$dir_name,$FN,$sub_nlink];
			splice @Stack, $stack_top, 0,
			         [$CdLvl,$dir_name,$FN,$sub_nlink];
		    }
		    else {
			$name = $dir_pref . $FN; # $File::Find::name
			$_= ($no_chdir ? $name : $FN); # $_
			{ $wanted_callback->() }; # protect against wild "next"
		    }
		}
		else {
		    $name = $dir_pref . $FN; # $File::Find::name
		    $_= ($no_chdir ? $name : $FN); # $_
		    { $wanted_callback->() }; # protect against wild "next"
		}
	    }
	}
    }
    continue {
	while ( defined ($SE = pop @Stack) ) {
	    ($Level, $p_dir, $dir_rel, $nlink) = @$SE;
	    if ($CdLvl > $Level && !$no_chdir) {
		my $tmp;
		if ($Is_VMS) {
		    $tmp = '[' . ('-' x ($CdLvl-$Level)) . ']';
		}
		else {
		    $tmp = join('/',('..') x ($CdLvl-$Level));
		}
		die "Can't cd to $tmp from $dir_name"
		    unless chdir ($tmp);
		$CdLvl = $Level;
	    }

	    if ($Is_Win32) {
		$dir_name = ($p_dir =~ m{^(?:\w:[/\\]?|[/\\])$}
		    ? "$p_dir$dir_rel" : "$p_dir/$dir_rel");
		$dir_pref = "$dir_name/";
	    }
	    elsif ($^O eq 'VMS') {
                if ($p_dir =~ m/[\]>]+$/) {
                    $dir_name = $p_dir;
                    $dir_name =~ s/([\]>]+)$/.$dir_rel$1/;
                    $dir_pref = $dir_name;
                }
                else {
                    $dir_name = "$p_dir/$dir_rel";
                    $dir_pref = "$dir_name/";
                }
	    }
	    else {
		$dir_name = ($p_dir eq '/' ? "/$dir_rel" : "$p_dir/$dir_rel");
		$dir_pref = "$dir_name/";
	    }

	    if ( $nlink == -2 ) {
		$name = $dir = $p_dir; # $File::Find::name / dir
                $_ = $File::Find::current_dir;
		$post_process->();		# End-of-directory processing
	    }
	    elsif ( $nlink < 0 ) {  # must be finddepth, report dirname now
		$name = $dir_name;
		if ( substr($name,-2) eq '/.' ) {
		    substr($name, length($name) == 2 ? -1 : -2) = '';
		}
		$dir = $p_dir;
		$_ = ($no_chdir ? $dir_name : $dir_rel );
		if ( substr($_,-2) eq '/.' ) {
		    substr($_, length($_) == 2 ? -1 : -2) = '';
		}
		{ $wanted_callback->() }; # protect against wild "next"
	     }
	     else {
		push @Stack,[$CdLvl,$p_dir,$dir_rel,-1]  if  $bydepth;
		last;
	    }
	}
    }
}


# API:
#  $wanted
#  $dir_loc : absolute location of a dir
#  $p_dir   : "parent directory"
# preconditions:
#  chdir (if not no_chdir) to dir

sub _find_dir_symlnk($$$) {
    my ($wanted, $dir_loc, $p_dir) = @_; # $dir_loc is the absolute directory
    my @Stack;
    my @filenames;
    my $new_loc;
    my $updir_loc = $dir_loc; # untainted parent directory
    my $SE = [];
    my $dir_name = $p_dir;
    my $dir_pref;
    my $loc_pref;
    my $dir_rel = $File::Find::current_dir;
    my $byd_flag; # flag for pending stack entry if $bydepth
    my $tainted = 0;
    my $ok = 1;

    $dir_pref = ( $p_dir   eq '/' ? '/' : "$p_dir/" );
    $loc_pref = ( $dir_loc eq '/' ? '/' : "$dir_loc/" );

    local ($dir, $name, $fullname, $prune, *DIR);

    unless ($no_chdir) {
	# untaint the topdir
	if (( $untaint ) && (is_tainted($dir_loc) )) {
	    ( $updir_loc ) = $dir_loc =~ m|$untaint_pat|; # parent dir, now untainted
	     # once untainted, $updir_loc is pushed on the stack (as parent directory);
	    # hence, we don't need to untaint the parent directory every time we chdir
	    # to it later
	    unless (defined $updir_loc) {
		if ($untaint_skip == 0) {
		    die "directory $dir_loc is still tainted";
		}
		else {
		    return;
		}
	    }
	}
	$ok = chdir($updir_loc) unless ($p_dir eq $File::Find::current_dir);
	unless ($ok) {
	    warnings::warnif "Can't cd to $updir_loc: $!\n";
	    return;
	}
    }

    push @Stack,[$dir_loc,$updir_loc,$p_dir,$dir_rel,-1]  if  $bydepth;

    while (defined $SE) {

	unless ($bydepth) {
	    # change (back) to parent directory (always untainted)
	    unless ($no_chdir) {
		unless (chdir $updir_loc) {
		    warnings::warnif "Can't cd to $updir_loc: $!\n";
		    next;
		}
	    }
	    $dir= $p_dir; # $File::Find::dir
	    $name= $dir_name; # $File::Find::name
	    $_= ($no_chdir ? $dir_name : $dir_rel ); # $_
	    $fullname= $dir_loc; # $File::Find::fullname
	    # prune may happen here
	    $prune= 0;
	    lstat($_); # make sure  file tests with '_' work
	    { $wanted_callback->() }; # protect against wild "next"
	    next if $prune;
	}

	# change to that directory
	unless ($no_chdir || ($dir_rel eq $File::Find::current_dir)) {
	    $updir_loc = $dir_loc;
	    if ( ($untaint) && (($tainted) || ($tainted = is_tainted($dir_loc) )) ) {
		# untaint $dir_loc, what will be pushed on the stack as (untainted) parent dir
		( $updir_loc ) = $dir_loc =~ m|$untaint_pat|;
		unless (defined $updir_loc) {
		    if ($untaint_skip == 0) {
			die "directory $dir_loc is still tainted";
		    }
		    else {
			next;
		    }
		}
	    }
	    unless (chdir $updir_loc) {
		warnings::warnif "Can't cd to $updir_loc: $!\n";
		next;
	    }
	}

	$dir = $dir_name; # $File::Find::dir

	# Get the list of files in the current directory.
	unless (opendir DIR, ($no_chdir ? $dir_loc : $File::Find::current_dir)) {
	    warnings::warnif "Can't opendir($dir_loc): $!\n";
	    next;
	}
	@filenames = readdir DIR;
	closedir(DIR);

	for my $FN (@filenames) {
	    if ($Is_VMS) {
	    # Big hammer here - Compensate for VMS trailing . and .dir
	    # No win situation until this is changed, but this
	    # will handle the majority of the cases with breaking the fewest.

		$FN =~ s/\.dir\z//i;
		$FN =~ s#\.$## if ($FN ne '.');
	    }
	    next if $FN =~ $File::Find::skip_pattern;

	    # follow symbolic links / do an lstat
	    $new_loc = Follow_SymLink($loc_pref.$FN);

	    # ignore if invalid symlink
	    unless (defined $new_loc) {
	        if (!defined -l _ && $dangling_symlinks) {
	            if (ref $dangling_symlinks eq 'CODE') {
	                $dangling_symlinks->($FN, $dir_pref);
	            } else {
	                warnings::warnif "$dir_pref$FN is a dangling symbolic link\n";
	            }
	        }

	        $fullname = undef;
	        $name = $dir_pref . $FN;
	        $_ = ($no_chdir ? $name : $FN);
	        { $wanted_callback->() };
	        next;
	    }

	    if (-d _) {
		if ($Is_VMS) {
		    $FN =~ s/\.dir\z//i;
		    $FN =~ s#\.$## if ($FN ne '.');
		    $new_loc =~ s/\.dir\z//i;
		    $new_loc =~ s#\.$## if ($new_loc ne '.');
		}
		push @Stack,[$new_loc,$updir_loc,$dir_name,$FN,1];
	    }
	    else {
		$fullname = $new_loc; # $File::Find::fullname
		$name = $dir_pref . $FN; # $File::Find::name
		$_ = ($no_chdir ? $name : $FN); # $_
		{ $wanted_callback->() }; # protect against wild "next"
	    }
	}

    }
    continue {
	while (defined($SE = pop @Stack)) {
	    ($dir_loc, $updir_loc, $p_dir, $dir_rel, $byd_flag) = @$SE;
	    $dir_name = ($p_dir eq '/' ? "/$dir_rel" : "$p_dir/$dir_rel");
	    $dir_pref = "$dir_name/";
	    $loc_pref = "$dir_loc/";
	    if ( $byd_flag < 0 ) {  # must be finddepth, report dirname now
		unless ($no_chdir || ($dir_rel eq $File::Find::current_dir)) {
		    unless (chdir $updir_loc) { # $updir_loc (parent dir) is always untainted
			warnings::warnif "Can't cd to $updir_loc: $!\n";
			next;
		    }
		}
		$fullname = $dir_loc; # $File::Find::fullname
		$name = $dir_name; # $File::Find::name
		if ( substr($name,-2) eq '/.' ) {
		    substr($name, length($name) == 2 ? -1 : -2) = ''; # $File::Find::name
		}
		$dir = $p_dir; # $File::Find::dir
		$_ = ($no_chdir ? $dir_name : $dir_rel); # $_
		if ( substr($_,-2) eq '/.' ) {
		    substr($_, length($_) == 2 ? -1 : -2) = '';
		}

		lstat($_); # make sure file tests with '_' work
		{ $wanted_callback->() }; # protect against wild "next"
	    }
	    else {
		push @Stack,[$dir_loc, $updir_loc, $p_dir, $dir_rel,-1]  if  $bydepth;
		last;
	    }
	}
    }
}


sub wrap_wanted {
    my $wanted = shift;
    if ( ref($wanted) eq 'HASH' ) {
        unless( exists $wanted->{wanted} and ref( $wanted->{wanted} ) eq 'CODE' ) {
            die 'no &wanted subroutine given';
        }
	if ( $wanted->{follow} || $wanted->{follow_fast}) {
	    $wanted->{follow_skip} = 1 unless defined $wanted->{follow_skip};
	}
	if ( $wanted->{untaint} ) {
	    $wanted->{untaint_pattern} = $File::Find::untaint_pattern
		unless defined $wanted->{untaint_pattern};
	    $wanted->{untaint_skip} = 0 unless defined $wanted->{untaint_skip};
	}
	return $wanted;
    }
    elsif( ref( $wanted ) eq 'CODE' ) {
	return { wanted => $wanted };
    }
    else {
       die 'no &wanted subroutine given';
    }
}

sub find {
    my $wanted = shift;
    _find_opt(wrap_wanted($wanted), @_);
}

sub finddepth {
    my $wanted = wrap_wanted(shift);
    $wanted->{bydepth} = 1;
    _find_opt($wanted, @_);
}

# default
$File::Find::skip_pattern    = qr/^\.{1,2}\z/;
$File::Find::untaint_pattern = qr|^([-+@\w./]+)$|;

# These are hard-coded for now, but may move to hint files.
if ($^O eq 'VMS') {
    $Is_VMS = 1;
    $File::Find::dont_use_nlink  = 1;
}
elsif ($^O eq 'MSWin32') {
    $Is_Win32 = 1;
}

# this _should_ work properly on all platforms
# where File::Find can be expected to work
$File::Find::current_dir = File::Spec->curdir || '.';

$File::Find::dont_use_nlink = 1
    if $^O eq 'os2' || $^O eq 'dos' || $^O eq 'amigaos' || $Is_Win32 ||
       $^O eq 'interix' || $^O eq 'cygwin' || $^O eq 'epoc' || $^O eq 'qnx' ||
	   $^O eq 'nto';

# Set dont_use_nlink in your hint file if your system's stat doesn't
# report the number of links in a directory as an indication
# of the number of files.
# See, e.g. hints/machten.sh for MachTen 2.2.
unless ($File::Find::dont_use_nlink) {
    require Config;
    $File::Find::dont_use_nlink = 1 if ($Config::Config{'dont_use_nlink'});
}

# We need a function that checks if a scalar is tainted. Either use the
# Scalar::Util module's tainted() function or our (slower) pure Perl
# fallback is_tainted_pp()
{
    local $@;
    eval { require Scalar::Util };
    *is_tainted = $@ ? \&is_tainted_pp : \&Scalar::Util::tainted;
}

1;
FILE   583db6de/File/GlobMapper.pm  �#line 1 "/usr/lib/perl5/5.14/File/GlobMapper.pm"
package File::GlobMapper;

use strict;
use warnings;
use Carp;

our ($CSH_GLOB);

BEGIN
{
    if ($] < 5.006)
    { 
        require File::BSDGlob; import File::BSDGlob qw(:glob) ;
        $CSH_GLOB = File::BSDGlob::GLOB_CSH() ;
        *globber = \&File::BSDGlob::csh_glob;
    }  
    else
    { 
        require File::Glob; import File::Glob qw(:glob) ;
        $CSH_GLOB = File::Glob::GLOB_CSH() ;
        #*globber = \&File::Glob::bsd_glob;
        *globber = \&File::Glob::csh_glob;
    }  
}

our ($Error);

our ($VERSION, @EXPORT_OK);
$VERSION = '1.000';
@EXPORT_OK = qw( globmap );


our ($noPreBS, $metachars, $matchMetaRE, %mapping, %wildCount);
$noPreBS = '(?<!\\\)' ; # no preceding backslash
$metachars = '.*?[](){}';
$matchMetaRE = '[' . quotemeta($metachars) . ']';

%mapping = (
                '*' => '([^/]*)',
                '?' => '([^/])',
                '.' => '\.',
                '[' => '([',
                '(' => '(',
                ')' => ')',
           );

%wildCount = map { $_ => 1 } qw/ * ? . { ( [ /;           

sub globmap ($$;)
{
    my $inputGlob = shift ;
    my $outputGlob = shift ;

    my $obj = new File::GlobMapper($inputGlob, $outputGlob, @_)
        or croak "globmap: $Error" ;
    return $obj->getFileMap();
}

sub new
{
    my $class = shift ;
    my $inputGlob = shift ;
    my $outputGlob = shift ;
    # TODO -- flags needs to default to whatever File::Glob does
    my $flags = shift || $CSH_GLOB ;
    #my $flags = shift ;

    $inputGlob =~ s/^\s*\<\s*//;
    $inputGlob =~ s/\s*\>\s*$//;

    $outputGlob =~ s/^\s*\<\s*//;
    $outputGlob =~ s/\s*\>\s*$//;

    my %object =
            (   InputGlob   => $inputGlob,
                OutputGlob  => $outputGlob,
                GlobFlags   => $flags,
                Braces      => 0,
                WildCount   => 0,
                Pairs       => [],
                Sigil       => '#',
            );

    my $self = bless \%object, ref($class) || $class ;

    $self->_parseInputGlob()
        or return undef ;

    $self->_parseOutputGlob()
        or return undef ;
    
    my @inputFiles = globber($self->{InputGlob}, $flags) ;

    if (GLOB_ERROR)
    {
        $Error = $!;
        return undef ;
    }

    #if (whatever)
    {
        my $missing = grep { ! -e $_ } @inputFiles ;

        if ($missing)
        {
            $Error = "$missing input files do not exist";
            return undef ;
        }
    }

    $self->{InputFiles} = \@inputFiles ;

    $self->_getFiles()
        or return undef ;

    return $self;
}

sub _retError
{
    my $string = shift ;
    $Error = "$string in input fileglob" ;
    return undef ;
}

sub _unmatched
{
    my $delimeter = shift ;

    _retError("Unmatched $delimeter");
    return undef ;
}

sub _parseBit
{
    my $self = shift ;

    my $string = shift ;

    my $out = '';
    my $depth = 0 ;

    while ($string =~ s/(.*?)$noPreBS(,|$matchMetaRE)//)
    {
        $out .= quotemeta($1) ;
        $out .= $mapping{$2} if defined $mapping{$2};

        ++ $self->{WildCount} if $wildCount{$2} ;

        if ($2 eq ',')
        { 
            return _unmatched "("
                if $depth ;
            
            $out .= '|';
        }
        elsif ($2 eq '(')
        { 
            ++ $depth ;
        }
        elsif ($2 eq ')')
        { 
            return _unmatched ")"
                if ! $depth ;

            -- $depth ;
        }
        elsif ($2 eq '[')
        {
            # TODO -- quotemeta & check no '/'
            # TODO -- check for \]  & other \ within the []
            $string =~ s#(.*?\])##
                or return _unmatched "[" ;
            $out .= "$1)" ;
        }
        elsif ($2 eq ']')
        {
            return _unmatched "]" ;
        }
        elsif ($2 eq '{' || $2 eq '}')
        {
            return _retError "Nested {} not allowed" ;
        }
    }

    $out .= quotemeta $string;

    return _unmatched "("
        if $depth ;

    return $out ;
}

sub _parseInputGlob
{
    my $self = shift ;

    my $string = $self->{InputGlob} ;
    my $inGlob = '';

    # Multiple concatenated *'s don't make sense
    #$string =~ s#\*\*+#*# ;

    # TODO -- Allow space to delimit patterns?
    #my @strings = split /\s+/, $string ;
    #for my $str (@strings)
    my $out = '';
    my $depth = 0 ;

    while ($string =~ s/(.*?)$noPreBS($matchMetaRE)//)
    {
        $out .= quotemeta($1) ;
        $out .= $mapping{$2} if defined $mapping{$2};
        ++ $self->{WildCount} if $wildCount{$2} ;

        if ($2 eq '(')
        { 
            ++ $depth ;
        }
        elsif ($2 eq ')')
        { 
            return _unmatched ")"
                if ! $depth ;

            -- $depth ;
        }
        elsif ($2 eq '[')
        {
            # TODO -- quotemeta & check no '/' or '(' or ')'
            # TODO -- check for \]  & other \ within the []
            $string =~ s#(.*?\])##
                or return _unmatched "[";
            $out .= "$1)" ;
        }
        elsif ($2 eq ']')
        {
            return _unmatched "]" ;
        }
        elsif ($2 eq '}')
        {
            return _unmatched "}" ;
        }
        elsif ($2 eq '{')
        {
            # TODO -- check no '/' within the {}
            # TODO -- check for \}  & other \ within the {}

            my $tmp ;
            unless ( $string =~ s/(.*?)$noPreBS\}//)
            {
                return _unmatched "{";
            }
            #$string =~ s#(.*?)\}##;

            #my $alt = join '|', 
            #          map { quotemeta $_ } 
            #          split "$noPreBS,", $1 ;
            my $alt = $self->_parseBit($1);
            defined $alt or return 0 ;
            $out .= "($alt)" ;

            ++ $self->{Braces} ;
        }
    }

    return _unmatched "("
        if $depth ;

    $out .= quotemeta $string ;


    $self->{InputGlob} =~ s/$noPreBS[\(\)]//g;
    $self->{InputPattern} = $out ;

    #print "# INPUT '$self->{InputGlob}' => '$out'\n";

    return 1 ;

}

sub _parseOutputGlob
{
    my $self = shift ;

    my $string = $self->{OutputGlob} ;
    my $maxwild = $self->{WildCount};

    if ($self->{GlobFlags} & GLOB_TILDE)
    #if (1)
    {
        $string =~ s{
              ^ ~             # find a leading tilde
              (               # save this in $1
                  [^/]        # a non-slash character
                        *     # repeated 0 or more times (0 means me)
              )
            }{
              $1
                  ? (getpwnam($1))[7]
                  : ( $ENV{HOME} || $ENV{LOGDIR} )
            }ex;

    }

    # max #1 must be == to max no of '*' in input
    while ( $string =~ m/#(\d)/g )
    {
        croak "Max wild is #$maxwild, you tried #$1"
            if $1 > $maxwild ;
    }

    my $noPreBS = '(?<!\\\)' ; # no preceding backslash
    #warn "noPreBS = '$noPreBS'\n";

    #$string =~ s/${noPreBS}\$(\d)/\${$1}/g;
    $string =~ s/${noPreBS}#(\d)/\${$1}/g;
    $string =~ s#${noPreBS}\*#\${inFile}#g;
    $string = '"' . $string . '"';

    #print "OUTPUT '$self->{OutputGlob}' => '$string'\n";
    $self->{OutputPattern} = $string ;

    return 1 ;
}

sub _getFiles
{
    my $self = shift ;

    my %outInMapping = ();
    my %inFiles = () ;

    foreach my $inFile (@{ $self->{InputFiles} })
    {
        next if $inFiles{$inFile} ++ ;

        my $outFile = $inFile ;

        if ( $inFile =~ m/$self->{InputPattern}/ )
        {
            no warnings 'uninitialized';
            eval "\$outFile = $self->{OutputPattern};" ;

            if (defined $outInMapping{$outFile})
            {
                $Error =  "multiple input files map to one output file";
                return undef ;
            }
            $outInMapping{$outFile} = $inFile;
            push @{ $self->{Pairs} }, [$inFile, $outFile];
        }
    }

    return 1 ;
}

sub getFileMap
{
    my $self = shift ;

    return $self->{Pairs} ;
}

sub getHash
{
    my $self = shift ;

    return { map { $_->[0] => $_->[1] } @{ $self->{Pairs} } } ;
}

1;

__END__

#line 680FILE   5f9d0539/File/Path.pm  ;#line 1 "/usr/lib/perl5/5.14/File/Path.pm"
package File::Path;

use 5.005_04;
use strict;

use Cwd 'getcwd';
use File::Basename ();
use File::Spec     ();

BEGIN {
    if ($] < 5.006) {
        # can't say 'opendir my $dh, $dirname'
        # need to initialise $dh
        eval "use Symbol";
    }
}

use Exporter ();
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);
$VERSION   = '2.08_01';
@ISA       = qw(Exporter);
@EXPORT    = qw(mkpath rmtree);
@EXPORT_OK = qw(make_path remove_tree);

my $Is_VMS     = $^O eq 'VMS';
my $Is_MacOS   = $^O eq 'MacOS';

# These OSes complain if you want to remove a file that you have no
# write permission to:
my $Force_Writeable = grep {$^O eq $_} qw(amigaos dos epoc MSWin32 MacOS os2);

# Unix-like systems need to stat each directory in order to detect
# race condition. MS-Windows is immune to this particular attack.
my $Need_Stat_Check = !($^O eq 'MSWin32');

sub _carp {
    require Carp;
    goto &Carp::carp;
}

sub _croak {
    require Carp;
    goto &Carp::croak;
}

sub _error {
    my $arg     = shift;
    my $message = shift;
    my $object  = shift;

    if ($arg->{error}) {
        $object = '' unless defined $object;
        $message .= ": $!" if $!;
        push @{${$arg->{error}}}, {$object => $message};
    }
    else {
        _carp(defined($object) ? "$message for $object: $!" : "$message: $!");
    }
}

sub make_path {
    push @_, {} unless @_ and UNIVERSAL::isa($_[-1],'HASH');
    goto &mkpath;
}

sub mkpath {
    my $old_style = !(@_ and UNIVERSAL::isa($_[-1],'HASH'));

    my $arg;
    my $paths;

    if ($old_style) {
        my ($verbose, $mode);
        ($paths, $verbose, $mode) = @_;
        $paths = [$paths] unless UNIVERSAL::isa($paths,'ARRAY');
        $arg->{verbose} = $verbose;
        $arg->{mode}    = defined $mode ? $mode : 0777;
    }
    else {
        $arg = pop @_;
        $arg->{mode}      = delete $arg->{mask} if exists $arg->{mask};
        $arg->{mode}      = 0777 unless exists $arg->{mode};
        ${$arg->{error}}  = [] if exists $arg->{error};
        $arg->{owner}     = delete $arg->{user} if exists $arg->{user};
        $arg->{owner}     = delete $arg->{uid}  if exists $arg->{uid};
        if (exists $arg->{owner} and $arg->{owner} =~ /\D/) {
            my $uid = (getpwnam $arg->{owner})[2];
            if (defined $uid) {
                $arg->{owner} = $uid;
            }
            else {
                _error($arg, "unable to map $arg->{owner} to a uid, ownership not changed");
                delete $arg->{owner};
            }
        }
        if (exists $arg->{group} and $arg->{group} =~ /\D/) {
            my $gid = (getgrnam $arg->{group})[2];
            if (defined $gid) {
                $arg->{group} = $gid;
            }
            else {
                _error($arg, "unable to map $arg->{group} to a gid, group ownership not changed");
                delete $arg->{group};
            }
        }
        if (exists $arg->{owner} and not exists $arg->{group}) {
            $arg->{group} = -1; # chown will leave group unchanged
        }
        if (exists $arg->{group} and not exists $arg->{owner}) {
            $arg->{owner} = -1; # chown will leave owner unchanged
        }
        $paths = [@_];
    }
    return _mkpath($arg, $paths);
}

sub _mkpath {
    my $arg   = shift;
    my $paths = shift;

    my(@created,$path);
    foreach $path (@$paths) {
        next unless defined($path) and length($path);
        $path .= '/' if $^O eq 'os2' and $path =~ /^\w:\z/s; # feature of CRT 
        # Logic wants Unix paths, so go with the flow.
        if ($Is_VMS) {
            next if $path eq '/';
            $path = VMS::Filespec::unixify($path);
        }
        next if -d $path;
        my $parent = File::Basename::dirname($path);
        unless (-d $parent or $path eq $parent) {
            push(@created,_mkpath($arg, [$parent]));
        }
        print "mkdir $path\n" if $arg->{verbose};
        if (mkdir($path,$arg->{mode})) {
            push(@created, $path);
            if (exists $arg->{owner}) {
				# NB: $arg->{group} guaranteed to be set during initialisation
                if (!chown $arg->{owner}, $arg->{group}, $path) {
                    _error($arg, "Cannot change ownership of $path to $arg->{owner}:$arg->{group}");
                }
            }
        }
        else {
            my $save_bang = $!;
            my ($e, $e1) = ($save_bang, $^E);
            $e .= "; $e1" if $e ne $e1;
            # allow for another process to have created it meanwhile
            if (!-d $path) {
                $! = $save_bang;
                if ($arg->{error}) {
                    push @{${$arg->{error}}}, {$path => $e};
                }
                else {
                    _croak("mkdir $path: $e");
                }
            }
        }
    }
    return @created;
}

sub remove_tree {
    push @_, {} unless @_ and UNIVERSAL::isa($_[-1],'HASH');
    goto &rmtree;
}

sub _is_subdir {
    my($dir, $test) = @_;

    my($dv, $dd) = File::Spec->splitpath($dir, 1);
    my($tv, $td) = File::Spec->splitpath($test, 1);

    # not on same volume
    return 0 if $dv ne $tv;

    my @d = File::Spec->splitdir($dd);
    my @t = File::Spec->splitdir($td);

    # @t can't be a subdir if it's shorter than @d
    return 0 if @t < @d;

    return join('/', @d) eq join('/', splice @t, 0, +@d);
}

sub rmtree {
    my $old_style = !(@_ and UNIVERSAL::isa($_[-1],'HASH'));

    my $arg;
    my $paths;

    if ($old_style) {
        my ($verbose, $safe);
        ($paths, $verbose, $safe) = @_;
        $arg->{verbose} = $verbose;
        $arg->{safe}    = defined $safe    ? $safe    : 0;

        if (defined($paths) and length($paths)) {
            $paths = [$paths] unless UNIVERSAL::isa($paths,'ARRAY');
        }
        else {
            _carp ("No root path(s) specified\n");
            return 0;
        }
    }
    else {
        $arg = pop @_;
        ${$arg->{error}}  = [] if exists $arg->{error};
        ${$arg->{result}} = [] if exists $arg->{result};
        $paths = [@_];
    }

    $arg->{prefix} = '';
    $arg->{depth}  = 0;

    my @clean_path;
    $arg->{cwd} = getcwd() or do {
        _error($arg, "cannot fetch initial working directory");
        return 0;
    };
    for ($arg->{cwd}) { /\A(.*)\Z/; $_ = $1 } # untaint

    for my $p (@$paths) {
        # need to fixup case and map \ to / on Windows
        my $ortho_root = $^O eq 'MSWin32' ? _slash_lc($p)          : $p;
        my $ortho_cwd  = $^O eq 'MSWin32' ? _slash_lc($arg->{cwd}) : $arg->{cwd};
        my $ortho_root_length = length($ortho_root);
        $ortho_root_length-- if $^O eq 'VMS'; # don't compare '.' with ']'
        if ($ortho_root_length && _is_subdir($ortho_root, $ortho_cwd)) {
            local $! = 0;
            _error($arg, "cannot remove path when cwd is $arg->{cwd}", $p);
            next;
        }

        if ($Is_MacOS) {
            $p  = ":$p" unless $p =~ /:/;
            $p .= ":"   unless $p =~ /:\z/;
        }
        elsif ($^O eq 'MSWin32') {
            $p =~ s{[/\\]\z}{};
        }
        else {
            $p =~ s{/\z}{};
        }
        push @clean_path, $p;
    }

    @{$arg}{qw(device inode perm)} = (lstat $arg->{cwd})[0,1] or do {
        _error($arg, "cannot stat initial working directory", $arg->{cwd});
        return 0;
    };

    return _rmtree($arg, \@clean_path);
}

sub _rmtree {
    my $arg   = shift;
    my $paths = shift;

    my $count  = 0;
    my $curdir = File::Spec->curdir();
    my $updir  = File::Spec->updir();

    my (@files, $root);
    ROOT_DIR:
    foreach $root (@$paths) {
        # since we chdir into each directory, it may not be obvious
        # to figure out where we are if we generate a message about
        # a file name. We therefore construct a semi-canonical
        # filename, anchored from the directory being unlinked (as
        # opposed to being truly canonical, anchored from the root (/).

        my $canon = $arg->{prefix}
            ? File::Spec->catfile($arg->{prefix}, $root)
            : $root
        ;

        my ($ldev, $lino, $perm) = (lstat $root)[0,1,2] or next ROOT_DIR;

        if ( -d _ ) {
            $root = VMS::Filespec::vmspath(VMS::Filespec::pathify($root)) if $Is_VMS;

            if (!chdir($root)) {
                # see if we can escalate privileges to get in
                # (e.g. funny protection mask such as -w- instead of rwx)
                $perm &= 07777;
                my $nperm = $perm | 0700;
                if (!($arg->{safe} or $nperm == $perm or chmod($nperm, $root))) {
                    _error($arg, "cannot make child directory read-write-exec", $canon);
                    next ROOT_DIR;
                }
                elsif (!chdir($root)) {
                    _error($arg, "cannot chdir to child", $canon);
                    next ROOT_DIR;
                }
            }

            my ($cur_dev, $cur_inode, $perm) = (stat $curdir)[0,1,2] or do {
                _error($arg, "cannot stat current working directory", $canon);
                next ROOT_DIR;
            };

            if ($Need_Stat_Check) {
                ($ldev eq $cur_dev and $lino eq $cur_inode)
                    or _croak("directory $canon changed before chdir, expected dev=$ldev ino=$lino, actual dev=$cur_dev ino=$cur_inode, aborting.");
            }

            $perm &= 07777; # don't forget setuid, setgid, sticky bits
            my $nperm = $perm | 0700;

            # notabene: 0700 is for making readable in the first place,
            # it's also intended to change it to writable in case we have
            # to recurse in which case we are better than rm -rf for 
            # subtrees with strange permissions

            if (!($arg->{safe} or $nperm == $perm or chmod($nperm, $curdir))) {
                _error($arg, "cannot make directory read+writeable", $canon);
                $nperm = $perm;
            }

            my $d;
            $d = gensym() if $] < 5.006;
            if (!opendir $d, $curdir) {
                _error($arg, "cannot opendir", $canon);
                @files = ();
            }
            else {
                no strict 'refs';
                if (!defined ${"\cTAINT"} or ${"\cTAINT"}) {
                    # Blindly untaint dir names if taint mode is
                    # active, or any perl < 5.006
                    @files = map { /\A(.*)\z/s; $1 } readdir $d;
                }
                else {
                    @files = readdir $d;
                }
                closedir $d;
            }

            if ($Is_VMS) {
                # Deleting large numbers of files from VMS Files-11
                # filesystems is faster if done in reverse ASCIIbetical order.
                # include '.' to '.;' from blead patch #31775
                @files = map {$_ eq '.' ? '.;' : $_} reverse @files;
            }

            @files = grep {$_ ne $updir and $_ ne $curdir} @files;

            if (@files) {
                # remove the contained files before the directory itself
                my $narg = {%$arg};
                @{$narg}{qw(device inode cwd prefix depth)}
                    = ($cur_dev, $cur_inode, $updir, $canon, $arg->{depth}+1);
                $count += _rmtree($narg, \@files);
            }

            # restore directory permissions of required now (in case the rmdir
            # below fails), while we are still in the directory and may do so
            # without a race via '.'
            if ($nperm != $perm and not chmod($perm, $curdir)) {
                _error($arg, "cannot reset chmod", $canon);
            }

            # don't leave the client code in an unexpected directory
            chdir($arg->{cwd})
                or _croak("cannot chdir to $arg->{cwd} from $canon: $!, aborting.");

            # ensure that a chdir upwards didn't take us somewhere other
            # than we expected (see CVE-2002-0435)
            ($cur_dev, $cur_inode) = (stat $curdir)[0,1]
                or _croak("cannot stat prior working directory $arg->{cwd}: $!, aborting.");

            if ($Need_Stat_Check) {
                ($arg->{device} eq $cur_dev and $arg->{inode} eq $cur_inode)
                    or _croak("previous directory $arg->{cwd} changed before entering $canon, expected dev=$ldev ino=$lino, actual dev=$cur_dev ino=$cur_inode, aborting.");
            }

            if ($arg->{depth} or !$arg->{keep_root}) {
                if ($arg->{safe} &&
                    ($Is_VMS ? !&VMS::Filespec::candelete($root) : !-w $root)) {
                    print "skipped $root\n" if $arg->{verbose};
                    next ROOT_DIR;
                }
                if ($Force_Writeable and !chmod $perm | 0700, $root) {
                    _error($arg, "cannot make directory writeable", $canon);
                }
                print "rmdir $root\n" if $arg->{verbose};
                if (rmdir $root) {
                    push @{${$arg->{result}}}, $root if $arg->{result};
                    ++$count;
                }
                else {
                    _error($arg, "cannot remove directory", $canon);
                    if ($Force_Writeable && !chmod($perm, ($Is_VMS ? VMS::Filespec::fileify($root) : $root))
                    ) {
                        _error($arg, sprintf("cannot restore permissions to 0%o",$perm), $canon);
                    }
                }
            }
        }
        else {
            # not a directory
            $root = VMS::Filespec::vmsify("./$root")
                if $Is_VMS
                   && !File::Spec->file_name_is_absolute($root)
                   && ($root !~ m/(?<!\^)[\]>]+/);  # not already in VMS syntax

            if ($arg->{safe} &&
                ($Is_VMS ? !&VMS::Filespec::candelete($root)
                         : !(-l $root || -w $root)))
            {
                print "skipped $root\n" if $arg->{verbose};
                next ROOT_DIR;
            }

            my $nperm = $perm & 07777 | 0600;
            if ($Force_Writeable and $nperm != $perm and not chmod $nperm, $root) {
                _error($arg, "cannot make file writeable", $canon);
            }
            print "unlink $canon\n" if $arg->{verbose};
            # delete all versions under VMS
            for (;;) {
                if (unlink $root) {
                    push @{${$arg->{result}}}, $root if $arg->{result};
                }
                else {
                    _error($arg, "cannot unlink file", $canon);
                    $Force_Writeable and chmod($perm, $root) or
                        _error($arg, sprintf("cannot restore permissions to 0%o",$perm), $canon);
                    last;
                }
                ++$count;
                last unless $Is_VMS && lstat $root;
            }
        }
    }
    return $count;
}

sub _slash_lc {
    # fix up slashes and case on MSWin32 so that we can determine that
    # c:\path\to\dir is underneath C:/Path/To
    my $path = shift;
    $path =~ tr{\\}{/};
    return lc($path);
}

1;
__END__

#line 982
FILE   057be1ea/File/Temp.pm  �8#line 1 "/usr/lib/perl5/5.14/File/Temp.pm"
package File::Temp;

#line 138

# 5.6.0 gives us S_IWOTH, S_IWGRP, our and auto-vivifying filehandls
# People would like a version on 5.004 so give them what they want :-)
use 5.004;
use strict;
use Carp;
use File::Spec 0.8;
use File::Path qw/ rmtree /;
use Fcntl 1.03;
use IO::Seekable;               # For SEEK_*
use Errno;
require VMS::Stdio if $^O eq 'VMS';

# pre-emptively load Carp::Heavy. If we don't when we run out of file
# handles and attempt to call croak() we get an error message telling
# us that Carp::Heavy won't load rather than an error telling us we
# have run out of file handles. We either preload croak() or we
# switch the calls to croak from _gettemp() to use die.
eval { require Carp::Heavy; };

# Need the Symbol package if we are running older perl
require Symbol if $] < 5.006;

### For the OO interface
use base qw/ IO::Handle IO::Seekable /;
use overload '""' => "STRINGIFY", fallback => 1;

# use 'our' on v5.6.0
use vars qw($VERSION @EXPORT_OK %EXPORT_TAGS $DEBUG $KEEP_ALL);

$DEBUG = 0;
$KEEP_ALL = 0;

# We are exporting functions

use base qw/Exporter/;

# Export list - to allow fine tuning of export table

@EXPORT_OK = qw{
                 tempfile
                 tempdir
                 tmpnam
                 tmpfile
                 mktemp
                 mkstemp
                 mkstemps
                 mkdtemp
                 unlink0
                 cleanup
                 SEEK_SET
                 SEEK_CUR
                 SEEK_END
             };

# Groups of functions for export

%EXPORT_TAGS = (
                'POSIX' => [qw/ tmpnam tmpfile /],
                'mktemp' => [qw/ mktemp mkstemp mkstemps mkdtemp/],
                'seekable' => [qw/ SEEK_SET SEEK_CUR SEEK_END /],
               );

# add contents of these tags to @EXPORT
Exporter::export_tags('POSIX','mktemp','seekable');

# Version number

$VERSION = '0.22';

# This is a list of characters that can be used in random filenames

my @CHARS = (qw/ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
                 a b c d e f g h i j k l m n o p q r s t u v w x y z
                 0 1 2 3 4 5 6 7 8 9 _
               /);

# Maximum number of tries to make a temp file before failing

use constant MAX_TRIES => 1000;

# Minimum number of X characters that should be in a template
use constant MINX => 4;

# Default template when no template supplied

use constant TEMPXXX => 'X' x 10;

# Constants for the security level

use constant STANDARD => 0;
use constant MEDIUM   => 1;
use constant HIGH     => 2;

# OPENFLAGS. If we defined the flag to use with Sysopen here this gives
# us an optimisation when many temporary files are requested

my $OPENFLAGS = O_CREAT | O_EXCL | O_RDWR;
my $LOCKFLAG;

unless ($^O eq 'MacOS') {
  for my $oflag (qw/ NOFOLLOW BINARY LARGEFILE NOINHERIT /) {
    my ($bit, $func) = (0, "Fcntl::O_" . $oflag);
    no strict 'refs';
    $OPENFLAGS |= $bit if eval {
      # Make sure that redefined die handlers do not cause problems
      # e.g. CGI::Carp
      local $SIG{__DIE__} = sub {};
      local $SIG{__WARN__} = sub {};
      $bit = &$func();
      1;
    };
  }
  # Special case O_EXLOCK
  $LOCKFLAG = eval {
    local $SIG{__DIE__} = sub {};
    local $SIG{__WARN__} = sub {};
    &Fcntl::O_EXLOCK();
  };
}

# On some systems the O_TEMPORARY flag can be used to tell the OS
# to automatically remove the file when it is closed. This is fine
# in most cases but not if tempfile is called with UNLINK=>0 and
# the filename is requested -- in the case where the filename is to
# be passed to another routine. This happens on windows. We overcome
# this by using a second open flags variable

my $OPENTEMPFLAGS = $OPENFLAGS;
unless ($^O eq 'MacOS') {
  for my $oflag (qw/ TEMPORARY /) {
    my ($bit, $func) = (0, "Fcntl::O_" . $oflag);
    local($@);
    no strict 'refs';
    $OPENTEMPFLAGS |= $bit if eval {
      # Make sure that redefined die handlers do not cause problems
      # e.g. CGI::Carp
      local $SIG{__DIE__} = sub {};
      local $SIG{__WARN__} = sub {};
      $bit = &$func();
      1;
    };
  }
}

# Private hash tracking which files have been created by each process id via the OO interface
my %FILES_CREATED_BY_OBJECT;

# INTERNAL ROUTINES - not to be used outside of package

# Generic routine for getting a temporary filename
# modelled on OpenBSD _gettemp() in mktemp.c

# The template must contain X's that are to be replaced
# with the random values

#  Arguments:

#  TEMPLATE   - string containing the XXXXX's that is converted
#           to a random filename and opened if required

# Optionally, a hash can also be supplied containing specific options
#   "open" => if true open the temp file, else just return the name
#             default is 0
#   "mkdir"=> if true, we are creating a temp directory rather than tempfile
#             default is 0
#   "suffixlen" => number of characters at end of PATH to be ignored.
#                  default is 0.
#   "unlink_on_close" => indicates that, if possible,  the OS should remove
#                        the file as soon as it is closed. Usually indicates
#                        use of the O_TEMPORARY flag to sysopen.
#                        Usually irrelevant on unix
#   "use_exlock" => Indicates that O_EXLOCK should be used. Default is true.

# Optionally a reference to a scalar can be passed into the function
# On error this will be used to store the reason for the error
#   "ErrStr"  => \$errstr

# "open" and "mkdir" can not both be true
# "unlink_on_close" is not used when "mkdir" is true.

# The default options are equivalent to mktemp().

# Returns:
#   filehandle - open file handle (if called with doopen=1, else undef)
#   temp name  - name of the temp file or directory

# For example:
#   ($fh, $name) = _gettemp($template, "open" => 1);

# for the current version, failures are associated with
# stored in an error string and returned to give the reason whilst debugging
# This routine is not called by any external function
sub _gettemp {

  croak 'Usage: ($fh, $name) = _gettemp($template, OPTIONS);'
    unless scalar(@_) >= 1;

  # the internal error string - expect it to be overridden
  # Need this in case the caller decides not to supply us a value
  # need an anonymous scalar
  my $tempErrStr;

  # Default options
  my %options = (
                 "open" => 0,
                 "mkdir" => 0,
                 "suffixlen" => 0,
                 "unlink_on_close" => 0,
                 "use_exlock" => 1,
                 "ErrStr" => \$tempErrStr,
                );

  # Read the template
  my $template = shift;
  if (ref($template)) {
    # Use a warning here since we have not yet merged ErrStr
    carp "File::Temp::_gettemp: template must not be a reference";
    return ();
  }

  # Check that the number of entries on stack are even
  if (scalar(@_) % 2 != 0) {
    # Use a warning here since we have not yet merged ErrStr
    carp "File::Temp::_gettemp: Must have even number of options";
    return ();
  }

  # Read the options and merge with defaults
  %options = (%options, @_)  if @_;

  # Make sure the error string is set to undef
  ${$options{ErrStr}} = undef;

  # Can not open the file and make a directory in a single call
  if ($options{"open"} && $options{"mkdir"}) {
    ${$options{ErrStr}} = "doopen and domkdir can not both be true\n";
    return ();
  }

  # Find the start of the end of the  Xs (position of last X)
  # Substr starts from 0
  my $start = length($template) - 1 - $options{"suffixlen"};

  # Check that we have at least MINX x X (e.g. 'XXXX") at the end of the string
  # (taking suffixlen into account). Any fewer is insecure.

  # Do it using substr - no reason to use a pattern match since
  # we know where we are looking and what we are looking for

  if (substr($template, $start - MINX + 1, MINX) ne 'X' x MINX) {
    ${$options{ErrStr}} = "The template must end with at least ".
      MINX . " 'X' characters\n";
    return ();
  }

  # Replace all the X at the end of the substring with a
  # random character or just all the XX at the end of a full string.
  # Do it as an if, since the suffix adjusts which section to replace
  # and suffixlen=0 returns nothing if used in the substr directly
  # and generate a full path from the template

  my $path = _replace_XX($template, $options{"suffixlen"});


  # Split the path into constituent parts - eventually we need to check
  # whether the directory exists
  # We need to know whether we are making a temp directory
  # or a tempfile

  my ($volume, $directories, $file);
  my $parent;                   # parent directory
  if ($options{"mkdir"}) {
    # There is no filename at the end
    ($volume, $directories, $file) = File::Spec->splitpath( $path, 1);

    # The parent is then $directories without the last directory
    # Split the directory and put it back together again
    my @dirs = File::Spec->splitdir($directories);

    # If @dirs only has one entry (i.e. the directory template) that means
    # we are in the current directory
    if ($#dirs == 0) {
      $parent = File::Spec->curdir;
    } else {

      if ($^O eq 'VMS') {     # need volume to avoid relative dir spec
        $parent = File::Spec->catdir($volume, @dirs[0..$#dirs-1]);
        $parent = 'sys$disk:[]' if $parent eq '';
      } else {

        # Put it back together without the last one
        $parent = File::Spec->catdir(@dirs[0..$#dirs-1]);

        # ...and attach the volume (no filename)
        $parent = File::Spec->catpath($volume, $parent, '');
      }

    }

  } else {

    # Get rid of the last filename (use File::Basename for this?)
    ($volume, $directories, $file) = File::Spec->splitpath( $path );

    # Join up without the file part
    $parent = File::Spec->catpath($volume,$directories,'');

    # If $parent is empty replace with curdir
    $parent = File::Spec->curdir
      unless $directories ne '';

  }

  # Check that the parent directories exist
  # Do this even for the case where we are simply returning a name
  # not a file -- no point returning a name that includes a directory
  # that does not exist or is not writable

  unless (-e $parent) {
    ${$options{ErrStr}} = "Parent directory ($parent) does not exist";
    return ();
  }
  unless (-d $parent) {
    ${$options{ErrStr}} = "Parent directory ($parent) is not a directory";
    return ();
  }

  # Check the stickiness of the directory and chown giveaway if required
  # If the directory is world writable the sticky bit
  # must be set

  if (File::Temp->safe_level == MEDIUM) {
    my $safeerr;
    unless (_is_safe($parent,\$safeerr)) {
      ${$options{ErrStr}} = "Parent directory ($parent) is not safe ($safeerr)";
      return ();
    }
  } elsif (File::Temp->safe_level == HIGH) {
    my $safeerr;
    unless (_is_verysafe($parent, \$safeerr)) {
      ${$options{ErrStr}} = "Parent directory ($parent) is not safe ($safeerr)";
      return ();
    }
  }


  # Now try MAX_TRIES time to open the file
  for (my $i = 0; $i < MAX_TRIES; $i++) {

    # Try to open the file if requested
    if ($options{"open"}) {
      my $fh;

      # If we are running before perl5.6.0 we can not auto-vivify
      if ($] < 5.006) {
        $fh = &Symbol::gensym;
      }

      # Try to make sure this will be marked close-on-exec
      # XXX: Win32 doesn't respect this, nor the proper fcntl,
      #      but may have O_NOINHERIT. This may or may not be in Fcntl.
      local $^F = 2;

      # Attempt to open the file
      my $open_success = undef;
      if ( $^O eq 'VMS' and $options{"unlink_on_close"} && !$KEEP_ALL) {
        # make it auto delete on close by setting FAB$V_DLT bit
        $fh = VMS::Stdio::vmssysopen($path, $OPENFLAGS, 0600, 'fop=dlt');
        $open_success = $fh;
      } else {
        my $flags = ( ($options{"unlink_on_close"} && !$KEEP_ALL) ?
                      $OPENTEMPFLAGS :
                      $OPENFLAGS );
        $flags |= $LOCKFLAG if (defined $LOCKFLAG && $options{use_exlock});
        $open_success = sysopen($fh, $path, $flags, 0600);
      }
      if ( $open_success ) {

        # in case of odd umask force rw
        chmod(0600, $path);

        # Opened successfully - return file handle and name
        return ($fh, $path);

      } else {

        # Error opening file - abort with error
        # if the reason was anything but EEXIST
        unless ($!{EEXIST}) {
          ${$options{ErrStr}} = "Could not create temp file $path: $!";
          return ();
        }

        # Loop round for another try

      }
    } elsif ($options{"mkdir"}) {

      # Open the temp directory
      if (mkdir( $path, 0700)) {
        # in case of odd umask
        chmod(0700, $path);

        return undef, $path;
      } else {

        # Abort with error if the reason for failure was anything
        # except EEXIST
        unless ($!{EEXIST}) {
          ${$options{ErrStr}} = "Could not create directory $path: $!";
          return ();
        }

        # Loop round for another try

      }

    } else {

      # Return true if the file can not be found
      # Directory has been checked previously

      return (undef, $path) unless -e $path;

      # Try again until MAX_TRIES

    }

    # Did not successfully open the tempfile/dir
    # so try again with a different set of random letters
    # No point in trying to increment unless we have only
    # 1 X say and the randomness could come up with the same
    # file MAX_TRIES in a row.

    # Store current attempt - in principal this implies that the
    # 3rd time around the open attempt that the first temp file
    # name could be generated again. Probably should store each
    # attempt and make sure that none are repeated

    my $original = $path;
    my $counter = 0;            # Stop infinite loop
    my $MAX_GUESS = 50;

    do {

      # Generate new name from original template
      $path = _replace_XX($template, $options{"suffixlen"});

      $counter++;

    } until ($path ne $original || $counter > $MAX_GUESS);

    # Check for out of control looping
    if ($counter > $MAX_GUESS) {
      ${$options{ErrStr}} = "Tried to get a new temp name different to the previous value $MAX_GUESS times.\nSomething wrong with template?? ($template)";
      return ();
    }

  }

  # If we get here, we have run out of tries
  ${ $options{ErrStr} } = "Have exceeded the maximum number of attempts ("
    . MAX_TRIES . ") to open temp file/dir";

  return ();

}

# Internal routine to replace the XXXX... with random characters
# This has to be done by _gettemp() every time it fails to
# open a temp file/dir

# Arguments:  $template (the template with XXX),
#             $ignore   (number of characters at end to ignore)

# Returns:    modified template

sub _replace_XX {

  croak 'Usage: _replace_XX($template, $ignore)'
    unless scalar(@_) == 2;

  my ($path, $ignore) = @_;

  # Do it as an if, since the suffix adjusts which section to replace
  # and suffixlen=0 returns nothing if used in the substr directly
  # Alternatively, could simply set $ignore to length($path)-1
  # Don't want to always use substr when not required though.
  my $end = ( $] >= 5.006 ? "\\z" : "\\Z" );

  if ($ignore) {
    substr($path, 0, - $ignore) =~ s/X(?=X*$end)/$CHARS[ int( rand( @CHARS ) ) ]/ge;
  } else {
    $path =~ s/X(?=X*$end)/$CHARS[ int( rand( @CHARS ) ) ]/ge;
  }
  return $path;
}

# Internal routine to force a temp file to be writable after
# it is created so that we can unlink it. Windows seems to occassionally
# force a file to be readonly when written to certain temp locations
sub _force_writable {
  my $file = shift;
  chmod 0600, $file;
}


# internal routine to check to see if the directory is safe
# First checks to see if the directory is not owned by the
# current user or root. Then checks to see if anyone else
# can write to the directory and if so, checks to see if
# it has the sticky bit set

# Will not work on systems that do not support sticky bit

#Args:  directory path to check
#       Optionally: reference to scalar to contain error message
# Returns true if the path is safe and false otherwise.
# Returns undef if can not even run stat() on the path

# This routine based on version written by Tom Christiansen

# Presumably, by the time we actually attempt to create the
# file or directory in this directory, it may not be safe
# anymore... Have to run _is_safe directly after the open.

sub _is_safe {

  my $path = shift;
  my $err_ref = shift;

  # Stat path
  my @info = stat($path);
  unless (scalar(@info)) {
    $$err_ref = "stat(path) returned no values";
    return 0;
  }
  ;
  return 1 if $^O eq 'VMS';     # owner delete control at file level

  # Check to see whether owner is neither superuser (or a system uid) nor me
  # Use the effective uid from the $> variable
  # UID is in [4]
  if ($info[4] > File::Temp->top_system_uid() && $info[4] != $>) {

    Carp::cluck(sprintf "uid=$info[4] topuid=%s euid=$> path='$path'",
                File::Temp->top_system_uid());

    $$err_ref = "Directory owned neither by root nor the current user"
      if ref($err_ref);
    return 0;
  }

  # check whether group or other can write file
  # use 066 to detect either reading or writing
  # use 022 to check writability
  # Do it with S_IWOTH and S_IWGRP for portability (maybe)
  # mode is in info[2]
  if (($info[2] & &Fcntl::S_IWGRP) ||  # Is group writable?
      ($info[2] & &Fcntl::S_IWOTH) ) { # Is world writable?
    # Must be a directory
    unless (-d $path) {
      $$err_ref = "Path ($path) is not a directory"
        if ref($err_ref);
      return 0;
    }
    # Must have sticky bit set
    unless (-k $path) {
      $$err_ref = "Sticky bit not set on $path when dir is group|world writable"
        if ref($err_ref);
      return 0;
    }
  }

  return 1;
}

# Internal routine to check whether a directory is safe
# for temp files. Safer than _is_safe since it checks for
# the possibility of chown giveaway and if that is a possibility
# checks each directory in the path to see if it is safe (with _is_safe)

# If _PC_CHOWN_RESTRICTED is not set, does the full test of each
# directory anyway.

# Takes optional second arg as scalar ref to error reason

sub _is_verysafe {

  # Need POSIX - but only want to bother if really necessary due to overhead
  require POSIX;

  my $path = shift;
  print "_is_verysafe testing $path\n" if $DEBUG;
  return 1 if $^O eq 'VMS';     # owner delete control at file level

  my $err_ref = shift;

  # Should Get the value of _PC_CHOWN_RESTRICTED if it is defined
  # and If it is not there do the extensive test
  local($@);
  my $chown_restricted;
  $chown_restricted = &POSIX::_PC_CHOWN_RESTRICTED()
    if eval { &POSIX::_PC_CHOWN_RESTRICTED(); 1};

  # If chown_resticted is set to some value we should test it
  if (defined $chown_restricted) {

    # Return if the current directory is safe
    return _is_safe($path,$err_ref) if POSIX::sysconf( $chown_restricted );

  }

  # To reach this point either, the _PC_CHOWN_RESTRICTED symbol
  # was not avialable or the symbol was there but chown giveaway
  # is allowed. Either way, we now have to test the entire tree for
  # safety.

  # Convert path to an absolute directory if required
  unless (File::Spec->file_name_is_absolute($path)) {
    $path = File::Spec->rel2abs($path);
  }

  # Split directory into components - assume no file
  my ($volume, $directories, undef) = File::Spec->splitpath( $path, 1);

  # Slightly less efficient than having a function in File::Spec
  # to chop off the end of a directory or even a function that
  # can handle ../ in a directory tree
  # Sometimes splitdir() returns a blank at the end
  # so we will probably check the bottom directory twice in some cases
  my @dirs = File::Spec->splitdir($directories);

  # Concatenate one less directory each time around
  foreach my $pos (0.. $#dirs) {
    # Get a directory name
    my $dir = File::Spec->catpath($volume,
                                  File::Spec->catdir(@dirs[0.. $#dirs - $pos]),
                                  ''
                                 );

    print "TESTING DIR $dir\n" if $DEBUG;

    # Check the directory
    return 0 unless _is_safe($dir,$err_ref);

  }

  return 1;
}



# internal routine to determine whether unlink works on this
# platform for files that are currently open.
# Returns true if we can, false otherwise.

# Currently WinNT, OS/2 and VMS can not unlink an opened file
# On VMS this is because the O_EXCL flag is used to open the
# temporary file. Currently I do not know enough about the issues
# on VMS to decide whether O_EXCL is a requirement.

sub _can_unlink_opened_file {

  if ($^O eq 'MSWin32' || $^O eq 'os2' || $^O eq 'VMS' || $^O eq 'dos' || $^O eq 'MacOS') {
    return 0;
  } else {
    return 1;
  }

}

# internal routine to decide which security levels are allowed
# see safe_level() for more information on this

# Controls whether the supplied security level is allowed

#   $cando = _can_do_level( $level )

sub _can_do_level {

  # Get security level
  my $level = shift;

  # Always have to be able to do STANDARD
  return 1 if $level == STANDARD;

  # Currently, the systems that can do HIGH or MEDIUM are identical
  if ( $^O eq 'MSWin32' || $^O eq 'os2' || $^O eq 'cygwin' || $^O eq 'dos' || $^O eq 'MacOS' || $^O eq 'mpeix') {
    return 0;
  } else {
    return 1;
  }

}

# This routine sets up a deferred unlinking of a specified
# filename and filehandle. It is used in the following cases:
#  - Called by unlink0 if an opened file can not be unlinked
#  - Called by tempfile() if files are to be removed on shutdown
#  - Called by tempdir() if directories are to be removed on shutdown

# Arguments:
#   _deferred_unlink( $fh, $fname, $isdir );
#
#   - filehandle (so that it can be expclicitly closed if open
#   - filename   (the thing we want to remove)
#   - isdir      (flag to indicate that we are being given a directory)
#                 [and hence no filehandle]

# Status is not referred to since all the magic is done with an END block

{
  # Will set up two lexical variables to contain all the files to be
  # removed. One array for files, another for directories They will
  # only exist in this block.

  #  This means we only have to set up a single END block to remove
  #  all files. 

  # in order to prevent child processes inadvertently deleting the parent
  # temp files we use a hash to store the temp files and directories
  # created by a particular process id.

  # %files_to_unlink contains values that are references to an array of
  # array references containing the filehandle and filename associated with
  # the temp file.
  my (%files_to_unlink, %dirs_to_unlink);

  # Set up an end block to use these arrays
  END {
    local($., $@, $!, $^E, $?);
    cleanup();
  }

  # Cleanup function. Always triggered on END but can be invoked
  # manually.
  sub cleanup {
    if (!$KEEP_ALL) {
      # Files
      my @files = (exists $files_to_unlink{$$} ?
                   @{ $files_to_unlink{$$} } : () );
      foreach my $file (@files) {
        # close the filehandle without checking its state
        # in order to make real sure that this is closed
        # if its already closed then I dont care about the answer
        # probably a better way to do this
        close($file->[0]);      # file handle is [0]

        if (-f $file->[1]) {       # file name is [1]
          _force_writable( $file->[1] ); # for windows
          unlink $file->[1] or warn "Error removing ".$file->[1];
        }
      }
      # Dirs
      my @dirs = (exists $dirs_to_unlink{$$} ?
                  @{ $dirs_to_unlink{$$} } : () );
      foreach my $dir (@dirs) {
        if (-d $dir) {
          # Some versions of rmtree will abort if you attempt to remove
          # the directory you are sitting in. We protect that and turn it
          # into a warning. We do this because this occurs during
          # cleanup and so can not be caught by the user.
          eval { rmtree($dir, $DEBUG, 0); };
          warn $@ if ($@ && $^W);
        }
      }

      # clear the arrays
      @{ $files_to_unlink{$$} } = ()
        if exists $files_to_unlink{$$};
      @{ $dirs_to_unlink{$$} } = ()
        if exists $dirs_to_unlink{$$};
    }
  }


  # This is the sub called to register a file for deferred unlinking
  # This could simply store the input parameters and defer everything
  # until the END block. For now we do a bit of checking at this
  # point in order to make sure that (1) we have a file/dir to delete
  # and (2) we have been called with the correct arguments.
  sub _deferred_unlink {

    croak 'Usage:  _deferred_unlink($fh, $fname, $isdir)'
      unless scalar(@_) == 3;

    my ($fh, $fname, $isdir) = @_;

    warn "Setting up deferred removal of $fname\n"
      if $DEBUG;

    # If we have a directory, check that it is a directory
    if ($isdir) {

      if (-d $fname) {

        # Directory exists so store it
        # first on VMS turn []foo into [.foo] for rmtree
        $fname = VMS::Filespec::vmspath($fname) if $^O eq 'VMS';
        $dirs_to_unlink{$$} = [] 
          unless exists $dirs_to_unlink{$$};
        push (@{ $dirs_to_unlink{$$} }, $fname);

      } else {
        carp "Request to remove directory $fname could not be completed since it does not exist!\n" if $^W;
      }

    } else {

      if (-f $fname) {

        # file exists so store handle and name for later removal
        $files_to_unlink{$$} = []
          unless exists $files_to_unlink{$$};
        push(@{ $files_to_unlink{$$} }, [$fh, $fname]);

      } else {
        carp "Request to remove file $fname could not be completed since it is not there!\n" if $^W;
      }

    }

  }


}

#line 1008

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;

  # read arguments and convert keys to upper case
  my %args = @_;
  %args = map { uc($_), $args{$_} } keys %args;

  # see if they are unlinking (defaulting to yes)
  my $unlink = (exists $args{UNLINK} ? $args{UNLINK} : 1 );
  delete $args{UNLINK};

  # template (store it in an array so that it will
  # disappear from the arg list of tempfile)
  my @template = ( exists $args{TEMPLATE} ? $args{TEMPLATE} : () );
  delete $args{TEMPLATE};

  # Protect OPEN
  delete $args{OPEN};

  # Open the file and retain file handle and file name
  my ($fh, $path) = tempfile( @template, %args );

  print "Tmp: $fh - $path\n" if $DEBUG;

  # Store the filename in the scalar slot
  ${*$fh} = $path;

  # Cache the filename by pid so that the destructor can decide whether to remove it
  $FILES_CREATED_BY_OBJECT{$$}{$path} = 1;

  # Store unlink information in hash slot (plus other constructor info)
  %{*$fh} = %args;

  # create the object
  bless $fh, $class;

  # final method-based configuration
  $fh->unlink_on_destroy( $unlink );

  return $fh;
}

#line 1066

sub newdir {
  my $self = shift;

  # need to handle args as in tempdir because we have to force CLEANUP
  # default without passing CLEANUP to tempdir
  my $template = (scalar(@_) % 2 == 1 ? shift(@_) : undef );
  my %options = @_;
  my $cleanup = (exists $options{CLEANUP} ? $options{CLEANUP} : 1 );

  delete $options{CLEANUP};

  my $tempdir;
  if (defined $template) {
    $tempdir = tempdir( $template, %options );
  } else {
    $tempdir = tempdir( %options );
  }
  return bless { DIRNAME => $tempdir,
                 CLEANUP => $cleanup,
                 LAUNCHPID => $$,
               }, "File::Temp::Dir";
}

#line 1101

sub filename {
  my $self = shift;
  return ${*$self};
}

sub STRINGIFY {
  my $self = shift;
  return $self->filename;
}

#line 1131

sub unlink_on_destroy {
  my $self = shift;
  if (@_) {
    ${*$self}{UNLINK} = shift;
  }
  return ${*$self}{UNLINK};
}

#line 1160

sub DESTROY {
  local($., $@, $!, $^E, $?);
  my $self = shift;

  # Make sure we always remove the file from the global hash
  # on destruction. This prevents the hash from growing uncontrollably
  # and post-destruction there is no reason to know about the file.
  my $file = $self->filename;
  my $was_created_by_proc;
  if (exists $FILES_CREATED_BY_OBJECT{$$}{$file}) {
    $was_created_by_proc = 1;
    delete $FILES_CREATED_BY_OBJECT{$$}{$file};
  }

  if (${*$self}{UNLINK} && !$KEEP_ALL) {
    print "# --------->   Unlinking $self\n" if $DEBUG;

    # only delete if this process created it
    return unless $was_created_by_proc;

    # The unlink1 may fail if the file has been closed
    # by the caller. This leaves us with the decision
    # of whether to refuse to remove the file or simply
    # do an unlink without test. Seems to be silly
    # to do this when we are trying to be careful
    # about security
    _force_writable( $file ); # for windows
    unlink1( $self, $file )
      or unlink($file);
  }
}

#line 1294

sub tempfile {

  # Can not check for argument count since we can have any
  # number of args

  # Default options
  my %options = (
                 "DIR"    => undef, # Directory prefix
                 "SUFFIX" => '',    # Template suffix
                 "UNLINK" => 0,     # Do not unlink file on exit
                 "OPEN"   => 1,     # Open file
                 "TMPDIR" => 0, # Place tempfile in tempdir if template specified
                 "EXLOCK" => 1, # Open file with O_EXLOCK
                );

  # Check to see whether we have an odd or even number of arguments
  my $template = (scalar(@_) % 2 == 1 ? shift(@_) : undef);

  # Read the options and merge with defaults
  %options = (%options, @_)  if @_;

  # First decision is whether or not to open the file
  if (! $options{"OPEN"}) {

    warn "tempfile(): temporary filename requested but not opened.\nPossibly unsafe, consider using tempfile() with OPEN set to true\n"
      if $^W;

  }

  if ($options{"DIR"} and $^O eq 'VMS') {

    # on VMS turn []foo into [.foo] for concatenation
    $options{"DIR"} = VMS::Filespec::vmspath($options{"DIR"});
  }

  # Construct the template

  # Have a choice of trying to work around the mkstemp/mktemp/tmpnam etc
  # functions or simply constructing a template and using _gettemp()
  # explicitly. Go for the latter

  # First generate a template if not defined and prefix the directory
  # If no template must prefix the temp directory
  if (defined $template) {
    # End up with current directory if neither DIR not TMPDIR are set
    if ($options{"DIR"}) {

      $template = File::Spec->catfile($options{"DIR"}, $template);

    } elsif ($options{TMPDIR}) {

      $template = File::Spec->catfile(File::Spec->tmpdir, $template );

    }

  } else {

    if ($options{"DIR"}) {

      $template = File::Spec->catfile($options{"DIR"}, TEMPXXX);

    } else {

      $template = File::Spec->catfile(File::Spec->tmpdir, TEMPXXX);

    }

  }

  # Now add a suffix
  $template .= $options{"SUFFIX"};

  # Determine whether we should tell _gettemp to unlink the file
  # On unix this is irrelevant and can be worked out after the file is
  # opened (simply by unlinking the open filehandle). On Windows or VMS
  # we have to indicate temporary-ness when we open the file. In general
  # we only want a true temporary file if we are returning just the
  # filehandle - if the user wants the filename they probably do not
  # want the file to disappear as soon as they close it (which may be
  # important if they want a child process to use the file)
  # For this reason, tie unlink_on_close to the return context regardless
  # of OS.
  my $unlink_on_close = ( wantarray ? 0 : 1);

  # Create the file
  my ($fh, $path, $errstr);
  croak "Error in tempfile() using $template: $errstr"
    unless (($fh, $path) = _gettemp($template,
                                    "open" => $options{'OPEN'},
                                    "mkdir"=> 0 ,
                                    "unlink_on_close" => $unlink_on_close,
                                    "suffixlen" => length($options{'SUFFIX'}),
                                    "ErrStr" => \$errstr,
                                    "use_exlock" => $options{EXLOCK},
                                   ) );

  # Set up an exit handler that can do whatever is right for the
  # system. This removes files at exit when requested explicitly or when
  # system is asked to unlink_on_close but is unable to do so because
  # of OS limitations.
  # The latter should be achieved by using a tied filehandle.
  # Do not check return status since this is all done with END blocks.
  _deferred_unlink($fh, $path, 0) if $options{"UNLINK"};

  # Return
  if (wantarray()) {

    if ($options{'OPEN'}) {
      return ($fh, $path);
    } else {
      return (undef, $path);
    }

  } else {

    # Unlink the file. It is up to unlink0 to decide what to do with
    # this (whether to unlink now or to defer until later)
    unlink0($fh, $path) or croak "Error unlinking file $path using unlink0";

    # Return just the filehandle.
    return $fh;
  }


}

#line 1483

# '

sub tempdir  {

  # Can not check for argument count since we can have any
  # number of args

  # Default options
  my %options = (
                 "CLEANUP"    => 0, # Remove directory on exit
                 "DIR"        => '', # Root directory
                 "TMPDIR"     => 0,  # Use tempdir with template
                );

  # Check to see whether we have an odd or even number of arguments
  my $template = (scalar(@_) % 2 == 1 ? shift(@_) : undef );

  # Read the options and merge with defaults
  %options = (%options, @_)  if @_;

  # Modify or generate the template

  # Deal with the DIR and TMPDIR options
  if (defined $template) {

    # Need to strip directory path if using DIR or TMPDIR
    if ($options{'TMPDIR'} || $options{'DIR'}) {

      # Strip parent directory from the filename
      #
      # There is no filename at the end
      $template = VMS::Filespec::vmspath($template) if $^O eq 'VMS';
      my ($volume, $directories, undef) = File::Spec->splitpath( $template, 1);

      # Last directory is then our template
      $template = (File::Spec->splitdir($directories))[-1];

      # Prepend the supplied directory or temp dir
      if ($options{"DIR"}) {

        $template = File::Spec->catdir($options{"DIR"}, $template);

      } elsif ($options{TMPDIR}) {

        # Prepend tmpdir
        $template = File::Spec->catdir(File::Spec->tmpdir, $template);

      }

    }

  } else {

    if ($options{"DIR"}) {

      $template = File::Spec->catdir($options{"DIR"}, TEMPXXX);

    } else {

      $template = File::Spec->catdir(File::Spec->tmpdir, TEMPXXX);

    }

  }

  # Create the directory
  my $tempdir;
  my $suffixlen = 0;
  if ($^O eq 'VMS') {           # dir names can end in delimiters
    $template =~ m/([\.\]:>]+)$/;
    $suffixlen = length($1);
  }
  if ( ($^O eq 'MacOS') && (substr($template, -1) eq ':') ) {
    # dir name has a trailing ':'
    ++$suffixlen;
  }

  my $errstr;
  croak "Error in tempdir() using $template: $errstr"
    unless ((undef, $tempdir) = _gettemp($template,
                                         "open" => 0,
                                         "mkdir"=> 1 ,
                                         "suffixlen" => $suffixlen,
                                         "ErrStr" => \$errstr,
                                        ) );

  # Install exit handler; must be dynamic to get lexical
  if ( $options{'CLEANUP'} && -d $tempdir) {
    _deferred_unlink(undef, $tempdir, 1);
  }

  # Return the dir name
  return $tempdir;

}

#line 1605



sub mkstemp {

  croak "Usage: mkstemp(template)"
    if scalar(@_) != 1;

  my $template = shift;

  my ($fh, $path, $errstr);
  croak "Error in mkstemp using $template: $errstr"
    unless (($fh, $path) = _gettemp($template,
                                    "open" => 1,
                                    "mkdir"=> 0 ,
                                    "suffixlen" => 0,
                                    "ErrStr" => \$errstr,
                                   ) );

  if (wantarray()) {
    return ($fh, $path);
  } else {
    return $fh;
  }

}


#line 1648

sub mkstemps {

  croak "Usage: mkstemps(template, suffix)"
    if scalar(@_) != 2;


  my $template = shift;
  my $suffix   = shift;

  $template .= $suffix;

  my ($fh, $path, $errstr);
  croak "Error in mkstemps using $template: $errstr"
    unless (($fh, $path) = _gettemp($template,
                                    "open" => 1,
                                    "mkdir"=> 0 ,
                                    "suffixlen" => length($suffix),
                                    "ErrStr" => \$errstr,
                                   ) );

  if (wantarray()) {
    return ($fh, $path);
  } else {
    return $fh;
  }

}

#line 1691

#' # for emacs

sub mkdtemp {

  croak "Usage: mkdtemp(template)"
    if scalar(@_) != 1;

  my $template = shift;
  my $suffixlen = 0;
  if ($^O eq 'VMS') {           # dir names can end in delimiters
    $template =~ m/([\.\]:>]+)$/;
    $suffixlen = length($1);
  }
  if ( ($^O eq 'MacOS') && (substr($template, -1) eq ':') ) {
    # dir name has a trailing ':'
    ++$suffixlen;
  }
  my ($junk, $tmpdir, $errstr);
  croak "Error creating temp directory from template $template\: $errstr"
    unless (($junk, $tmpdir) = _gettemp($template,
                                        "open" => 0,
                                        "mkdir"=> 1 ,
                                        "suffixlen" => $suffixlen,
                                        "ErrStr" => \$errstr,
                                       ) );

  return $tmpdir;

}

#line 1734

sub mktemp {

  croak "Usage: mktemp(template)"
    if scalar(@_) != 1;

  my $template = shift;

  my ($tmpname, $junk, $errstr);
  croak "Error getting name to temp file from template $template: $errstr"
    unless (($junk, $tmpname) = _gettemp($template,
                                         "open" => 0,
                                         "mkdir"=> 0 ,
                                         "suffixlen" => 0,
                                         "ErrStr" => \$errstr,
                                        ) );

  return $tmpname;
}

#line 1796

sub tmpnam {

  # Retrieve the temporary directory name
  my $tmpdir = File::Spec->tmpdir;

  croak "Error temporary directory is not writable"
    if $tmpdir eq '';

  # Use a ten character template and append to tmpdir
  my $template = File::Spec->catfile($tmpdir, TEMPXXX);

  if (wantarray() ) {
    return mkstemp($template);
  } else {
    return mktemp($template);
  }

}

#line 1832

sub tmpfile {

  # Simply call tmpnam() in a list context
  my ($fh, $file) = tmpnam();

  # Make sure file is removed when filehandle is closed
  # This will fail on NFS
  unlink0($fh, $file)
    or return undef;

  return $fh;

}

#line 1877

sub tempnam {

  croak 'Usage tempnam($dir, $prefix)' unless scalar(@_) == 2;

  my ($dir, $prefix) = @_;

  # Add a string to the prefix
  $prefix .= 'XXXXXXXX';

  # Concatenate the directory to the file
  my $template = File::Spec->catfile($dir, $prefix);

  return mktemp($template);

}

#line 1949

sub unlink0 {

  croak 'Usage: unlink0(filehandle, filename)'
    unless scalar(@_) == 2;

  # Read args
  my ($fh, $path) = @_;

  cmpstat($fh, $path) or return 0;

  # attempt remove the file (does not work on some platforms)
  if (_can_unlink_opened_file()) {

    # return early (Without unlink) if we have been instructed to retain files.
    return 1 if $KEEP_ALL;

    # XXX: do *not* call this on a directory; possible race
    #      resulting in recursive removal
    croak "unlink0: $path has become a directory!" if -d $path;
    unlink($path) or return 0;

    # Stat the filehandle
    my @fh = stat $fh;

    print "Link count = $fh[3] \n" if $DEBUG;

    # Make sure that the link count is zero
    # - Cygwin provides deferred unlinking, however,
    #   on Win9x the link count remains 1
    # On NFS the link count may still be 1 but we cant know that
    # we are on NFS
    return ( $fh[3] == 0 or $^O eq 'cygwin' ? 1 : 0);

  } else {
    _deferred_unlink($fh, $path, 0);
    return 1;
  }

}

#line 2014

sub cmpstat {

  croak 'Usage: cmpstat(filehandle, filename)'
    unless scalar(@_) == 2;

  # Read args
  my ($fh, $path) = @_;

  warn "Comparing stat\n"
    if $DEBUG;

  # Stat the filehandle - which may be closed if someone has manually
  # closed the file. Can not turn off warnings without using $^W
  # unless we upgrade to 5.006 minimum requirement
  my @fh;
  {
    local ($^W) = 0;
    @fh = stat $fh;
  }
  return unless @fh;

  if ($fh[3] > 1 && $^W) {
    carp "unlink0: fstat found too many links; SB=@fh" if $^W;
  }

  # Stat the path
  my @path = stat $path;

  unless (@path) {
    carp "unlink0: $path is gone already" if $^W;
    return;
  }

  # this is no longer a file, but may be a directory, or worse
  unless (-f $path) {
    confess "panic: $path is no longer a file: SB=@fh";
  }

  # Do comparison of each member of the array
  # On WinNT dev and rdev seem to be different
  # depending on whether it is a file or a handle.
  # Cannot simply compare all members of the stat return
  # Select the ones we can use
  my @okstat = (0..$#fh);       # Use all by default
  if ($^O eq 'MSWin32') {
    @okstat = (1,2,3,4,5,7,8,9,10);
  } elsif ($^O eq 'os2') {
    @okstat = (0, 2..$#fh);
  } elsif ($^O eq 'VMS') {      # device and file ID are sufficient
    @okstat = (0, 1);
  } elsif ($^O eq 'dos') {
    @okstat = (0,2..7,11..$#fh);
  } elsif ($^O eq 'mpeix') {
    @okstat = (0..4,8..10);
  }

  # Now compare each entry explicitly by number
  for (@okstat) {
    print "Comparing: $_ : $fh[$_] and $path[$_]\n" if $DEBUG;
    # Use eq rather than == since rdev, blksize, and blocks (6, 11,
    # and 12) will be '' on platforms that do not support them.  This
    # is fine since we are only comparing integers.
    unless ($fh[$_] eq $path[$_]) {
      warn "Did not match $_ element of stat\n" if $DEBUG;
      return 0;
    }
  }

  return 1;
}

#line 2107

sub unlink1 {
  croak 'Usage: unlink1(filehandle, filename)'
    unless scalar(@_) == 2;

  # Read args
  my ($fh, $path) = @_;

  cmpstat($fh, $path) or return 0;

  # Close the file
  close( $fh ) or return 0;

  # Make sure the file is writable (for windows)
  _force_writable( $path );

  # return early (without unlink) if we have been instructed to retain files.
  return 1 if $KEEP_ALL;

  # remove the file
  return unlink($path);
}

#line 2222

{
  # protect from using the variable itself
  my $LEVEL = STANDARD;
  sub safe_level {
    my $self = shift;
    if (@_) {
      my $level = shift;
      if (($level != STANDARD) && ($level != MEDIUM) && ($level != HIGH)) {
        carp "safe_level: Specified level ($level) not STANDARD, MEDIUM or HIGH - ignoring\n" if $^W;
      } else {
        # Dont allow this on perl 5.005 or earlier
        if ($] < 5.006 && $level != STANDARD) {
          # Cant do MEDIUM or HIGH checks
          croak "Currently requires perl 5.006 or newer to do the safe checks";
        }
        # Check that we are allowed to change level
        # Silently ignore if we can not.
        $LEVEL = $level if _can_do_level($level);
      }
    }
    return $LEVEL;
  }
}

#line 2267

{
  my $TopSystemUID = 10;
  $TopSystemUID = 197108 if $^O eq 'interix'; # "Administrator"
  sub top_system_uid {
    my $self = shift;
    if (@_) {
      my $newuid = shift;
      croak "top_system_uid: UIDs should be numeric"
        unless $newuid =~ /^\d+$/s;
      $TopSystemUID = $newuid;
    }
    return $TopSystemUID;
  }
}

#line 2402

package File::Temp::Dir;

use File::Path qw/ rmtree /;
use strict;
use overload '""' => "STRINGIFY", fallback => 1;

# private class specifically to support tempdir objects
# created by File::Temp->newdir

# ostensibly the same method interface as File::Temp but without
# inheriting all the IO::Seekable methods and other cruft

# Read-only - returns the name of the temp directory

sub dirname {
  my $self = shift;
  return $self->{DIRNAME};
}

sub STRINGIFY {
  my $self = shift;
  return $self->dirname;
}

sub unlink_on_destroy {
  my $self = shift;
  if (@_) {
    $self->{CLEANUP} = shift;
  }
  return $self->{CLEANUP};
}

sub DESTROY {
  my $self = shift;
  local($., $@, $!, $^E, $?);
  if ($self->unlink_on_destroy && 
      $$ == $self->{LAUNCHPID} && !$File::Temp::KEEP_ALL) {
    if (-d $self->{DIRNAME}) {
      # Some versions of rmtree will abort if you attempt to remove
      # the directory you are sitting in. We protect that and turn it
      # into a warning. We do this because this occurs during object
      # destruction and so can not be caught by the user.
      eval { rmtree($self->{DIRNAME}, $File::Temp::DEBUG, 0); };
      warn $@ if ($@ && $^W);
    }
  }
}


1;
FILE   5f93d474/FileHandle.pm  o#line 1 "/usr/lib/perl5/5.14/FileHandle.pm"
package FileHandle;

use 5.006;
use strict;
our($VERSION, @ISA, @EXPORT, @EXPORT_OK);

$VERSION = "2.02";

require IO::File;
@ISA = qw(IO::File);

@EXPORT = qw(_IOFBF _IOLBF _IONBF);

@EXPORT_OK = qw(
    pipe

    autoflush
    output_field_separator
    output_record_separator
    input_record_separator
    input_line_number
    format_page_number
    format_lines_per_page
    format_lines_left
    format_name
    format_top_name
    format_line_break_characters
    format_formfeed

    print
    printf
    getline
    getlines
);

#
# Everything we're willing to export, we must first import.
#
import IO::Handle grep { !defined(&$_) } @EXPORT, @EXPORT_OK;

#
# Some people call "FileHandle::function", so all the functions
# that were in the old FileHandle class must be imported, too.
#
{
    no strict 'refs';

    my %import = (
	'IO::Handle' =>
	    [qw(DESTROY new_from_fd fdopen close fileno getc ungetc gets
		eof flush error clearerr setbuf setvbuf _open_mode_string)],
	'IO::Seekable' =>
	    [qw(seek tell getpos setpos)],
	'IO::File' =>
	    [qw(new new_tmpfile open)]
    );
    for my $pkg (keys %import) {
	for my $func (@{$import{$pkg}}) {
	    my $c = *{"${pkg}::$func"}{CODE}
		or die "${pkg}::$func missing";
	    *$func = $c;
	}
    }
}

#
# Specialized importer for Fcntl magic.
#
sub import {
    my $pkg = shift;
    my $callpkg = caller;
    require Exporter;
    Exporter::export($pkg, $callpkg, @_);

    #
    # If the Fcntl extension is available,
    #  export its constants.
    #
    eval {
	require Fcntl;
	Exporter::export('Fcntl', $callpkg);
    };
}

################################################
# This is the only exported function we define;
# the rest come from other classes.
#

sub pipe {
    my $r = new IO::Handle;
    my $w = new IO::Handle;
    CORE::pipe($r, $w) or return undef;
    ($r, $w);
}

# Rebless standard file handles
bless *STDIN{IO},  "FileHandle" if ref *STDIN{IO}  eq "IO::Handle";
bless *STDOUT{IO}, "FileHandle" if ref *STDOUT{IO} eq "IO::Handle";
bless *STDERR{IO}, "FileHandle" if ref *STDERR{IO} eq "IO::Handle";

1;

__END__

#line 263
FILE   '4a72f0d1/IO/Compress/Adapter/Deflate.pm  :#line 1 "/usr/lib/perl5/5.14/IO/Compress/Adapter/Deflate.pm"
package IO::Compress::Adapter::Deflate ;

use strict;
use warnings;
use bytes;

use IO::Compress::Base::Common  2.033 qw(:Status);

use Compress::Raw::Zlib  2.033 qw(Z_OK Z_FINISH MAX_WBITS) ;
our ($VERSION);

$VERSION = '2.033';

sub mkCompObject
{
    my $crc32    = shift ;
    my $adler32  = shift ;
    my $level    = shift ;
    my $strategy = shift ;

    my ($def, $status) = new Compress::Raw::Zlib::Deflate
                                -AppendOutput   => 1,
                                -CRC32          => $crc32,
                                -ADLER32        => $adler32,
                                -Level          => $level,
                                -Strategy       => $strategy,
                                -WindowBits     => - MAX_WBITS;

    return (undef, "Cannot create Deflate object: $status", $status) 
        if $status != Z_OK;    

    return bless {'Def'        => $def,
                  'Error'      => '',
                 } ;     
}

sub compr
{
    my $self = shift ;

    my $def   = $self->{Def};

    my $status = $def->deflate($_[0], $_[1]) ;
    $self->{ErrorNo} = $status;

    if ($status != Z_OK)
    {
        $self->{Error} = "Deflate Error: $status"; 
        return STATUS_ERROR;
    }

    return STATUS_OK;    
}

sub flush
{
    my $self = shift ;

    my $def   = $self->{Def};

    my $opt = $_[1] || Z_FINISH;
    my $status = $def->flush($_[0], $opt);
    $self->{ErrorNo} = $status;

    if ($status != Z_OK)
    {
        $self->{Error} = "Deflate Error: $status"; 
        return STATUS_ERROR;
    }

    return STATUS_OK;    
    
}

sub close
{
    my $self = shift ;

    my $def   = $self->{Def};

    $def->flush($_[0], Z_FINISH)
        if defined $def ;
}

sub reset
{
    my $self = shift ;

    my $def   = $self->{Def};

    my $status = $def->deflateReset() ;
    $self->{ErrorNo} = $status;
    if ($status != Z_OK)
    {
        $self->{Error} = "Deflate Error: $status"; 
        return STATUS_ERROR;
    }

    return STATUS_OK;    
}

sub deflateParams 
{
    my $self = shift ;

    my $def   = $self->{Def};

    my $status = $def->deflateParams(@_);
    $self->{ErrorNo} = $status;
    if ($status != Z_OK)
    {
        $self->{Error} = "deflateParams Error: $status"; 
        return STATUS_ERROR;
    }

    return STATUS_OK;   
}



#sub total_out
#{
#    my $self = shift ;
#    $self->{Def}->total_out();
#}
#
#sub total_in
#{
#    my $self = shift ;
#    $self->{Def}->total_in();
#}

sub compressedBytes
{
    my $self = shift ;

    $self->{Def}->compressedBytes();
}

sub uncompressedBytes
{
    my $self = shift ;
    $self->{Def}->uncompressedBytes();
}




sub crc32
{
    my $self = shift ;
    $self->{Def}->crc32();
}

sub adler32
{
    my $self = shift ;
    $self->{Def}->adler32();
}


1;

__END__

FILE   5e835e0f/IO/Compress/Base.pm  QB#line 1 "/usr/lib/perl5/5.14/IO/Compress/Base.pm"

package IO::Compress::Base ;

require 5.004 ;

use strict ;
use warnings;

use IO::Compress::Base::Common 2.033 ;

use IO::File ;
use Scalar::Util qw(blessed readonly);

#use File::Glob;
#require Exporter ;
use Carp ;
use Symbol;
use bytes;

our (@ISA, $VERSION);
@ISA    = qw(Exporter IO::File);

$VERSION = '2.033';

#Can't locate object method "SWASHNEW" via package "utf8" (perhaps you forgot to load "utf8"?) at .../ext/Compress-Zlib/Gzip/blib/lib/Compress/Zlib/Common.pm line 16.

sub saveStatus
{
    my $self   = shift ;
    ${ *$self->{ErrorNo} } = shift() + 0 ;
    ${ *$self->{Error} } = '' ;

    return ${ *$self->{ErrorNo} } ;
}


sub saveErrorString
{
    my $self   = shift ;
    my $retval = shift ;
    ${ *$self->{Error} } = shift ;
    ${ *$self->{ErrorNo} } = shift() + 0 if @_ ;

    return $retval;
}

sub croakError
{
    my $self   = shift ;
    $self->saveErrorString(0, $_[0]);
    croak $_[0];
}

sub closeError
{
    my $self = shift ;
    my $retval = shift ;

    my $errno = *$self->{ErrorNo};
    my $error = ${ *$self->{Error} };

    $self->close();

    *$self->{ErrorNo} = $errno ;
    ${ *$self->{Error} } = $error ;

    return $retval;
}



sub error
{
    my $self   = shift ;
    return ${ *$self->{Error} } ;
}

sub errorNo
{
    my $self   = shift ;
    return ${ *$self->{ErrorNo} } ;
}


sub writeAt
{
    my $self = shift ;
    my $offset = shift;
    my $data = shift;

    if (defined *$self->{FH}) {
        my $here = tell(*$self->{FH});
        return $self->saveErrorString(undef, "Cannot seek to end of output filehandle: $!", $!) 
            if $here < 0 ;
        seek(*$self->{FH}, $offset, SEEK_SET)
            or return $self->saveErrorString(undef, "Cannot seek to end of output filehandle: $!", $!) ;
        defined *$self->{FH}->write($data, length $data)
            or return $self->saveErrorString(undef, $!, $!) ;
        seek(*$self->{FH}, $here, SEEK_SET)
            or return $self->saveErrorString(undef, "Cannot seek to end of output filehandle: $!", $!) ;
    }
    else {
        substr(${ *$self->{Buffer} }, $offset, length($data)) = $data ;
    }

    return 1;
}

sub output
{
    my $self = shift ;
    my $data = shift ;
    my $last = shift ;

    return 1 
        if length $data == 0 && ! $last ;

    if ( *$self->{FilterEnvelope} ) {
        *_ = \$data;
        &{ *$self->{FilterEnvelope} }();
    }

    if (length $data) {
        if ( defined *$self->{FH} ) {
                defined *$self->{FH}->write( $data, length $data )
                or return $self->saveErrorString(0, $!, $!); 
        }
        else {
                ${ *$self->{Buffer} } .= $data ;
        }
    }

    return 1;
}

sub getOneShotParams
{
    return ( 'MultiStream' => [1, 1, Parse_boolean,   1],
           );
}

sub checkParams
{
    my $self = shift ;
    my $class = shift ;

    my $got = shift || IO::Compress::Base::Parameters::new();

    $got->parse(
        {
            # Generic Parameters
            'AutoClose' => [1, 1, Parse_boolean,   0],
            #'Encode'    => [1, 1, Parse_any,       undef],
            'Strict'    => [0, 1, Parse_boolean,   1],
            'Append'    => [1, 1, Parse_boolean,   0],
            'BinModeIn' => [1, 1, Parse_boolean,   0],

            'FilterEnvelope' => [1, 1, Parse_any,   undef],

            $self->getExtraParams(),
            *$self->{OneShot} ? $self->getOneShotParams() 
                              : (),
        }, 
        @_) or $self->croakError("${class}: $got->{Error}")  ;

    return $got ;
}

sub _create
{
    my $obj = shift;
    my $got = shift;

    *$obj->{Closed} = 1 ;

    my $class = ref $obj;
    $obj->croakError("$class: Missing Output parameter")
        if ! @_ && ! $got ;

    my $outValue = shift ;
    my $oneShot = 1 ;

    if (! $got)
    {
        $oneShot = 0 ;
        $got = $obj->checkParams($class, undef, @_)
            or return undef ;
    }

    my $lax = ! $got->value('Strict') ;

    my $outType = whatIsOutput($outValue);

    $obj->ckOutputParam($class, $outValue)
        or return undef ;

    if ($outType eq 'buffer') {
        *$obj->{Buffer} = $outValue;
    }
    else {
        my $buff = "" ;
        *$obj->{Buffer} = \$buff ;
    }

    # Merge implies Append
    my $merge = $got->value('Merge') ;
    my $appendOutput = $got->value('Append') || $merge ;
    *$obj->{Append} = $appendOutput;
    *$obj->{FilterEnvelope} = $got->value('FilterEnvelope') ;

    if ($merge)
    {
        # Switch off Merge mode if output file/buffer is empty/doesn't exist
        if (($outType eq 'buffer' && length $$outValue == 0 ) ||
            ($outType ne 'buffer' && (! -e $outValue || (-w _ && -z _))) )
          { $merge = 0 }
    }

    # If output is a file, check that it is writable
    #no warnings;
    #if ($outType eq 'filename' && -e $outValue && ! -w _)
    #  { return $obj->saveErrorString(undef, "Output file '$outValue' is not writable" ) }



    if ($got->parsed('Encode')) { 
        my $want_encoding = $got->value('Encode');
        *$obj->{Encoding} = getEncoding($obj, $class, $want_encoding);
    }

    $obj->ckParams($got)
        or $obj->croakError("${class}: " . $obj->error());


    $obj->saveStatus(STATUS_OK) ;

    my $status ;
    if (! $merge)
    {
        *$obj->{Compress} = $obj->mkComp($got)
            or return undef;
        
        *$obj->{UnCompSize} = new U64 ;
        *$obj->{CompSize} = new U64 ;

        if ( $outType eq 'buffer') {
            ${ *$obj->{Buffer} }  = ''
                unless $appendOutput ;
        }
        else {
            if ($outType eq 'handle') {
                *$obj->{FH} = $outValue ;
                setBinModeOutput(*$obj->{FH}) ;
                $outValue->flush() ;
                *$obj->{Handle} = 1 ;
                if ($appendOutput)
                {
                    seek(*$obj->{FH}, 0, SEEK_END)
                        or return $obj->saveErrorString(undef, "Cannot seek to end of output filehandle: $!", $!) ;

                }
            }
            elsif ($outType eq 'filename') {    
                no warnings;
                my $mode = '>' ;
                $mode = '>>'
                    if $appendOutput;
                *$obj->{FH} = new IO::File "$mode $outValue" 
                    or return $obj->saveErrorString(undef, "cannot open file '$outValue': $!", $!) ;
                *$obj->{StdIO} = ($outValue eq '-'); 
                setBinModeOutput(*$obj->{FH}) ;
            }
        }

        *$obj->{Header} = $obj->mkHeader($got) ;
        $obj->output( *$obj->{Header} )
            or return undef;
    }
    else
    {
        *$obj->{Compress} = $obj->createMerge($outValue, $outType)
            or return undef;
    }

    *$obj->{Closed} = 0 ;
    *$obj->{AutoClose} = $got->value('AutoClose') ;
    *$obj->{Output} = $outValue;
    *$obj->{ClassName} = $class;
    *$obj->{Got} = $got;
    *$obj->{OneShot} = 0 ;

    return $obj ;
}

sub ckOutputParam 
{
    my $self = shift ;
    my $from = shift ;
    my $outType = whatIsOutput($_[0]);

    $self->croakError("$from: output parameter not a filename, filehandle or scalar ref")
        if ! $outType ;

    #$self->croakError("$from: output filename is undef or null string")
        #if $outType eq 'filename' && (! defined $_[0] || $_[0] eq '')  ;

    $self->croakError("$from: output buffer is read-only")
        if $outType eq 'buffer' && readonly(${ $_[0] });
    
    return 1;    
}


sub _def
{
    my $obj = shift ;
    
    my $class= (caller)[0] ;
    my $name = (caller(1))[3] ;

    $obj->croakError("$name: expected at least 1 parameters\n")
        unless @_ >= 1 ;

    my $input = shift ;
    my $haveOut = @_ ;
    my $output = shift ;

    my $x = new IO::Compress::Base::Validator($class, *$obj->{Error}, $name, $input, $output)
        or return undef ;

    push @_, $output if $haveOut && $x->{Hash};

    *$obj->{OneShot} = 1 ;

    my $got = $obj->checkParams($name, undef, @_)
        or return undef ;

    $x->{Got} = $got ;

#    if ($x->{Hash})
#    {
#        while (my($k, $v) = each %$input)
#        {
#            $v = \$input->{$k} 
#                unless defined $v ;
#
#            $obj->_singleTarget($x, 1, $k, $v, @_)
#                or return undef ;
#        }
#
#        return keys %$input ;
#    }

    if ($x->{GlobMap})
    {
        $x->{oneInput} = 1 ;
        foreach my $pair (@{ $x->{Pairs} })
        {
            my ($from, $to) = @$pair ;
            $obj->_singleTarget($x, 1, $from, $to, @_)
                or return undef ;
        }

        return scalar @{ $x->{Pairs} } ;
    }

    if (! $x->{oneOutput} )
    {
        my $inFile = ($x->{inType} eq 'filenames' 
                        || $x->{inType} eq 'filename');

        $x->{inType} = $inFile ? 'filename' : 'buffer';
        
        foreach my $in ($x->{oneInput} ? $input : @$input)
        {
            my $out ;
            $x->{oneInput} = 1 ;

            $obj->_singleTarget($x, $inFile, $in, \$out, @_)
                or return undef ;

            push @$output, \$out ;
            #if ($x->{outType} eq 'array')
            #  { push @$output, \$out }
            #else
            #  { $output->{$in} = \$out }
        }

        return 1 ;
    }

    # finally the 1 to 1 and n to 1
    return $obj->_singleTarget($x, 1, $input, $output, @_);

    croak "should not be here" ;
}

sub _singleTarget
{
    my $obj             = shift ;
    my $x               = shift ;
    my $inputIsFilename = shift;
    my $input           = shift;
    
    if ($x->{oneInput})
    {
        $obj->getFileInfo($x->{Got}, $input)
            if isaFilename($input) and $inputIsFilename ;

        my $z = $obj->_create($x->{Got}, @_)
            or return undef ;


        defined $z->_wr2($input, $inputIsFilename) 
            or return $z->closeError(undef) ;

        return $z->close() ;
    }
    else
    {
        my $afterFirst = 0 ;
        my $inputIsFilename = ($x->{inType} ne 'array');
        my $keep = $x->{Got}->clone();

        #for my $element ( ($x->{inType} eq 'hash') ? keys %$input : @$input)
        for my $element ( @$input)
        {
            my $isFilename = isaFilename($element);

            if ( $afterFirst ++ )
            {
                defined addInterStream($obj, $element, $isFilename)
                    or return $obj->closeError(undef) ;
            }
            else
            {
                $obj->getFileInfo($x->{Got}, $element)
                    if $isFilename;

                $obj->_create($x->{Got}, @_)
                    or return undef ;
            }

            defined $obj->_wr2($element, $isFilename) 
                or return $obj->closeError(undef) ;

            *$obj->{Got} = $keep->clone();
        }
        return $obj->close() ;
    }

}

sub _wr2
{
    my $self = shift ;

    my $source = shift ;
    my $inputIsFilename = shift;

    my $input = $source ;
    if (! $inputIsFilename)
    {
        $input = \$source 
            if ! ref $source;
    }

    if ( ref $input && ref $input eq 'SCALAR' )
    {
        return $self->syswrite($input, @_) ;
    }

    if ( ! ref $input  || isaFilehandle($input))
    {
        my $isFilehandle = isaFilehandle($input) ;

        my $fh = $input ;

        if ( ! $isFilehandle )
        {
            $fh = new IO::File "<$input"
                or return $self->saveErrorString(undef, "cannot open file '$input': $!", $!) ;
        }
        binmode $fh if *$self->{Got}->valueOrDefault('BinModeIn') ;

        my $status ;
        my $buff ;
        my $count = 0 ;
        while ($status = read($fh, $buff, 16 * 1024)) {
            $count += length $buff;
            defined $self->syswrite($buff, @_) 
                or return undef ;
        }

        return $self->saveErrorString(undef, $!, $!) 
            if ! defined $status ;

        if ( (!$isFilehandle || *$self->{AutoClose}) && $input ne '-')
        {    
            $fh->close() 
                or return undef ;
        }

        return $count ;
    }

    croak "Should not be here";
    return undef;
}

sub addInterStream
{
    my $self = shift ;
    my $input = shift ;
    my $inputIsFilename = shift ;

    if (*$self->{Got}->value('MultiStream'))
    {
        $self->getFileInfo(*$self->{Got}, $input)
            #if isaFilename($input) and $inputIsFilename ;
            if isaFilename($input) ;

        # TODO -- newStream needs to allow gzip/zip header to be modified
        return $self->newStream();
    }
    elsif (*$self->{Got}->value('AutoFlush'))
    {
        #return $self->flush(Z_FULL_FLUSH);
    }

    return 1 ;
}

sub getFileInfo
{
}

sub TIEHANDLE
{
    return $_[0] if ref($_[0]);
    die "OOPS\n" ;
}
  
sub UNTIE
{
    my $self = shift ;
}

sub DESTROY
{
    my $self = shift ;
    local ($., $@, $!, $^E, $?);
    
    $self->close() ;

    # TODO - memory leak with 5.8.0 - this isn't called until 
    #        global destruction
    #
    %{ *$self } = () ;
    undef $self ;
}



sub filterUncompressed
{
}

sub syswrite
{
    my $self = shift ;

    my $buffer ;
    if (ref $_[0] ) {
        $self->croakError( *$self->{ClassName} . "::write: not a scalar reference" )
            unless ref $_[0] eq 'SCALAR' ;
        $buffer = $_[0] ;
    }
    else {
        $buffer = \$_[0] ;
    }

    $] >= 5.008 and ( utf8::downgrade($$buffer, 1) 
        or croak "Wide character in " .  *$self->{ClassName} . "::write:");


    if (@_ > 1) {
        my $slen = defined $$buffer ? length($$buffer) : 0;
        my $len = $slen;
        my $offset = 0;
        $len = $_[1] if $_[1] < $len;

        if (@_ > 2) {
            $offset = $_[2] || 0;
            $self->croakError(*$self->{ClassName} . "::write: offset outside string") 
                if $offset > $slen;
            if ($offset < 0) {
                $offset += $slen;
                $self->croakError( *$self->{ClassName} . "::write: offset outside string") if $offset < 0;
            }
            my $rem = $slen - $offset;
            $len = $rem if $rem < $len;
        }

        $buffer = \substr($$buffer, $offset, $len) ;
    }

    return 0 if ! defined $$buffer || length $$buffer == 0 ;

    if (*$self->{Encoding}) {
        $$buffer = *$self->{Encoding}->encode($$buffer);
    }

    $self->filterUncompressed($buffer);

    my $buffer_length = defined $$buffer ? length($$buffer) : 0 ;
    *$self->{UnCompSize}->add($buffer_length) ;

    my $outBuffer='';
    my $status = *$self->{Compress}->compr($buffer, $outBuffer) ;

    return $self->saveErrorString(undef, *$self->{Compress}{Error}, 
                                         *$self->{Compress}{ErrorNo})
        if $status == STATUS_ERROR;

    *$self->{CompSize}->add(length $outBuffer) ;

    $self->output($outBuffer)
        or return undef;

    return $buffer_length;
}

sub print
{
    my $self = shift;

    #if (ref $self) {
    #    $self = *$self{GLOB} ;
    #}

    if (defined $\) {
        if (defined $,) {
            defined $self->syswrite(join($,, @_) . $\);
        } else {
            defined $self->syswrite(join("", @_) . $\);
        }
    } else {
        if (defined $,) {
            defined $self->syswrite(join($,, @_));
        } else {
            defined $self->syswrite(join("", @_));
        }
    }
}

sub printf
{
    my $self = shift;
    my $fmt = shift;
    defined $self->syswrite(sprintf($fmt, @_));
}



sub flush
{
    my $self = shift ;

    my $outBuffer='';
    my $status = *$self->{Compress}->flush($outBuffer, @_) ;
    return $self->saveErrorString(0, *$self->{Compress}{Error}, 
                                    *$self->{Compress}{ErrorNo})
        if $status == STATUS_ERROR;

    if ( defined *$self->{FH} ) {
        *$self->{FH}->clearerr();
    }

    *$self->{CompSize}->add(length $outBuffer) ;

    $self->output($outBuffer)
        or return 0;

    if ( defined *$self->{FH} ) {
        defined *$self->{FH}->flush()
            or return $self->saveErrorString(0, $!, $!); 
    }

    return 1;
}

sub newStream
{
    my $self = shift ;
  
    $self->_writeTrailer()
        or return 0 ;

    my $got = $self->checkParams('newStream', *$self->{Got}, @_)
        or return 0 ;    

    $self->ckParams($got)
        or $self->croakError("newStream: $self->{Error}");

    *$self->{Compress} = $self->mkComp($got)
        or return 0;

    *$self->{Header} = $self->mkHeader($got) ;
    $self->output(*$self->{Header} )
        or return 0;
    
    *$self->{UnCompSize}->reset();
    *$self->{CompSize}->reset();

    return 1 ;
}

sub reset
{
    my $self = shift ;
    return *$self->{Compress}->reset() ;
}

sub _writeTrailer
{
    my $self = shift ;

    my $trailer = '';

    my $status = *$self->{Compress}->close($trailer) ;
    return $self->saveErrorString(0, *$self->{Compress}{Error}, *$self->{Compress}{ErrorNo})
        if $status == STATUS_ERROR;

    *$self->{CompSize}->add(length $trailer) ;

    $trailer .= $self->mkTrailer();
    defined $trailer
      or return 0;

    return $self->output($trailer);
}

sub _writeFinalTrailer
{
    my $self = shift ;

    return $self->output($self->mkFinalTrailer());
}

sub close
{
    my $self = shift ;

    return 1 if *$self->{Closed} || ! *$self->{Compress} ;
    *$self->{Closed} = 1 ;

    untie *$self 
        if $] >= 5.008 ;

    $self->_writeTrailer()
        or return 0 ;

    $self->_writeFinalTrailer()
        or return 0 ;

    $self->output( "", 1 )
        or return 0;

    if (defined *$self->{FH}) {

        #if (! *$self->{Handle} || *$self->{AutoClose}) {
        if ((! *$self->{Handle} || *$self->{AutoClose}) && ! *$self->{StdIO}) {
            $! = 0 ;
            *$self->{FH}->close()
                or return $self->saveErrorString(0, $!, $!); 
        }
        delete *$self->{FH} ;
        # This delete can set $! in older Perls, so reset the errno
        $! = 0 ;
    }

    return 1;
}


#sub total_in
#sub total_out
#sub msg
#
#sub crc
#{
#    my $self = shift ;
#    return *$self->{Compress}->crc32() ;
#}
#
#sub msg
#{
#    my $self = shift ;
#    return *$self->{Compress}->msg() ;
#}
#
#sub dict_adler
#{
#    my $self = shift ;
#    return *$self->{Compress}->dict_adler() ;
#}
#
#sub get_Level
#{
#    my $self = shift ;
#    return *$self->{Compress}->get_Level() ;
#}
#
#sub get_Strategy
#{
#    my $self = shift ;
#    return *$self->{Compress}->get_Strategy() ;
#}


sub tell
{
    my $self = shift ;

    return *$self->{UnCompSize}->get32bit() ;
}

sub eof
{
    my $self = shift ;

    return *$self->{Closed} ;
}


sub seek
{
    my $self     = shift ;
    my $position = shift;
    my $whence   = shift ;

    my $here = $self->tell() ;
    my $target = 0 ;

    #use IO::Handle qw(SEEK_SET SEEK_CUR SEEK_END);
    use IO::Handle ;

    if ($whence == IO::Handle::SEEK_SET) {
        $target = $position ;
    }
    elsif ($whence == IO::Handle::SEEK_CUR || $whence == IO::Handle::SEEK_END) {
        $target = $here + $position ;
    }
    else {
        $self->croakError(*$self->{ClassName} . "::seek: unknown value, $whence, for whence parameter");
    }

    # short circuit if seeking to current offset
    return 1 if $target == $here ;    

    # Outlaw any attempt to seek backwards
    $self->croakError(*$self->{ClassName} . "::seek: cannot seek backwards")
        if $target < $here ;

    # Walk the file to the new offset
    my $offset = $target - $here ;

    my $buffer ;
    defined $self->syswrite("\x00" x $offset)
        or return 0;

    return 1 ;
}

sub binmode
{
    1;
#    my $self     = shift ;
#    return defined *$self->{FH} 
#            ? binmode *$self->{FH} 
#            : 1 ;
}

sub fileno
{
    my $self     = shift ;
    return defined *$self->{FH} 
            ? *$self->{FH}->fileno() 
            : undef ;
}

sub opened
{
    my $self     = shift ;
    return ! *$self->{Closed} ;
}

sub autoflush
{
    my $self     = shift ;
    return defined *$self->{FH} 
            ? *$self->{FH}->autoflush(@_) 
            : undef ;
}

sub input_line_number
{
    return undef ;
}


sub _notAvailable
{
    my $name = shift ;
    return sub { croak "$name Not Available: File opened only for output" ; } ;
}

*read     = _notAvailable('read');
*READ     = _notAvailable('read');
*readline = _notAvailable('readline');
*READLINE = _notAvailable('readline');
*getc     = _notAvailable('getc');
*GETC     = _notAvailable('getc');

*FILENO   = \&fileno;
*PRINT    = \&print;
*PRINTF   = \&printf;
*WRITE    = \&syswrite;
*write    = \&syswrite;
*SEEK     = \&seek; 
*TELL     = \&tell;
*EOF      = \&eof;
*CLOSE    = \&close;
*BINMODE  = \&binmode;

#*sysread  = \&_notAvailable;
#*syswrite = \&_write;

1; 

__END__

#line 982
FILE   #f6572ee4/IO/Compress/Base/Common.pm  T�#line 1 "/usr/lib/perl5/5.14/IO/Compress/Base/Common.pm"
package IO::Compress::Base::Common;

use strict ;
use warnings;
use bytes;

use Carp;
use Scalar::Util qw(blessed readonly);
use File::GlobMapper;

require Exporter;
our ($VERSION, @ISA, @EXPORT, %EXPORT_TAGS, $HAS_ENCODE);
@ISA = qw(Exporter);
$VERSION = '2.033';

@EXPORT = qw( isaFilehandle isaFilename whatIsInput whatIsOutput 
              isaFileGlobString cleanFileGlobString oneTarget
              setBinModeInput setBinModeOutput
              ckInOutParams 
              createSelfTiedObject
              getEncoding

              WANT_CODE
              WANT_EXT
              WANT_UNDEF
              WANT_HASH

              STATUS_OK
              STATUS_ENDSTREAM
              STATUS_EOF
              STATUS_ERROR
          );  

%EXPORT_TAGS = ( Status => [qw( STATUS_OK
                                 STATUS_ENDSTREAM
                                 STATUS_EOF
                                 STATUS_ERROR
                           )]);

                       
use constant STATUS_OK        => 0;
use constant STATUS_ENDSTREAM => 1;
use constant STATUS_EOF       => 2;
use constant STATUS_ERROR     => -1;
          
sub hasEncode()
{
    if (! defined $HAS_ENCODE) {
        eval
        {
            require Encode;
            Encode->import();
        };

        $HAS_ENCODE = $@ ? 0 : 1 ;
    }

    return $HAS_ENCODE;
}

sub getEncoding($$$)
{
    my $obj = shift;
    my $class = shift ;
    my $want_encoding = shift ;

    $obj->croakError("$class: Encode module needed to use -Encode")
        if ! hasEncode();

    my $encoding = Encode::find_encoding($want_encoding);

    $obj->croakError("$class: Encoding '$want_encoding' is not available")
       if ! $encoding;

    return $encoding;
}

our ($needBinmode);
$needBinmode = ($^O eq 'MSWin32' || 
                    ($] >= 5.006 && eval ' ${^UNICODE} || ${^UTF8LOCALE} '))
                    ? 1 : 1 ;

sub setBinModeInput($)
{
    my $handle = shift ;

    binmode $handle 
        if  $needBinmode;
}

sub setBinModeOutput($)
{
    my $handle = shift ;

    binmode $handle 
        if  $needBinmode;
}

sub isaFilehandle($)
{
    use utf8; # Pragma needed to keep Perl 5.6.0 happy
    return (defined $_[0] and 
             (UNIVERSAL::isa($_[0],'GLOB') or 
              UNIVERSAL::isa($_[0],'IO::Handle') or
              UNIVERSAL::isa(\$_[0],'GLOB')) 
          )
}

sub isaFilename($)
{
    return (defined $_[0] and 
           ! ref $_[0]    and 
           UNIVERSAL::isa(\$_[0], 'SCALAR'));
}

sub isaFileGlobString
{
    return defined $_[0] && $_[0] =~ /^<.*>$/;
}

sub cleanFileGlobString
{
    my $string = shift ;

    $string =~ s/^\s*<\s*(.*)\s*>\s*$/$1/;

    return $string;
}

use constant WANT_CODE  => 1 ;
use constant WANT_EXT   => 2 ;
use constant WANT_UNDEF => 4 ;
#use constant WANT_HASH  => 8 ;
use constant WANT_HASH  => 0 ;

sub whatIsInput($;$)
{
    my $got = whatIs(@_);
    
    if (defined $got && $got eq 'filename' && defined $_[0] && $_[0] eq '-')
    {
        #use IO::File;
        $got = 'handle';
        $_[0] = *STDIN;
        #$_[0] = new IO::File("<-");
    }

    return $got;
}

sub whatIsOutput($;$)
{
    my $got = whatIs(@_);
    
    if (defined $got && $got eq 'filename' && defined $_[0] && $_[0] eq '-')
    {
        $got = 'handle';
        $_[0] = *STDOUT;
        #$_[0] = new IO::File(">-");
    }
    
    return $got;
}

sub whatIs ($;$)
{
    return 'handle' if isaFilehandle($_[0]);

    my $wantCode = defined $_[1] && $_[1] & WANT_CODE ;
    my $extended = defined $_[1] && $_[1] & WANT_EXT ;
    my $undef    = defined $_[1] && $_[1] & WANT_UNDEF ;
    my $hash     = defined $_[1] && $_[1] & WANT_HASH ;

    return 'undef'  if ! defined $_[0] && $undef ;

    if (ref $_[0]) {
        return ''       if blessed($_[0]); # is an object
        #return ''       if UNIVERSAL::isa($_[0], 'UNIVERSAL'); # is an object
        return 'buffer' if UNIVERSAL::isa($_[0], 'SCALAR');
        return 'array'  if UNIVERSAL::isa($_[0], 'ARRAY')  && $extended ;
        return 'hash'   if UNIVERSAL::isa($_[0], 'HASH')   && $hash ;
        return 'code'   if UNIVERSAL::isa($_[0], 'CODE')   && $wantCode ;
        return '';
    }

    return 'fileglob' if $extended && isaFileGlobString($_[0]);
    return 'filename';
}

sub oneTarget
{
    return $_[0] =~ /^(code|handle|buffer|filename)$/;
}

sub IO::Compress::Base::Validator::new
{
    my $class = shift ;

    my $Class = shift ;
    my $error_ref = shift ;
    my $reportClass = shift ;

    my %data = (Class       => $Class, 
                Error       => $error_ref,
                reportClass => $reportClass, 
               ) ;

    my $obj = bless \%data, $class ;

    local $Carp::CarpLevel = 1;

    my $inType    = $data{inType}    = whatIsInput($_[0], WANT_EXT|WANT_HASH);
    my $outType   = $data{outType}   = whatIsOutput($_[1], WANT_EXT|WANT_HASH);

    my $oneInput  = $data{oneInput}  = oneTarget($inType);
    my $oneOutput = $data{oneOutput} = oneTarget($outType);

    if (! $inType)
    {
        $obj->croakError("$reportClass: illegal input parameter") ;
        #return undef ;
    }    

#    if ($inType eq 'hash')
#    {
#        $obj->{Hash} = 1 ;
#        $obj->{oneInput} = 1 ;
#        return $obj->validateHash($_[0]);
#    }

    if (! $outType)
    {
        $obj->croakError("$reportClass: illegal output parameter") ;
        #return undef ;
    }    


    if ($inType ne 'fileglob' && $outType eq 'fileglob')
    {
        $obj->croakError("Need input fileglob for outout fileglob");
    }    

#    if ($inType ne 'fileglob' && $outType eq 'hash' && $inType ne 'filename' )
#    {
#        $obj->croakError("input must ne filename or fileglob when output is a hash");
#    }    

    if ($inType eq 'fileglob' && $outType eq 'fileglob')
    {
        $data{GlobMap} = 1 ;
        $data{inType} = $data{outType} = 'filename';
        my $mapper = new File::GlobMapper($_[0], $_[1]);
        if ( ! $mapper )
        {
            return $obj->saveErrorString($File::GlobMapper::Error) ;
        }
        $data{Pairs} = $mapper->getFileMap();

        return $obj;
    }
    
    $obj->croakError("$reportClass: input and output $inType are identical")
        if $inType eq $outType && $_[0] eq $_[1] && $_[0] ne '-' ;

    if ($inType eq 'fileglob') # && $outType ne 'fileglob'
    {
        my $glob = cleanFileGlobString($_[0]);
        my @inputs = glob($glob);

        if (@inputs == 0)
        {
            # TODO -- legal or die?
            die "globmap matched zero file -- legal or die???" ;
        }
        elsif (@inputs == 1)
        {
            $obj->validateInputFilenames($inputs[0])
                or return undef;
            $_[0] = $inputs[0]  ;
            $data{inType} = 'filename' ;
            $data{oneInput} = 1;
        }
        else
        {
            $obj->validateInputFilenames(@inputs)
                or return undef;
            $_[0] = [ @inputs ] ;
            $data{inType} = 'filenames' ;
        }
    }
    elsif ($inType eq 'filename')
    {
        $obj->validateInputFilenames($_[0])
            or return undef;
    }
    elsif ($inType eq 'array')
    {
        $data{inType} = 'filenames' ;
        $obj->validateInputArray($_[0])
            or return undef ;
    }

    return $obj->saveErrorString("$reportClass: output buffer is read-only")
        if $outType eq 'buffer' && readonly(${ $_[1] });

    if ($outType eq 'filename' )
    {
        $obj->croakError("$reportClass: output filename is undef or null string")
            if ! defined $_[1] || $_[1] eq ''  ;

        if (-e $_[1])
        {
            if (-d _ )
            {
                return $obj->saveErrorString("output file '$_[1]' is a directory");
            }
        }
    }
    
    return $obj ;
}

sub IO::Compress::Base::Validator::saveErrorString
{
    my $self   = shift ;
    ${ $self->{Error} } = shift ;
    return undef;
    
}

sub IO::Compress::Base::Validator::croakError
{
    my $self   = shift ;
    $self->saveErrorString($_[0]);
    croak $_[0];
}



sub IO::Compress::Base::Validator::validateInputFilenames
{
    my $self = shift ;

    foreach my $filename (@_)
    {
        $self->croakError("$self->{reportClass}: input filename is undef or null string")
            if ! defined $filename || $filename eq ''  ;

        next if $filename eq '-';

        if (! -e $filename )
        {
            return $self->saveErrorString("input file '$filename' does not exist");
        }

        if (-d _ )
        {
            return $self->saveErrorString("input file '$filename' is a directory");
        }

        if (! -r _ )
        {
            return $self->saveErrorString("cannot open file '$filename': $!");
        }
    }

    return 1 ;
}

sub IO::Compress::Base::Validator::validateInputArray
{
    my $self = shift ;

    if ( @{ $_[0] } == 0 )
    {
        return $self->saveErrorString("empty array reference") ;
    }    

    foreach my $element ( @{ $_[0] } )
    {
        my $inType  = whatIsInput($element);
    
        if (! $inType)
        {
            $self->croakError("unknown input parameter") ;
        }    
        elsif($inType eq 'filename')
        {
            $self->validateInputFilenames($element)
                or return undef ;
        }
        else
        {
            $self->croakError("not a filename") ;
        }
    }

    return 1 ;
}

#sub IO::Compress::Base::Validator::validateHash
#{
#    my $self = shift ;
#    my $href = shift ;
#
#    while (my($k, $v) = each %$href)
#    {
#        my $ktype = whatIsInput($k);
#        my $vtype = whatIsOutput($v, WANT_EXT|WANT_UNDEF) ;
#
#        if ($ktype ne 'filename')
#        {
#            return $self->saveErrorString("hash key not filename") ;
#        }    
#
#        my %valid = map { $_ => 1 } qw(filename buffer array undef handle) ;
#        if (! $valid{$vtype})
#        {
#            return $self->saveErrorString("hash value not ok") ;
#        }    
#    }
#
#    return $self ;
#}

sub createSelfTiedObject
{
    my $class = shift || (caller)[0] ;
    my $error_ref = shift ;

    my $obj = bless Symbol::gensym(), ref($class) || $class;
    tie *$obj, $obj if $] >= 5.005;
    *$obj->{Closed} = 1 ;
    $$error_ref = '';
    *$obj->{Error} = $error_ref ;
    my $errno = 0 ;
    *$obj->{ErrorNo} = \$errno ;

    return $obj;
}



#package Parse::Parameters ;
#
#
#require Exporter;
#our ($VERSION, @ISA, @EXPORT);
#$VERSION = '2.000_08';
#@ISA = qw(Exporter);

$EXPORT_TAGS{Parse} = [qw( ParseParameters 
                           Parse_any Parse_unsigned Parse_signed 
                           Parse_boolean Parse_custom Parse_string
                           Parse_multiple Parse_writable_scalar
                         )
                      ];              

push @EXPORT, @{ $EXPORT_TAGS{Parse} } ;

use constant Parse_any      => 0x01;
use constant Parse_unsigned => 0x02;
use constant Parse_signed   => 0x04;
use constant Parse_boolean  => 0x08;
use constant Parse_string   => 0x10;
use constant Parse_custom   => 0x12;

#use constant Parse_store_ref        => 0x100 ;
use constant Parse_multiple         => 0x100 ;
use constant Parse_writable         => 0x200 ;
use constant Parse_writable_scalar  => 0x400 | Parse_writable ;

use constant OFF_PARSED     => 0 ;
use constant OFF_TYPE       => 1 ;
use constant OFF_DEFAULT    => 2 ;
use constant OFF_FIXED      => 3 ;
use constant OFF_FIRST_ONLY => 4 ;
use constant OFF_STICKY     => 5 ;



sub ParseParameters
{
    my $level = shift || 0 ; 

    my $sub = (caller($level + 1))[3] ;
    local $Carp::CarpLevel = 1 ;
    
    return $_[1]
        if @_ == 2 && defined $_[1] && UNIVERSAL::isa($_[1], "IO::Compress::Base::Parameters");
    
    my $p = new IO::Compress::Base::Parameters() ;            
    $p->parse(@_)
        or croak "$sub: $p->{Error}" ;

    return $p;
}

#package IO::Compress::Base::Parameters;

use strict;
use warnings;
use Carp;

sub IO::Compress::Base::Parameters::new
{
    my $class = shift ;

    my $obj = { Error => '',
                Got   => {},
              } ;

    #return bless $obj, ref($class) || $class || __PACKAGE__ ;
    return bless $obj, 'IO::Compress::Base::Parameters' ;
}

sub IO::Compress::Base::Parameters::setError
{
    my $self = shift ;
    my $error = shift ;
    my $retval = @_ ? shift : undef ;

    $self->{Error} = $error ;
    return $retval;
}
          
#sub getError
#{
#    my $self = shift ;
#    return $self->{Error} ;
#}
          
sub IO::Compress::Base::Parameters::parse
{
    my $self = shift ;

    my $default = shift ;

    my $got = $self->{Got} ;
    my $firstTime = keys %{ $got } == 0 ;
    my $other;

    my (@Bad) ;
    my @entered = () ;

    # Allow the options to be passed as a hash reference or
    # as the complete hash.
    if (@_ == 0) {
        @entered = () ;
    }
    elsif (@_ == 1) {
        my $href = $_[0] ;
    
        return $self->setError("Expected even number of parameters, got 1")
            if ! defined $href or ! ref $href or ref $href ne "HASH" ;
 
        foreach my $key (keys %$href) {
            push @entered, $key ;
            push @entered, \$href->{$key} ;
        }
    }
    else {
        my $count = @_;
        return $self->setError("Expected even number of parameters, got $count")
            if $count % 2 != 0 ;
        
        for my $i (0.. $count / 2 - 1) {
            if ($_[2 * $i] eq '__xxx__') {
                $other = $_[2 * $i + 1] ;
            }
            else {
                push @entered, $_[2 * $i] ;
                push @entered, \$_[2 * $i + 1] ;
            }
        }
    }


    while (my ($key, $v) = each %$default)
    {
        croak "need 4 params [@$v]"
            if @$v != 4 ;

        my ($first_only, $sticky, $type, $value) = @$v ;
        my $x ;
        $self->_checkType($key, \$value, $type, 0, \$x) 
            or return undef ;

        $key = lc $key;

        if ($firstTime || ! $sticky) {
            $x = []
                if $type & Parse_multiple;

            $got->{$key} = [0, $type, $value, $x, $first_only, $sticky] ;
        }

        $got->{$key}[OFF_PARSED] = 0 ;
    }

    my %parsed = ();
    
    if ($other) 
    {
        for my $key (keys %$default)  
        {
            my $canonkey = lc $key;
            if ($other->parsed($canonkey))
            {
                my $value = $other->value($canonkey);
#print "SET '$canonkey' to $value [$$value]\n";
                ++ $parsed{$canonkey};
                $got->{$canonkey}[OFF_PARSED]  = 1;
                $got->{$canonkey}[OFF_DEFAULT] = $value;
                $got->{$canonkey}[OFF_FIXED]   = $value;
            }
        }
    }
    
    for my $i (0.. @entered / 2 - 1) {
        my $key = $entered[2* $i] ;
        my $value = $entered[2* $i+1] ;

        #print "Key [$key] Value [$value]" ;
        #print defined $$value ? "[$$value]\n" : "[undef]\n";

        $key =~ s/^-// ;
        my $canonkey = lc $key;
 
        if ($got->{$canonkey} && ($firstTime ||
                                  ! $got->{$canonkey}[OFF_FIRST_ONLY]  ))
        {
            my $type = $got->{$canonkey}[OFF_TYPE] ;
            my $parsed = $parsed{$canonkey};
            ++ $parsed{$canonkey};

            return $self->setError("Muliple instances of '$key' found") 
                if $parsed && ($type & Parse_multiple) == 0 ;

            my $s ;
            $self->_checkType($key, $value, $type, 1, \$s)
                or return undef ;

            $value = $$value ;
            if ($type & Parse_multiple) {
                $got->{$canonkey}[OFF_PARSED] = 1;
                push @{ $got->{$canonkey}[OFF_FIXED] }, $s ;
            }
            else {
                $got->{$canonkey} = [1, $type, $value, $s] ;
            }
        }
        else
          { push (@Bad, $key) }
    }
 
    if (@Bad) {
        my ($bad) = join(", ", @Bad) ;
        return $self->setError("unknown key value(s) $bad") ;
    }

    return 1;
}

sub IO::Compress::Base::Parameters::_checkType
{
    my $self = shift ;

    my $key   = shift ;
    my $value = shift ;
    my $type  = shift ;
    my $validate  = shift ;
    my $output  = shift;

    #local $Carp::CarpLevel = $level ;
    #print "PARSE $type $key $value $validate $sub\n" ;

    if ($type & Parse_writable_scalar)
    {
        return $self->setError("Parameter '$key' not writable")
            if $validate &&  readonly $$value ;

        if (ref $$value) 
        {
            return $self->setError("Parameter '$key' not a scalar reference")
                if $validate &&  ref $$value ne 'SCALAR' ;

            $$output = $$value ;
        }
        else  
        {
            return $self->setError("Parameter '$key' not a scalar")
                if $validate &&  ref $value ne 'SCALAR' ;

            $$output = $value ;
        }

        return 1;
    }

#    if ($type & Parse_store_ref)
#    {
#        #$value = $$value
#        #    if ref ${ $value } ;
#
#        $$output = $value ;
#        return 1;
#    }

    $value = $$value ;

    if ($type & Parse_any)
    {
        $$output = $value ;
        return 1;
    }
    elsif ($type & Parse_unsigned)
    {
        return $self->setError("Parameter '$key' must be an unsigned int, got 'undef'")
            if $validate && ! defined $value ;
        return $self->setError("Parameter '$key' must be an unsigned int, got '$value'")
            if $validate && $value !~ /^\d+$/;

        $$output = defined $value ? $value : 0 ;    
        return 1;
    }
    elsif ($type & Parse_signed)
    {
        return $self->setError("Parameter '$key' must be a signed int, got 'undef'")
            if $validate && ! defined $value ;
        return $self->setError("Parameter '$key' must be a signed int, got '$value'")
            if $validate && $value !~ /^-?\d+$/;

        $$output = defined $value ? $value : 0 ;    
        return 1 ;
    }
    elsif ($type & Parse_boolean)
    {
        return $self->setError("Parameter '$key' must be an int, got '$value'")
            if $validate && defined $value && $value !~ /^\d*$/;
        $$output =  defined $value ? $value != 0 : 0 ;    
        return 1;
    }
    elsif ($type & Parse_string)
    {
        $$output = defined $value ? $value : "" ;    
        return 1;
    }

    $$output = $value ;
    return 1;
}



sub IO::Compress::Base::Parameters::parsed
{
    my $self = shift ;
    my $name = shift ;

    return $self->{Got}{lc $name}[OFF_PARSED] ;
}

sub IO::Compress::Base::Parameters::value
{
    my $self = shift ;
    my $name = shift ;

    if (@_)
    {
        $self->{Got}{lc $name}[OFF_PARSED]  = 1;
        $self->{Got}{lc $name}[OFF_DEFAULT] = $_[0] ;
        $self->{Got}{lc $name}[OFF_FIXED]   = $_[0] ;
    }

    return $self->{Got}{lc $name}[OFF_FIXED] ;
}

sub IO::Compress::Base::Parameters::valueOrDefault
{
    my $self = shift ;
    my $name = shift ;
    my $default = shift ;

    my $value = $self->{Got}{lc $name}[OFF_DEFAULT] ;

    return $value if defined $value ;
    return $default ;
}

sub IO::Compress::Base::Parameters::wantValue
{
    my $self = shift ;
    my $name = shift ;

    return defined $self->{Got}{lc $name}[OFF_DEFAULT] ;

}

sub IO::Compress::Base::Parameters::clone
{
    my $self = shift ;
    my $obj = { };
    my %got ;

    while (my ($k, $v) = each %{ $self->{Got} }) {
        $got{$k} = [ @$v ];
    }

    $obj->{Error} = $self->{Error};
    $obj->{Got} = \%got ;

    return bless $obj, 'IO::Compress::Base::Parameters' ;
}

package U64;

use constant MAX32 => 0xFFFFFFFF ;
use constant HI_1 => MAX32 + 1 ;
use constant LOW   => 0 ;
use constant HIGH  => 1;

sub new
{
    my $class = shift ;

    my $high = 0 ;
    my $low  = 0 ;

    if (@_ == 2) {
        $high = shift ;
        $low  = shift ;
    }
    elsif (@_ == 1) {
        $low  = shift ;
    }

    bless [$low, $high], $class;
}

sub newUnpack_V64
{
    my $string = shift;

    my ($low, $hi) = unpack "V V", $string ;
    bless [ $low, $hi ], "U64";
}

sub newUnpack_V32
{
    my $string = shift;

    my $low = unpack "V", $string ;
    bless [ $low, 0 ], "U64";
}

sub reset
{
    my $self = shift;
    $self->[HIGH] = $self->[LOW] = 0;
}

sub clone
{
    my $self = shift;
    bless [ @$self ], ref $self ;
}

sub getHigh
{
    my $self = shift;
    return $self->[HIGH];
}

sub getLow
{
    my $self = shift;
    return $self->[LOW];
}

sub get32bit
{
    my $self = shift;
    return $self->[LOW];
}

sub get64bit
{
    my $self = shift;
    # Not using << here because the result will still be
    # a 32-bit value on systems where int size is 32-bits
    return $self->[HIGH] * HI_1 + $self->[LOW];
}

sub add
{
    my $self = shift;
    my $value = shift;

    if (ref $value eq 'U64') {
        $self->[HIGH] += $value->[HIGH] ;
        $value = $value->[LOW];
    }
     
    my $available = MAX32 - $self->[LOW] ;

    if ($value > $available) {
       ++ $self->[HIGH] ;
       $self->[LOW] = $value - $available - 1;
    }
    else {
       $self->[LOW] += $value ;
    }

}

sub equal
{
    my $self = shift;
    my $other = shift;

    return $self->[LOW]  == $other->[LOW] &&
           $self->[HIGH] == $other->[HIGH] ;
}

sub is64bit
{
    my $self = shift;
    return $self->[HIGH] > 0 ;
}

sub getPacked_V64
{
    my $self = shift;

    return pack "V V", @$self ;
}

sub getPacked_V32
{
    my $self = shift;

    return pack "V", $self->[LOW] ;
}

sub pack_V64
{
    my $low  = shift;

    return pack "V V", $low, 0;
}


package IO::Compress::Base::Common;

1;
FILE   6d248b91/IO/Compress/Gzip.pm  �#line 1 "/usr/lib/perl5/5.14/IO/Compress/Gzip.pm"

package IO::Compress::Gzip ;

require 5.004 ;

use strict ;
use warnings;
use bytes;


use IO::Compress::RawDeflate 2.033 ;

use Compress::Raw::Zlib  2.033 ;
use IO::Compress::Base::Common  2.033 qw(:Status :Parse createSelfTiedObject);
use IO::Compress::Gzip::Constants 2.033 ;
use IO::Compress::Zlib::Extra 2.033 ;

BEGIN
{
    if (defined &utf8::downgrade ) 
      { *noUTF8 = \&utf8::downgrade }
    else
      { *noUTF8 = sub {} }  
}

require Exporter ;

our ($VERSION, @ISA, @EXPORT_OK, %EXPORT_TAGS, $GzipError);

$VERSION = '2.033';
$GzipError = '' ;

@ISA    = qw(Exporter IO::Compress::RawDeflate);
@EXPORT_OK = qw( $GzipError gzip ) ;
%EXPORT_TAGS = %IO::Compress::RawDeflate::DEFLATE_CONSTANTS ;
push @{ $EXPORT_TAGS{all} }, @EXPORT_OK ;
Exporter::export_ok_tags('all');

sub new
{
    my $class = shift ;

    my $obj = createSelfTiedObject($class, \$GzipError);

    $obj->_create(undef, @_);
}


sub gzip
{
    my $obj = createSelfTiedObject(undef, \$GzipError);
    return $obj->_def(@_);
}

#sub newHeader
#{
#    my $self = shift ;
#    #return GZIP_MINIMUM_HEADER ;
#    return $self->mkHeader(*$self->{Got});
#}

sub getExtraParams
{
    my $self = shift ;

    return (
            # zlib behaviour
            $self->getZlibParams(),

            # Gzip header fields
            'Minimal'   => [0, 1, Parse_boolean,   0],
            'Comment'   => [0, 1, Parse_any,       undef],
            'Name'      => [0, 1, Parse_any,       undef],
            'Time'      => [0, 1, Parse_any,       undef],
            'TextFlag'  => [0, 1, Parse_boolean,   0],
            'HeaderCRC' => [0, 1, Parse_boolean,   0],
            'OS_Code'   => [0, 1, Parse_unsigned,  $Compress::Raw::Zlib::gzip_os_code],
            'ExtraField'=> [0, 1, Parse_any,       undef],
            'ExtraFlags'=> [0, 1, Parse_any,       undef],

        );
}


sub ckParams
{
    my $self = shift ;
    my $got = shift ;

    # gzip always needs crc32
    $got->value('CRC32' => 1);

    return 1
        if $got->value('Merge') ;

    my $strict = $got->value('Strict') ;


    {
        if (! $got->parsed('Time') ) {
            # Modification time defaults to now.
            $got->value('Time' => time) ;
        }

        # Check that the Name & Comment don't have embedded NULLs
        # Also check that they only contain ISO 8859-1 chars.
        if ($got->parsed('Name') && defined $got->value('Name')) {
            my $name = $got->value('Name');
                
            return $self->saveErrorString(undef, "Null Character found in Name",
                                                Z_DATA_ERROR)
                if $strict && $name =~ /\x00/ ;

            return $self->saveErrorString(undef, "Non ISO 8859-1 Character found in Name",
                                                Z_DATA_ERROR)
                if $strict && $name =~ /$GZIP_FNAME_INVALID_CHAR_RE/o ;
        }

        if ($got->parsed('Comment') && defined $got->value('Comment')) {
            my $comment = $got->value('Comment');

            return $self->saveErrorString(undef, "Null Character found in Comment",
                                                Z_DATA_ERROR)
                if $strict && $comment =~ /\x00/ ;

            return $self->saveErrorString(undef, "Non ISO 8859-1 Character found in Comment",
                                                Z_DATA_ERROR)
                if $strict && $comment =~ /$GZIP_FCOMMENT_INVALID_CHAR_RE/o;
        }

        if ($got->parsed('OS_Code') ) {
            my $value = $got->value('OS_Code');

            return $self->saveErrorString(undef, "OS_Code must be between 0 and 255, got '$value'")
                if $value < 0 || $value > 255 ;
            
        }

        # gzip only supports Deflate at present
        $got->value('Method' => Z_DEFLATED) ;

        if ( ! $got->parsed('ExtraFlags')) {
            $got->value('ExtraFlags' => 2) 
                if $got->value('Level') == Z_BEST_COMPRESSION ;
            $got->value('ExtraFlags' => 4) 
                if $got->value('Level') == Z_BEST_SPEED ;
        }

        my $data = $got->value('ExtraField') ;
        if (defined $data) {
            my $bad = IO::Compress::Zlib::Extra::parseExtraField($data, $strict, 1) ;
            return $self->saveErrorString(undef, "Error with ExtraField Parameter: $bad", Z_DATA_ERROR)
                if $bad ;

            $got->value('ExtraField', $data) ;
        }
    }

    return 1;
}

sub mkTrailer
{
    my $self = shift ;
    return pack("V V", *$self->{Compress}->crc32(), 
                       *$self->{UnCompSize}->get32bit());
}

sub getInverseClass
{
    return ('IO::Uncompress::Gunzip',
                \$IO::Uncompress::Gunzip::GunzipError);
}

sub getFileInfo
{
    my $self = shift ;
    my $params = shift;
    my $filename = shift ;

    my $defaultTime = (stat($filename))[9] ;

    $params->value('Name' => $filename)
        if ! $params->parsed('Name') ;

    $params->value('Time' => $defaultTime) 
        if ! $params->parsed('Time') ;
}


sub mkHeader
{
    my $self = shift ;
    my $param = shift ;

    # stort-circuit if a minimal header is requested.
    return GZIP_MINIMUM_HEADER if $param->value('Minimal') ;

    # METHOD
    my $method = $param->valueOrDefault('Method', GZIP_CM_DEFLATED) ;

    # FLAGS
    my $flags       = GZIP_FLG_DEFAULT ;
    $flags |= GZIP_FLG_FTEXT    if $param->value('TextFlag') ;
    $flags |= GZIP_FLG_FHCRC    if $param->value('HeaderCRC') ;
    $flags |= GZIP_FLG_FEXTRA   if $param->wantValue('ExtraField') ;
    $flags |= GZIP_FLG_FNAME    if $param->wantValue('Name') ;
    $flags |= GZIP_FLG_FCOMMENT if $param->wantValue('Comment') ;
    
    # MTIME
    my $time = $param->valueOrDefault('Time', GZIP_MTIME_DEFAULT) ;

    # EXTRA FLAGS
    my $extra_flags = $param->valueOrDefault('ExtraFlags', GZIP_XFL_DEFAULT);

    # OS CODE
    my $os_code = $param->valueOrDefault('OS_Code', GZIP_OS_DEFAULT) ;


    my $out = pack("C4 V C C", 
            GZIP_ID1,   # ID1
            GZIP_ID2,   # ID2
            $method,    # Compression Method
            $flags,     # Flags
            $time,      # Modification Time
            $extra_flags, # Extra Flags
            $os_code,   # Operating System Code
            ) ;

    # EXTRA
    if ($flags & GZIP_FLG_FEXTRA) {
        my $extra = $param->value('ExtraField') ;
        $out .= pack("v", length $extra) . $extra ;
    }

    # NAME
    if ($flags & GZIP_FLG_FNAME) {
        my $name .= $param->value('Name') ;
        $name =~ s/\x00.*$//;
        $out .= $name ;
        # Terminate the filename with NULL unless it already is
        $out .= GZIP_NULL_BYTE 
            if !length $name or
               substr($name, 1, -1) ne GZIP_NULL_BYTE ;
    }

    # COMMENT
    if ($flags & GZIP_FLG_FCOMMENT) {
        my $comment .= $param->value('Comment') ;
        $comment =~ s/\x00.*$//;
        $out .= $comment ;
        # Terminate the comment with NULL unless it already is
        $out .= GZIP_NULL_BYTE
            if ! length $comment or
               substr($comment, 1, -1) ne GZIP_NULL_BYTE;
    }

    # HEADER CRC
    $out .= pack("v", crc32($out) & 0x00FF ) if $param->value('HeaderCRC') ;

    noUTF8($out);

    return $out ;
}

sub mkFinalTrailer
{
    return '';
}

1; 

__END__

#line 1243
FILE   &d18cdd44/IO/Compress/Gzip/Constants.pm  {#line 1 "/usr/lib/perl5/5.14/IO/Compress/Gzip/Constants.pm"
package IO::Compress::Gzip::Constants;

use strict ;
use warnings;
use bytes;

require Exporter;

our ($VERSION, @ISA, @EXPORT, %GZIP_OS_Names);
our ($GZIP_FNAME_INVALID_CHAR_RE, $GZIP_FCOMMENT_INVALID_CHAR_RE);

$VERSION = '2.033';

@ISA = qw(Exporter);

@EXPORT= qw(

    GZIP_ID_SIZE
    GZIP_ID1
    GZIP_ID2

    GZIP_FLG_DEFAULT
    GZIP_FLG_FTEXT
    GZIP_FLG_FHCRC
    GZIP_FLG_FEXTRA
    GZIP_FLG_FNAME
    GZIP_FLG_FCOMMENT
    GZIP_FLG_RESERVED

    GZIP_CM_DEFLATED

    GZIP_MIN_HEADER_SIZE
    GZIP_TRAILER_SIZE

    GZIP_MTIME_DEFAULT
    GZIP_XFL_DEFAULT
    GZIP_FEXTRA_HEADER_SIZE
    GZIP_FEXTRA_MAX_SIZE
    GZIP_FEXTRA_SUBFIELD_HEADER_SIZE
    GZIP_FEXTRA_SUBFIELD_ID_SIZE
    GZIP_FEXTRA_SUBFIELD_LEN_SIZE
    GZIP_FEXTRA_SUBFIELD_MAX_SIZE

    $GZIP_FNAME_INVALID_CHAR_RE
    $GZIP_FCOMMENT_INVALID_CHAR_RE

    GZIP_FHCRC_SIZE

    GZIP_ISIZE_MAX
    GZIP_ISIZE_MOD_VALUE


    GZIP_NULL_BYTE

    GZIP_OS_DEFAULT

    %GZIP_OS_Names

    GZIP_MINIMUM_HEADER

    );

# Constant names derived from RFC 1952

use constant GZIP_ID_SIZE                     => 2 ;
use constant GZIP_ID1                         => 0x1F;
use constant GZIP_ID2                         => 0x8B;

use constant GZIP_MIN_HEADER_SIZE             => 10 ;# minimum gzip header size
use constant GZIP_TRAILER_SIZE                => 8 ;


use constant GZIP_FLG_DEFAULT                 => 0x00 ;
use constant GZIP_FLG_FTEXT                   => 0x01 ;
use constant GZIP_FLG_FHCRC                   => 0x02 ; # called CONTINUATION in gzip
use constant GZIP_FLG_FEXTRA                  => 0x04 ;
use constant GZIP_FLG_FNAME                   => 0x08 ;
use constant GZIP_FLG_FCOMMENT                => 0x10 ;
#use constant GZIP_FLG_ENCRYPTED              => 0x20 ; # documented in gzip sources
use constant GZIP_FLG_RESERVED                => (0x20 | 0x40 | 0x80) ;

use constant GZIP_XFL_DEFAULT                 => 0x00 ;

use constant GZIP_MTIME_DEFAULT               => 0x00 ;

use constant GZIP_FEXTRA_HEADER_SIZE          => 2 ;
use constant GZIP_FEXTRA_MAX_SIZE             => 0xFFFF ;
use constant GZIP_FEXTRA_SUBFIELD_ID_SIZE     => 2 ;
use constant GZIP_FEXTRA_SUBFIELD_LEN_SIZE    => 2 ;
use constant GZIP_FEXTRA_SUBFIELD_HEADER_SIZE => GZIP_FEXTRA_SUBFIELD_ID_SIZE +
                                                 GZIP_FEXTRA_SUBFIELD_LEN_SIZE;
use constant GZIP_FEXTRA_SUBFIELD_MAX_SIZE    => GZIP_FEXTRA_MAX_SIZE - 
                                                 GZIP_FEXTRA_SUBFIELD_HEADER_SIZE ;


if (ord('A') == 193)
{
    # EBCDIC 
    $GZIP_FNAME_INVALID_CHAR_RE = '[\x00-\x3f\xff]';
    $GZIP_FCOMMENT_INVALID_CHAR_RE = '[\x00-\x0a\x11-\x14\x16-\x3f\xff]';
    
}
else
{
    $GZIP_FNAME_INVALID_CHAR_RE       =  '[\x00-\x1F\x7F-\x9F]';
    $GZIP_FCOMMENT_INVALID_CHAR_RE    =  '[\x00-\x09\x11-\x1F\x7F-\x9F]';
}            

use constant GZIP_FHCRC_SIZE        => 2 ; # aka CONTINUATION in gzip

use constant GZIP_CM_DEFLATED       => 8 ;

use constant GZIP_NULL_BYTE         => "\x00";
use constant GZIP_ISIZE_MAX         => 0xFFFFFFFF ;
use constant GZIP_ISIZE_MOD_VALUE   => GZIP_ISIZE_MAX + 1 ;

# OS Names sourced from http://www.gzip.org/format.txt

use constant GZIP_OS_DEFAULT=> 0xFF ;
%GZIP_OS_Names = (
    0   => 'MS-DOS',
    1   => 'Amiga',
    2   => 'VMS',
    3   => 'Unix',
    4   => 'VM/CMS',
    5   => 'Atari TOS',
    6   => 'HPFS (OS/2, NT)',
    7   => 'Macintosh',
    8   => 'Z-System',
    9   => 'CP/M',
    10  => 'TOPS-20',
    11  => 'NTFS (NT)',
    12  => 'SMS QDOS',
    13  => 'Acorn RISCOS',
    14  => 'VFAT file system (Win95, NT)',
    15  => 'MVS',
    16  => 'BeOS',
    17  => 'Tandem/NSK',
    18  => 'THEOS',
    GZIP_OS_DEFAULT()   => 'Unknown',
    ) ;

use constant GZIP_MINIMUM_HEADER =>   pack("C4 V C C",  
        GZIP_ID1, GZIP_ID2, GZIP_CM_DEFLATED, GZIP_FLG_DEFAULT,
        GZIP_MTIME_DEFAULT, GZIP_XFL_DEFAULT, GZIP_OS_DEFAULT) ;


1;
FILE   "99f91916/IO/Compress/RawDeflate.pm  q#line 1 "/usr/lib/perl5/5.14/IO/Compress/RawDeflate.pm"
package IO::Compress::RawDeflate ;

# create RFC1951
#
use strict ;
use warnings;
use bytes;


use IO::Compress::Base 2.033 ;
use IO::Compress::Base::Common  2.033 qw(:Status createSelfTiedObject);
use IO::Compress::Adapter::Deflate  2.033 ;

require Exporter ;


our ($VERSION, @ISA, @EXPORT_OK, %DEFLATE_CONSTANTS, %EXPORT_TAGS, $RawDeflateError);

$VERSION = '2.033';
$RawDeflateError = '';

@ISA = qw(Exporter IO::Compress::Base);
@EXPORT_OK = qw( $RawDeflateError rawdeflate ) ;

%EXPORT_TAGS = ( flush     => [qw{  
                                    Z_NO_FLUSH
                                    Z_PARTIAL_FLUSH
                                    Z_SYNC_FLUSH
                                    Z_FULL_FLUSH
                                    Z_FINISH
                                    Z_BLOCK
                              }],
                 level     => [qw{  
                                    Z_NO_COMPRESSION
                                    Z_BEST_SPEED
                                    Z_BEST_COMPRESSION
                                    Z_DEFAULT_COMPRESSION
                              }],
                 strategy  => [qw{  
                                    Z_FILTERED
                                    Z_HUFFMAN_ONLY
                                    Z_RLE
                                    Z_FIXED
                                    Z_DEFAULT_STRATEGY
                              }],

              );

{
    my %seen;
    foreach (keys %EXPORT_TAGS )
    {
        push @{$EXPORT_TAGS{constants}}, 
                 grep { !$seen{$_}++ } 
                 @{ $EXPORT_TAGS{$_} }
    }
    $EXPORT_TAGS{all} = $EXPORT_TAGS{constants} ;
}


%DEFLATE_CONSTANTS = %EXPORT_TAGS;

push @{ $EXPORT_TAGS{all} }, @EXPORT_OK ;

Exporter::export_ok_tags('all');
              


sub new
{
    my $class = shift ;

    my $obj = createSelfTiedObject($class, \$RawDeflateError);

    return $obj->_create(undef, @_);
}

sub rawdeflate
{
    my $obj = createSelfTiedObject(undef, \$RawDeflateError);
    return $obj->_def(@_);
}

sub ckParams
{
    my $self = shift ;
    my $got = shift;

    return 1 ;
}

sub mkComp
{
    my $self = shift ;
    my $got = shift ;

    my ($obj, $errstr, $errno) = IO::Compress::Adapter::Deflate::mkCompObject(
                                                 $got->value('CRC32'),
                                                 $got->value('Adler32'),
                                                 $got->value('Level'),
                                                 $got->value('Strategy')
                                                 );

   return $self->saveErrorString(undef, $errstr, $errno)
       if ! defined $obj;

   return $obj;    
}


sub mkHeader
{
    my $self = shift ;
    return '';
}

sub mkTrailer
{
    my $self = shift ;
    return '';
}

sub mkFinalTrailer
{
    return '';
}


#sub newHeader
#{
#    my $self = shift ;
#    return '';
#}

sub getExtraParams
{
    my $self = shift ;
    return $self->getZlibParams();
}

sub getZlibParams
{
    my $self = shift ;

    use IO::Compress::Base::Common  2.033 qw(:Parse);
    use Compress::Raw::Zlib  2.033 qw(Z_DEFLATED Z_DEFAULT_COMPRESSION Z_DEFAULT_STRATEGY);

    
    return (
        
            # zlib behaviour
            #'Method'   => [0, 1, Parse_unsigned,  Z_DEFLATED],
            'Level'     => [0, 1, Parse_signed,    Z_DEFAULT_COMPRESSION],
            'Strategy'  => [0, 1, Parse_signed,    Z_DEFAULT_STRATEGY],

            'CRC32'     => [0, 1, Parse_boolean,   0],
            'ADLER32'   => [0, 1, Parse_boolean,   0],
            'Merge'     => [1, 1, Parse_boolean,   0],
        );
    
    
}

sub getInverseClass
{
    return ('IO::Uncompress::RawInflate', 
                \$IO::Uncompress::RawInflate::RawInflateError);
}

sub getFileInfo
{
    my $self = shift ;
    my $params = shift;
    my $file = shift ;
    
}

use IO::Seekable qw(SEEK_SET);

sub createMerge
{
    my $self = shift ;
    my $outValue = shift ;
    my $outType = shift ;

    my ($invClass, $error_ref) = $self->getInverseClass();
    eval "require $invClass" 
        or die "aaaahhhh" ;

    my $inf = $invClass->new( $outValue, 
                             Transparent => 0, 
                             #Strict     => 1,
                             AutoClose   => 0,
                             Scan        => 1)
       or return $self->saveErrorString(undef, "Cannot create InflateScan object: $$error_ref" ) ;

    my $end_offset = 0;
    $inf->scan() 
        or return $self->saveErrorString(undef, "Error Scanning: $$error_ref", $inf->errorNo) ;
    $inf->zap($end_offset) 
        or return $self->saveErrorString(undef, "Error Zapping: $$error_ref", $inf->errorNo) ;

    my $def = *$self->{Compress} = $inf->createDeflate();

    *$self->{Header} = *$inf->{Info}{Header};
    *$self->{UnCompSize} = *$inf->{UnCompSize}->clone();
    *$self->{CompSize} = *$inf->{CompSize}->clone();
    # TODO -- fix this
    #*$self->{CompSize} = new U64(0, *$self->{UnCompSize_32bit});


    if ( $outType eq 'buffer') 
      { substr( ${ *$self->{Buffer} }, $end_offset) = '' }
    elsif ($outType eq 'handle' || $outType eq 'filename') {
        *$self->{FH} = *$inf->{FH} ;
        delete *$inf->{FH};
        *$self->{FH}->flush() ;
        *$self->{Handle} = 1 if $outType eq 'handle';

        #seek(*$self->{FH}, $end_offset, SEEK_SET) 
        *$self->{FH}->seek($end_offset, SEEK_SET) 
            or return $self->saveErrorString(undef, $!, $!) ;
    }

    return $def ;
}

#### zlib specific methods

sub deflateParams 
{
    my $self = shift ;

    my $level = shift ;
    my $strategy = shift ;

    my $status = *$self->{Compress}->deflateParams(Level => $level, Strategy => $strategy) ;
    return $self->saveErrorString(0, *$self->{Compress}{Error}, *$self->{Compress}{ErrorNo})
        if $status == STATUS_ERROR;

    return 1;    
}




1;

__END__

#line 1018
FILE   "46b14abe/IO/Compress/Zlib/Extra.pm  �#line 1 "/usr/lib/perl5/5.14/IO/Compress/Zlib/Extra.pm"
package IO::Compress::Zlib::Extra;

require 5.004 ;

use strict ;
use warnings;
use bytes;

our ($VERSION, @ISA, @EXPORT_OK, %EXPORT_TAGS);

$VERSION = '2.033';

use IO::Compress::Gzip::Constants 2.033 ;

sub ExtraFieldError
{
    return $_[0];
    return "Error with ExtraField Parameter: $_[0]" ;
}

sub validateExtraFieldPair
{
    my $pair = shift ;
    my $strict = shift;
    my $gzipMode = shift ;

    return ExtraFieldError("Not an array ref")
        unless ref $pair &&  ref $pair eq 'ARRAY';

    return ExtraFieldError("SubField must have two parts")
        unless @$pair == 2 ;

    return ExtraFieldError("SubField ID is a reference")
        if ref $pair->[0] ;

    return ExtraFieldError("SubField Data is a reference")
        if ref $pair->[1] ;

    # ID is exactly two chars   
    return ExtraFieldError("SubField ID not two chars long")
        unless length $pair->[0] == GZIP_FEXTRA_SUBFIELD_ID_SIZE ;

    # Check that the 2nd byte of the ID isn't 0    
    return ExtraFieldError("SubField ID 2nd byte is 0x00")
        if $strict && $gzipMode && substr($pair->[0], 1, 1) eq "\x00" ;

    return ExtraFieldError("SubField Data too long")
        if length $pair->[1] > GZIP_FEXTRA_SUBFIELD_MAX_SIZE ;


    return undef ;
}

sub parseRawExtra
{
    my $data     = shift ;
    my $extraRef = shift;
    my $strict   = shift;
    my $gzipMode = shift ;

    #my $lax = shift ;

    #return undef
    #    if $lax ;

    my $XLEN = length $data ;

    return ExtraFieldError("Too Large")
        if $XLEN > GZIP_FEXTRA_MAX_SIZE;

    my $offset = 0 ;
    while ($offset < $XLEN) {

        return ExtraFieldError("Truncated in FEXTRA Body Section")
            if $offset + GZIP_FEXTRA_SUBFIELD_HEADER_SIZE  > $XLEN ;

        my $id = substr($data, $offset, GZIP_FEXTRA_SUBFIELD_ID_SIZE);    
        $offset += GZIP_FEXTRA_SUBFIELD_ID_SIZE;

        my $subLen =  unpack("v", substr($data, $offset,
                                            GZIP_FEXTRA_SUBFIELD_LEN_SIZE));
        $offset += GZIP_FEXTRA_SUBFIELD_LEN_SIZE ;

        return ExtraFieldError("Truncated in FEXTRA Body Section")
            if $offset + $subLen > $XLEN ;

        my $bad = validateExtraFieldPair( [$id, 
                                           substr($data, $offset, $subLen)], 
                                           $strict, $gzipMode );
        return $bad if $bad ;
        push @$extraRef, [$id => substr($data, $offset, $subLen)]
            if defined $extraRef;;

        $offset += $subLen ;
    }

        
    return undef ;
}


sub mkSubField
{
    my $id = shift ;
    my $data = shift ;

    return $id . pack("v", length $data) . $data ;
}

sub parseExtraField
{
    my $dataRef  = $_[0];
    my $strict   = $_[1];
    my $gzipMode = $_[2];
    #my $lax     = @_ == 2 ? $_[1] : 1;


    # ExtraField can be any of
    #
    #    -ExtraField => $data
    #
    #    -ExtraField => [$id1, $data1,
    #                    $id2, $data2]
    #                     ...
    #                   ]
    #
    #    -ExtraField => [ [$id1 => $data1],
    #                     [$id2 => $data2],
    #                     ...
    #                   ]
    #
    #    -ExtraField => { $id1 => $data1,
    #                     $id2 => $data2,
    #                     ...
    #                   }
    
    if ( ! ref $dataRef ) {

        return undef
            if ! $strict;

        return parseRawExtra($dataRef, undef, 1, $gzipMode);
    }

    #my $data = $$dataRef;
    my $data = $dataRef;
    my $out = '' ;

    if (ref $data eq 'ARRAY') {    
        if (ref $data->[0]) {

            foreach my $pair (@$data) {
                return ExtraFieldError("Not list of lists")
                    unless ref $pair eq 'ARRAY' ;

                my $bad = validateExtraFieldPair($pair, $strict, $gzipMode) ;
                return $bad if $bad ;

                $out .= mkSubField(@$pair);
            }   
        }   
        else {
            return ExtraFieldError("Not even number of elements")
                unless @$data % 2  == 0;

            for (my $ix = 0; $ix <= length(@$data) -1 ; $ix += 2) {
                my $bad = validateExtraFieldPair([$data->[$ix],
                                                  $data->[$ix+1]], 
                                                 $strict, $gzipMode) ;
                return $bad if $bad ;

                $out .= mkSubField($data->[$ix], $data->[$ix+1]);
            }   
        }
    }   
    elsif (ref $data eq 'HASH') {    
        while (my ($id, $info) = each %$data) {
            my $bad = validateExtraFieldPair([$id, $info], $strict, $gzipMode);
            return $bad if $bad ;

            $out .= mkSubField($id, $info);
        }   
    }   
    else {
        return ExtraFieldError("Not a scalar, array ref or hash ref") ;
    }

    return ExtraFieldError("Too Large")
        if length $out > GZIP_FEXTRA_MAX_SIZE;

    $_[0] = $out ;

    return undef;
}

1;

__END__
FILE   )63d26eda/IO/Uncompress/Adapter/Inflate.pm  
package IO::Uncompress::Adapter::Inflate;

use strict;
use warnings;
use bytes;

use IO::Compress::Base::Common  2.033 qw(:Status);
use Compress::Raw::Zlib  2.033 qw(Z_OK Z_BUF_ERROR Z_STREAM_END Z_FINISH MAX_WBITS);

our ($VERSION);
$VERSION = '2.033';



sub mkUncompObject
{
    my $crc32   = shift || 1;
    my $adler32 = shift || 1;
    my $scan    = shift || 0;

    my $inflate ;
    my $status ;

    if ($scan)
    {
        ($inflate, $status) = new Compress::Raw::Zlib::InflateScan
                                    #LimitOutput  => 1,
                                    CRC32        => $crc32,
                                    ADLER32      => $adler32,
                                    WindowBits   => - MAX_WBITS ;
    }
    else
    {
        ($inflate, $status) = new Compress::Raw::Zlib::Inflate
                                    AppendOutput => 1,
                                    LimitOutput  => 1,
                                    CRC32        => $crc32,
                                    ADLER32      => $adler32,
                                    WindowBits   => - MAX_WBITS ;
    }

    return (undef, "Could not create Inflation object: $status", $status) 
        if $status != Z_OK ;

    return bless {'Inf'        => $inflate,
                  'CompSize'   => 0,
                  'UnCompSize' => 0,
                  'Error'      => '',
                  'ConsumesInput' => 1,
                 } ;     
    
}

sub uncompr
{
    my $self = shift ;
    my $from = shift ;
    my $to   = shift ;
    my $eof  = shift ;

    my $inf   = $self->{Inf};

    my $status = $inf->inflate($from, $to, $eof);
    $self->{ErrorNo} = $status;

    if ($status != Z_OK && $status != Z_STREAM_END && $status != Z_BUF_ERROR)
    {
        $self->{Error} = "Inflation Error: $status";
        return STATUS_ERROR;
    }
            
    return STATUS_OK        if $status == Z_BUF_ERROR ; # ???
    return STATUS_OK        if $status == Z_OK ;
    return STATUS_ENDSTREAM if $status == Z_STREAM_END ;
    return STATUS_ERROR ;
}

sub reset
{
    my $self = shift ;
    $self->{Inf}->inflateReset();

    return STATUS_OK ;
}

#sub count
#{
#    my $self = shift ;
#    $self->{Inf}->inflateCount();
#}

sub crc32
{
    my $self = shift ;
    $self->{Inf}->crc32();
}

sub compressedBytes
{
    my $self = shift ;
    $self->{Inf}->compressedBytes();
}

sub uncompressedBytes
{
    my $self = shift ;
    $self->{Inf}->uncompressedBytes();
}

sub adler32
{
    my $self = shift ;
    $self->{Inf}->adler32();
}

sub sync
{
    my $self = shift ;
    ( $self->{Inf}->inflateSync(@_) == Z_OK) 
            ? STATUS_OK 
            : STATUS_ERROR ;
}


sub getLastBlockOffset
{
    my $self = shift ;
    $self->{Inf}->getLastBlockOffset();
}

sub getEndOffset
{
    my $self = shift ;
    $self->{Inf}->getEndOffset();
}

sub resetLastBlockByte
{
    my $self = shift ;
    $self->{Inf}->resetLastBlockByte(@_);
}

sub createDeflateStream
{
    my $self = shift ;
    my $deflate = $self->{Inf}->createDeflateStream(@_);
    return bless {'Def'        => $deflate,
                  'CompSize'   => 0,
                  'UnCompSize' => 0,
                  'Error'      => '',
                 }, 'IO::Compress::Adapter::Deflate';
}

1;


__END__

FILE   6ca3e05b/IO/Uncompress/Base.pm  �'#line 1 "/usr/lib/perl5/5.14/IO/Uncompress/Base.pm"

package IO::Uncompress::Base ;

use strict ;
use warnings;
use bytes;

our (@ISA, $VERSION, @EXPORT_OK, %EXPORT_TAGS);
@ISA    = qw(Exporter IO::File);


$VERSION = '2.033';

use constant G_EOF => 0 ;
use constant G_ERR => -1 ;

use IO::Compress::Base::Common 2.033 ;
#use Parse::Parameters ;

use IO::File ;
use Symbol;
use Scalar::Util qw(readonly);
use List::Util qw(min);
use Carp ;

%EXPORT_TAGS = ( );
push @{ $EXPORT_TAGS{all} }, @EXPORT_OK ;
#Exporter::export_ok_tags('all') ;



sub smartRead
{
    my $self = $_[0];
    my $out = $_[1];
    my $size = $_[2];
    #$$out = "" ;
    $$out = "" ;

    my $offset = 0 ;
    my $status = 1;


    if (defined *$self->{InputLength}) {
        return 0
            if *$self->{InputLengthRemaining} <= 0 ;
        $size = min($size, *$self->{InputLengthRemaining});
    }

    if ( length *$self->{Prime} ) {
        #$$out = substr(*$self->{Prime}, 0, $size, '') ;
        $$out = substr(*$self->{Prime}, 0, $size) ;
        substr(*$self->{Prime}, 0, $size) =  '' ;
        if (length $$out == $size) {
            *$self->{InputLengthRemaining} -= length $$out
                if defined *$self->{InputLength};

            return length $$out ;
        }
        $offset = length $$out ;
    }

    my $get_size = $size - $offset ;

    if (defined *$self->{FH}) {
        if ($offset) {
            # Not using this 
            #
            #  *$self->{FH}->read($$out, $get_size, $offset);
            #
            # because the filehandle may not support the offset parameter
            # An example is Net::FTP
            my $tmp = '';
            $status = *$self->{FH}->read($tmp, $get_size) ;
            substr($$out, $offset) = $tmp
                if defined $status && $status > 0 ;
        }
        else
          { $status = *$self->{FH}->read($$out, $get_size) }
    }
    elsif (defined *$self->{InputEvent}) {
        my $got = 1 ;
        while (length $$out < $size) {
            last 
                if ($got = *$self->{InputEvent}->($$out, $get_size)) <= 0;
        }

        if (length $$out > $size ) {
            #*$self->{Prime} = substr($$out, $size, length($$out), '');
            *$self->{Prime} = substr($$out, $size, length($$out));
            substr($$out, $size, length($$out)) =  '';
        }

       *$self->{EventEof} = 1 if $got <= 0 ;
    }
    else {
       no warnings 'uninitialized';
       my $buf = *$self->{Buffer} ;
       $$buf = '' unless defined $$buf ;
       #$$out = '' unless defined $$out ;
       substr($$out, $offset) = substr($$buf, *$self->{BufferOffset}, $get_size);
       if (*$self->{ConsumeInput})
         { substr($$buf, 0, $get_size) = '' }
       else  
         { *$self->{BufferOffset} += length($$out) - $offset }
    }

    *$self->{InputLengthRemaining} -= length($$out) #- $offset 
        if defined *$self->{InputLength};
        
    if (! defined $status) {
        $self->saveStatus($!) ;
        return STATUS_ERROR;
    }

    $self->saveStatus(length $$out < 0 ? STATUS_ERROR : STATUS_OK) ;

    return length $$out;
}

sub pushBack
{
    my $self = shift ;

    return if ! defined $_[0] || length $_[0] == 0 ;

    if (defined *$self->{FH} || defined *$self->{InputEvent} ) {
        *$self->{Prime} = $_[0] . *$self->{Prime} ;
        *$self->{InputLengthRemaining} += length($_[0]);
    }
    else {
        my $len = length $_[0];

        if($len > *$self->{BufferOffset}) {
            *$self->{Prime} = substr($_[0], 0, $len - *$self->{BufferOffset}) . *$self->{Prime} ;
            *$self->{InputLengthRemaining} = *$self->{InputLength};
            *$self->{BufferOffset} = 0
        }
        else {
            *$self->{InputLengthRemaining} += length($_[0]);
            *$self->{BufferOffset} -= length($_[0]) ;
        }
    }
}

sub smartSeek
{
    my $self   = shift ;
    my $offset = shift ;
    my $truncate = shift;
    #print "smartSeek to $offset\n";

    # TODO -- need to take prime into account
    if (defined *$self->{FH})
      { *$self->{FH}->seek($offset, SEEK_SET) }
    else {
        *$self->{BufferOffset} = $offset ;
        substr(${ *$self->{Buffer} }, *$self->{BufferOffset}) = ''
            if $truncate;
        return 1;
    }
}

sub smartWrite
{
    my $self   = shift ;
    my $out_data = shift ;

    if (defined *$self->{FH}) {
        # flush needed for 5.8.0 
        defined *$self->{FH}->write($out_data, length $out_data) &&
        defined *$self->{FH}->flush() ;
    }
    else {
       my $buf = *$self->{Buffer} ;
       substr($$buf, *$self->{BufferOffset}, length $out_data) = $out_data ;
       *$self->{BufferOffset} += length($out_data) ;
       return 1;
    }
}

sub smartReadExact
{
    return $_[0]->smartRead($_[1], $_[2]) == $_[2];
}

sub smartEof
{
    my ($self) = $_[0];
    local $.; 

    return 0 if length *$self->{Prime} || *$self->{PushMode};

    if (defined *$self->{FH})
    {
        # Could use
        #
        #  *$self->{FH}->eof() 
        #
        # here, but this can cause trouble if
        # the filehandle is itself a tied handle, but it uses sysread.
        # Then we get into mixing buffered & non-buffered IO, which will cause trouble

        my $info = $self->getErrInfo();
        
        my $buffer = '';
        my $status = $self->smartRead(\$buffer, 1);
        $self->pushBack($buffer) if length $buffer;
        $self->setErrInfo($info);
        
        return $status == 0 ;
    }
    elsif (defined *$self->{InputEvent})
     { *$self->{EventEof} }
    else 
     { *$self->{BufferOffset} >= length(${ *$self->{Buffer} }) }
}

sub clearError
{
    my $self   = shift ;

    *$self->{ErrorNo}  =  0 ;
    ${ *$self->{Error} } = '' ;
}

sub getErrInfo
{
    my $self   = shift ;

    return [ *$self->{ErrorNo}, ${ *$self->{Error} } ] ;
}

sub setErrInfo
{
    my $self   = shift ;
    my $ref    = shift;

    *$self->{ErrorNo}  =  $ref->[0] ;
    ${ *$self->{Error} } = $ref->[1] ;
}

sub saveStatus
{
    my $self   = shift ;
    my $errno = shift() + 0 ;
    #return $errno unless $errno || ! defined *$self->{ErrorNo};
    #return $errno unless $errno ;

    *$self->{ErrorNo}  = $errno;
    ${ *$self->{Error} } = '' ;

    return *$self->{ErrorNo} ;
}


sub saveErrorString
{
    my $self   = shift ;
    my $retval = shift ;

    #return $retval if ${ *$self->{Error} };

    ${ *$self->{Error} } = shift ;
    *$self->{ErrorNo} = shift() + 0 if @_ ;

    #warn "saveErrorString: " . ${ *$self->{Error} } . " " . *$self->{Error} . "\n" ;
    return $retval;
}

sub croakError
{
    my $self   = shift ;
    $self->saveErrorString(0, $_[0]);
    croak $_[0];
}


sub closeError
{
    my $self = shift ;
    my $retval = shift ;

    my $errno = *$self->{ErrorNo};
    my $error = ${ *$self->{Error} };

    $self->close();

    *$self->{ErrorNo} = $errno ;
    ${ *$self->{Error} } = $error ;

    return $retval;
}

sub error
{
    my $self   = shift ;
    return ${ *$self->{Error} } ;
}

sub errorNo
{
    my $self   = shift ;
    return *$self->{ErrorNo};
}

sub HeaderError
{
    my ($self) = shift;
    return $self->saveErrorString(undef, "Header Error: $_[0]", STATUS_ERROR);
}

sub TrailerError
{
    my ($self) = shift;
    return $self->saveErrorString(G_ERR, "Trailer Error: $_[0]", STATUS_ERROR);
}

sub TruncatedHeader
{
    my ($self) = shift;
    return $self->HeaderError("Truncated in $_[0] Section");
}

sub TruncatedTrailer
{
    my ($self) = shift;
    return $self->TrailerError("Truncated in $_[0] Section");
}

sub postCheckParams
{
    return 1;
}

sub checkParams
{
    my $self = shift ;
    my $class = shift ;

    my $got = shift || IO::Compress::Base::Parameters::new();
    
    my $Valid = {
                    'BlockSize'     => [1, 1, Parse_unsigned, 16 * 1024],
                    'AutoClose'     => [1, 1, Parse_boolean,  0],
                    'Strict'        => [1, 1, Parse_boolean,  0],
                    'Append'        => [1, 1, Parse_boolean,  0],
                    'Prime'         => [1, 1, Parse_any,      undef],
                    'MultiStream'   => [1, 1, Parse_boolean,  0],
                    'Transparent'   => [1, 1, Parse_any,      1],
                    'Scan'          => [1, 1, Parse_boolean,  0],
                    'InputLength'   => [1, 1, Parse_unsigned, undef],
                    'BinModeOut'    => [1, 1, Parse_boolean,  0],
                    #'Encode'        => [1, 1, Parse_any,       undef],

                   #'ConsumeInput'  => [1, 1, Parse_boolean,  0],

                    $self->getExtraParams(),

                    #'Todo - Revert to ordinary file on end Z_STREAM_END'=> 0,
                    # ContinueAfterEof
                } ;

    $Valid->{TrailingData} = [1, 1, Parse_writable_scalar, undef]
        if  *$self->{OneShot} ;
        
    $got->parse($Valid, @_ ) 
        or $self->croakError("${class}: $got->{Error}")  ;

    $self->postCheckParams($got) 
        or $self->croakError("${class}: " . $self->error())  ;

    return $got;
}

sub _create
{
    my $obj = shift;
    my $got = shift;
    my $append_mode = shift ;

    my $class = ref $obj;
    $obj->croakError("$class: Missing Input parameter")
        if ! @_ && ! $got ;

    my $inValue = shift ;

    *$obj->{OneShot}           = 0 ;

    if (! $got)
    {
        $got = $obj->checkParams($class, undef, @_)
            or return undef ;
    }

    my $inType  = whatIsInput($inValue, 1);

    $obj->ckInputParam($class, $inValue, 1) 
        or return undef ;

    *$obj->{InNew} = 1;

    $obj->ckParams($got)
        or $obj->croakError("${class}: " . *$obj->{Error});

    if ($inType eq 'buffer' || $inType eq 'code') {
        *$obj->{Buffer} = $inValue ;        
        *$obj->{InputEvent} = $inValue 
           if $inType eq 'code' ;
    }
    else {
        if ($inType eq 'handle') {
            *$obj->{FH} = $inValue ;
            *$obj->{Handle} = 1 ;

            # Need to rewind for Scan
            *$obj->{FH}->seek(0, SEEK_SET) 
                if $got->value('Scan');
        }  
        else {    
            no warnings ;
            my $mode = '<';
            $mode = '+<' if $got->value('Scan');
            *$obj->{StdIO} = ($inValue eq '-');
            *$obj->{FH} = new IO::File "$mode $inValue"
                or return $obj->saveErrorString(undef, "cannot open file '$inValue': $!", $!) ;
        }
        
        *$obj->{LineNo} = $. = 0;
        setBinModeInput(*$obj->{FH}) ;

        my $buff = "" ;
        *$obj->{Buffer} = \$buff ;
    }

    if ($got->parsed('Encode')) { 
        my $want_encoding = $got->value('Encode');
        *$obj->{Encoding} = getEncoding($obj, $class, $want_encoding);
    }


    *$obj->{InputLength}       = $got->parsed('InputLength') 
                                    ? $got->value('InputLength')
                                    : undef ;
    *$obj->{InputLengthRemaining} = $got->value('InputLength');
    *$obj->{BufferOffset}      = 0 ;
    *$obj->{AutoClose}         = $got->value('AutoClose');
    *$obj->{Strict}            = $got->value('Strict');
    *$obj->{BlockSize}         = $got->value('BlockSize');
    *$obj->{Append}            = $got->value('Append');
    *$obj->{AppendOutput}      = $append_mode || $got->value('Append');
    *$obj->{ConsumeInput}      = $got->value('ConsumeInput');
    *$obj->{Transparent}       = $got->value('Transparent');
    *$obj->{MultiStream}       = $got->value('MultiStream');

    # TODO - move these two into RawDeflate
    *$obj->{Scan}              = $got->value('Scan');
    *$obj->{ParseExtra}        = $got->value('ParseExtra') 
                                  || $got->value('Strict')  ;
    *$obj->{Type}              = '';
    *$obj->{Prime}             = $got->value('Prime') || '' ;
    *$obj->{Pending}           = '';
    *$obj->{Plain}             = 0;
    *$obj->{PlainBytesRead}    = 0;
    *$obj->{InflatedBytesRead} = 0;
    *$obj->{UnCompSize}        = new U64;
    *$obj->{CompSize}          = new U64;
    *$obj->{TotalInflatedBytesRead} = 0;
    *$obj->{NewStream}         = 0 ;
    *$obj->{EventEof}          = 0 ;
    *$obj->{ClassName}         = $class ;
    *$obj->{Params}            = $got ;

    if (*$obj->{ConsumeInput}) {
        *$obj->{InNew} = 0;
        *$obj->{Closed} = 0;
        return $obj
    }

    my $status = $obj->mkUncomp($got);

    return undef
        unless defined $status;

    if ( !  $status) {
        return undef 
            unless *$obj->{Transparent};

        $obj->clearError();
        *$obj->{Type} = 'plain';
        *$obj->{Plain} = 1;
        #$status = $obj->mkIdentityUncomp($class, $got);
        $obj->pushBack(*$obj->{HeaderPending})  ;
    }

    push @{ *$obj->{InfoList} }, *$obj->{Info} ;

    $obj->saveStatus(STATUS_OK) ;
    *$obj->{InNew} = 0;
    *$obj->{Closed} = 0;

    return $obj;
}

sub ckInputParam
{
    my $self = shift ;
    my $from = shift ;
    my $inType = whatIsInput($_[0], $_[1]);

    $self->croakError("$from: input parameter not a filename, filehandle, array ref or scalar ref")
        if ! $inType ;

#    if ($inType  eq 'filename' )
#    {
#        return $self->saveErrorString(1, "$from: input filename is undef or null string", STATUS_ERROR)
#            if ! defined $_[0] || $_[0] eq ''  ;
#
#        if ($_[0] ne '-' && ! -e $_[0] )
#        {
#            return $self->saveErrorString(1, 
#                            "input file '$_[0]' does not exist", STATUS_ERROR);
#        }
#    }

    return 1;
}


sub _inf
{
    my $obj = shift ;

    my $class = (caller)[0] ;
    my $name = (caller(1))[3] ;

    $obj->croakError("$name: expected at least 1 parameters\n")
        unless @_ >= 1 ;

    my $input = shift ;
    my $haveOut = @_ ;
    my $output = shift ;


    my $x = new IO::Compress::Base::Validator($class, *$obj->{Error}, $name, $input, $output)
        or return undef ;
    
    push @_, $output if $haveOut && $x->{Hash};

    *$obj->{OneShot} = 1 ;
    
    my $got = $obj->checkParams($name, undef, @_)
        or return undef ;

    if ($got->parsed('TrailingData'))
    {
        *$obj->{TrailingData} = $got->value('TrailingData');
    }

    *$obj->{MultiStream} = $got->value('MultiStream');
    $got->value('MultiStream', 0);

    $x->{Got} = $got ;

#    if ($x->{Hash})
#    {
#        while (my($k, $v) = each %$input)
#        {
#            $v = \$input->{$k} 
#                unless defined $v ;
#
#            $obj->_singleTarget($x, $k, $v, @_)
#                or return undef ;
#        }
#
#        return keys %$input ;
#    }
    
    if ($x->{GlobMap})
    {
        $x->{oneInput} = 1 ;
        foreach my $pair (@{ $x->{Pairs} })
        {
            my ($from, $to) = @$pair ;
            $obj->_singleTarget($x, $from, $to, @_)
                or return undef ;
        }

        return scalar @{ $x->{Pairs} } ;
    }

    if (! $x->{oneOutput} )
    {
        my $inFile = ($x->{inType} eq 'filenames' 
                        || $x->{inType} eq 'filename');

        $x->{inType} = $inFile ? 'filename' : 'buffer';
        
        foreach my $in ($x->{oneInput} ? $input : @$input)
        {
            my $out ;
            $x->{oneInput} = 1 ;

            $obj->_singleTarget($x, $in, $output, @_)
                or return undef ;
        }

        return 1 ;
    }

    # finally the 1 to 1 and n to 1
    return $obj->_singleTarget($x, $input, $output, @_);

    croak "should not be here" ;
}

sub retErr
{
    my $x = shift ;
    my $string = shift ;

    ${ $x->{Error} } = $string ;

    return undef ;
}

sub _singleTarget
{
    my $self      = shift ;
    my $x         = shift ;
    my $input     = shift;
    my $output    = shift;
    
    my $buff = '';
    $x->{buff} = \$buff ;

    my $fh ;
    if ($x->{outType} eq 'filename') {
        my $mode = '>' ;
        $mode = '>>'
            if $x->{Got}->value('Append') ;
        $x->{fh} = new IO::File "$mode $output" 
            or return retErr($x, "cannot open file '$output': $!") ;
        binmode $x->{fh} if $x->{Got}->valueOrDefault('BinModeOut');

    }

    elsif ($x->{outType} eq 'handle') {
        $x->{fh} = $output;
        binmode $x->{fh} if $x->{Got}->valueOrDefault('BinModeOut');
        if ($x->{Got}->value('Append')) {
                seek($x->{fh}, 0, SEEK_END)
                    or return retErr($x, "Cannot seek to end of output filehandle: $!") ;
            }
    }

    
    elsif ($x->{outType} eq 'buffer' )
    {
        $$output = '' 
            unless $x->{Got}->value('Append');
        $x->{buff} = $output ;
    }

    if ($x->{oneInput})
    {
        defined $self->_rd2($x, $input, $output)
            or return undef; 
    }
    else
    {
        for my $element ( ($x->{inType} eq 'hash') ? keys %$input : @$input)
        {
            defined $self->_rd2($x, $element, $output) 
                or return undef ;
        }
    }


    if ( ($x->{outType} eq 'filename' && $output ne '-') || 
         ($x->{outType} eq 'handle' && $x->{Got}->value('AutoClose'))) {
        $x->{fh}->close() 
            or return retErr($x, $!); 
        delete $x->{fh};
    }

    return 1 ;
}

sub _rd2
{
    my $self      = shift ;
    my $x         = shift ;
    my $input     = shift;
    my $output    = shift;
        
    my $z = createSelfTiedObject($x->{Class}, *$self->{Error});
    
    $z->_create($x->{Got}, 1, $input, @_)
        or return undef ;

    my $status ;
    my $fh = $x->{fh};
    
    while (1) {

        while (($status = $z->read($x->{buff})) > 0) {
            if ($fh) {
                print $fh ${ $x->{buff} }
                    or return $z->saveErrorString(undef, "Error writing to output file: $!", $!);
                ${ $x->{buff} } = '' ;
            }
        }

        if (! $x->{oneOutput} ) {
            my $ot = $x->{outType} ;

            if ($ot eq 'array') 
              { push @$output, $x->{buff} }
            elsif ($ot eq 'hash') 
              { $output->{$input} = $x->{buff} }

            my $buff = '';
            $x->{buff} = \$buff;
        }

        last if $status < 0 || $z->smartEof();
        #last if $status < 0 ;

        last 
            unless *$self->{MultiStream};

        $status = $z->nextStream();

        last 
            unless $status == 1 ;
    }

    return $z->closeError(undef)
        if $status < 0 ;

    ${ *$self->{TrailingData} } = $z->trailingData()
        if defined *$self->{TrailingData} ;

    $z->close() 
        or return undef ;

    return 1 ;
}

sub TIEHANDLE
{
    return $_[0] if ref($_[0]);
    die "OOPS\n" ;

}
  
sub UNTIE
{
    my $self = shift ;
}


sub getHeaderInfo
{
    my $self = shift ;
    wantarray ? @{ *$self->{InfoList} } : *$self->{Info};
}

sub readBlock
{
    my $self = shift ;
    my $buff = shift ;
    my $size = shift ;

    if (defined *$self->{CompressedInputLength}) {
        if (*$self->{CompressedInputLengthRemaining} == 0) {
            delete *$self->{CompressedInputLength};
            *$self->{CompressedInputLengthDone} = 1;
            return STATUS_OK ;
        }
        $size = min($size, *$self->{CompressedInputLengthRemaining} );
        *$self->{CompressedInputLengthRemaining} -= $size ;
    }
    
    my $status = $self->smartRead($buff, $size) ;
    return $self->saveErrorString(STATUS_ERROR, "Error Reading Data: $!")
        if $status == STATUS_ERROR  ;

    if ($status == 0 ) {
        *$self->{Closed} = 1 ;
        *$self->{EndStream} = 1 ;
        return $self->saveErrorString(STATUS_ERROR, "unexpected end of file", STATUS_ERROR);
    }

    return STATUS_OK;
}

sub postBlockChk
{
    return STATUS_OK;
}

sub _raw_read
{
    # return codes
    # >0 - ok, number of bytes read
    # =0 - ok, eof
    # <0 - not ok
    
    my $self = shift ;

    return G_EOF if *$self->{Closed} ;
    #return G_EOF if !length *$self->{Pending} && *$self->{EndStream} ;
    return G_EOF if *$self->{EndStream} ;

    my $buffer = shift ;
    my $scan_mode = shift ;

    if (*$self->{Plain}) {
        my $tmp_buff ;
        my $len = $self->smartRead(\$tmp_buff, *$self->{BlockSize}) ;
        
        return $self->saveErrorString(G_ERR, "Error reading data: $!", $!) 
                if $len == STATUS_ERROR ;

        if ($len == 0 ) {
            *$self->{EndStream} = 1 ;
        }
        else {
            *$self->{PlainBytesRead} += $len ;
            $$buffer .= $tmp_buff;
        }

        return $len ;
    }

    if (*$self->{NewStream}) {

        $self->gotoNextStream() > 0
            or return G_ERR;

        # For the headers that actually uncompressed data, put the
        # uncompressed data into the output buffer.
        $$buffer .=  *$self->{Pending} ;
        my $len = length  *$self->{Pending} ;
        *$self->{Pending} = '';
        return $len; 
    }

    my $temp_buf = '';
    my $outSize = 0;
    my $status = $self->readBlock(\$temp_buf, *$self->{BlockSize}, $outSize) ;
    return G_ERR
        if $status == STATUS_ERROR  ;

    my $buf_len = 0;
    if ($status == STATUS_OK) {
        my $beforeC_len = length $temp_buf;
        my $before_len = defined $$buffer ? length $$buffer : 0 ;
        $status = *$self->{Uncomp}->uncompr(\$temp_buf, $buffer,
                                    defined *$self->{CompressedInputLengthDone} ||
                                                $self->smartEof(), $outSize);
                                                
        # Remember the input buffer if it wasn't consumed completely
        $self->pushBack($temp_buf) if *$self->{Uncomp}{ConsumesInput};

        return $self->saveErrorString(G_ERR, *$self->{Uncomp}{Error}, *$self->{Uncomp}{ErrorNo})
            if $self->saveStatus($status) == STATUS_ERROR;    

        $self->postBlockChk($buffer, $before_len) == STATUS_OK
            or return G_ERR;

        $buf_len = defined $$buffer ? length($$buffer) - $before_len : 0;
    
        *$self->{CompSize}->add($beforeC_len - length $temp_buf) ;

        *$self->{InflatedBytesRead} += $buf_len ;
        *$self->{TotalInflatedBytesRead} += $buf_len ;
        *$self->{UnCompSize}->add($buf_len) ;

        $self->filterUncompressed($buffer);

        if (*$self->{Encoding}) {
            $$buffer = *$self->{Encoding}->decode($$buffer);
        }
    }

    if ($status == STATUS_ENDSTREAM) {

        *$self->{EndStream} = 1 ;
#$self->pushBack($temp_buf)  ;
#$temp_buf = '';

        my $trailer;
        my $trailer_size = *$self->{Info}{TrailerLength} ;
        my $got = 0;
        if (*$self->{Info}{TrailerLength})
        {
            $got = $self->smartRead(\$trailer, $trailer_size) ;
        }

        if ($got == $trailer_size) {
            $self->chkTrailer($trailer) == STATUS_OK
                or return G_ERR;
        }
        else {
            return $self->TrailerError("trailer truncated. Expected " . 
                                      "$trailer_size bytes, got $got")
                if *$self->{Strict};
            $self->pushBack($trailer)  ;
        }

        # TODO - if want to file file pointer, do it here

        if (! $self->smartEof()) {
            *$self->{NewStream} = 1 ;

            if (*$self->{MultiStream}) {
                *$self->{EndStream} = 0 ;
                return $buf_len ;
            }
        }

    }
    

    # return the number of uncompressed bytes read
    return $buf_len ;
}

sub reset
{
    my $self = shift ;

    return *$self->{Uncomp}->reset();
}

sub filterUncompressed
{
}

#sub isEndStream
#{
#    my $self = shift ;
#    return *$self->{NewStream} ||
#           *$self->{EndStream} ;
#}

sub nextStream
{
    my $self = shift ;

    my $status = $self->gotoNextStream();
    $status == 1
        or return $status ;

    *$self->{TotalInflatedBytesRead} = 0 ;
    *$self->{LineNo} = $. = 0;

    return 1;
}

sub gotoNextStream
{
    my $self = shift ;

    if (! *$self->{NewStream}) {
        my $status = 1;
        my $buffer ;

        # TODO - make this more efficient if know the offset for the end of
        # the stream and seekable
        $status = $self->read($buffer) 
            while $status > 0 ;

        return $status
            if $status < 0;
    }

    *$self->{NewStream} = 0 ;
    *$self->{EndStream} = 0 ;
    $self->reset();
    *$self->{UnCompSize}->reset();
    *$self->{CompSize}->reset();

    my $magic = $self->ckMagic();
    #*$self->{EndStream} = 0 ;

    if ( ! defined $magic) {
        if (! *$self->{Transparent} || $self->eof())
        {
            *$self->{EndStream} = 1 ;
            return 0;
        }

        $self->clearError();
        *$self->{Type} = 'plain';
        *$self->{Plain} = 1;
        $self->pushBack(*$self->{HeaderPending})  ;
    }
    else
    {
        *$self->{Info} = $self->readHeader($magic);

        if ( ! defined *$self->{Info} ) {
            *$self->{EndStream} = 1 ;
            return -1;
        }
    }

    push @{ *$self->{InfoList} }, *$self->{Info} ;

    return 1; 
}

sub streamCount
{
    my $self = shift ;
    return 1 if ! defined *$self->{InfoList};
    return scalar @{ *$self->{InfoList} }  ;
}

sub read
{
    # return codes
    # >0 - ok, number of bytes read
    # =0 - ok, eof
    # <0 - not ok
    
    my $self = shift ;

    return G_EOF if *$self->{Closed} ;

    my $buffer ;

    if (ref $_[0] ) {
        $self->croakError(*$self->{ClassName} . "::read: buffer parameter is read-only")
            if readonly(${ $_[0] });

        $self->croakError(*$self->{ClassName} . "::read: not a scalar reference $_[0]" )
            unless ref $_[0] eq 'SCALAR' ;
        $buffer = $_[0] ;
    }
    else {
        $self->croakError(*$self->{ClassName} . "::read: buffer parameter is read-only")
            if readonly($_[0]);

        $buffer = \$_[0] ;
    }

    my $length = $_[1] ;
    my $offset = $_[2] || 0;

    if (! *$self->{AppendOutput}) {
        if (! $offset) {    
            $$buffer = '' ;
        }
        else {
            if ($offset > length($$buffer)) {
                $$buffer .= "\x00" x ($offset - length($$buffer));
            }
            else {
                substr($$buffer, $offset) = '';
            }
        }
    }

    return G_EOF if !length *$self->{Pending} && *$self->{EndStream} ;

    # the core read will return 0 if asked for 0 bytes
    return 0 if defined $length && $length == 0 ;

    $length = $length || 0;

    $self->croakError(*$self->{ClassName} . "::read: length parameter is negative")
        if $length < 0 ;

    # Short-circuit if this is a simple read, with no length
    # or offset specified.
    unless ( $length || $offset) {
        if (length *$self->{Pending}) {
            $$buffer .= *$self->{Pending} ;
            my $len = length *$self->{Pending};
            *$self->{Pending} = '' ;
            return $len ;
        }
        else {
            my $len = 0;
            $len = $self->_raw_read($buffer) 
                while ! *$self->{EndStream} && $len == 0 ;
            return $len ;
        }
    }

    # Need to jump through more hoops - either length or offset 
    # or both are specified.
    my $out_buffer = *$self->{Pending} ;
    *$self->{Pending} = '';


    while (! *$self->{EndStream} && length($out_buffer) < $length)
    {
        my $buf_len = $self->_raw_read(\$out_buffer);
        return $buf_len 
            if $buf_len < 0 ;
    }

    $length = length $out_buffer 
        if length($out_buffer) < $length ;

    return 0 
        if $length == 0 ;

    $$buffer = '' 
        if ! defined $$buffer;

    $offset = length $$buffer
        if *$self->{AppendOutput} ;

    *$self->{Pending} = $out_buffer;
    $out_buffer = \*$self->{Pending} ;

    #substr($$buffer, $offset) = substr($$out_buffer, 0, $length, '') ;
    substr($$buffer, $offset) = substr($$out_buffer, 0, $length) ;
    substr($$out_buffer, 0, $length) =  '' ;

    return $length ;
}

sub _getline
{
    my $self = shift ;
    my $status = 0 ;

    # Slurp Mode
    if ( ! defined $/ ) {
        my $data ;
        1 while ($status = $self->read($data)) > 0 ;
        return $status < 0 ? \undef : \$data ;
    }

    # Record Mode
    if ( ref $/ eq 'SCALAR' && ${$/} =~ /^\d+$/ && ${$/} > 0) {
        my $reclen = ${$/} ;
        my $data ;
        $status = $self->read($data, $reclen) ;
        return $status < 0 ? \undef : \$data ;
    }

    # Paragraph Mode
    if ( ! length $/ ) {
        my $paragraph ;    
        while (($status = $self->read($paragraph)) > 0 ) {
            if ($paragraph =~ s/^(.*?\n\n+)//s) {
                *$self->{Pending}  = $paragraph ;
                my $par = $1 ;
                return \$par ;
            }
        }
        return $status < 0 ? \undef : \$paragraph;
    }

    # $/ isn't empty, or a reference, so it's Line Mode.
    {
        my $line ;    
        my $offset;
        my $p = \*$self->{Pending}  ;

        if (length(*$self->{Pending}) && 
                    ($offset = index(*$self->{Pending}, $/)) >=0) {
            my $l = substr(*$self->{Pending}, 0, $offset + length $/ );
            substr(*$self->{Pending}, 0, $offset + length $/) = '';    
            return \$l;
        }

        while (($status = $self->read($line)) > 0 ) {
            my $offset = index($line, $/);
            if ($offset >= 0) {
                my $l = substr($line, 0, $offset + length $/ );
                substr($line, 0, $offset + length $/) = '';    
                $$p = $line;
                return \$l;
            }
        }

        return $status < 0 ? \undef : \$line;
    }
}

sub getline
{
    my $self = shift;
    my $current_append = *$self->{AppendOutput} ;
    *$self->{AppendOutput} = 1;
    my $lineref = $self->_getline();
    $. = ++ *$self->{LineNo} if defined $$lineref ;
    *$self->{AppendOutput} = $current_append;
    return $$lineref ;
}

sub getlines
{
    my $self = shift;
    $self->croakError(*$self->{ClassName} . 
            "::getlines: called in scalar context\n") unless wantarray;
    my($line, @lines);
    push(@lines, $line) 
        while defined($line = $self->getline);
    return @lines;
}

sub READLINE
{
    goto &getlines if wantarray;
    goto &getline;
}

sub getc
{
    my $self = shift;
    my $buf;
    return $buf if $self->read($buf, 1);
    return undef;
}

sub ungetc
{
    my $self = shift;
    *$self->{Pending} = ""  unless defined *$self->{Pending} ;    
    *$self->{Pending} = $_[0] . *$self->{Pending} ;    
}


sub trailingData
{
    my $self = shift ;

    if (defined *$self->{FH} || defined *$self->{InputEvent} ) {
        return *$self->{Prime} ;
    }
    else {
        my $buf = *$self->{Buffer} ;
        my $offset = *$self->{BufferOffset} ;
        return substr($$buf, $offset) ;
    }
}


sub eof
{
    my $self = shift ;

    return (*$self->{Closed} ||
              (!length *$self->{Pending} 
                && ( $self->smartEof() || *$self->{EndStream}))) ;
}

sub tell
{
    my $self = shift ;

    my $in ;
    if (*$self->{Plain}) {
        $in = *$self->{PlainBytesRead} ;
    }
    else {
        $in = *$self->{TotalInflatedBytesRead} ;
    }

    my $pending = length *$self->{Pending} ;

    return 0 if $pending > $in ;
    return $in - $pending ;
}

sub close
{
    # todo - what to do if close is called before the end of the gzip file
    #        do we remember any trailing data?
    my $self = shift ;

    return 1 if *$self->{Closed} ;

    untie *$self 
        if $] >= 5.008 ;

    my $status = 1 ;

    if (defined *$self->{FH}) {
        if ((! *$self->{Handle} || *$self->{AutoClose}) && ! *$self->{StdIO}) {
        #if ( *$self->{AutoClose}) {
            local $.; 
            $! = 0 ;
            $status = *$self->{FH}->close();
            return $self->saveErrorString(0, $!, $!)
                if !*$self->{InNew} && $self->saveStatus($!) != 0 ;
        }
        delete *$self->{FH} ;
        $! = 0 ;
    }
    *$self->{Closed} = 1 ;

    return 1;
}

sub DESTROY
{
    my $self = shift ;
    local ($., $@, $!, $^E, $?);

    $self->close() ;
}

sub seek
{
    my $self     = shift ;
    my $position = shift;
    my $whence   = shift ;

    my $here = $self->tell() ;
    my $target = 0 ;


    if ($whence == SEEK_SET) {
        $target = $position ;
    }
    elsif ($whence == SEEK_CUR) {
        $target = $here + $position ;
    }
    elsif ($whence == SEEK_END) {
        $target = $position ;
        $self->croakError(*$self->{ClassName} . "::seek: SEEK_END not allowed") ;
    }
    else {
        $self->croakError(*$self->{ClassName} ."::seek: unknown value, $whence, for whence parameter");
    }

    # short circuit if seeking to current offset
    if ($target == $here) {
        # On ordinary filehandles, seeking to the current
        # position also clears the EOF condition, so we
        # emulate this behavior locally while simultaneously
        # cascading it to the underlying filehandle
        if (*$self->{Plain}) {
            *$self->{EndStream} = 0;
            seek(*$self->{FH},0,1) if *$self->{FH};
        }
        return 1;
    }

    # Outlaw any attempt to seek backwards
    $self->croakError( *$self->{ClassName} ."::seek: cannot seek backwards")
        if $target < $here ;

    # Walk the file to the new offset
    my $offset = $target - $here ;

    my $got;
    while (($got = $self->read(my $buffer, min($offset, *$self->{BlockSize})) ) > 0)
    {
        $offset -= $got;
        last if $offset == 0 ;
    }

    $here = $self->tell() ;
    return $offset == 0 ? 1 : 0 ;
}

sub fileno
{
    my $self = shift ;
    return defined *$self->{FH} 
           ? fileno *$self->{FH} 
           : undef ;
}

sub binmode
{
    1;
#    my $self     = shift ;
#    return defined *$self->{FH} 
#            ? binmode *$self->{FH} 
#            : 1 ;
}

sub opened
{
    my $self     = shift ;
    return ! *$self->{Closed} ;
}

sub autoflush
{
    my $self     = shift ;
    return defined *$self->{FH} 
            ? *$self->{FH}->autoflush(@_) 
            : undef ;
}

sub input_line_number
{
    my $self = shift ;
    my $last = *$self->{LineNo};
    $. = *$self->{LineNo} = $_[1] if @_ ;
    return $last;
}


*BINMODE  = \&binmode;
*SEEK     = \&seek; 
*READ     = \&read;
*sysread  = \&read;
*TELL     = \&tell;
*EOF      = \&eof;

*FILENO   = \&fileno;
*CLOSE    = \&close;

sub _notAvailable
{
    my $name = shift ;
    #return sub { croak "$name Not Available" ; } ;
    return sub { croak "$name Not Available: File opened only for intput" ; } ;
}


*print    = _notAvailable('print');
*PRINT    = _notAvailable('print');
*printf   = _notAvailable('printf');
*PRINTF   = _notAvailable('printf');
*write    = _notAvailable('write');
*WRITE    = _notAvailable('write');

#*sysread  = \&read;
#*syswrite = \&_notAvailable;



package IO::Uncompress::Base ;


1 ;
__END__

#line 1484
FILE    1a0f7a54/IO/Uncompress/Gunzip.pm  "#line 1 "/usr/lib/perl5/5.14/IO/Uncompress/Gunzip.pm"

package IO::Uncompress::Gunzip ;

require 5.004 ;

# for RFC1952

use strict ;
use warnings;
use bytes;

use IO::Uncompress::RawInflate 2.033 ;

use Compress::Raw::Zlib 2.033 qw( crc32 ) ;
use IO::Compress::Base::Common 2.033 qw(:Status createSelfTiedObject);
use IO::Compress::Gzip::Constants 2.033 ;
use IO::Compress::Zlib::Extra 2.033 ;

require Exporter ;

our ($VERSION, @ISA, @EXPORT_OK, %EXPORT_TAGS, $GunzipError);

@ISA = qw( Exporter IO::Uncompress::RawInflate );
@EXPORT_OK = qw( $GunzipError gunzip );
%EXPORT_TAGS = %IO::Uncompress::RawInflate::DEFLATE_CONSTANTS ;
push @{ $EXPORT_TAGS{all} }, @EXPORT_OK ;
Exporter::export_ok_tags('all');

$GunzipError = '';

$VERSION = '2.033';

sub new
{
    my $class = shift ;
    $GunzipError = '';
    my $obj = createSelfTiedObject($class, \$GunzipError);

    $obj->_create(undef, 0, @_);
}

sub gunzip
{
    my $obj = createSelfTiedObject(undef, \$GunzipError);
    return $obj->_inf(@_) ;
}

sub getExtraParams
{
    use IO::Compress::Base::Common  2.033 qw(:Parse);
    return ( 'ParseExtra' => [1, 1, Parse_boolean,  0] ) ;
}

sub ckParams
{
    my $self = shift ;
    my $got = shift ;

    # gunzip always needs crc32
    $got->value('CRC32' => 1);

    return 1;
}

sub ckMagic
{
    my $self = shift;

    my $magic ;
    $self->smartReadExact(\$magic, GZIP_ID_SIZE);

    *$self->{HeaderPending} = $magic ;

    return $self->HeaderError("Minimum header size is " . 
                              GZIP_MIN_HEADER_SIZE . " bytes") 
        if length $magic != GZIP_ID_SIZE ;                                    

    return $self->HeaderError("Bad Magic")
        if ! isGzipMagic($magic) ;

    *$self->{Type} = 'rfc1952';

    return $magic ;
}

sub readHeader
{
    my $self = shift;
    my $magic = shift;

    return $self->_readGzipHeader($magic);
}

sub chkTrailer
{
    my $self = shift;
    my $trailer = shift;

    # Check CRC & ISIZE 
    my ($CRC32, $ISIZE) = unpack("V V", $trailer) ;
    *$self->{Info}{CRC32} = $CRC32;    
    *$self->{Info}{ISIZE} = $ISIZE;    

    if (*$self->{Strict}) {
        return $self->TrailerError("CRC mismatch")
            if $CRC32 != *$self->{Uncomp}->crc32() ;

        my $exp_isize = *$self->{UnCompSize}->get32bit();
        return $self->TrailerError("ISIZE mismatch. Got $ISIZE"
                                  . ", expected $exp_isize")
            if $ISIZE != $exp_isize ;
    }

    return STATUS_OK;
}

sub isGzipMagic
{
    my $buffer = shift ;
    return 0 if length $buffer < GZIP_ID_SIZE ;
    my ($id1, $id2) = unpack("C C", $buffer) ;
    return $id1 == GZIP_ID1 && $id2 == GZIP_ID2 ;
}

sub _readFullGzipHeader($)
{
    my ($self) = @_ ;
    my $magic = '' ;

    $self->smartReadExact(\$magic, GZIP_ID_SIZE);

    *$self->{HeaderPending} = $magic ;

    return $self->HeaderError("Minimum header size is " . 
                              GZIP_MIN_HEADER_SIZE . " bytes") 
        if length $magic != GZIP_ID_SIZE ;                                    


    return $self->HeaderError("Bad Magic")
        if ! isGzipMagic($magic) ;

    my $status = $self->_readGzipHeader($magic);
    delete *$self->{Transparent} if ! defined $status ;
    return $status ;
}

sub _readGzipHeader($)
{
    my ($self, $magic) = @_ ;
    my ($HeaderCRC) ;
    my ($buffer) = '' ;

    $self->smartReadExact(\$buffer, GZIP_MIN_HEADER_SIZE - GZIP_ID_SIZE)
        or return $self->HeaderError("Minimum header size is " . 
                                     GZIP_MIN_HEADER_SIZE . " bytes") ;

    my $keep = $magic . $buffer ;
    *$self->{HeaderPending} = $keep ;

    # now split out the various parts
    my ($cm, $flag, $mtime, $xfl, $os) = unpack("C C V C C", $buffer) ;

    $cm == GZIP_CM_DEFLATED 
        or return $self->HeaderError("Not Deflate (CM is $cm)") ;

    # check for use of reserved bits
    return $self->HeaderError("Use of Reserved Bits in FLG field.")
        if $flag & GZIP_FLG_RESERVED ; 

    my $EXTRA ;
    my @EXTRA = () ;
    if ($flag & GZIP_FLG_FEXTRA) {
        $EXTRA = "" ;
        $self->smartReadExact(\$buffer, GZIP_FEXTRA_HEADER_SIZE) 
            or return $self->TruncatedHeader("FEXTRA Length") ;

        my ($XLEN) = unpack("v", $buffer) ;
        $self->smartReadExact(\$EXTRA, $XLEN) 
            or return $self->TruncatedHeader("FEXTRA Body");
        $keep .= $buffer . $EXTRA ;

        if ($XLEN && *$self->{'ParseExtra'}) {
            my $bad = IO::Compress::Zlib::Extra::parseRawExtra($EXTRA,
                                                \@EXTRA, 1, 1);
            return $self->HeaderError($bad)
                if defined $bad;
        }
    }

    my $origname ;
    if ($flag & GZIP_FLG_FNAME) {
        $origname = "" ;
        while (1) {
            $self->smartReadExact(\$buffer, 1) 
                or return $self->TruncatedHeader("FNAME");
            last if $buffer eq GZIP_NULL_BYTE ;
            $origname .= $buffer 
        }
        $keep .= $origname . GZIP_NULL_BYTE ;

        return $self->HeaderError("Non ISO 8859-1 Character found in Name")
            if *$self->{Strict} && $origname =~ /$GZIP_FNAME_INVALID_CHAR_RE/o ;
    }

    my $comment ;
    if ($flag & GZIP_FLG_FCOMMENT) {
        $comment = "";
        while (1) {
            $self->smartReadExact(\$buffer, 1) 
                or return $self->TruncatedHeader("FCOMMENT");
            last if $buffer eq GZIP_NULL_BYTE ;
            $comment .= $buffer 
        }
        $keep .= $comment . GZIP_NULL_BYTE ;

        return $self->HeaderError("Non ISO 8859-1 Character found in Comment")
            if *$self->{Strict} && $comment =~ /$GZIP_FCOMMENT_INVALID_CHAR_RE/o ;
    }

    if ($flag & GZIP_FLG_FHCRC) {
        $self->smartReadExact(\$buffer, GZIP_FHCRC_SIZE) 
            or return $self->TruncatedHeader("FHCRC");

        $HeaderCRC = unpack("v", $buffer) ;
        my $crc16 = crc32($keep) & 0xFF ;

        return $self->HeaderError("CRC16 mismatch.")
            if *$self->{Strict} && $crc16 != $HeaderCRC;

        $keep .= $buffer ;
    }

    # Assume compression method is deflated for xfl tests
    #if ($xfl) {
    #}

    *$self->{Type} = 'rfc1952';

    return {
        'Type'          => 'rfc1952',
        'FingerprintLength'  => 2,
        'HeaderLength'  => length $keep,
        'TrailerLength' => GZIP_TRAILER_SIZE,
        'Header'        => $keep,
        'isMinimalHeader' => $keep eq GZIP_MINIMUM_HEADER ? 1 : 0,

        'MethodID'      => $cm,
        'MethodName'    => $cm == GZIP_CM_DEFLATED ? "Deflated" : "Unknown" ,
        'TextFlag'      => $flag & GZIP_FLG_FTEXT ? 1 : 0,
        'HeaderCRCFlag' => $flag & GZIP_FLG_FHCRC ? 1 : 0,
        'NameFlag'      => $flag & GZIP_FLG_FNAME ? 1 : 0,
        'CommentFlag'   => $flag & GZIP_FLG_FCOMMENT ? 1 : 0,
        'ExtraFlag'     => $flag & GZIP_FLG_FEXTRA ? 1 : 0,
        'Name'          => $origname,
        'Comment'       => $comment,
        'Time'          => $mtime,
        'OsID'          => $os,
        'OsName'        => defined $GZIP_OS_Names{$os} 
                                 ? $GZIP_OS_Names{$os} : "Unknown",
        'HeaderCRC'     => $HeaderCRC,
        'Flags'         => $flag,
        'ExtraFlags'    => $xfl,
        'ExtraFieldRaw' => $EXTRA,
        'ExtraField'    => [ @EXTRA ],


        #'CompSize'=> $compsize,
        #'CRC32'=> $CRC32,
        #'OrigSize'=> $ISIZE,
      }
}


1;

__END__


#line 1112
FILE   $91a362e5/IO/Uncompress/RawInflate.pm  "#line 1 "/usr/lib/perl5/5.14/IO/Uncompress/RawInflate.pm"
package IO::Uncompress::RawInflate ;
# for RFC1951

use strict ;
use warnings;
use bytes;

use Compress::Raw::Zlib  2.033 ;
use IO::Compress::Base::Common  2.033 qw(:Status createSelfTiedObject);

use IO::Uncompress::Base  2.033 ;
use IO::Uncompress::Adapter::Inflate  2.033 ;

require Exporter ;
our ($VERSION, @ISA, @EXPORT_OK, %EXPORT_TAGS, %DEFLATE_CONSTANTS, $RawInflateError);

$VERSION = '2.033';
$RawInflateError = '';

@ISA    = qw( Exporter IO::Uncompress::Base );
@EXPORT_OK = qw( $RawInflateError rawinflate ) ;
%DEFLATE_CONSTANTS = ();
%EXPORT_TAGS = %IO::Uncompress::Base::EXPORT_TAGS ;
push @{ $EXPORT_TAGS{all} }, @EXPORT_OK ;
Exporter::export_ok_tags('all');

#{
#    # Execute at runtime  
#    my %bad;
#    for my $module (qw(Compress::Raw::Zlib IO::Compress::Base::Common IO::Uncompress::Base IO::Uncompress::Adapter::Inflate))
#    {
#        my $ver = ${ $module . "::VERSION"} ;
#        
#        $bad{$module} = $ver
#            if $ver ne $VERSION;
#    }
#    
#    if (keys %bad)
#    {
#        my $string = join "\n", map { "$_ $bad{$_}" } keys %bad;
#        die caller(0)[0] . "needs version $VERSION mismatch\n$string\n";
#    }
#}

sub new
{
    my $class = shift ;
    my $obj = createSelfTiedObject($class, \$RawInflateError);
    $obj->_create(undef, 0, @_);
}

sub rawinflate
{
    my $obj = createSelfTiedObject(undef, \$RawInflateError);
    return $obj->_inf(@_);
}

sub getExtraParams
{
    return ();
}

sub ckParams
{
    my $self = shift ;
    my $got = shift ;

    return 1;
}

sub mkUncomp
{
    my $self = shift ;
    my $got = shift ;

    my ($obj, $errstr, $errno) = IO::Uncompress::Adapter::Inflate::mkUncompObject(
                                                                $got->value('CRC32'),
                                                                $got->value('ADLER32'),
                                                                $got->value('Scan'),
                                                            );

    return $self->saveErrorString(undef, $errstr, $errno)
        if ! defined $obj;

    *$self->{Uncomp} = $obj;

     my $magic = $self->ckMagic()
        or return 0;

    *$self->{Info} = $self->readHeader($magic)
        or return undef ;

    return 1;

}


sub ckMagic
{
    my $self = shift;

    return $self->_isRaw() ;
}

sub readHeader
{
    my $self = shift;
    my $magic = shift ;

    return {
        'Type'          => 'rfc1951',
        'FingerprintLength'  => 0,
        'HeaderLength'  => 0,
        'TrailerLength' => 0,
        'Header'        => ''
        };
}

sub chkTrailer
{
    return STATUS_OK ;
}

sub _isRaw
{
    my $self   = shift ;

    my $got = $self->_isRawx(@_);

    if ($got) {
        *$self->{Pending} = *$self->{HeaderPending} ;
    }
    else {
        $self->pushBack(*$self->{HeaderPending});
        *$self->{Uncomp}->reset();
    }
    *$self->{HeaderPending} = '';

    return $got ;
}

sub _isRawx
{
    my $self   = shift ;
    my $magic = shift ;

    $magic = '' unless defined $magic ;

    my $buffer = '';

    $self->smartRead(\$buffer, *$self->{BlockSize}) >= 0  
        or return $self->saveErrorString(undef, "No data to read");

    my $temp_buf = $magic . $buffer ;
    *$self->{HeaderPending} = $temp_buf ;    
    $buffer = '';
    my $status = *$self->{Uncomp}->uncompr(\$temp_buf, \$buffer, $self->smartEof()) ;
    
    return $self->saveErrorString(undef, *$self->{Uncomp}{Error}, STATUS_ERROR)
        if $status == STATUS_ERROR;

    $self->pushBack($temp_buf)  ;

    return $self->saveErrorString(undef, "unexpected end of file", STATUS_ERROR)
        if $self->smartEof() && $status != STATUS_ENDSTREAM;
            
    #my $buf_len = *$self->{Uncomp}->uncompressedBytes();
    my $buf_len = length $buffer;

    if ($status == STATUS_ENDSTREAM) {
        if (*$self->{MultiStream} 
                    && (length $temp_buf || ! $self->smartEof())){
            *$self->{NewStream} = 1 ;
            *$self->{EndStream} = 0 ;
        }
        else {
            *$self->{EndStream} = 1 ;
        }
    }
    *$self->{HeaderPending} = $buffer ;    
    *$self->{InflatedBytesRead} = $buf_len ;    
    *$self->{TotalInflatedBytesRead} += $buf_len ;    
    *$self->{Type} = 'rfc1951';

    $self->saveStatus(STATUS_OK);

    return {
        'Type'          => 'rfc1951',
        'HeaderLength'  => 0,
        'TrailerLength' => 0,
        'Header'        => ''
        };
}


sub inflateSync
{
    my $self = shift ;

    # inflateSync is a no-op in Plain mode
    return 1
        if *$self->{Plain} ;

    return 0 if *$self->{Closed} ;
    #return G_EOF if !length *$self->{Pending} && *$self->{EndStream} ;
    return 0 if ! length *$self->{Pending} && *$self->{EndStream} ;

    # Disable CRC check
    *$self->{Strict} = 0 ;

    my $status ;
    while (1)
    {
        my $temp_buf ;

        if (length *$self->{Pending} )
        {
            $temp_buf = *$self->{Pending} ;
            *$self->{Pending} = '';
        }
        else
        {
            $status = $self->smartRead(\$temp_buf, *$self->{BlockSize}) ;
            return $self->saveErrorString(0, "Error Reading Data")
                if $status < 0  ;

            if ($status == 0 ) {
                *$self->{EndStream} = 1 ;
                return $self->saveErrorString(0, "unexpected end of file", STATUS_ERROR);
            }
        }
        
        $status = *$self->{Uncomp}->sync($temp_buf) ;

        if ($status == STATUS_OK)
        {
            *$self->{Pending} .= $temp_buf ;
            return 1 ;
        }

        last unless $status == STATUS_ERROR ;
    }

    return 0;
}

#sub performScan
#{
#    my $self = shift ;
#
#    my $status ;
#    my $end_offset = 0;
#
#    $status = $self->scan() 
#    #or return $self->saveErrorString(undef, "Error Scanning: $$error_ref", $self->errorNo) ;
#        or return $self->saveErrorString(G_ERR, "Error Scanning: $status")
#
#    $status = $self->zap($end_offset) 
#        or return $self->saveErrorString(G_ERR, "Error Zapping: $status");
#    #or return $self->saveErrorString(undef, "Error Zapping: $$error_ref", $self->errorNo) ;
#
#    #(*$obj->{Deflate}, $status) = $inf->createDeflate();
#
##    *$obj->{Header} = *$inf->{Info}{Header};
##    *$obj->{UnCompSize_32bit} = 
##        *$obj->{BytesWritten} = *$inf->{UnCompSize_32bit} ;
##    *$obj->{CompSize_32bit} = *$inf->{CompSize_32bit} ;
#
#
##    if ( $outType eq 'buffer') 
##      { substr( ${ *$self->{Buffer} }, $end_offset) = '' }
##    elsif ($outType eq 'handle' || $outType eq 'filename') {
##        *$self->{FH} = *$inf->{FH} ;
##        delete *$inf->{FH};
##        *$obj->{FH}->flush() ;
##        *$obj->{Handle} = 1 if $outType eq 'handle';
##
##        #seek(*$obj->{FH}, $end_offset, SEEK_SET) 
##        *$obj->{FH}->seek($end_offset, SEEK_SET) 
##            or return $obj->saveErrorString(undef, $!, $!) ;
##    }
#    
#}

sub scan
{
    my $self = shift ;

    return 1 if *$self->{Closed} ;
    return 1 if !length *$self->{Pending} && *$self->{EndStream} ;

    my $buffer = '' ;
    my $len = 0;

    $len = $self->_raw_read(\$buffer, 1) 
        while ! *$self->{EndStream} && $len >= 0 ;

    #return $len if $len < 0 ? $len : 0 ;
    return $len < 0 ? 0 : 1 ;
}

sub zap
{
    my $self  = shift ;

    my $headerLength = *$self->{Info}{HeaderLength};
    my $block_offset =  $headerLength + *$self->{Uncomp}->getLastBlockOffset();
    $_[0] = $headerLength + *$self->{Uncomp}->getEndOffset();
    #printf "# End $_[0], headerlen $headerLength \n";;
    #printf "# block_offset $block_offset %x\n", $block_offset;
    my $byte ;
    ( $self->smartSeek($block_offset) &&
      $self->smartRead(\$byte, 1) ) 
        or return $self->saveErrorString(0, $!, $!); 

    #printf "#byte is %x\n", unpack('C*',$byte);
    *$self->{Uncomp}->resetLastBlockByte($byte);
    #printf "#to byte is %x\n", unpack('C*',$byte);

    ( $self->smartSeek($block_offset) && 
      $self->smartWrite($byte) )
        or return $self->saveErrorString(0, $!, $!); 

    #$self->smartSeek($end_offset, 1);

    return 1 ;
}

sub createDeflate
{
    my $self  = shift ;
    my ($def, $status) = *$self->{Uncomp}->createDeflateStream(
                                    -AppendOutput   => 1,
                                    -WindowBits => - MAX_WBITS,
                                    -CRC32      => *$self->{Params}->value('CRC32'),
                                    -ADLER32    => *$self->{Params}->value('ADLER32'),
                                );
    
    return wantarray ? ($status, $def) : $def ;                                
}


1; 

__END__


#line 1111
FILE   45f96a19/PerlIO.pm  �#line 1 "/usr/lib/perl5/5.14/PerlIO.pm"
package PerlIO;

our $VERSION = '1.07';

# Map layer name to package that defines it
our %alias;

sub import
{
 my $class = shift;
 while (@_)
  {
   my $layer = shift;
   if (exists $alias{$layer})
    {
     $layer = $alias{$layer}
    }
   else
    {
     $layer = "${class}::$layer";
    }
   eval "require $layer";
   warn $@ if $@;
  }
}

sub F_UTF8 () { 0x8000 }

1;
__END__

#line 333
FILE   66983d6c/SelectSaver.pm  �#line 1 "/usr/lib/perl5/5.14/SelectSaver.pm"
package SelectSaver;

our $VERSION = '1.02';

#line 36

require 5.000;
use Carp;
use Symbol;

sub new {
    @_ >= 1 && @_ <= 2 or croak 'usage: SelectSaver->new( [FILEHANDLE] )';
    my $fh = select;
    my $self = bless \$fh, $_[0];
    select qualify($_[1], caller) if @_ > 1;
    $self;
}

sub DESTROY {
    my $self = $_[0];
    select $$self;
}

1;
FILE   24c526a9/Symbol.pm  f#line 1 "/usr/lib/perl5/5.14/Symbol.pm"
package Symbol;

#line 80

BEGIN { require 5.005; }

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(gensym ungensym qualify qualify_to_ref);
@EXPORT_OK = qw(delete_package geniosym);

$VERSION = '1.07';

my $genpkg = "Symbol::";
my $genseq = 0;

my %global = map {$_ => 1} qw(ARGV ARGVOUT ENV INC SIG STDERR STDIN STDOUT);

#
# Note that we never _copy_ the glob; we just make a ref to it.
# If we did copy it, then SVf_FAKE would be set on the copy, and
# glob-specific behaviors (e.g. C<*$ref = \&func>) wouldn't work.
#
sub gensym () {
    my $name = "GEN" . $genseq++;
    my $ref = \*{$genpkg . $name};
    delete $$genpkg{$name};
    $ref;
}

sub geniosym () {
    my $sym = gensym();
    # force the IO slot to be filled
    select(select $sym);
    *$sym{IO};
}

sub ungensym ($) {}

sub qualify ($;$) {
    my ($name) = @_;
    if (!ref($name) && index($name, '::') == -1 && index($name, "'") == -1) {
	my $pkg;
	# Global names: special character, "^xyz", or other. 
	if ($name =~ /^(([^a-z])|(\^[a-z_]+))\z/i || $global{$name}) {
	    # RGS 2001-11-05 : translate leading ^X to control-char
	    $name =~ s/^\^([a-z_])/'qq(\c'.$1.')'/eei;
	    $pkg = "main";
	}
	else {
	    $pkg = (@_ > 1) ? $_[1] : caller;
	}
	$name = $pkg . "::" . $name;
    }
    $name;
}

sub qualify_to_ref ($;$) {
    return \*{ qualify $_[0], @_ > 1 ? $_[1] : caller };
}

#
# of Safe.pm lineage
#
sub delete_package ($) {
    my $pkg = shift;

    # expand to full symbol table name if needed

    unless ($pkg =~ /^main::.*::$/) {
        $pkg = "main$pkg"	if	$pkg =~ /^::/;
        $pkg = "main::$pkg"	unless	$pkg =~ /^main::/;
        $pkg .= '::'		unless	$pkg =~ /::$/;
    }

    my($stem, $leaf) = $pkg =~ m/(.*::)(\w+::)$/;
    my $stem_symtab = *{$stem}{HASH};
    return unless defined $stem_symtab and exists $stem_symtab->{$leaf};


    # free all the symbols in the package

    my $leaf_symtab = *{$stem_symtab->{$leaf}}{HASH};
    foreach my $name (keys %$leaf_symtab) {
        undef *{$pkg . $name};
    }

    # delete the symbol table

    %$leaf_symtab = ();
    delete $stem_symtab->{$leaf};
}

1;
FILE   c083f404/Time/Local.pm  u#line 1 "/usr/lib/perl5/5.14/Time/Local.pm"
package Time::Local;

require Exporter;
use Carp;
use Config;
use strict;

use vars qw( $VERSION @ISA @EXPORT @EXPORT_OK );
$VERSION   = '1.2000';

@ISA       = qw( Exporter );
@EXPORT    = qw( timegm timelocal );
@EXPORT_OK = qw( timegm_nocheck timelocal_nocheck );

my @MonthDays = ( 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 );

# Determine breakpoint for rolling century
my $ThisYear    = ( localtime() )[5];
my $Breakpoint  = ( $ThisYear + 50 ) % 100;
my $NextCentury = $ThisYear - $ThisYear % 100;
$NextCentury += 100 if $Breakpoint < 50;
my $Century = $NextCentury - 100;
my $SecOff  = 0;

my ( %Options, %Cheat );

use constant SECS_PER_MINUTE => 60;
use constant SECS_PER_HOUR   => 3600;
use constant SECS_PER_DAY    => 86400;

my $MaxDay;
if ($] < 5.012000) {
    my $MaxInt;
    if ( $^O eq 'MacOS' ) {
        # time_t is unsigned...
        $MaxInt = ( 1 << ( 8 * $Config{ivsize} ) ) - 1;
    }
    else {
        $MaxInt = ( ( 1 << ( 8 * $Config{ivsize} - 2 ) ) - 1 ) * 2 + 1;
    }

    $MaxDay = int( ( $MaxInt - ( SECS_PER_DAY / 2 ) ) / SECS_PER_DAY ) - 1;
}
else {
    # recent localtime()'s limit is the year 2**31
    $MaxDay = 365 * (2**31);
}

# Determine the EPOC day for this machine
my $Epoc = 0;
if ( $^O eq 'vos' ) {
    # work around posix-977 -- VOS doesn't handle dates in the range
    # 1970-1980.
    $Epoc = _daygm( 0, 0, 0, 1, 0, 70, 4, 0 );
}
elsif ( $^O eq 'MacOS' ) {
    $MaxDay *=2 if $^O eq 'MacOS';  # time_t unsigned ... quick hack?
    # MacOS time() is seconds since 1 Jan 1904, localtime
    # so we need to calculate an offset to apply later
    $Epoc = 693901;
    $SecOff = timelocal( localtime(0)) - timelocal( gmtime(0) ) ;
    $Epoc += _daygm( gmtime(0) );
}
else {
    $Epoc = _daygm( gmtime(0) );
}

%Cheat = ();    # clear the cache as epoc has changed

sub _daygm {

    # This is written in such a byzantine way in order to avoid
    # lexical variables and sub calls, for speed
    return $_[3] + (
        $Cheat{ pack( 'ss', @_[ 4, 5 ] ) } ||= do {
            my $month = ( $_[4] + 10 ) % 12;
            my $year  = $_[5] + 1900 - int($month / 10);

            ( ( 365 * $year )
              + int( $year / 4 )
              - int( $year / 100 )
              + int( $year / 400 )
              + int( ( ( $month * 306 ) + 5 ) / 10 )
            )
            - $Epoc;
        }
    );
}

sub _timegm {
    my $sec =
        $SecOff + $_[0] + ( SECS_PER_MINUTE * $_[1] ) + ( SECS_PER_HOUR * $_[2] );

    return $sec + ( SECS_PER_DAY * &_daygm );
}

sub timegm {
    my ( $sec, $min, $hour, $mday, $month, $year ) = @_;

    if ( $year >= 1000 ) {
        $year -= 1900;
    }
    elsif ( $year < 100 and $year >= 0 ) {
        $year += ( $year > $Breakpoint ) ? $Century : $NextCentury;
    }

    unless ( $Options{no_range_check} ) {
        croak "Month '$month' out of range 0..11"
            if $month > 11
            or $month < 0;

	my $md = $MonthDays[$month];
        ++$md
            if $month == 1 && _is_leap_year( $year + 1900 );

        croak "Day '$mday' out of range 1..$md"  if $mday > $md or $mday < 1;
        croak "Hour '$hour' out of range 0..23"  if $hour > 23  or $hour < 0;
        croak "Minute '$min' out of range 0..59" if $min > 59   or $min < 0;
        croak "Second '$sec' out of range 0..59" if $sec > 59   or $sec < 0;
    }

    my $days = _daygm( undef, undef, undef, $mday, $month, $year );

    unless ($Options{no_range_check} or abs($days) < $MaxDay) {
        my $msg = '';
        $msg .= "Day too big - $days > $MaxDay\n" if $days > $MaxDay;

	$year += 1900;
        $msg .=  "Cannot handle date ($sec, $min, $hour, $mday, $month, $year)";

	croak $msg;
    }

    return $sec
           + $SecOff
           + ( SECS_PER_MINUTE * $min )
           + ( SECS_PER_HOUR * $hour )
           + ( SECS_PER_DAY * $days );
}

sub _is_leap_year {
    return 0 if $_[0] % 4;
    return 1 if $_[0] % 100;
    return 0 if $_[0] % 400;

    return 1;
}

sub timegm_nocheck {
    local $Options{no_range_check} = 1;
    return &timegm;
}

sub timelocal {
    my $ref_t = &timegm;
    my $loc_for_ref_t = _timegm( localtime($ref_t) );

    my $zone_off = $loc_for_ref_t - $ref_t
        or return $loc_for_ref_t;

    # Adjust for timezone
    my $loc_t = $ref_t - $zone_off;

    # Are we close to a DST change or are we done
    my $dst_off = $ref_t - _timegm( localtime($loc_t) );

    # If this evaluates to true, it means that the value in $loc_t is
    # the _second_ hour after a DST change where the local time moves
    # backward.
    if ( ! $dst_off &&
         ( ( $ref_t - SECS_PER_HOUR ) - _timegm( localtime( $loc_t - SECS_PER_HOUR ) ) < 0 )
       ) {
        return $loc_t - SECS_PER_HOUR;
    }

    # Adjust for DST change
    $loc_t += $dst_off;

    return $loc_t if $dst_off > 0;

    # If the original date was a non-extent gap in a forward DST jump,
    # we should now have the wrong answer - undo the DST adjustment
    my ( $s, $m, $h ) = localtime($loc_t);
    $loc_t -= $dst_off if $s != $_[0] || $m != $_[1] || $h != $_[2];

    return $loc_t;
}

sub timelocal_nocheck {
    local $Options{no_range_check} = 1;
    return &timelocal;
}

1;

__END__

#line 385
FILE   4a26df63/XSLoader.pm  �#line 1 "/usr/lib/perl5/5.14/XSLoader.pm"
# Generated from XSLoader.pm.PL (resolved %Config::Config value)

package XSLoader;

$VERSION = "0.13";

#use strict;

# enable debug/trace messages from DynaLoader perl code
# $dl_debug = $ENV{PERL_DL_DEBUG} || 0 unless defined $dl_debug;

package DynaLoader;

# No prizes for guessing why we don't say 'bootstrap DynaLoader;' here.
# NOTE: All dl_*.xs (including dl_none.xs) define a dl_error() XSUB
boot_DynaLoader('DynaLoader') if defined(&boot_DynaLoader) &&
                                !defined(&dl_error);
package XSLoader;

sub load {
    package DynaLoader;

    my ($module, $modlibname) = caller();

    if (@_) {
	$module = $_[0];
    } else {
	$_[0] = $module;
    }

    # work with static linking too
    my $boots = "$module\::bootstrap";
    goto &$boots if defined &$boots;

    goto \&XSLoader::bootstrap_inherit;

    my @modparts = split(/::/,$module);
    my $modfname = $modparts[-1];

    my $modpname = join('/',@modparts);
    my $c = @modparts;
    $modlibname =~ s,[\\/][^\\/]+$,, while $c--;	# Q&D basename
    my $file = "$modlibname/auto/$modpname/$modfname.dll";

#   print STDERR "XSLoader::load for $module ($file)\n" if $dl_debug;

    my $bs = $file;
    $bs =~ s/(\.\w+)?(;\d*)?$/\.bs/; # look for .bs 'beside' the library

    if (-s $bs) { # only read file if it's not empty
#       print STDERR "BS: $bs ($^O, $dlsrc)\n" if $dl_debug;
        eval { do $bs; };
        warn "$bs: $@\n" if $@;
    }

    goto \&XSLoader::bootstrap_inherit if not -f $file or -s $bs;

    my $bootname = "boot_$module";
    $bootname =~ s/\W/_/g;
    @DynaLoader::dl_require_symbols = ($bootname);

    my $boot_symbol_ref;

    # Many dynamic extension loading problems will appear to come from
    # this section of code: XYZ failed at line 123 of DynaLoader.pm.
    # Often these errors are actually occurring in the initialisation
    # C code of the extension XS file. Perl reports the error as being
    # in this perl code simply because this was the last perl code
    # it executed.

    my $libref = dl_load_file($file, 0) or do { 
        require Carp;
        Carp::croak("Can't load '$file' for module $module: " . dl_error());
    };
    push(@DynaLoader::dl_librefs,$libref);  # record loaded object

    my @unresolved = dl_undef_symbols();
    if (@unresolved) {
        require Carp;
        Carp::carp("Undefined symbols present after loading $file: @unresolved\n");
    }

    $boot_symbol_ref = dl_find_symbol($libref, $bootname) or do {
        require Carp;
        Carp::croak("Can't find '$bootname' symbol in $file\n");
    };

    push(@DynaLoader::dl_modules, $module); # record loaded module

  boot:
    my $xs = dl_install_xsub($boots, $boot_symbol_ref, $file);

    # See comment block above
    push(@DynaLoader::dl_shared_objects, $file); # record files loaded
    return &$xs(@_);
}

sub bootstrap_inherit {
    require DynaLoader;
    goto \&DynaLoader::bootstrap_inherit;
}

1;


__END__

#line 353
FILE   cd25b871/base.pm  G#line 1 "/usr/lib/perl5/5.14/base.pm"
package base;

use strict 'vars';
use vars qw($VERSION);
$VERSION = '2.16';
$VERSION = eval $VERSION;

# constant.pm is slow
sub SUCCESS () { 1 }

sub PUBLIC     () { 2**0  }
sub PRIVATE    () { 2**1  }
sub INHERITED  () { 2**2  }
sub PROTECTED  () { 2**3  }


my $Fattr = \%fields::attr;

sub has_fields {
    my($base) = shift;
    my $fglob = ${"$base\::"}{FIELDS};
    return( ($fglob && 'GLOB' eq ref($fglob) && *$fglob{HASH}) ? 1 : 0 );
}

sub has_version {
    my($base) = shift;
    my $vglob = ${$base.'::'}{VERSION};
    return( ($vglob && *$vglob{SCALAR}) ? 1 : 0 );
}

sub has_attr {
    my($proto) = shift;
    my($class) = ref $proto || $proto;
    return exists $Fattr->{$class};
}

sub get_attr {
    $Fattr->{$_[0]} = [1] unless $Fattr->{$_[0]};
    return $Fattr->{$_[0]};
}

if ($] < 5.009) {
    *get_fields = sub {
        # Shut up a possible typo warning.
        () = \%{$_[0].'::FIELDS'};
        my $f = \%{$_[0].'::FIELDS'};

        # should be centralized in fields? perhaps
        # fields::mk_FIELDS_be_OK. Peh. As long as %{ $package . '::FIELDS' }
        # is used here anyway, it doesn't matter.
        bless $f, 'pseudohash' if (ref($f) ne 'pseudohash');

        return $f;
    }
}
else {
    *get_fields = sub {
        # Shut up a possible typo warning.
        () = \%{$_[0].'::FIELDS'};
        return \%{$_[0].'::FIELDS'};
    }
}

sub import {
    my $class = shift;

    return SUCCESS unless @_;

    # List of base classes from which we will inherit %FIELDS.
    my $fields_base;

    my $inheritor = caller(0);
    my @isa_classes;

    my @bases;
    foreach my $base (@_) {
        if ( $inheritor eq $base ) {
            warn "Class '$inheritor' tried to inherit from itself\n";
        }

        next if grep $_->isa($base), ($inheritor, @bases);

        if (has_version($base)) {
            ${$base.'::VERSION'} = '-1, set by base.pm' 
              unless defined ${$base.'::VERSION'};
        }
        else {
            my $sigdie;
            {
                local $SIG{__DIE__};
                eval "require $base";
                # Only ignore "Can't locate" errors from our eval require.
                # Other fatal errors (syntax etc) must be reported.
                die if $@ && $@ !~ /^Can't locate .*? at \(eval /;
                unless (%{"$base\::"}) {
                    require Carp;
                    local $" = " ";
                    Carp::croak(<<ERROR);
Base class package "$base" is empty.
    (Perhaps you need to 'use' the module which defines that package first,
    or make that module available in \@INC (\@INC contains: @INC).
ERROR
                }
                $sigdie = $SIG{__DIE__} || undef;
            }
            # Make sure a global $SIG{__DIE__} makes it out of the localization.
            $SIG{__DIE__} = $sigdie if defined $sigdie;
            ${$base.'::VERSION'} = "-1, set by base.pm"
              unless defined ${$base.'::VERSION'};
        }
        push @bases, $base;

        if ( has_fields($base) || has_attr($base) ) {
            # No multiple fields inheritance *suck*
            if ($fields_base) {
                require Carp;
                Carp::croak("Can't multiply inherit fields");
            } else {
                $fields_base = $base;
            }
        }
    }
    # Save this until the end so it's all or nothing if the above loop croaks.
    push @{"$inheritor\::ISA"}, @isa_classes;

    push @{"$inheritor\::ISA"}, @bases;

    if( defined $fields_base ) {
        inherit_fields($inheritor, $fields_base);
    }
}


sub inherit_fields {
    my($derived, $base) = @_;

    return SUCCESS unless $base;

    my $battr = get_attr($base);
    my $dattr = get_attr($derived);
    my $dfields = get_fields($derived);
    my $bfields = get_fields($base);

    $dattr->[0] = @$battr;

    if( keys %$dfields ) {
        warn <<"END";
$derived is inheriting from $base but already has its own fields!
This will cause problems.  Be sure you use base BEFORE declaring fields.
END

    }

    # Iterate through the base's fields adding all the non-private
    # ones to the derived class.  Hang on to the original attribute
    # (Public, Private, etc...) and add Inherited.
    # This is all too complicated to do efficiently with add_fields().
    while (my($k,$v) = each %$bfields) {
        my $fno;
        if ($fno = $dfields->{$k} and $fno != $v) {
            require Carp;
            Carp::croak ("Inherited fields can't override existing fields");
        }

        if( $battr->[$v] & PRIVATE ) {
            $dattr->[$v] = PRIVATE | INHERITED;
        }
        else {
            $dattr->[$v] = INHERITED | $battr->[$v];
            $dfields->{$k} = $v;
        }
    }

    foreach my $idx (1..$#{$battr}) {
        next if defined $dattr->[$idx];
        $dattr->[$idx] = $battr->[$idx] & INHERITED;
    }
}


1;

__END__

#line 266
FILE   4df8ab1b/bytes.pm  �#line 1 "/usr/lib/perl5/5.14/bytes.pm"
package bytes;

our $VERSION = '1.04';

$bytes::hint_bits = 0x00000008;

sub import {
    $^H |= $bytes::hint_bits;
}

sub unimport {
    $^H &= ~$bytes::hint_bits;
}

sub AUTOLOAD {
    require "bytes_heavy.pl";
    goto &$AUTOLOAD if defined &$AUTOLOAD;
    require Carp;
    Carp::croak("Undefined subroutine $AUTOLOAD called");
}

sub length (_);
sub chr (_);
sub ord (_);
sub substr ($$;$$);
sub index ($$;$);
sub rindex ($$;$);

1;
__END__

#line 101
FILE   d842315b/constant.pm  �#line 1 "/usr/lib/perl5/5.14/constant.pm"
package constant;
use 5.005;
use strict;
use warnings::register;

use vars qw($VERSION %declared);
$VERSION = '1.21';

#=======================================================================

# Some names are evil choices.
my %keywords = map +($_, 1), qw{ BEGIN INIT CHECK END DESTROY AUTOLOAD };
$keywords{UNITCHECK}++ if $] > 5.009;

my %forced_into_main = map +($_, 1),
    qw{ STDIN STDOUT STDERR ARGV ARGVOUT ENV INC SIG };

my %forbidden = (%keywords, %forced_into_main);

my $str_end = $] >= 5.006 ? "\\z" : "\\Z";
my $normal_constant_name = qr/^_?[^\W_0-9]\w*$str_end/;
my $tolerable = qr/^[A-Za-z_]\w*$str_end/;
my $boolean = qr/^[01]?$str_end/;

BEGIN {
    # We'd like to do use constant _CAN_PCS => $] > 5.009002
    # but that's a bit tricky before we load the constant module :-)
    # By doing this, we save 1 run time check for *every* call to import.
    no strict 'refs';
    my $const = $] > 5.009002;
    *_CAN_PCS = sub () {$const};
}

#=======================================================================
# import() - import symbols into user's namespace
#
# What we actually do is define a function in the caller's namespace
# which returns the value. The function we create will normally
# be inlined as a constant, thereby avoiding further sub calling 
# overhead.
#=======================================================================
sub import {
    my $class = shift;
    return unless @_;			# Ignore 'use constant;'
    my $constants;
    my $multiple  = ref $_[0];
    my $pkg = caller;
    my $flush_mro;
    my $symtab;

    if (_CAN_PCS) {
	no strict 'refs';
	$symtab = \%{$pkg . '::'};
    };

    if ( $multiple ) {
	if (ref $_[0] ne 'HASH') {
	    require Carp;
	    Carp::croak("Invalid reference type '".ref(shift)."' not 'HASH'");
	}
	$constants = shift;
    } else {
	unless (defined $_[0]) {
	    require Carp;
	    Carp::croak("Can't use undef as constant name");
	}
	$constants->{+shift} = undef;
    }

    foreach my $name ( keys %$constants ) {
	# Normal constant name
	if ($name =~ $normal_constant_name and !$forbidden{$name}) {
	    # Everything is okay

	# Name forced into main, but we're not in main. Fatal.
	} elsif ($forced_into_main{$name} and $pkg ne 'main') {
	    require Carp;
	    Carp::croak("Constant name '$name' is forced into main::");

	# Starts with double underscore. Fatal.
	} elsif ($name =~ /^__/) {
	    require Carp;
	    Carp::croak("Constant name '$name' begins with '__'");

	# Maybe the name is tolerable
	} elsif ($name =~ $tolerable) {
	    # Then we'll warn only if you've asked for warnings
	    if (warnings::enabled()) {
		if ($keywords{$name}) {
		    warnings::warn("Constant name '$name' is a Perl keyword");
		} elsif ($forced_into_main{$name}) {
		    warnings::warn("Constant name '$name' is " .
			"forced into package main::");
		}
	    }

	# Looks like a boolean
	# use constant FRED == fred;
	} elsif ($name =~ $boolean) {
            require Carp;
	    if (@_) {
		Carp::croak("Constant name '$name' is invalid");
	    } else {
		Carp::croak("Constant name looks like boolean value");
	    }

	} else {
	   # Must have bad characters
            require Carp;
	    Carp::croak("Constant name '$name' has invalid characters");
	}

	{
	    no strict 'refs';
	    my $full_name = "${pkg}::$name";
	    $declared{$full_name}++;
	    if ($multiple || @_ == 1) {
		my $scalar = $multiple ? $constants->{$name} : $_[0];

		# Work around perl bug #xxxxx: Sub names (actually glob
		# names in general) ignore the UTF8 flag. So we have to
		# turn it off to get the "right" symbol table entry.
		utf8::is_utf8 $name and utf8::encode $name;

		# The constant serves to optimise this entire block out on
		# 5.8 and earlier.
		if (_CAN_PCS && $symtab && !exists $symtab->{$name}) {
		    # No typeglob yet, so we can use a reference as space-
		    # efficient proxy for a constant subroutine
		    # The check in Perl_ck_rvconst knows that inlinable
		    # constants from cv_const_sv are read only. So we have to:
		    Internals::SvREADONLY($scalar, 1);
		    $symtab->{$name} = \$scalar;
		    ++$flush_mro;
		} else {
		    *$full_name = sub () { $scalar };
		}
	    } elsif (@_) {
		my @list = @_;
		*$full_name = sub () { @list };
	    } else {
		*$full_name = sub () { };
	    }
	}
    }
    # Flush the cache exactly once if we make any direct symbol table changes.
    mro::method_changed_in($pkg) if _CAN_PCS && $flush_mro;
}

1;

__END__

#line 398
FILE   c500527d/feature.pm  
T#line 1 "/usr/lib/perl5/5.14/feature.pm"
package feature;

our $VERSION = '1.20';

# (feature name) => (internal name, used in %^H)
my %feature = (
    switch          => 'feature_switch',
    say             => "feature_say",
    state           => "feature_state",
    unicode_strings => "feature_unicode",
);

# This gets set (for now) in $^H as well as in %^H,
# for runtime speed of the uc/lc/ucfirst/lcfirst functions.
# See HINT_UNI_8_BIT in perl.h.
our $hint_uni8bit = 0x00000800;

# NB. the latest bundle must be loaded by the -E switch (see toke.c)

my %feature_bundle = (
    "5.10" => [qw(switch say state)],
    "5.11" => [qw(switch say state unicode_strings)],
    "5.12" => [qw(switch say state unicode_strings)],
    "5.13" => [qw(switch say state unicode_strings)],
    "5.14" => [qw(switch say state unicode_strings)],
);

# special case
$feature_bundle{"5.9.5"} = $feature_bundle{"5.10"};

# TODO:
# - think about versioned features (use feature switch => 2)

#line 172

sub import {
    my $class = shift;
    if (@_ == 0) {
	croak("No features specified");
    }
    while (@_) {
	my $name = shift(@_);
	if (substr($name, 0, 1) eq ":") {
	    my $v = substr($name, 1);
	    if (!exists $feature_bundle{$v}) {
		$v =~ s/^([0-9]+)\.([0-9]+).[0-9]+$/$1.$2/;
		if (!exists $feature_bundle{$v}) {
		    unknown_feature_bundle(substr($name, 1));
		}
	    }
	    unshift @_, @{$feature_bundle{$v}};
	    next;
	}
	if (!exists $feature{$name}) {
	    unknown_feature($name);
	}
	$^H{$feature{$name}} = 1;
        $^H |= $hint_uni8bit if $name eq 'unicode_strings';
    }
}

sub unimport {
    my $class = shift;

    # A bare C<no feature> should disable *all* features
    if (!@_) {
	delete @^H{ values(%feature) };
        $^H &= ~ $hint_uni8bit;
	return;
    }

    while (@_) {
	my $name = shift;
	if (substr($name, 0, 1) eq ":") {
	    my $v = substr($name, 1);
	    if (!exists $feature_bundle{$v}) {
		$v =~ s/^([0-9]+)\.([0-9]+).[0-9]+$/$1.$2/;
		if (!exists $feature_bundle{$v}) {
		    unknown_feature_bundle(substr($name, 1));
		}
	    }
	    unshift @_, @{$feature_bundle{$v}};
	    next;
	}
	if (!exists($feature{$name})) {
	    unknown_feature($name);
	}
	else {
	    delete $^H{$feature{$name}};
            $^H &= ~ $hint_uni8bit if $name eq 'unicode_strings';
	}
    }
}

sub unknown_feature {
    my $feature = shift;
    croak(sprintf('Feature "%s" is not supported by Perl %vd',
	    $feature, $^V));
}

sub unknown_feature_bundle {
    my $feature = shift;
    croak(sprintf('Feature bundle "%s" is not supported by Perl %vd',
	    $feature, $^V));
}

sub croak {
    require Carp;
    Carp::croak(@_);
}

1;
FILE   38caad36/Compress/Raw/Zlib.pm  ;#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Compress/Raw/Zlib.pm"

package Compress::Raw::Zlib;

require 5.004 ;
require Exporter;
use AutoLoader;
use Carp ;

#use Parse::Parameters;

use strict ;
use warnings ;
use bytes ;
our ($VERSION, $XS_VERSION, @ISA, @EXPORT, $AUTOLOAD);

$VERSION = '2.033';
$XS_VERSION = $VERSION; 
$VERSION = eval $VERSION;

@ISA = qw(Exporter);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw(
        adler32 crc32

        ZLIB_VERSION
        ZLIB_VERNUM

        DEF_WBITS
        OS_CODE

        MAX_MEM_LEVEL
        MAX_WBITS

        Z_ASCII
        Z_BEST_COMPRESSION
        Z_BEST_SPEED
        Z_BINARY
        Z_BLOCK
        Z_BUF_ERROR
        Z_DATA_ERROR
        Z_DEFAULT_COMPRESSION
        Z_DEFAULT_STRATEGY
        Z_DEFLATED
        Z_ERRNO
        Z_FILTERED
        Z_FIXED
        Z_FINISH
        Z_FULL_FLUSH
        Z_HUFFMAN_ONLY
        Z_MEM_ERROR
        Z_NEED_DICT
        Z_NO_COMPRESSION
        Z_NO_FLUSH
        Z_NULL
        Z_OK
        Z_PARTIAL_FLUSH
        Z_RLE
        Z_STREAM_END
        Z_STREAM_ERROR
        Z_SYNC_FLUSH
        Z_TREES
        Z_UNKNOWN
        Z_VERSION_ERROR

        WANT_GZIP
        WANT_GZIP_OR_ZLIB
);

use constant WANT_GZIP           => 16;
use constant WANT_GZIP_OR_ZLIB   => 32;

sub AUTOLOAD {
    my($constname);
    ($constname = $AUTOLOAD) =~ s/.*:://;
    my ($error, $val) = constant($constname);
    Carp::croak $error if $error;
    no strict 'refs';
    *{$AUTOLOAD} = sub { $val };
    goto &{$AUTOLOAD};
}

use constant FLAG_APPEND             => 1 ;
use constant FLAG_CRC                => 2 ;
use constant FLAG_ADLER              => 4 ;
use constant FLAG_CONSUME_INPUT      => 8 ;
use constant FLAG_LIMIT_OUTPUT       => 16 ;

eval {
    require XSLoader;
    XSLoader::load('Compress::Raw::Zlib', $XS_VERSION);
    1;
} 
or do {
    require DynaLoader;
    local @ISA = qw(DynaLoader);
    bootstrap Compress::Raw::Zlib $XS_VERSION ; 
};
 

use constant Parse_any      => 0x01;
use constant Parse_unsigned => 0x02;
use constant Parse_signed   => 0x04;
use constant Parse_boolean  => 0x08;
use constant Parse_string   => 0x10;
use constant Parse_custom   => 0x12;

use constant Parse_store_ref => 0x100 ;

use constant OFF_PARSED     => 0 ;
use constant OFF_TYPE       => 1 ;
use constant OFF_DEFAULT    => 2 ;
use constant OFF_FIXED      => 3 ;
use constant OFF_FIRST_ONLY => 4 ;
use constant OFF_STICKY     => 5 ;



sub ParseParameters
{
    my $level = shift || 0 ; 

    my $sub = (caller($level + 1))[3] ;
    #local $Carp::CarpLevel = 1 ;
    my $p = new Compress::Raw::Zlib::Parameters() ;
    $p->parse(@_)
        or croak "$sub: $p->{Error}" ;

    return $p;
}


sub Compress::Raw::Zlib::Parameters::new
{
    my $class = shift ;

    my $obj = { Error => '',
                Got   => {},
              } ;

    #return bless $obj, ref($class) || $class || __PACKAGE__ ;
    return bless $obj, 'Compress::Raw::Zlib::Parameters' ;
}

sub Compress::Raw::Zlib::Parameters::setError
{
    my $self = shift ;
    my $error = shift ;
    my $retval = @_ ? shift : undef ;

    $self->{Error} = $error ;
    return $retval;
}
          
#sub getError
#{
#    my $self = shift ;
#    return $self->{Error} ;
#}
          
sub Compress::Raw::Zlib::Parameters::parse
{
    my $self = shift ;

    my $default = shift ;

    my $got = $self->{Got} ;
    my $firstTime = keys %{ $got } == 0 ;

    my (@Bad) ;
    my @entered = () ;

    # Allow the options to be passed as a hash reference or
    # as the complete hash.
    if (@_ == 0) {
        @entered = () ;
    }
    elsif (@_ == 1) {
        my $href = $_[0] ;    
        return $self->setError("Expected even number of parameters, got 1")
            if ! defined $href or ! ref $href or ref $href ne "HASH" ;
 
        foreach my $key (keys %$href) {
            push @entered, $key ;
            push @entered, \$href->{$key} ;
        }
    }
    else {
        my $count = @_;
        return $self->setError("Expected even number of parameters, got $count")
            if $count % 2 != 0 ;
        
        for my $i (0.. $count / 2 - 1) {
            push @entered, $_[2* $i] ;
            push @entered, \$_[2* $i+1] ;
        }
    }


    while (my ($key, $v) = each %$default)
    {
        croak "need 4 params [@$v]"
            if @$v != 4 ;

        my ($first_only, $sticky, $type, $value) = @$v ;
        my $x ;
        $self->_checkType($key, \$value, $type, 0, \$x) 
            or return undef ;

        $key = lc $key;

        if ($firstTime || ! $sticky) {
            $got->{$key} = [0, $type, $value, $x, $first_only, $sticky] ;
        }

        $got->{$key}[OFF_PARSED] = 0 ;
    }

    for my $i (0.. @entered / 2 - 1) {
        my $key = $entered[2* $i] ;
        my $value = $entered[2* $i+1] ;

        #print "Key [$key] Value [$value]" ;
        #print defined $$value ? "[$$value]\n" : "[undef]\n";

        $key =~ s/^-// ;
        my $canonkey = lc $key;
 
        if ($got->{$canonkey} && ($firstTime ||
                                  ! $got->{$canonkey}[OFF_FIRST_ONLY]  ))
        {
            my $type = $got->{$canonkey}[OFF_TYPE] ;
            my $s ;
            $self->_checkType($key, $value, $type, 1, \$s)
                or return undef ;
            #$value = $$value unless $type & Parse_store_ref ;
            $value = $$value ;
            $got->{$canonkey} = [1, $type, $value, $s] ;
        }
        else
          { push (@Bad, $key) }
    }
 
    if (@Bad) {
        my ($bad) = join(", ", @Bad) ;
        return $self->setError("unknown key value(s) @Bad") ;
    }

    return 1;
}

sub Compress::Raw::Zlib::Parameters::_checkType
{
    my $self = shift ;

    my $key   = shift ;
    my $value = shift ;
    my $type  = shift ;
    my $validate  = shift ;
    my $output  = shift;

    #local $Carp::CarpLevel = $level ;
    #print "PARSE $type $key $value $validate $sub\n" ;
    if ( $type & Parse_store_ref)
    {
        #$value = $$value
        #    if ref ${ $value } ;

        $$output = $value ;
        return 1;
    }

    $value = $$value ;

    if ($type & Parse_any)
    {
        $$output = $value ;
        return 1;
    }
    elsif ($type & Parse_unsigned)
    {
        return $self->setError("Parameter '$key' must be an unsigned int, got 'undef'")
            if $validate && ! defined $value ;
        return $self->setError("Parameter '$key' must be an unsigned int, got '$value'")
            if $validate && $value !~ /^\d+$/;

        $$output = defined $value ? $value : 0 ;    
        return 1;
    }
    elsif ($type & Parse_signed)
    {
        return $self->setError("Parameter '$key' must be a signed int, got 'undef'")
            if $validate && ! defined $value ;
        return $self->setError("Parameter '$key' must be a signed int, got '$value'")
            if $validate && $value !~ /^-?\d+$/;

        $$output = defined $value ? $value : 0 ;    
        return 1 ;
    }
    elsif ($type & Parse_boolean)
    {
        return $self->setError("Parameter '$key' must be an int, got '$value'")
            if $validate && defined $value && $value !~ /^\d*$/;
        $$output =  defined $value ? $value != 0 : 0 ;    
        return 1;
    }
    elsif ($type & Parse_string)
    {
        $$output = defined $value ? $value : "" ;    
        return 1;
    }

    $$output = $value ;
    return 1;
}



sub Compress::Raw::Zlib::Parameters::parsed
{
    my $self = shift ;
    my $name = shift ;

    return $self->{Got}{lc $name}[OFF_PARSED] ;
}

sub Compress::Raw::Zlib::Parameters::value
{
    my $self = shift ;
    my $name = shift ;

    if (@_)
    {
        $self->{Got}{lc $name}[OFF_PARSED]  = 1;
        $self->{Got}{lc $name}[OFF_DEFAULT] = $_[0] ;
        $self->{Got}{lc $name}[OFF_FIXED]   = $_[0] ;
    }

    return $self->{Got}{lc $name}[OFF_FIXED] ;
}

sub Compress::Raw::Zlib::Deflate::new
{
    my $pkg = shift ;
    my ($got) = ParseParameters(0,
            {
                'AppendOutput'  => [1, 1, Parse_boolean,  0],
                'CRC32'         => [1, 1, Parse_boolean,  0],
                'ADLER32'       => [1, 1, Parse_boolean,  0],
                'Bufsize'       => [1, 1, Parse_unsigned, 4096],
 
                'Level'         => [1, 1, Parse_signed,   Z_DEFAULT_COMPRESSION()],
                'Method'        => [1, 1, Parse_unsigned, Z_DEFLATED()],
                'WindowBits'    => [1, 1, Parse_signed,   MAX_WBITS()],
                'MemLevel'      => [1, 1, Parse_unsigned, MAX_MEM_LEVEL()],
                'Strategy'      => [1, 1, Parse_unsigned, Z_DEFAULT_STRATEGY()],
                'Dictionary'    => [1, 1, Parse_any,      ""],
            }, @_) ;


    croak "Compress::Raw::Zlib::Deflate::new: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $flags = 0 ;
    $flags |= FLAG_APPEND if $got->value('AppendOutput') ;
    $flags |= FLAG_CRC    if $got->value('CRC32') ;
    $flags |= FLAG_ADLER  if $got->value('ADLER32') ;

    my $windowBits =  $got->value('WindowBits');
    $windowBits += MAX_WBITS()
        if ($windowBits & MAX_WBITS()) == 0 ;

    _deflateInit($flags,
                $got->value('Level'), 
                $got->value('Method'), 
                $windowBits, 
                $got->value('MemLevel'), 
                $got->value('Strategy'), 
                $got->value('Bufsize'),
                $got->value('Dictionary')) ;

}

sub Compress::Raw::Zlib::Inflate::new
{
    my $pkg = shift ;
    my ($got) = ParseParameters(0,
                    {
                        'AppendOutput'  => [1, 1, Parse_boolean,  0],
                        'LimitOutput'   => [1, 1, Parse_boolean,  0],
                        'CRC32'         => [1, 1, Parse_boolean,  0],
                        'ADLER32'       => [1, 1, Parse_boolean,  0],
                        'ConsumeInput'  => [1, 1, Parse_boolean,  1],
                        'Bufsize'       => [1, 1, Parse_unsigned, 4096],
                 
                        'WindowBits'    => [1, 1, Parse_signed,   MAX_WBITS()],
                        'Dictionary'    => [1, 1, Parse_any,      ""],
            }, @_) ;


    croak "Compress::Raw::Zlib::Inflate::new: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $flags = 0 ;
    $flags |= FLAG_APPEND if $got->value('AppendOutput') ;
    $flags |= FLAG_CRC    if $got->value('CRC32') ;
    $flags |= FLAG_ADLER  if $got->value('ADLER32') ;
    $flags |= FLAG_CONSUME_INPUT if $got->value('ConsumeInput') ;
    $flags |= FLAG_LIMIT_OUTPUT if $got->value('LimitOutput') ;


    my $windowBits =  $got->value('WindowBits');
    $windowBits += MAX_WBITS()
        if ($windowBits & MAX_WBITS()) == 0 ;

    _inflateInit($flags, $windowBits, $got->value('Bufsize'), 
                 $got->value('Dictionary')) ;
}

sub Compress::Raw::Zlib::InflateScan::new
{
    my $pkg = shift ;
    my ($got) = ParseParameters(0,
                    {
                        'CRC32'         => [1, 1, Parse_boolean,  0],
                        'ADLER32'       => [1, 1, Parse_boolean,  0],
                        'Bufsize'       => [1, 1, Parse_unsigned, 4096],
                 
                        'WindowBits'    => [1, 1, Parse_signed,   -MAX_WBITS()],
                        'Dictionary'    => [1, 1, Parse_any,      ""],
            }, @_) ;


    croak "Compress::Raw::Zlib::InflateScan::new: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $flags = 0 ;
    #$flags |= FLAG_APPEND if $got->value('AppendOutput') ;
    $flags |= FLAG_CRC    if $got->value('CRC32') ;
    $flags |= FLAG_ADLER  if $got->value('ADLER32') ;
    #$flags |= FLAG_CONSUME_INPUT if $got->value('ConsumeInput') ;

    _inflateScanInit($flags, $got->value('WindowBits'), $got->value('Bufsize'), 
                 '') ;
}

sub Compress::Raw::Zlib::inflateScanStream::createDeflateStream
{
    my $pkg = shift ;
    my ($got) = ParseParameters(0,
            {
                'AppendOutput'  => [1, 1, Parse_boolean,  0],
                'CRC32'         => [1, 1, Parse_boolean,  0],
                'ADLER32'       => [1, 1, Parse_boolean,  0],
                'Bufsize'       => [1, 1, Parse_unsigned, 4096],
 
                'Level'         => [1, 1, Parse_signed,   Z_DEFAULT_COMPRESSION()],
                'Method'        => [1, 1, Parse_unsigned, Z_DEFLATED()],
                'WindowBits'    => [1, 1, Parse_signed,   - MAX_WBITS()],
                'MemLevel'      => [1, 1, Parse_unsigned, MAX_MEM_LEVEL()],
                'Strategy'      => [1, 1, Parse_unsigned, Z_DEFAULT_STRATEGY()],
            }, @_) ;

    croak "Compress::Raw::Zlib::InflateScan::createDeflateStream: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        unless $got->value('Bufsize') >= 1;

    my $flags = 0 ;
    $flags |= FLAG_APPEND if $got->value('AppendOutput') ;
    $flags |= FLAG_CRC    if $got->value('CRC32') ;
    $flags |= FLAG_ADLER  if $got->value('ADLER32') ;

    $pkg->_createDeflateStream($flags,
                $got->value('Level'), 
                $got->value('Method'), 
                $got->value('WindowBits'), 
                $got->value('MemLevel'), 
                $got->value('Strategy'), 
                $got->value('Bufsize'),
                ) ;

}

sub Compress::Raw::Zlib::inflateScanStream::inflate
{
    my $self = shift ;
    my $buffer = $_[1];
    my $eof = $_[2];

    my $status = $self->scan(@_);

    if ($status == Z_OK() && $_[2]) {
        my $byte = ' ';
        
        $status = $self->scan(\$byte, $_[1]) ;
    }
    
    return $status ;
}

sub Compress::Raw::Zlib::deflateStream::deflateParams
{
    my $self = shift ;
    my ($got) = ParseParameters(0, {
                'Level'      => [1, 1, Parse_signed,   undef],
                'Strategy'   => [1, 1, Parse_unsigned, undef],
                'Bufsize'    => [1, 1, Parse_unsigned, undef],
                }, 
                @_) ;

    croak "Compress::Raw::Zlib::deflateParams needs Level and/or Strategy"
        unless $got->parsed('Level') + $got->parsed('Strategy') +
            $got->parsed('Bufsize');

    croak "Compress::Raw::Zlib::Inflate::deflateParams: Bufsize must be >= 1, you specified " . 
            $got->value('Bufsize')
        if $got->parsed('Bufsize') && $got->value('Bufsize') <= 1;

    my $flags = 0;
    $flags |= 1 if $got->parsed('Level') ;
    $flags |= 2 if $got->parsed('Strategy') ;
    $flags |= 4 if $got->parsed('Bufsize') ;

    $self->_deflateParams($flags, $got->value('Level'), 
                          $got->value('Strategy'), $got->value('Bufsize'));

}


# Autoload methods go after __END__, and are processed by the autosplit program.

1;
__END__


#line 1430
FILE   f3dea8e3/Config.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Config.pm"
# This file was created by configpm when Perl was built. Any changes
# made to this file will be lost the next time perl is built.

# for a description of the variables, please have a look at the
# Glossary file, as written in the Porting folder, or use the url:
# http://perl5.git.perl.org/perl.git/blob/HEAD:/Porting/Glossary

package Config;
use strict;
use warnings;
use vars '%Config';

# Skip @Config::EXPORT because it only contains %Config, which we special
# case below as it's not a function. @Config::EXPORT won't change in the
# lifetime of Perl 5.
my %Export_Cache = (myconfig => 1, config_sh => 1, config_vars => 1,
		    config_re => 1, compile_date => 1, local_patches => 1,
		    bincompat_options => 1, non_bincompat_options => 1,
		    header_files => 1);

@Config::EXPORT = qw(%Config);
@Config::EXPORT_OK = keys %Export_Cache;

# Need to stub all the functions to make code such as print Config::config_sh
# keep working

sub bincompat_options;
sub compile_date;
sub config_re;
sub config_sh;
sub config_vars;
sub header_files;
sub local_patches;
sub myconfig;
sub non_bincompat_options;

# Define our own import method to avoid pulling in the full Exporter:
sub import {
    shift;
    @_ = @Config::EXPORT unless @_;

    my @funcs = grep $_ ne '%Config', @_;
    my $export_Config = @funcs < @_ ? 1 : 0;

    no strict 'refs';
    my $callpkg = caller(0);
    foreach my $func (@funcs) {
	die qq{"$func" is not exported by the Config module\n}
	    unless $Export_Cache{$func};
	*{$callpkg.'::'.$func} = \&{$func};
    }

    *{"$callpkg\::Config"} = \%Config if $export_Config;
    return;
}

die "Perl lib version (5.14.2) doesn't match executable '$0' version ($])"
    unless $^V;

$^V eq 5.14.2
    or die "Perl lib version (5.14.2) doesn't match executable '$0' version (" .
	sprintf("v%vd",$^V) . ")";


sub FETCH {
    my($self, $key) = @_;

    # check for cached value (which may be undef so we use exists not defined)
    return exists $self->{$key} ? $self->{$key} : $self->fetch_string($key);
}

sub TIEHASH {
    bless $_[1], $_[0];
}

sub DESTROY { }

sub AUTOLOAD {
    require 'Config_heavy.pl';
    goto \&launcher unless $Config::AUTOLOAD =~ /launcher$/;
    die "&Config::AUTOLOAD failed on $Config::AUTOLOAD";
}

# tie returns the object, so the value returned to require will be true.
tie %Config, 'Config', {
    archlibexp => '/usr/lib/perl5/5.14/i686-cygwin-threads-64int',
    archname => 'cygwin-thread-multi-64int',
    cc => 'gcc-4',
    d_readlink => 'define',
    d_symlink => 'define',
    dlext => 'dll',
    dlsrc => 'dl_dlopen.xs',
    dont_use_nlink => undef,
    exe_ext => '.exe',
    inc_version_list => ' ',
    intsize => '4',
    ldlibpthname => 'PATH',
    libpth => '/usr/local/lib /usr/lib /lib',
    osname => 'cygwin',
    osvers => '1.7.15(0.26053)',
    path_sep => ':',
    privlibexp => '/usr/lib/perl5/5.14',
    scriptdir => '/usr/bin',
    sitearchexp => '/usr/lib/perl5/site_perl/5.14/i686-cygwin-threads-64int',
    sitelibexp => '/usr/lib/perl5/site_perl/5.14',
    so => 'dll',
    useithreads => 'define',
    usevendorprefix => 'define',
    version => '5.14.2',
};
FILE   c33fbebe/Config_git.pl  �######################################################################
# WARNING: 'lib/Config_git.pl' is generated by make_patchnum.pl
#          DO NOT EDIT DIRECTLY - edit make_patchnum.pl instead
######################################################################
$Config::Git_Data=<<'ENDOFGIT';
git_commit_id=''
git_describe=''
git_branch=''
git_uncommitted_changes=''
git_commit_id_title=''

ENDOFGIT
FILE   dbae88bf/Config_heavy.pl  �c# This file was created by configpm when Perl was built. Any changes
# made to this file will be lost the next time perl is built.

package Config;
use strict;
use warnings;
use vars '%Config';

sub bincompat_options {
    return split ' ', (Internals::V())[0];
}

sub non_bincompat_options {
    return split ' ', (Internals::V())[1];
}

sub compile_date {
    return (Internals::V())[2]
}

sub local_patches {
    my (undef, undef, undef, @patches) = Internals::V();
    return @patches;
}

sub _V {
    my ($bincompat, $non_bincompat, $date, @patches) = Internals::V();

    my $opts = join ' ', sort split ' ', "$bincompat $non_bincompat";

    # wrap at 76 columns.

    $opts =~ s/(?=.{53})(.{1,53}) /$1\n                        /mg;

    print Config::myconfig();
    if ($^O eq 'VMS') {
        print "\nCharacteristics of this PERLSHR image: \n";
    } else {
        print "\nCharacteristics of this binary (from libperl): \n";
    }

    print "  Compile-time options: $opts\n";

    if (@patches) {
        print "  Locally applied patches:\n";
        print "\t$_\n" foreach @patches;
    }

    print "  Built under $^O\n";

    print "  $date\n" if defined $date;

    my @env = map { "$_=\"$ENV{$_}\"" } sort grep {/^PERL/} keys %ENV;
    push @env, "CYGWIN=\"$ENV{CYGWIN}\"" if $^O eq 'cygwin' and $ENV{CYGWIN};

    if (@env) {
        print "  \%ENV:\n";
        print "    $_\n" foreach @env;
    }
    print "  \@INC:\n";
    print "    $_\n" foreach @INC;
}

sub header_files {
    return qw(EXTERN.h INTERN.h XSUB.h av.h config.h cop.h cv.h
              dosish.h embed.h embedvar.h form.h gv.h handy.h hv.h intrpvar.h
              iperlsys.h keywords.h mg.h nostdio.h op.h opcode.h pad.h
              parser.h patchlevel.h perl.h perlio.h perliol.h perlsdio.h
              perlsfio.h perlvars.h perly.h pp.h pp_proto.h proto.h regcomp.h
              regexp.h regnodes.h scope.h sv.h thread.h time64.h unixish.h
              utf8.h util.h);
}

##
## This file was produced by running the Configure script. It holds all the
## definitions figured out by Configure. Should you modify one of these values,
## do not forget to propagate your changes by running "Configure -der". You may
## instead choose to run each of the .SH files by yourself, or "Configure -S".
##
#
## Package name      : perl5
## Source directory  : /usr/src/perl/perl-5.14/perl-5.14.2
## Configuration time: Thu Jul 12 13:58:56 CDT 2012
## Configured by     : rurban
## Target system     : cygwin_nt-5.1 winxp 1.7.15(0.26053) 2012-05-09 10:25 i686 cygwin 
#
#: Configure command line arguments.
#

our $summary = <<'!END!';
Summary of my $package (revision $revision $version_patchlevel_string) configuration:
  $git_commit_id_title $git_commit_id$git_ancestor_line
  Platform:
    osname=$osname, osvers=$osvers, archname=$archname
    uname='$myuname'
    config_args='$config_args'
    hint=$hint, useposix=$useposix, d_sigaction=$d_sigaction
    useithreads=$useithreads, usemultiplicity=$usemultiplicity
    useperlio=$useperlio, d_sfio=$d_sfio, uselargefiles=$uselargefiles, usesocks=$usesocks
    use64bitint=$use64bitint, use64bitall=$use64bitall, uselongdouble=$uselongdouble
    usemymalloc=$usemymalloc, bincompat5005=undef
  Compiler:
    cc='$cc', ccflags ='$ccflags',
    optimize='$optimize',
    cppflags='$cppflags'
    ccversion='$ccversion', gccversion='$gccversion', gccosandvers='$gccosandvers'
    intsize=$intsize, longsize=$longsize, ptrsize=$ptrsize, doublesize=$doublesize, byteorder=$byteorder
    d_longlong=$d_longlong, longlongsize=$longlongsize, d_longdbl=$d_longdbl, longdblsize=$longdblsize
    ivtype='$ivtype', ivsize=$ivsize, nvtype='$nvtype', nvsize=$nvsize, Off_t='$lseektype', lseeksize=$lseeksize
    alignbytes=$alignbytes, prototype=$prototype
  Linker and Libraries:
    ld='$ld', ldflags ='$ldflags'
    libpth=$libpth
    libs=$libs
    perllibs=$perllibs
    libc=$libc, so=$so, useshrplib=$useshrplib, libperl=$libperl
    gnulibc_version='$gnulibc_version'
  Dynamic Linking:
    dlsrc=$dlsrc, dlext=$dlext, d_dlsymun=$d_dlsymun, ccdlflags='$ccdlflags'
    cccdlflags='$cccdlflags', lddlflags='$lddlflags'

!END!
my $summary_expanded;

sub myconfig {
    return $summary_expanded if $summary_expanded;
    ($summary_expanded = $summary) =~ s{\$(\w+)}
		 { 
			my $c;
			if ($1 eq 'git_ancestor_line') {
				if ($Config::Config{git_ancestor}) {
					$c= "\n  Ancestor: $Config::Config{git_ancestor}";
				} else {
					$c= "";
				}
			} else {
                     		$c = $Config::Config{$1}; 
			}
			defined($c) ? $c : 'undef' 
		}ge;
    $summary_expanded;
}

local *_ = \my $a;
$_ = <<'!END!';
Author=''
CONFIG='true'
Date='$Date'
Header=''
Id='$Id'
Locker=''
Log='$Log'
PATCHLEVEL='14'
PERL_API_REVISION='5'
PERL_API_SUBVERSION='0'
PERL_API_VERSION='14'
PERL_CONFIG_SH='true'
PERL_PATCHLEVEL=''
PERL_REVISION='5'
PERL_SUBVERSION='2'
PERL_VERSION='14'
RCSfile='$RCSfile'
Revision='$Revision'
SUBVERSION='2'
Source=''
State=''
_a='.a'
_exe='.exe'
_o='.o'
afs='false'
afsroot='/afs'
alignbytes='8'
ansi2knr=''
aphostname='/usr/bin/hostname'
api_revision='5'
api_subversion='0'
api_version='14'
api_versionstring='5.14.0'
ar='ar'
archlib='/usr/lib/perl5/5.14/i686-cygwin-threads-64int'
archlibexp='/usr/lib/perl5/5.14/i686-cygwin-threads-64int'
archname64='64int'
archname='cygwin-thread-multi-64int'
archobjs='cygwin.o'
asctime_r_proto='REENTRANT_PROTO_B_SB'
awk='awk'
baserev='5.0'
bash=''
bin='/usr/bin'
bin_ELF='undef'
binexp='/usr/bin'
bison='bison'
byacc='byacc'
byteorder='12345678'
c=''
castflags='0'
cat='cat'
cc='gcc-4'
cccdlflags=' '
ccdlflags=' '
ccflags='-DPERL_USE_SAFE_PUTENV -U__STRICT_ANSI__ -g -fno-strict-aliasing -pipe -fstack-protector'
ccflags_uselargefiles=''
ccname='gcc'
ccsymbols='__CYGWIN__=1 __CYGWIN32__=1 __unix=1 __unix__=1'
ccversion=''
cf_by='rurban'
cf_email='cygwin@cygwin.com'
cf_time='Thu Jul 12 13:58:56 CDT 2012'
charbits='8'
charsize='1'
chgrp=''
chmod='chmod'
chown=''
clocktype='clock_t'
comm='comm'
compress=''
config_arg0='/usr/src/perl/perl-5.14/perl-5.14.2/Configure'
config_arg1='-de'
config_arg2='-Dlibperl=cygperl5_14.dll'
config_arg3='-Dcc=gcc-4'
config_arg4='-Dld=g++-4'
config_arg5='-Darchname=i686-cygwin-threads-64int'
config_arg6='-Dmksymlinks'
config_arg7='-Dusethreads'
config_arg8='-Accflags=-g'
config_argc='8'
config_args='-de -Dlibperl=cygperl5_14.dll -Dcc=gcc-4 -Dld=g++-4 -Darchname=i686-cygwin-threads-64int -Dmksymlinks -Dusethreads -Accflags=-g'
contains='grep'
cp='cp'
cpio=''
cpp='cpp'
cpp_stuff='42'
cppccsymbols='unix=1'
cppflags='-DPERL_USE_SAFE_PUTENV -U__STRICT_ANSI__ -g -fno-strict-aliasing -pipe -fstack-protector'
cpplast='-'
cppminus='-'
cpprun='gcc-4 -E'
cppstdin='gcc-4 -E'
cppsymbols='BIG_ENDIAN=4321 __BIG_ENDIAN=4321 __CYGWIN__=1 __GNUC__=4 __GNUC_MINOR__=5 i386=1 __i386=1 __i386__=1 __i686=1 __i686__=1 LITTLE_ENDIAN=1234 __LITTLE_ENDIAN=1234 _LONG_DOUBLE=long\ double __STDC__=1 __unix=1 __unix__=1'
crypt_r_proto='0'
cryptlib=''
csh='csh'
ctermid_r_proto='0'
ctime_r_proto='REENTRANT_PROTO_B_SB'
d_Gconvert='sprintf((b),"%.*g",(n),(x))'
d_PRIEUldbl='define'
d_PRIFUldbl='define'
d_PRIGUldbl='define'
d_PRIXU64='define'
d_PRId64='define'
d_PRIeldbl='define'
d_PRIfldbl='define'
d_PRIgldbl='define'
d_PRIi64='define'
d_PRIo64='define'
d_PRIu64='define'
d_PRIx64='define'
d_SCNfldbl='define'
d__fwalk='undef'
d_access='define'
d_accessx='undef'
d_aintl='undef'
d_alarm='define'
d_archlib='define'
d_asctime64='undef'
d_asctime_r='define'
d_atolf='undef'
d_atoll='define'
d_attribute_deprecated='define'
d_attribute_format='define'
d_attribute_malloc='define'
d_attribute_nonnull='define'
d_attribute_noreturn='define'
d_attribute_pure='define'
d_attribute_unused='define'
d_attribute_warn_unused_result='define'
d_bcmp='define'
d_bcopy='define'
d_bsd='define'
d_bsdgetpgrp='undef'
d_bsdsetpgrp='undef'
d_builtin_choose_expr='define'
d_builtin_expect='define'
d_bzero='define'
d_c99_variadic_macros='define'
d_casti32='undef'
d_castneg='define'
d_charvspr='undef'
d_chown='define'
d_chroot='define'
d_chsize='undef'
d_class='undef'
d_clearenv='undef'
d_closedir='define'
d_cmsghdr_s='define'
d_const='define'
d_copysignl='undef'
d_cplusplus='undef'
d_crypt='define'
d_crypt_r='undef'
d_csh='undef'
d_ctermid='define'
d_ctermid_r='undef'
d_ctime64='undef'
d_ctime_r='define'
d_cuserid='define'
d_dbl_dig='define'
d_dbminitproto='define'
d_difftime64='undef'
d_difftime='define'
d_dir_dd_fd='undef'
d_dirfd='define'
d_dirnamlen='undef'
d_dlerror='define'
d_dlopen='define'
d_dlsymun='undef'
d_dosuid='undef'
d_drand48_r='undef'
d_drand48proto='define'
d_dup2='define'
d_eaccess='define'
d_endgrent='define'
d_endgrent_r='undef'
d_endhent='define'
d_endhostent_r='undef'
d_endnent='undef'
d_endnetent_r='undef'
d_endpent='define'
d_endprotoent_r='undef'
d_endpwent='define'
d_endpwent_r='undef'
d_endsent='define'
d_endservent_r='undef'
d_eofnblk='define'
d_eunice='undef'
d_faststdio='define'
d_fchdir='define'
d_fchmod='define'
d_fchown='define'
d_fcntl='define'
d_fcntl_can_lock='define'
d_fd_macros='define'
d_fd_set='define'
d_fds_bits='define'
d_fgetpos='define'
d_finite='define'
d_finitel='undef'
d_flexfnam='define'
d_flock='define'
d_flockproto='define'
d_fork='define'
d_fp_class='undef'
d_fpathconf='define'
d_fpclass='undef'
d_fpclassify='undef'
d_fpclassl='undef'
d_fpos64_t='undef'
d_frexpl='undef'
d_fs_data_s='undef'
d_fseeko='define'
d_fsetpos='define'
d_fstatfs='define'
d_fstatvfs='define'
d_fsync='define'
d_ftello='define'
d_ftime='undef'
d_futimes='define'
d_gdbm_ndbm_h_uses_prototypes='undef'
d_gdbmndbm_h_uses_prototypes='undef'
d_getaddrinfo='define'
d_getcwd='define'
d_getespwnam='undef'
d_getfsstat='undef'
d_getgrent='define'
d_getgrent_r='undef'
d_getgrgid_r='define'
d_getgrnam_r='define'
d_getgrps='define'
d_gethbyaddr='define'
d_gethbyname='define'
d_gethent='undef'
d_gethname='define'
d_gethostbyaddr_r='undef'
d_gethostbyname_r='undef'
d_gethostent_r='undef'
d_gethostprotos='define'
d_getitimer='define'
d_getlogin='define'
d_getlogin_r='define'
d_getmnt='undef'
d_getmntent='define'
d_getnameinfo='define'
d_getnbyaddr='undef'
d_getnbyname='undef'
d_getnent='undef'
d_getnetbyaddr_r='undef'
d_getnetbyname_r='undef'
d_getnetent_r='undef'
d_getnetprotos='define'
d_getpagsz='define'
d_getpbyname='define'
d_getpbynumber='define'
d_getpent='define'
d_getpgid='define'
d_getpgrp2='undef'
d_getpgrp='define'
d_getppid='define'
d_getprior='define'
d_getprotobyname_r='undef'
d_getprotobynumber_r='undef'
d_getprotoent_r='undef'
d_getprotoprotos='define'
d_getprpwnam='undef'
d_getpwent='define'
d_getpwent_r='undef'
d_getpwnam_r='define'
d_getpwuid_r='define'
d_getsbyname='define'
d_getsbyport='define'
d_getsent='define'
d_getservbyname_r='undef'
d_getservbyport_r='undef'
d_getservent_r='undef'
d_getservprotos='define'
d_getspnam='undef'
d_getspnam_r='undef'
d_gettimeod='define'
d_gmtime64='undef'
d_gmtime_r='define'
d_gnulibc='undef'
d_grpasswd='define'
d_hasmntopt='undef'
d_htonl='define'
d_ilogbl='undef'
d_inc_version_list='undef'
d_index='undef'
d_inetaton='define'
d_inetntop='define'
d_inetpton='define'
d_int64_t='define'
d_isascii='define'
d_isfinite='undef'
d_isinf='define'
d_isnan='define'
d_isnanl='undef'
d_killpg='define'
d_lchown='define'
d_ldbl_dig='define'
d_libm_lib_version='undef'
d_link='define'
d_localtime64='undef'
d_localtime_r='define'
d_localtime_r_needs_tzset='undef'
d_locconv='define'
d_lockf='define'
d_longdbl='define'
d_longlong='define'
d_lseekproto='define'
d_lstat='define'
d_madvise='define'
d_malloc_good_size='undef'
d_malloc_size='undef'
d_mblen='define'
d_mbstowcs='define'
d_mbtowc='define'
d_memchr='define'
d_memcmp='define'
d_memcpy='define'
d_memmove='define'
d_memset='define'
d_mkdir='define'
d_mkdtemp='define'
d_mkfifo='define'
d_mkstemp='define'
d_mkstemps='define'
d_mktime64='undef'
d_mktime='define'
d_mmap='define'
d_modfl='undef'
d_modfl_pow32_bug='undef'
d_modflproto='undef'
d_mprotect='define'
d_msg='define'
d_msg_ctrunc='define'
d_msg_dontroute='define'
d_msg_oob='define'
d_msg_peek='define'
d_msg_proxy='undef'
d_msgctl='define'
d_msgget='define'
d_msghdr_s='define'
d_msgrcv='define'
d_msgsnd='define'
d_msync='define'
d_munmap='define'
d_mymalloc='undef'
d_ndbm='define'
d_ndbm_h_uses_prototypes='undef'
d_nice='define'
d_nl_langinfo='define'
d_nv_preserves_uv='undef'
d_nv_zero_is_allbits_zero='define'
d_off64_t='undef'
d_old_pthread_create_joinable='undef'
d_oldpthreads='undef'
d_oldsock='undef'
d_open3='define'
d_pathconf='define'
d_pause='define'
d_perl_otherlibdirs='define'
d_phostname='undef'
d_pipe='define'
d_poll='define'
d_portable='define'
d_prctl='undef'
d_prctl_set_name='undef'
d_printf_format_null='define'
d_procselfexe='define'
d_pseudofork='undef'
d_pthread_atfork='define'
d_pthread_attr_setscope='undef'
d_pthread_yield='define'
d_pwage='undef'
d_pwchange='undef'
d_pwclass='undef'
d_pwcomment='define'
d_pwexpire='undef'
d_pwgecos='define'
d_pwpasswd='define'
d_pwquota='undef'
d_qgcvt='undef'
d_quad='define'
d_random_r='undef'
d_readdir64_r='undef'
d_readdir='define'
d_readdir_r='define'
d_readlink='define'
d_readv='define'
d_recvmsg='define'
d_rename='define'
d_rewinddir='define'
d_rmdir='define'
d_safebcpy='undef'
d_safemcpy='undef'
d_sanemcmp='define'
d_sbrkproto='define'
d_scalbnl='undef'
d_sched_yield='define'
d_scm_rights='define'
d_seekdir='define'
d_select='define'
d_sem='define'
d_semctl='define'
d_semctl_semid_ds='define'
d_semctl_semun='define'
d_semget='define'
d_semop='define'
d_sendmsg='define'
d_setegid='define'
d_seteuid='define'
d_setgrent='define'
d_setgrent_r='undef'
d_setgrps='define'
d_sethent='define'
d_sethostent_r='undef'
d_setitimer='define'
d_setlinebuf='define'
d_setlocale='define'
d_setlocale_r='undef'
d_setnent='undef'
d_setnetent_r='undef'
d_setpent='define'
d_setpgid='define'
d_setpgrp2='undef'
d_setpgrp='define'
d_setprior='define'
d_setproctitle='undef'
d_setprotoent_r='undef'
d_setpwent='define'
d_setpwent_r='undef'
d_setregid='define'
d_setresgid='undef'
d_setresuid='undef'
d_setreuid='define'
d_setrgid='undef'
d_setruid='undef'
d_setsent='define'
d_setservent_r='undef'
d_setsid='define'
d_setvbuf='define'
d_sfio='undef'
d_shm='define'
d_shmat='define'
d_shmatprototype='define'
d_shmctl='define'
d_shmdt='define'
d_shmget='define'
d_sigaction='define'
d_signbit='define'
d_sigprocmask='define'
d_sigsetjmp='define'
d_sin6_scope_id='define'
d_sitearch='define'
d_snprintf='define'
d_sockaddr_sa_len='undef'
d_sockatmark='undef'
d_sockatmarkproto='undef'
d_socket='define'
d_socklen_t='define'
d_sockpair='define'
d_socks5_init='undef'
d_sprintf_returns_strlen='define'
d_sqrtl='undef'
d_srand48_r='undef'
d_srandom_r='undef'
d_sresgproto='undef'
d_sresuproto='undef'
d_statblks='define'
d_statfs_f_flags='undef'
d_statfs_s='define'
d_static_inline='define'
d_statvfs='define'
d_stdio_cnt_lval='define'
d_stdio_ptr_lval='define'
d_stdio_ptr_lval_nochange_cnt='define'
d_stdio_ptr_lval_sets_cnt='undef'
d_stdio_stream_array='undef'
d_stdiobase='define'
d_stdstdio='define'
d_strchr='define'
d_strcoll='define'
d_strctcpy='define'
d_strerrm='strerror(e)'
d_strerror='define'
d_strerror_r='define'
d_strftime='define'
d_strlcat='define'
d_strlcpy='define'
d_strtod='define'
d_strtol='define'
d_strtold='define'
d_strtoll='define'
d_strtoq='undef'
d_strtoul='define'
d_strtoull='define'
d_strtouq='undef'
d_strxfrm='define'
d_suidsafe='undef'
d_symlink='define'
d_syscall='undef'
d_syscallproto='undef'
d_sysconf='define'
d_sysernlst=''
d_syserrlst='undef'
d_system='define'
d_tcgetpgrp='define'
d_tcsetpgrp='define'
d_telldir='define'
d_telldirproto='define'
d_time='define'
d_timegm='define'
d_times='define'
d_tm_tm_gmtoff='undef'
d_tm_tm_zone='undef'
d_tmpnam_r='undef'
d_truncate='define'
d_ttyname_r='define'
d_tzname='undef'
d_u32align='define'
d_ualarm='define'
d_umask='define'
d_uname='define'
d_union_semun='undef'
d_unordered='undef'
d_unsetenv='define'
d_usleep='define'
d_usleepproto='define'
d_ustat='undef'
d_vendorarch='define'
d_vendorbin='define'
d_vendorlib='define'
d_vendorscript='define'
d_vfork='undef'
d_void_closedir='undef'
d_voidsig='define'
d_voidtty=''
d_volatile='define'
d_vprintf='define'
d_vsnprintf='define'
d_wait4='define'
d_waitpid='define'
d_wcstombs='define'
d_wctomb='define'
d_writev='define'
d_xenix='undef'
date='date'
db_hashtype='u_int32_t'
db_prefixtype='size_t'
db_version_major='4'
db_version_minor='8'
db_version_patch='30'
defvoidused='15'
direntrytype='struct dirent'
dlext='dll'
dlsrc='dl_dlopen.xs'
doublesize='8'
drand01='drand48()'
drand48_r_proto='0'
dtrace=''
dynamic_ext='attributes B Compress/Raw/Bzip2 Compress/Raw/Zlib Cwd Data/Dumper DB_File Devel/DProf Devel/Peek Devel/PPPort Digest/MD5 Digest/SHA Encode Fcntl File/Glob Filter/Util/Call GDBM_File Hash/Util Hash/Util/FieldHash I18N/Langinfo IO IPC/SysV List/Util Math/BigInt/FastCalc MIME/Base64 mro NDBM_File ODBM_File Opcode PerlIO/encoding PerlIO/scalar PerlIO/via POSIX re SDBM_File Socket Storable Sys/Hostname Sys/Syslog Text/Soundex threads threads/shared Tie/Hash/NamedCapture Time/HiRes Time/Piece Unicode/Collate Unicode/Normalize Win32 Win32API/File XS/APItest XS/Typemap'
eagain='EAGAIN'
ebcdic='undef'
echo='echo'
egrep='egrep'
emacs=''
endgrent_r_proto='0'
endhostent_r_proto='0'
endnetent_r_proto='0'
endprotoent_r_proto='0'
endpwent_r_proto='0'
endservent_r_proto='0'
eunicefix=':'
exe_ext='.exe'
expr='expr'
extensions='attributes B Compress/Raw/Bzip2 Compress/Raw/Zlib Cwd Data/Dumper DB_File Devel/DProf Devel/Peek Devel/PPPort Digest/MD5 Digest/SHA Encode Fcntl File/Glob Filter/Util/Call GDBM_File Hash/Util Hash/Util/FieldHash I18N/Langinfo IO IPC/SysV List/Util Math/BigInt/FastCalc MIME/Base64 mro NDBM_File ODBM_File Opcode PerlIO/encoding PerlIO/scalar PerlIO/via POSIX re SDBM_File Socket Storable Sys/Hostname Sys/Syslog Text/Soundex threads threads/shared Tie/Hash/NamedCapture Time/HiRes Time/Piece Unicode/Collate Unicode/Normalize Win32 Win32API/File XS/APItest XS/Typemap Win32CORE Archive/Extract Archive/Tar Attribute/Handlers autodie AutoLoader autouse B/Debug B/Deparse B/Lint base bignum CGI constant CPAN CPAN/Meta CPAN/Meta/YAML CPANPLUS CPANPLUS/Dist/Build Devel/SelfStubber Digest Dumpvalue encoding/warnings Env Errno ExtUtils/CBuilder ExtUtils/Command ExtUtils/Constant ExtUtils/Install ExtUtils/MakeMaker ExtUtils/Manifest ExtUtils/ParseXS File/CheckTree File/Fetch File/Path File/Temp FileCache Filter/Simple Getopt/Long HTTP/Tiny I18N/Collate I18N/LangTags if IO/Compress IO/Zlib IPC/Cmd IPC/Open2 IPC/Open3 JSON/PP lib libnet Locale/Codes Locale/Maketext Locale/Maketext/Simple Log/Message Log/Message/Simple Math/BigInt Math/BigRat Math/Complex Memoize Module/Build Module/CoreList Module/Load Module/Load/Conditional Module/Loaded Module/Metadata Module/Pluggable Net/Ping NEXT Object/Accessor Package/Constants Params/Check parent Parse/CPAN/Meta Perl/OSType PerlIO/via/QuotedPrint Pod/Escapes Pod/Html Pod/LaTeX Pod/Parser Pod/Perldoc Pod/Simple podlators Safe SelfLoader Shell Term/ANSIColor Term/Cap Term/UI Test Test/Harness Test/Simple Text/Balanced Text/ParseWords Text/Tabs Thread/Queue Thread/Semaphore Tie/File Tie/Memoize Tie/RefHash Time/Local Version/Requirements XSLoader'
extern_C='extern'
extras=''
fflushNULL='define'
fflushall='undef'
find=''
firstmakefile='GNUmakefile'
flex=''
fpossize='8'
fpostype='fpos_t'
freetype='void'
from=':'
full_ar='/usr/bin/ar'
full_csh='csh'
full_sed='/usr/bin/sed'
gccansipedantic=''
gccosandvers=''
gccversion='4.5.3'
getgrent_r_proto='0'
getgrgid_r_proto='REENTRANT_PROTO_I_TSBWR'
getgrnam_r_proto='REENTRANT_PROTO_I_CSBWR'
gethostbyaddr_r_proto='0'
gethostbyname_r_proto='0'
gethostent_r_proto='0'
getlogin_r_proto='REENTRANT_PROTO_I_BW'
getnetbyaddr_r_proto='0'
getnetbyname_r_proto='0'
getnetent_r_proto='0'
getprotobyname_r_proto='0'
getprotobynumber_r_proto='0'
getprotoent_r_proto='0'
getpwent_r_proto='0'
getpwnam_r_proto='REENTRANT_PROTO_I_CSBWR'
getpwuid_r_proto='REENTRANT_PROTO_I_TSBWR'
getservbyname_r_proto='0'
getservbyport_r_proto='0'
getservent_r_proto='0'
getspnam_r_proto='0'
gidformat='"lu"'
gidsign='1'
gidsize='4'
gidtype='gid_t'
glibpth='/usr/shlib  /lib /usr/lib /usr/lib/386 /lib/386 /usr/ccs/lib /usr/ucblib /usr/local/lib '
gmake='gmake'
gmtime_r_proto='REENTRANT_PROTO_S_TS'
gnulibc_version=''
grep='grep'
groupcat='cat /etc/group'
groupstype='gid_t'
gzip='gzip'
h_fcntl='false'
h_sysfile='true'
hint='recommended'
hostcat='cat /etc/hosts'
html1dir='/usr/share/doc/perl/html/html1'
html1direxp='/usr/share/doc/perl/html/html1'
html3dir='/usr/share/doc/perl/html/html3'
html3direxp='/usr/share/doc/perl/html/html3'
i16size='2'
i16type='short'
i32size='4'
i32type='long'
i64size='8'
i64type='long long'
i8size='1'
i8type='signed char'
i_arpainet='define'
i_assert='define'
i_bsdioctl=''
i_crypt='define'
i_db='define'
i_dbm='define'
i_dirent='define'
i_dld='undef'
i_dlfcn='define'
i_fcntl='undef'
i_float='define'
i_fp='undef'
i_fp_class='undef'
i_gdbm='define'
i_gdbm_ndbm='undef'
i_gdbmndbm='undef'
i_grp='define'
i_ieeefp='define'
i_inttypes='define'
i_langinfo='define'
i_libutil='undef'
i_limits='define'
i_locale='define'
i_machcthr='undef'
i_malloc='define'
i_mallocmalloc='undef'
i_math='define'
i_memory='undef'
i_mntent='define'
i_ndbm='define'
i_netdb='define'
i_neterrno='undef'
i_netinettcp='define'
i_niin='define'
i_poll='define'
i_prot='undef'
i_pthread='define'
i_pwd='define'
i_rpcsvcdbm='undef'
i_sfio='undef'
i_sgtty='undef'
i_shadow='undef'
i_socks='undef'
i_stdarg='define'
i_stddef='define'
i_stdlib='define'
i_string='define'
i_sunmath='undef'
i_sysaccess='undef'
i_sysdir='define'
i_sysfile='define'
i_sysfilio='undef'
i_sysin='undef'
i_sysioctl='define'
i_syslog='define'
i_sysmman='define'
i_sysmode='undef'
i_sysmount='define'
i_sysndir='undef'
i_sysparam='define'
i_syspoll='define'
i_sysresrc='define'
i_syssecrt='undef'
i_sysselct='define'
i_syssockio='undef'
i_sysstat='define'
i_sysstatfs='define'
i_sysstatvfs='define'
i_systime='define'
i_systimek='undef'
i_systimes='define'
i_systypes='define'
i_sysuio='define'
i_sysun='define'
i_sysutsname='define'
i_sysvfs='define'
i_syswait='define'
i_termio='undef'
i_termios='define'
i_time='undef'
i_unistd='define'
i_ustat='undef'
i_utime='define'
i_values='undef'
i_varargs='undef'
i_varhdr='stdarg.h'
i_vfork='undef'
ignore_versioned_solibs=''
inc_version_list=' '
inc_version_list_init='0'
incpath=''
inews=''
initialinstalllocation='/usr/bin'
installarchlib='/usr/lib/perl5/5.14/i686-cygwin-threads-64int'
installbin='/usr/bin'
installhtml1dir='/usr/share/doc/perl/html/html1'
installhtml3dir='/usr/share/doc/perl/html/html3'
installman1dir='/usr/share/man/man1'
installman3dir='/usr/share/man/man3'
installprefix='/usr'
installprefixexp='/usr'
installprivlib='/usr/lib/perl5/5.14'
installscript='/usr/bin'
installsitearch='/usr/lib/perl5/site_perl/5.14/i686-cygwin-threads-64int'
installsitebin='/usr/local/bin'
installsitehtml1dir='/usr/share/doc/perl/html/html1'
installsitehtml3dir='/usr/share/doc/perl/html/html3'
installsitelib='/usr/lib/perl5/site_perl/5.14'
installsiteman1dir='/usr/share/man/man1'
installsiteman3dir='/usr/share/man/man3'
installsitescript='/usr/local/bin'
installstyle='lib/perl5'
installusrbinperl='undef'
installvendorarch='/usr/lib/perl5/vendor_perl/5.14/i686-cygwin-threads-64int'
installvendorbin='/usr/bin'
installvendorhtml1dir='/usr/share/doc/perl/html/html1'
installvendorhtml3dir='/usr/share/doc/perl/html/html3'
installvendorlib='/usr/lib/perl5/vendor_perl/5.14'
installvendorman1dir='/usr/share/man/man1'
installvendorman3dir='/usr/share/man/man3'
installvendorscript='/usr/bin'
intsize='4'
issymlink='test -h'
ivdformat='"lld"'
ivsize='8'
ivtype='long long'
known_extensions='attributes B Compress/Raw/Bzip2 Compress/Raw/Zlib Cwd Data/Dumper DB_File Devel/DProf Devel/Peek Devel/PPPort Digest/MD5 Digest/SHA Encode Fcntl File/Glob Filter/Util/Call GDBM_File Hash/Util Hash/Util/FieldHash I18N/Langinfo IO IPC/SysV List/Util Math/BigInt/FastCalc MIME/Base64 mro NDBM_File ODBM_File Opcode PerlIO/encoding PerlIO/scalar PerlIO/via POSIX re SDBM_File Socket Storable Sys/Hostname Sys/Syslog Text/Soundex threads threads/shared Tie/Hash/NamedCapture Time/HiRes Time/Piece Unicode/Collate Unicode/Normalize VMS/DCLsym VMS/Stdio Win32 Win32API/File Win32CORE XS/APItest XS/Typemap '
ksh=''
ld='g++-4'
lddlflags=' --shared  -Wl,--enable-auto-import -Wl,--export-all-symbols -Wl,--enable-auto-image-base -L/usr/local/lib -fstack-protector'
ldflags=' -Wl,--enable-auto-import -Wl,--export-all-symbols -Wl,--enable-auto-image-base -fstack-protector -L/usr/local/lib'
ldflags_uselargefiles=''
ldlibpthname='PATH'
less='less.exe'
lib_ext='.a'
libc='/usr/lib/libc.a'
libperl='cygperl5_14.dll'
libpth='/usr/local/lib /usr/lib /lib'
libs='-lgdbm -ldb -ldl -lcrypt -lgdbm_compat'
libsdirs=' /usr/lib'
libsfiles=' libgdbm.a libdb.a libdl.a libcrypt.a libgdbm_compat.a'
libsfound=' /usr/lib/libgdbm.a /usr/lib/libdb.a /usr/lib/libdl.a /usr/lib/libcrypt.a /usr/lib/libgdbm_compat.a'
libspath=' /usr/local/lib /usr/lib /lib'
libswanted='   sfio socket bind inet nsl nm ndbm gdbm dbm db malloc dl dld ld sun crypt sec cposix posix ucb bsd BSD    gdbm_compat'
libswanted_uselargefiles=''
line=''
lint=''
lkflags=''
ln='ln'
lns='/usr/bin/ln.exe -s'
localtime_r_proto='REENTRANT_PROTO_S_TS'
locincpth='/usr/local/include /opt/local/include /usr/gnu/include /opt/gnu/include /usr/GNU/include /opt/GNU/include'
loclibpth='/usr/local/lib /opt/local/lib /usr/gnu/lib /opt/gnu/lib /usr/GNU/lib /opt/GNU/lib'
longdblsize='12'
longlongsize='8'
longsize='4'
lp=''
lpr=''
ls='ls'
lseeksize='8'
lseektype='off_t'
mad='undef'
madlyh=''
madlyobj=''
madlysrc=''
mail=''
mailx=''
make='make'
make_set_make='#'
mallocobj=''
mallocsrc=''
malloctype='void *'
man1dir='/usr/share/man/man1'
man1direxp='/usr/share/man/man1'
man1ext='1'
man3dir='/usr/share/man/man3'
man3direxp='/usr/share/man/man3'
man3ext='3pm'
mips_type=''
mistrustnm=''
mkdir='mkdir'
mmaptype='void *'
modetype='mode_t'
more='more'
multiarch='undef'
mv=''
myarchname='i686-cygwin'
mydomain='.x-ray.at'
myhostname='winxp'
myuname='cygwin_nt-5.1 winxp 1.7.15(0.26053) 2012-05-09 10:25 i686 cygwin '
n='-n'
need_va_copy='undef'
netdb_hlen_type='int'
netdb_host_type='const char *'
netdb_name_type='const char *'
netdb_net_type='long'
nm='nm'
nm_opt='-p'
nm_so_opt=''
nonxs_ext='Archive/Extract Archive/Tar Attribute/Handlers autodie AutoLoader autouse B/Debug B/Deparse B/Lint base bignum CGI constant CPAN CPAN/Meta CPAN/Meta/YAML CPANPLUS CPANPLUS/Dist/Build Devel/SelfStubber Digest Dumpvalue encoding/warnings Env Errno ExtUtils/CBuilder ExtUtils/Command ExtUtils/Constant ExtUtils/Install ExtUtils/MakeMaker ExtUtils/Manifest ExtUtils/ParseXS File/CheckTree File/Fetch File/Path File/Temp FileCache Filter/Simple Getopt/Long HTTP/Tiny I18N/Collate I18N/LangTags if IO/Compress IO/Zlib IPC/Cmd IPC/Open2 IPC/Open3 JSON/PP lib libnet Locale/Codes Locale/Maketext Locale/Maketext/Simple Log/Message Log/Message/Simple Math/BigInt Math/BigRat Math/Complex Memoize Module/Build Module/CoreList Module/Load Module/Load/Conditional Module/Loaded Module/Metadata Module/Pluggable Net/Ping NEXT Object/Accessor Package/Constants Params/Check parent Parse/CPAN/Meta Perl/OSType PerlIO/via/QuotedPrint Pod/Escapes Pod/Html Pod/LaTeX Pod/Parser Pod/Perldoc Pod/Simple podlators Safe SelfLoader Shell Term/ANSIColor Term/Cap Term/UI Test Test/Harness Test/Simple Text/Balanced Text/ParseWords Text/Tabs Thread/Queue Thread/Semaphore Tie/File Tie/Memoize Tie/RefHash Time/Local Version/Requirements XSLoader'
nroff='nroff'
nvEUformat='"E"'
nvFUformat='"F"'
nvGUformat='"G"'
nv_overflows_integers_at='256.0*256.0*256.0*256.0*256.0*256.0*2.0*2.0*2.0*2.0*2.0'
nv_preserves_uv_bits='53'
nveformat='"e"'
nvfformat='"f"'
nvgformat='"g"'
nvsize='8'
nvtype='double'
o_nonblock='O_NONBLOCK'
obj_ext='.o'
old_pthread_create_joinable=''
optimize='-O3'
orderlib='false'
osname='cygwin'
osvers='1.7.15(0.26053)'
otherlibdirs='/usr/lib/perl5/site_perl/5.10:/usr/lib/perl5/vendor_perl/5.10:/usr/lib/perl5/site_perl/5.8'
package='perl5'
pager='/usr/bin/less.exe -R'
passcat='cat /etc/passwd'
patchlevel='14'
path_sep=':'
perl5='/usr/bin/perl'
perl='perl'
perl_patchlevel=''
perl_static_inline='static __inline__'
perladmin='rurban@x-ray.at'
perllibs='-ldl -lcrypt'
perlpath='/usr/bin/perl'
pg='pg'
phostname='hostname'
pidtype='pid_t'
plibpth='/usr/lib'
pmake=''
pr=''
prefix='/usr'
prefixexp='/usr'
privlib='/usr/lib/perl5/5.14'
privlibexp='/usr/lib/perl5/5.14'
procselfexe='"/proc/self/exe"'
prototype='define'
ptrsize='4'
quadkind='3'
quadtype='long long'
randbits='48'
randfunc='drand48'
random_r_proto='0'
randseedtype='long'
ranlib=':'
rd_nodata='-1'
readdir64_r_proto='0'
readdir_r_proto='REENTRANT_PROTO_I_TSR'
revision='5'
rm='rm'
rm_try='/usr/bin/rm -f try try.exe a.out .out try.[cho] try..o core core.try* try.core*'
rmail=''
run=''
runnm='true'
sGMTIME_max='2147483647'
sGMTIME_min='-2147483648'
sLOCALTIME_max='2147483647'
sLOCALTIME_min='-2147483648'
sPRIEUldbl='"LE"'
sPRIFUldbl='"LF"'
sPRIGUldbl='"LG"'
sPRIXU64='"llX"'
sPRId64='"lld"'
sPRIeldbl='"Le"'
sPRIfldbl='"Lf"'
sPRIgldbl='"Lg"'
sPRIi64='"lli"'
sPRIo64='"llo"'
sPRIu64='"llu"'
sPRIx64='"llx"'
sSCNfldbl='"Lf"'
sched_yield='sched_yield()'
scriptdir='/usr/bin'
scriptdirexp='/usr/bin'
sed='sed'
seedfunc='srand48'
selectminbits='32'
selecttype='fd_set *'
sendmail=''
setgrent_r_proto='0'
sethostent_r_proto='0'
setlocale_r_proto='0'
setnetent_r_proto='0'
setprotoent_r_proto='0'
setpwent_r_proto='0'
setservent_r_proto='0'
sh='/bin/sh'
shar=''
sharpbang='#!'
shmattype='void *'
shortsize='2'
shrpenv=''
shsharp='true'
sig_count='33'
sig_name='ZERO HUP INT QUIT ILL TRAP ABRT EMT FPE KILL BUS SEGV SYS PIPE ALRM TERM URG STOP TSTP CONT CHLD TTIN TTOU IO XCPU XFSZ VTALRM PROF WINCH LOST USR1 USR2 RTMAX CLD POLL PWR RTMIN '
sig_name_init='"ZERO", "HUP", "INT", "QUIT", "ILL", "TRAP", "ABRT", "EMT", "FPE", "KILL", "BUS", "SEGV", "SYS", "PIPE", "ALRM", "TERM", "URG", "STOP", "TSTP", "CONT", "CHLD", "TTIN", "TTOU", "IO", "XCPU", "XFSZ", "VTALRM", "PROF", "WINCH", "LOST", "USR1", "USR2", "RTMAX", "CLD", "POLL", "PWR", "RTMIN", 0'
sig_num='0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 20 23 29 32 '
sig_num_init='0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 20, 23, 29, 32, 0'
sig_size='37'
signal_t='void'
sitearch='/usr/lib/perl5/site_perl/5.14/i686-cygwin-threads-64int'
sitearchexp='/usr/lib/perl5/site_perl/5.14/i686-cygwin-threads-64int'
sitebin='/usr/local/bin'
sitebinexp='/usr/local/bin'
sitehtml1dir='/usr/share/doc/perl/html/html1'
sitehtml1direxp='/usr/share/doc/perl/html/html1'
sitehtml3dir='/usr/share/doc/perl/html/html3'
sitehtml3direxp='/usr/share/doc/perl/html/html3'
sitelib='/usr/lib/perl5/site_perl/5.14'
sitelib_stem='/usr/lib/perl5/site_perl/5.14'
sitelibexp='/usr/lib/perl5/site_perl/5.14'
siteman1dir='/usr/share/man/man1'
siteman1direxp='/usr/share/man/man1'
siteman3dir='/usr/share/man/man3'
siteman3direxp='/usr/share/man/man3'
siteprefix='/usr'
siteprefixexp='/usr'
sitescript='/usr/local/bin'
sitescriptexp='/usr/local/bin'
sizesize='4'
sizetype='size_t'
sleep=''
smail=''
so='dll'
sockethdr=''
socketlib=''
socksizetype='socklen_t'
sort='sort'
spackage='Perl5'
spitshell='cat'
srand48_r_proto='0'
srandom_r_proto='0'
src='/usr/src/perl/perl-5.14/perl-5.14.2'
ssizetype='ssize_t'
startperl='#!/usr/bin/perl'
startsh='#!/bin/sh'
static_ext='Win32CORE'
stdchar='char'
stdio_base='((fp)->_ub._base ? (fp)->_ub._base : (fp)->_bf._base)'
stdio_bufsiz='((fp)->_ub._base ? (fp)->_ub._size : (fp)->_bf._size)'
stdio_cnt='((fp)->_r)'
stdio_filbuf=''
stdio_ptr='((fp)->_p)'
stdio_stream_array=''
strerror_r_proto='REENTRANT_PROTO_I_IBW'
strings='/usr/include/string.h'
submit=''
subversion='2'
sysman='/usr/share/man/man1'
tail=''
tar=''
targetarch=''
tbl=''
tee=''
test='test'
timeincl='/usr/include/sys/time.h '
timetype='time_t'
tmpnam_r_proto='0'
to=':'
touch='touch'
tr='tr'
trnl='\n'
troff=''
ttyname_r_proto='REENTRANT_PROTO_I_IBW'
u16size='2'
u16type='unsigned short'
u32size='4'
u32type='unsigned long'
u64size='8'
u64type='unsigned long long'
u8size='1'
u8type='unsigned char'
uidformat='"lu"'
uidsign='1'
uidsize='4'
uidtype='uid_t'
uname='uname'
uniq='uniq'
uquadtype='unsigned long long'
use5005threads='undef'
use64bitall='undef'
use64bitint='define'
usecrosscompile='undef'
usedevel='undef'
usedl='define'
usedtrace='undef'
usefaststdio='undef'
useithreads='define'
uselargefiles='define'
uselongdouble='undef'
usemallocwrap='undef'
usemorebits='undef'
usemultiplicity='define'
usemymalloc='n'
usenm='true'
useopcode='true'
useperlio='define'
useposix='true'
usereentrant='undef'
userelocatableinc='undef'
usesfio='false'
useshrplib='true'
usesitecustomize='undef'
usesocks='undef'
usethreads='define'
usevendorprefix='define'
usevfork='false'
usrinc='/usr/include'
uuname=''
uvXUformat='"llX"'
uvoformat='"llo"'
uvsize='8'
uvtype='unsigned long long'
uvuformat='"llu"'
uvxformat='"llx"'
vaproto='define'
vendorarch='/usr/lib/perl5/vendor_perl/5.14/i686-cygwin-threads-64int'
vendorarchexp='/usr/lib/perl5/vendor_perl/5.14/i686-cygwin-threads-64int'
vendorbin='/usr/bin'
vendorbinexp='/usr/bin'
vendorhtml1dir='/usr/share/doc/perl/html/html1'
vendorhtml1direxp='/usr/share/doc/perl/html/html1'
vendorhtml3dir='/usr/share/doc/perl/html/html3'
vendorhtml3direxp='/usr/share/doc/perl/html/html3'
vendorlib='/usr/lib/perl5/vendor_perl/5.14'
vendorlib_stem='/usr/lib/perl5/vendor_perl/5.14'
vendorlibexp='/usr/lib/perl5/vendor_perl/5.14'
vendorman1dir='/usr/share/man/man1'
vendorman1direxp='/usr/share/man/man1'
vendorman3dir='/usr/share/man/man3'
vendorman3direxp='/usr/share/man/man3'
vendorprefix='/usr'
vendorprefixexp='/usr'
vendorscript='/usr/bin'
vendorscriptexp='/usr/bin'
version='5.14.2'
version_patchlevel_string='version 14 subversion 2'
versiononly='undef'
vi=''
voidflags='15'
xlibpth='/usr/lib/386 /lib/386'
yacc='/usr/bin/byacc'
yaccflags=''
zcat=''
zip='zip'
!END!

my $i = 0;
foreach my $c (8,7,6,5,4,3,2) { $i |= ord($c); $i <<= 8 }
$i |= ord(1);
our $byteorder = join('', unpack('aaaaaaaa', pack('Q', $i)));
s/(byteorder=)(['"]).*?\2/$1$2$Config::byteorder$2/m;

my $config_sh_len = length $_;

our $Config_SH_expanded = "\n$_" . << 'EOVIRTUAL';
ccflags_nolargefiles='-DPERL_USE_SAFE_PUTENV -U__STRICT_ANSI__ -g -fno-strict-aliasing -pipe -fstack-protector'
ldflags_nolargefiles=' -Wl,--enable-auto-import -Wl,--export-all-symbols -Wl,--enable-auto-image-base -fstack-protector -L/usr/local/lib'
libs_nolargefiles='-lgdbm -ldb -ldl -lcrypt -lgdbm_compat'
libswanted_nolargefiles='   sfio socket bind inet nsl nm ndbm gdbm dbm db malloc dl dld ld sun crypt sec cposix posix ucb bsd BSD    gdbm_compat'
EOVIRTUAL
eval {
	# do not have hairy conniptions if this isnt available
	require 'Config_git.pl';
	$Config_SH_expanded .= $Config::Git_Data;
	1;
} or warn "Warning: failed to load Config_git.pl, something strange about this perl...\n";

# Search for it in the big string
sub fetch_string {
    my($self, $key) = @_;

    return undef unless $Config_SH_expanded =~ /\n$key=\'(.*?)\'\n/s;
    # So we can say "if $Config{'foo'}".
    $self->{$key} = $1 eq 'undef' ? undef : $1;
}

my $prevpos = 0;

sub FIRSTKEY {
    $prevpos = 0;
    substr($Config_SH_expanded, 1, index($Config_SH_expanded, '=') - 1 );
}

sub NEXTKEY {
    my $pos = index($Config_SH_expanded, qq('\n), $prevpos) + 2;
    my $len = index($Config_SH_expanded, "=", $pos) - $pos;
    $prevpos = $pos;
    $len > 0 ? substr($Config_SH_expanded, $pos, $len) : undef;
}

sub EXISTS {
    return 1 if exists($_[0]->{$_[1]});

    return(index($Config_SH_expanded, "\n$_[1]='") != -1
          );
}

sub STORE  { die "\%Config::Config is read-only\n" }
*DELETE = *CLEAR = \*STORE; # Typeglob aliasing uses less space

sub config_sh {
    substr $Config_SH_expanded, 1, $config_sh_len;
}

sub config_re {
    my $re = shift;
    return map { chomp; $_ } grep eval{ /^(?:$re)=/ }, split /^/,
    $Config_SH_expanded;
}

sub config_vars {
    # implements -V:cfgvar option (see perlrun -V:)
    foreach (@_) {
	# find optional leading, trailing colons; and query-spec
	my ($notag,$qry,$lncont) = m/^(:)?(.*?)(:)?$/;	# flags fore and aft, 
	# map colon-flags to print decorations
	my $prfx = $notag ? '': "$qry=";		# tag-prefix for print
	my $lnend = $lncont ? ' ' : ";\n";		# line ending for print

	# all config-vars are by definition \w only, any \W means regex
	if ($qry =~ /\W/) {
	    my @matches = config_re($qry);
	    print map "$_$lnend", @matches ? @matches : "$qry: not found"		if !$notag;
	    print map { s/\w+=//; "$_$lnend" } @matches ? @matches : "$qry: not found"	if  $notag;
	} else {
	    my $v = (exists $Config::Config{$qry}) ? $Config::Config{$qry}
						   : 'UNKNOWN';
	    $v = 'undef' unless defined $v;
	    print "${prfx}'${v}'$lnend";
	}
    }
}

# Called by the real AUTOLOAD
sub launcher {
    undef &AUTOLOAD;
    goto \&$Config::AUTOLOAD;
}

1;
FILE   fc138029/Cwd.pm  C[#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Cwd.pm"
package Cwd;

#line 169

use strict;
use Exporter;
use vars qw(@ISA @EXPORT @EXPORT_OK $VERSION);

$VERSION = '3.36';
my $xs_version = $VERSION;
$VERSION = eval $VERSION;

@ISA = qw/ Exporter /;
@EXPORT = qw(cwd getcwd fastcwd fastgetcwd);
push @EXPORT, qw(getdcwd) if $^O eq 'MSWin32';
@EXPORT_OK = qw(chdir abs_path fast_abs_path realpath fast_realpath);

# sys_cwd may keep the builtin command

# All the functionality of this module may provided by builtins,
# there is no sense to process the rest of the file.
# The best choice may be to have this in BEGIN, but how to return from BEGIN?

if ($^O eq 'os2') {
    local $^W = 0;

    *cwd                = defined &sys_cwd ? \&sys_cwd : \&_os2_cwd;
    *getcwd             = \&cwd;
    *fastgetcwd         = \&cwd;
    *fastcwd            = \&cwd;

    *fast_abs_path      = \&sys_abspath if defined &sys_abspath;
    *abs_path           = \&fast_abs_path;
    *realpath           = \&fast_abs_path;
    *fast_realpath      = \&fast_abs_path;

    return 1;
}

# Need to look up the feature settings on VMS.  The preferred way is to use the
# VMS::Feature module, but that may not be available to dual life modules.

my $use_vms_feature;
BEGIN {
    if ($^O eq 'VMS') {
        if (eval { local $SIG{__DIE__}; require VMS::Feature; }) {
            $use_vms_feature = 1;
        }
    }
}

# Need to look up the UNIX report mode.  This may become a dynamic mode
# in the future.
sub _vms_unix_rpt {
    my $unix_rpt;
    if ($use_vms_feature) {
        $unix_rpt = VMS::Feature::current("filename_unix_report");
    } else {
        my $env_unix_rpt = $ENV{'DECC$FILENAME_UNIX_REPORT'} || '';
        $unix_rpt = $env_unix_rpt =~ /^[ET1]/i; 
    }
    return $unix_rpt;
}

# Need to look up the EFS character set mode.  This may become a dynamic
# mode in the future.
sub _vms_efs {
    my $efs;
    if ($use_vms_feature) {
        $efs = VMS::Feature::current("efs_charset");
    } else {
        my $env_efs = $ENV{'DECC$EFS_CHARSET'} || '';
        $efs = $env_efs =~ /^[ET1]/i; 
    }
    return $efs;
}


# If loading the XS stuff doesn't work, we can fall back to pure perl
eval {
  if ( $] >= 5.006 ) {
    require XSLoader;
    XSLoader::load( __PACKAGE__, $xs_version);
  } else {
    require DynaLoader;
    push @ISA, 'DynaLoader';
    __PACKAGE__->bootstrap( $xs_version );
  }
};

# Must be after the DynaLoader stuff:
$VERSION = eval $VERSION;

# Big nasty table of function aliases
my %METHOD_MAP =
  (
   VMS =>
   {
    cwd			=> '_vms_cwd',
    getcwd		=> '_vms_cwd',
    fastcwd		=> '_vms_cwd',
    fastgetcwd		=> '_vms_cwd',
    abs_path		=> '_vms_abs_path',
    fast_abs_path	=> '_vms_abs_path',
   },

   MSWin32 =>
   {
    # We assume that &_NT_cwd is defined as an XSUB or in the core.
    cwd			=> '_NT_cwd',
    getcwd		=> '_NT_cwd',
    fastcwd		=> '_NT_cwd',
    fastgetcwd		=> '_NT_cwd',
    abs_path		=> 'fast_abs_path',
    realpath		=> 'fast_abs_path',
   },

   dos => 
   {
    cwd			=> '_dos_cwd',
    getcwd		=> '_dos_cwd',
    fastgetcwd		=> '_dos_cwd',
    fastcwd		=> '_dos_cwd',
    abs_path		=> 'fast_abs_path',
   },

   # QNX4.  QNX6 has a $os of 'nto'.
   qnx =>
   {
    cwd			=> '_qnx_cwd',
    getcwd		=> '_qnx_cwd',
    fastgetcwd		=> '_qnx_cwd',
    fastcwd		=> '_qnx_cwd',
    abs_path		=> '_qnx_abs_path',
    fast_abs_path	=> '_qnx_abs_path',
   },

   cygwin =>
   {
    getcwd		=> 'cwd',
    fastgetcwd		=> 'cwd',
    fastcwd		=> 'cwd',
    abs_path		=> 'fast_abs_path',
    realpath		=> 'fast_abs_path',
   },

   epoc =>
   {
    cwd			=> '_epoc_cwd',
    getcwd	        => '_epoc_cwd',
    fastgetcwd		=> '_epoc_cwd',
    fastcwd		=> '_epoc_cwd',
    abs_path		=> 'fast_abs_path',
   },

   MacOS =>
   {
    getcwd		=> 'cwd',
    fastgetcwd		=> 'cwd',
    fastcwd		=> 'cwd',
    abs_path		=> 'fast_abs_path',
   },
  );

$METHOD_MAP{NT} = $METHOD_MAP{MSWin32};


# Find the pwd command in the expected locations.  We assume these
# are safe.  This prevents _backtick_pwd() consulting $ENV{PATH}
# so everything works under taint mode.
my $pwd_cmd;
foreach my $try ('/bin/pwd',
		 '/usr/bin/pwd',
		 '/QOpenSys/bin/pwd', # OS/400 PASE.
		) {

    if( -x $try ) {
        $pwd_cmd = $try;
        last;
    }
}
my $found_pwd_cmd = defined($pwd_cmd);
unless ($pwd_cmd) {
    # Isn't this wrong?  _backtick_pwd() will fail if somenone has
    # pwd in their path but it is not /bin/pwd or /usr/bin/pwd?
    # See [perl #16774]. --jhi
    $pwd_cmd = 'pwd';
}

# Lazy-load Carp
sub _carp  { require Carp; Carp::carp(@_)  }
sub _croak { require Carp; Carp::croak(@_) }

# The 'natural and safe form' for UNIX (pwd may be setuid root)
sub _backtick_pwd {
    # Localize %ENV entries in a way that won't create new hash keys
    my @localize = grep exists $ENV{$_}, qw(PATH IFS CDPATH ENV BASH_ENV);
    local @ENV{@localize};
    
    my $cwd = `$pwd_cmd`;
    # Belt-and-suspenders in case someone said "undef $/".
    local $/ = "\n";
    # `pwd` may fail e.g. if the disk is full
    chomp($cwd) if defined $cwd;
    $cwd;
}

# Since some ports may predefine cwd internally (e.g., NT)
# we take care not to override an existing definition for cwd().

unless ($METHOD_MAP{$^O}{cwd} or defined &cwd) {
    # The pwd command is not available in some chroot(2)'ed environments
    my $sep = $Config::Config{path_sep} || ':';
    my $os = $^O;  # Protect $^O from tainting


    # Try again to find a pwd, this time searching the whole PATH.
    if (defined $ENV{PATH} and $os ne 'MSWin32') {  # no pwd on Windows
	my @candidates = split($sep, $ENV{PATH});
	while (!$found_pwd_cmd and @candidates) {
	    my $candidate = shift @candidates;
	    $found_pwd_cmd = 1 if -x "$candidate/pwd";
	}
    }

    # MacOS has some special magic to make `pwd` work.
    if( $os eq 'MacOS' || $found_pwd_cmd )
    {
	*cwd = \&_backtick_pwd;
    }
    else {
	*cwd = \&getcwd;
    }
}

if ($^O eq 'cygwin') {
  # We need to make sure cwd() is called with no args, because it's
  # got an arg-less prototype and will die if args are present.
  local $^W = 0;
  my $orig_cwd = \&cwd;
  *cwd = sub { &$orig_cwd() }
}


# set a reasonable (and very safe) default for fastgetcwd, in case it
# isn't redefined later (20001212 rspier)
*fastgetcwd = \&cwd;

# A non-XS version of getcwd() - also used to bootstrap the perl build
# process, when miniperl is running and no XS loading happens.
sub _perl_getcwd
{
    abs_path('.');
}

# By John Bazik
#
# Usage: $cwd = &fastcwd;
#
# This is a faster version of getcwd.  It's also more dangerous because
# you might chdir out of a directory that you can't chdir back into.
    
sub fastcwd_ {
    my($odev, $oino, $cdev, $cino, $tdev, $tino);
    my(@path, $path);
    local(*DIR);

    my($orig_cdev, $orig_cino) = stat('.');
    ($cdev, $cino) = ($orig_cdev, $orig_cino);
    for (;;) {
	my $direntry;
	($odev, $oino) = ($cdev, $cino);
	CORE::chdir('..') || return undef;
	($cdev, $cino) = stat('.');
	last if $odev == $cdev && $oino == $cino;
	opendir(DIR, '.') || return undef;
	for (;;) {
	    $direntry = readdir(DIR);
	    last unless defined $direntry;
	    next if $direntry eq '.';
	    next if $direntry eq '..';

	    ($tdev, $tino) = lstat($direntry);
	    last unless $tdev != $odev || $tino != $oino;
	}
	closedir(DIR);
	return undef unless defined $direntry; # should never happen
	unshift(@path, $direntry);
    }
    $path = '/' . join('/', @path);
    if ($^O eq 'apollo') { $path = "/".$path; }
    # At this point $path may be tainted (if tainting) and chdir would fail.
    # Untaint it then check that we landed where we started.
    $path =~ /^(.*)\z/s		# untaint
	&& CORE::chdir($1) or return undef;
    ($cdev, $cino) = stat('.');
    die "Unstable directory path, current directory changed unexpectedly"
	if $cdev != $orig_cdev || $cino != $orig_cino;
    $path;
}
if (not defined &fastcwd) { *fastcwd = \&fastcwd_ }


# Keeps track of current working directory in PWD environment var
# Usage:
#	use Cwd 'chdir';
#	chdir $newdir;

my $chdir_init = 0;

sub chdir_init {
    if ($ENV{'PWD'} and $^O ne 'os2' and $^O ne 'dos' and $^O ne 'MSWin32') {
	my($dd,$di) = stat('.');
	my($pd,$pi) = stat($ENV{'PWD'});
	if (!defined $dd or !defined $pd or $di != $pi or $dd != $pd) {
	    $ENV{'PWD'} = cwd();
	}
    }
    else {
	my $wd = cwd();
	$wd = Win32::GetFullPathName($wd) if $^O eq 'MSWin32';
	$ENV{'PWD'} = $wd;
    }
    # Strip an automounter prefix (where /tmp_mnt/foo/bar == /foo/bar)
    if ($^O ne 'MSWin32' and $ENV{'PWD'} =~ m|(/[^/]+(/[^/]+/[^/]+))(.*)|s) {
	my($pd,$pi) = stat($2);
	my($dd,$di) = stat($1);
	if (defined $pd and defined $dd and $di == $pi and $dd == $pd) {
	    $ENV{'PWD'}="$2$3";
	}
    }
    $chdir_init = 1;
}

sub chdir {
    my $newdir = @_ ? shift : '';	# allow for no arg (chdir to HOME dir)
    $newdir =~ s|///*|/|g unless $^O eq 'MSWin32';
    chdir_init() unless $chdir_init;
    my $newpwd;
    if ($^O eq 'MSWin32') {
	# get the full path name *before* the chdir()
	$newpwd = Win32::GetFullPathName($newdir);
    }

    return 0 unless CORE::chdir $newdir;

    if ($^O eq 'VMS') {
	return $ENV{'PWD'} = $ENV{'DEFAULT'}
    }
    elsif ($^O eq 'MacOS') {
	return $ENV{'PWD'} = cwd();
    }
    elsif ($^O eq 'MSWin32') {
	$ENV{'PWD'} = $newpwd;
	return 1;
    }

    if (ref $newdir eq 'GLOB') { # in case a file/dir handle is passed in
	$ENV{'PWD'} = cwd();
    } elsif ($newdir =~ m#^/#s) {
	$ENV{'PWD'} = $newdir;
    } else {
	my @curdir = split(m#/#,$ENV{'PWD'});
	@curdir = ('') unless @curdir;
	my $component;
	foreach $component (split(m#/#, $newdir)) {
	    next if $component eq '.';
	    pop(@curdir),next if $component eq '..';
	    push(@curdir,$component);
	}
	$ENV{'PWD'} = join('/',@curdir) || '/';
    }
    1;
}


sub _perl_abs_path
{
    my $start = @_ ? shift : '.';
    my($dotdots, $cwd, @pst, @cst, $dir, @tst);

    unless (@cst = stat( $start ))
    {
	_carp("stat($start): $!");
	return '';
    }

    unless (-d _) {
        # Make sure we can be invoked on plain files, not just directories.
        # NOTE that this routine assumes that '/' is the only directory separator.
	
        my ($dir, $file) = $start =~ m{^(.*)/(.+)$}
	    or return cwd() . '/' . $start;
	
	# Can't use "-l _" here, because the previous stat was a stat(), not an lstat().
	if (-l $start) {
	    my $link_target = readlink($start);
	    die "Can't resolve link $start: $!" unless defined $link_target;
	    
	    require File::Spec;
            $link_target = $dir . '/' . $link_target
                unless File::Spec->file_name_is_absolute($link_target);
	    
	    return abs_path($link_target);
	}
	
	return $dir ? abs_path($dir) . "/$file" : "/$file";
    }

    $cwd = '';
    $dotdots = $start;
    do
    {
	$dotdots .= '/..';
	@pst = @cst;
	local *PARENT;
	unless (opendir(PARENT, $dotdots))
	{
	    # probably a permissions issue.  Try the native command.
	    return File::Spec->rel2abs( $start, _backtick_pwd() );
	}
	unless (@cst = stat($dotdots))
	{
	    _carp("stat($dotdots): $!");
	    closedir(PARENT);
	    return '';
	}
	if ($pst[0] == $cst[0] && $pst[1] == $cst[1])
	{
	    $dir = undef;
	}
	else
	{
	    do
	    {
		unless (defined ($dir = readdir(PARENT)))
	        {
		    _carp("readdir($dotdots): $!");
		    closedir(PARENT);
		    return '';
		}
		$tst[0] = $pst[0]+1 unless (@tst = lstat("$dotdots/$dir"))
	    }
	    while ($dir eq '.' || $dir eq '..' || $tst[0] != $pst[0] ||
		   $tst[1] != $pst[1]);
	}
	$cwd = (defined $dir ? "$dir" : "" ) . "/$cwd" ;
	closedir(PARENT);
    } while (defined $dir);
    chop($cwd) unless $cwd eq '/'; # drop the trailing /
    $cwd;
}


my $Curdir;
sub fast_abs_path {
    local $ENV{PWD} = $ENV{PWD} || ''; # Guard against clobberage
    my $cwd = getcwd();
    require File::Spec;
    my $path = @_ ? shift : ($Curdir ||= File::Spec->curdir);

    # Detaint else we'll explode in taint mode.  This is safe because
    # we're not doing anything dangerous with it.
    ($path) = $path =~ /(.*)/;
    ($cwd)  = $cwd  =~ /(.*)/;

    unless (-e $path) {
 	_croak("$path: No such file or directory");
    }

    unless (-d _) {
        # Make sure we can be invoked on plain files, not just directories.
	
	my ($vol, $dir, $file) = File::Spec->splitpath($path);
	return File::Spec->catfile($cwd, $path) unless length $dir;

	if (-l $path) {
	    my $link_target = readlink($path);
	    die "Can't resolve link $path: $!" unless defined $link_target;
	    
	    $link_target = File::Spec->catpath($vol, $dir, $link_target)
                unless File::Spec->file_name_is_absolute($link_target);
	    
	    return fast_abs_path($link_target);
	}
	
	return $dir eq File::Spec->rootdir
	  ? File::Spec->catpath($vol, $dir, $file)
	  : fast_abs_path(File::Spec->catpath($vol, $dir, '')) . '/' . $file;
    }

    if (!CORE::chdir($path)) {
 	_croak("Cannot chdir to $path: $!");
    }
    my $realpath = getcwd();
    if (! ((-d $cwd) && (CORE::chdir($cwd)))) {
 	_croak("Cannot chdir back to $cwd: $!");
    }
    $realpath;
}

# added function alias to follow principle of least surprise
# based on previous aliasing.  --tchrist 27-Jan-00
*fast_realpath = \&fast_abs_path;


# --- PORTING SECTION ---

# VMS: $ENV{'DEFAULT'} points to default directory at all times
# 06-Mar-1996  Charles Bailey  bailey@newman.upenn.edu
# Note: Use of Cwd::chdir() causes the logical name PWD to be defined
#   in the process logical name table as the default device and directory
#   seen by Perl. This may not be the same as the default device
#   and directory seen by DCL after Perl exits, since the effects
#   the CRTL chdir() function persist only until Perl exits.

sub _vms_cwd {
    return $ENV{'DEFAULT'};
}

sub _vms_abs_path {
    return $ENV{'DEFAULT'} unless @_;
    my $path = shift;

    my $efs = _vms_efs;
    my $unix_rpt = _vms_unix_rpt;

    if (defined &VMS::Filespec::vmsrealpath) {
        my $path_unix = 0;
        my $path_vms = 0;

        $path_unix = 1 if ($path =~ m#(?<=\^)/#);
        $path_unix = 1 if ($path =~ /^\.\.?$/);
        $path_vms = 1 if ($path =~ m#[\[<\]]#);
        $path_vms = 1 if ($path =~ /^--?$/);

        my $unix_mode = $path_unix;
        if ($efs) {
            # In case of a tie, the Unix report mode decides.
            if ($path_vms == $path_unix) {
                $unix_mode = $unix_rpt;
            } else {
                $unix_mode = 0 if $path_vms;
            }
        }

        if ($unix_mode) {
            # Unix format
            return VMS::Filespec::unixrealpath($path);
        }

	# VMS format

	my $new_path = VMS::Filespec::vmsrealpath($path);

	# Perl expects directories to be in directory format
	$new_path = VMS::Filespec::pathify($new_path) if -d $path;
	return $new_path;
    }

    # Fallback to older algorithm if correct ones are not
    # available.

    if (-l $path) {
        my $link_target = readlink($path);
        die "Can't resolve link $path: $!" unless defined $link_target;

        return _vms_abs_path($link_target);
    }

    # may need to turn foo.dir into [.foo]
    my $pathified = VMS::Filespec::pathify($path);
    $path = $pathified if defined $pathified;
	
    return VMS::Filespec::rmsexpand($path);
}

sub _os2_cwd {
    $ENV{'PWD'} = `cmd /c cd`;
    chomp $ENV{'PWD'};
    $ENV{'PWD'} =~ s:\\:/:g ;
    return $ENV{'PWD'};
}

sub _win32_cwd_simple {
    $ENV{'PWD'} = `cd`;
    chomp $ENV{'PWD'};
    $ENV{'PWD'} =~ s:\\:/:g ;
    return $ENV{'PWD'};
}

sub _win32_cwd {
    if (eval 'defined &DynaLoader::boot_DynaLoader') {
	$ENV{'PWD'} = Win32::GetCwd();
    }
    else { # miniperl
	chomp($ENV{'PWD'} = `cd`);
    }
    $ENV{'PWD'} =~ s:\\:/:g ;
    return $ENV{'PWD'};
}

*_NT_cwd = defined &Win32::GetCwd ? \&_win32_cwd : \&_win32_cwd_simple;

sub _dos_cwd {
    if (!defined &Dos::GetCwd) {
        $ENV{'PWD'} = `command /c cd`;
        chomp $ENV{'PWD'};
        $ENV{'PWD'} =~ s:\\:/:g ;
    } else {
        $ENV{'PWD'} = Dos::GetCwd();
    }
    return $ENV{'PWD'};
}

sub _qnx_cwd {
	local $ENV{PATH} = '';
	local $ENV{CDPATH} = '';
	local $ENV{ENV} = '';
    $ENV{'PWD'} = `/usr/bin/fullpath -t`;
    chomp $ENV{'PWD'};
    return $ENV{'PWD'};
}

sub _qnx_abs_path {
	local $ENV{PATH} = '';
	local $ENV{CDPATH} = '';
	local $ENV{ENV} = '';
    my $path = @_ ? shift : '.';
    local *REALPATH;

    defined( open(REALPATH, '-|') || exec '/usr/bin/fullpath', '-t', $path ) or
      die "Can't open /usr/bin/fullpath: $!";
    my $realpath = <REALPATH>;
    close REALPATH;
    chomp $realpath;
    return $realpath;
}

sub _epoc_cwd {
    $ENV{'PWD'} = EPOC::getcwd();
    return $ENV{'PWD'};
}


# Now that all the base-level functions are set up, alias the
# user-level functions to the right places

if (exists $METHOD_MAP{$^O}) {
  my $map = $METHOD_MAP{$^O};
  foreach my $name (keys %$map) {
    local $^W = 0;  # assignments trigger 'subroutine redefined' warning
    no strict 'refs';
    *{$name} = \&{$map->{$name}};
  }
}

# In case the XS version doesn't load.
*abs_path = \&_perl_abs_path unless defined &abs_path;
*getcwd = \&_perl_getcwd unless defined &getcwd;

# added function alias for those of us more
# used to the libc function.  --tchrist 27-Jan-00
*realpath = \&abs_path;

1;
FILE   3eef6077/DynaLoader.pm  *)#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/DynaLoader.pm"

# Generated from DynaLoader_pm.PL

package DynaLoader;

#   And Gandalf said: 'Many folk like to know beforehand what is to
#   be set on the table; but those who have laboured to prepare the
#   feast like to keep their secret; for wonder makes the words of
#   praise louder.'

#   (Quote from Tolkien suggested by Anno Siegel.)
#
# See pod text at end of file for documentation.
# See also ext/DynaLoader/README in source tree for other information.
#
# Tim.Bunce@ig.co.uk, August 1994

BEGIN {
    $VERSION = '1.13';
}

use Config;

# enable debug/trace messages from DynaLoader perl code
$dl_debug = $ENV{PERL_DL_DEBUG} || 0 unless defined $dl_debug;

#
# Flags to alter dl_load_file behaviour.  Assigned bits:
#   0x01  make symbols available for linking later dl_load_file's.
#         (only known to work on Solaris 2 using dlopen(RTLD_GLOBAL))
#         (ignored under VMS; effect is built-in to image linking)
#
# This is called as a class method $module->dl_load_flags.  The
# definition here will be inherited and result on "default" loading
# behaviour unless a sub-class of DynaLoader defines its own version.
#

sub dl_load_flags { 0x00 }

($dl_dlext, $dl_so, $dlsrc) = @Config::Config{qw(dlext so dlsrc)};


$do_expand = 0;

@dl_require_symbols = ();       # names of symbols we need
@dl_resolve_using   = ();       # names of files to link with
@dl_library_path    = ();       # path to look for files

#XSLoader.pm may have added elements before we were required
#@dl_shared_objects  = ();       # shared objects for symbols we have 
#@dl_librefs         = ();       # things we have loaded
#@dl_modules         = ();       # Modules we have loaded

# This is a fix to support DLD's unfortunate desire to relink -lc
@dl_resolve_using = dl_findfile('-lc') if $dlsrc eq "dl_dld.xs";

# Initialise @dl_library_path with the 'standard' library path
# for this platform as determined by Configure.

push(@dl_library_path, split(' ', $Config::Config{libpth}));


my $ldlibpthname         = $Config::Config{ldlibpthname};
my $ldlibpthname_defined = defined $Config::Config{ldlibpthname};
my $pthsep               = $Config::Config{path_sep};

# Add to @dl_library_path any extra directories we can gather from environment
# during runtime.

if ($ldlibpthname_defined &&
    exists $ENV{$ldlibpthname}) {
    push(@dl_library_path, split(/$pthsep/, $ENV{$ldlibpthname}));
}

# E.g. HP-UX supports both its native SHLIB_PATH *and* LD_LIBRARY_PATH.

if ($ldlibpthname_defined &&
    $ldlibpthname ne 'LD_LIBRARY_PATH' &&
    exists $ENV{LD_LIBRARY_PATH}) {
    push(@dl_library_path, split(/$pthsep/, $ENV{LD_LIBRARY_PATH}));
}


# No prizes for guessing why we don't say 'bootstrap DynaLoader;' here.
# NOTE: All dl_*.xs (including dl_none.xs) define a dl_error() XSUB
boot_DynaLoader('DynaLoader') if defined(&boot_DynaLoader) &&
                                !defined(&dl_error);

if ($dl_debug) {
    print STDERR "DynaLoader.pm loaded (@INC, @dl_library_path)\n";
    print STDERR "DynaLoader not linked into this perl\n"
	    unless defined(&boot_DynaLoader);
}

1; # End of main code


sub croak   { require Carp; Carp::croak(@_)   }

sub bootstrap_inherit {
    my $module = $_[0];
    local *isa = *{"$module\::ISA"};
    local @isa = (@isa, 'DynaLoader');
    # Cannot goto due to delocalization.  Will report errors on a wrong line?
    bootstrap(@_);
}

sub bootstrap {
    # use local vars to enable $module.bs script to edit values
    local(@args) = @_;
    local($module) = $args[0];
    local(@dirs, $file);

    unless ($module) {
	require Carp;
	Carp::confess("Usage: DynaLoader::bootstrap(module)");
    }

    # A common error on platforms which don't support dynamic loading.
    # Since it's fatal and potentially confusing we give a detailed message.
    croak("Can't load module $module, dynamic loading not available in this perl.\n".
	"  (You may need to build a new perl executable which either supports\n".
	"  dynamic loading or has the $module module statically linked into it.)\n")
	unless defined(&dl_load_file);


    
    my @modparts = split(/::/,$module);
    my $modfname = $modparts[-1];

    # Some systems have restrictions on files names for DLL's etc.
    # mod2fname returns appropriate file base name (typically truncated)
    # It may also edit @modparts if required.
    $modfname = &mod2fname(\@modparts) if defined &mod2fname;

    

    my $modpname = join('/',@modparts);

    print STDERR "DynaLoader::bootstrap for $module ",
		       "(auto/$modpname/$modfname.$dl_dlext)\n"
	if $dl_debug;

    foreach (@INC) {
	
	    my $dir = "$_/auto/$modpname";
	
	next unless -d $dir; # skip over uninteresting directories
	
	# check for common cases to avoid autoload of dl_findfile
	my $try = "$dir/$modfname.$dl_dlext";
	last if $file = ($do_expand) ? dl_expandspec($try) : ((-f $try) && $try);
	
	# no luck here, save dir for possible later dl_findfile search
	push @dirs, $dir;
    }
    # last resort, let dl_findfile have a go in all known locations
    $file = dl_findfile(map("-L$_",@dirs,@INC), $modfname) unless $file;

    croak("Can't locate loadable object for module $module in \@INC (\@INC contains: @INC)")
	unless $file;	# wording similar to error from 'require'

    
    my $bootname = "boot_$module";
    $bootname =~ s/\W/_/g;
    @dl_require_symbols = ($bootname);

    # Execute optional '.bootstrap' perl script for this module.
    # The .bs file can be used to configure @dl_resolve_using etc to
    # match the needs of the individual module on this architecture.
    my $bs = $file;
    $bs =~ s/(\.\w+)?(;\d*)?$/\.bs/; # look for .bs 'beside' the library
    if (-s $bs) { # only read file if it's not empty
        print STDERR "BS: $bs ($^O, $dlsrc)\n" if $dl_debug;
        eval { do $bs; };
        warn "$bs: $@\n" if $@;
    }

    my $boot_symbol_ref;

    

    # Many dynamic extension loading problems will appear to come from
    # this section of code: XYZ failed at line 123 of DynaLoader.pm.
    # Often these errors are actually occurring in the initialisation
    # C code of the extension XS file. Perl reports the error as being
    # in this perl code simply because this was the last perl code
    # it executed.

    my $libref = dl_load_file($file, $module->dl_load_flags) or
	croak("Can't load '$file' for module $module: ".dl_error());

    push(@dl_librefs,$libref);  # record loaded object

    my @unresolved = dl_undef_symbols();
    if (@unresolved) {
	require Carp;
	Carp::carp("Undefined symbols present after loading $file: @unresolved\n");
    }

    $boot_symbol_ref = dl_find_symbol($libref, $bootname) or
         croak("Can't find '$bootname' symbol in $file\n");

    push(@dl_modules, $module); # record loaded module

  boot:
    my $xs = dl_install_xsub("${module}::bootstrap", $boot_symbol_ref, $file);

    # See comment block above

	push(@dl_shared_objects, $file); # record files loaded

    &$xs(@args);
}

sub dl_findfile {
    # Read ext/DynaLoader/DynaLoader.doc for detailed information.
    # This function does not automatically consider the architecture
    # or the perl library auto directories.
    my (@args) = @_;
    my (@dirs,  $dir);   # which directories to search
    my (@found);         # full paths to real files we have found
    #my $dl_ext= 'dll'; # $Config::Config{'dlext'} suffix for perl extensions
    #my $dl_so = 'dll'; # $Config::Config{'so'} suffix for shared libraries

    print STDERR "dl_findfile(@args)\n" if $dl_debug;

    # accumulate directories but process files as they appear
    arg: foreach(@args) {
        #  Special fast case: full filepath requires no search
	
	
        if (m:/: && -f $_) {
	    push(@found,$_);
	    last arg unless wantarray;
	    next;
	}
	

        # Deal with directories first:
        #  Using a -L prefix is the preferred option (faster and more robust)
        if (m:^-L:) { s/^-L//; push(@dirs, $_); next; }

        #  Otherwise we try to try to spot directories by a heuristic
        #  (this is a more complicated issue than it first appears)
        if (m:/: && -d $_) {   push(@dirs, $_); next; }

	

        #  Only files should get this far...
        my(@names, $name);    # what filenames to look for
        if (m:-l: ) {          # convert -lname to appropriate library name
            s/-l//;
            push(@names,"lib$_.$dl_so");
            push(@names,"lib$_.a");
        } else {                # Umm, a bare name. Try various alternatives:
            # these should be ordered with the most likely first
            push(@names,"$_.$dl_dlext")    unless m/\.$dl_dlext$/o;
            push(@names,"$_.$dl_so")     unless m/\.$dl_so$/o;
	    
            push(@names,"cyg$_.$dl_so")  unless m:/:;
	    
            push(@names,"lib$_.$dl_so")  unless m:/:;
            push(@names,"$_.a")          if !m/\.a$/ and $dlsrc eq "dl_dld.xs";
            push(@names, $_);
        }
	my $dirsep = '/';
	
        foreach $dir (@dirs, @dl_library_path) {
            next unless -d $dir;
	    
            foreach $name (@names) {
		my($file) = "$dir$dirsep$name";
                print STDERR " checking in $dir for $name\n" if $dl_debug;
		$file = ($do_expand) ? dl_expandspec($file) : (-f $file && $file);
		#$file = _check_file($file);
		if ($file) {
                    push(@found, $file);
                    next arg; # no need to look any further
                }
            }
        }
    }
    if ($dl_debug) {
        foreach(@dirs) {
            print STDERR " dl_findfile ignored non-existent directory: $_\n" unless -d $_;
        }
        print STDERR "dl_findfile found: @found\n";
    }
    return $found[0] unless wantarray;
    @found;
}



sub dl_expandspec {
    my($spec) = @_;
    # Optional function invoked if DynaLoader.pm sets $do_expand.
    # Most systems do not require or use this function.
    # Some systems may implement it in the dl_*.xs file in which case
    # this Perl version should be excluded at build time.

    # This function is designed to deal with systems which treat some
    # 'filenames' in a special way. For example VMS 'Logical Names'
    # (something like unix environment variables - but different).
    # This function should recognise such names and expand them into
    # full file paths.
    # Must return undef if $spec is invalid or file does not exist.

    my $file = $spec; # default output to input

	return undef unless -f $file;
    print STDERR "dl_expandspec($spec) => $file\n" if $dl_debug;
    $file;
}

sub dl_find_symbol_anywhere
{
    my $sym = shift;
    my $libref;
    foreach $libref (@dl_librefs) {
	my $symref = dl_find_symbol($libref,$sym);
	return $symref if $symref;
    }
    return undef;
}

__END__

#line 756
FILE   ac17c63d/Errno.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Errno.pm"
# -*- buffer-read-only: t -*-
#
# This file is auto-generated. ***ANY*** changes here will be lost
#

package Errno;
require Exporter;
use Config;
use strict;

"$Config{'archname'}-$Config{'osvers'}" eq
"cygwin-thread-multi-64int-1.7.15(0.26053)" or
	die "Errno architecture (cygwin-thread-multi-64int-1.7.15(0.26053)) does not match executable architecture ($Config{'archname'}-$Config{'osvers'})";

our $VERSION = "1.13";
$VERSION = eval $VERSION;
our @ISA = 'Exporter';

my %err;

BEGIN {
    %err = (
	EPERM => 1,
	ENOENT => 2,
	ESRCH => 3,
	EINTR => 4,
	EIO => 5,
	ENXIO => 6,
	E2BIG => 7,
	ENOEXEC => 8,
	EBADF => 9,
	ECHILD => 10,
	EWOULDBLOCK => 11,
	EAGAIN => 11,
	ENOMEM => 12,
	EACCES => 13,
	EFAULT => 14,
	ENOTBLK => 15,
	EBUSY => 16,
	EEXIST => 17,
	EXDEV => 18,
	ENODEV => 19,
	ENOTDIR => 20,
	EISDIR => 21,
	EINVAL => 22,
	ENFILE => 23,
	EMFILE => 24,
	ENOTTY => 25,
	ETXTBSY => 26,
	EFBIG => 27,
	ENOSPC => 28,
	ESPIPE => 29,
	EROFS => 30,
	EMLINK => 31,
	EPIPE => 32,
	EDOM => 33,
	ERANGE => 34,
	ENOMSG => 35,
	EIDRM => 36,
	ECHRNG => 37,
	EL2NSYNC => 38,
	EL3HLT => 39,
	EL3RST => 40,
	ELNRNG => 41,
	EUNATCH => 42,
	ENOCSI => 43,
	EL2HLT => 44,
	EDEADLK => 45,
	ENOLCK => 46,
	EBADE => 50,
	EBADR => 51,
	EXFULL => 52,
	ENOANO => 53,
	EBADRQC => 54,
	EBADSLT => 55,
	EDEADLOCK => 56,
	EBFONT => 57,
	ENOSTR => 60,
	ENODATA => 61,
	ETIME => 62,
	ENOSR => 63,
	ENONET => 64,
	ENOPKG => 65,
	EREMOTE => 66,
	ENOLINK => 67,
	EADV => 68,
	ESRMNT => 69,
	ECOMM => 70,
	EPROTO => 71,
	EMULTIHOP => 74,
	ELBIN => 75,
	EDOTDOT => 76,
	EBADMSG => 77,
	EFTYPE => 79,
	ENOTUNIQ => 80,
	EBADFD => 81,
	EREMCHG => 82,
	ELIBACC => 83,
	ELIBBAD => 84,
	ELIBSCN => 85,
	ELIBMAX => 86,
	ELIBEXEC => 87,
	ENOSYS => 88,
	ENMFILE => 89,
	ENOTEMPTY => 90,
	ENAMETOOLONG => 91,
	ELOOP => 92,
	EOPNOTSUPP => 95,
	EPFNOSUPPORT => 96,
	ECONNRESET => 104,
	ENOBUFS => 105,
	EAFNOSUPPORT => 106,
	EPROTOTYPE => 107,
	ENOTSOCK => 108,
	ENOPROTOOPT => 109,
	ESHUTDOWN => 110,
	ECONNREFUSED => 111,
	EADDRINUSE => 112,
	ECONNABORTED => 113,
	ENETUNREACH => 114,
	ENETDOWN => 115,
	ETIMEDOUT => 116,
	EHOSTDOWN => 117,
	EHOSTUNREACH => 118,
	EINPROGRESS => 119,
	EALREADY => 120,
	EDESTADDRREQ => 121,
	EMSGSIZE => 122,
	EPROTONOSUPPORT => 123,
	ESOCKTNOSUPPORT => 124,
	EADDRNOTAVAIL => 125,
	ENETRESET => 126,
	EISCONN => 127,
	ENOTCONN => 128,
	ETOOMANYREFS => 129,
	EPROCLIM => 130,
	EUSERS => 131,
	EDQUOT => 132,
	ESTALE => 133,
	ENOTSUP => 134,
	ENOMEDIUM => 135,
	ENOSHARE => 136,
	ECASECLASH => 137,
	EILSEQ => 138,
	EOVERFLOW => 139,
	ECANCELED => 140,
	ENOTRECOVERABLE => 141,
	EOWNERDEAD => 142,
	ESTRPIPE => 143,
    );
    # Generate proxy constant subroutines for all the values.
    # Well, almost all the values. Unfortunately we can't assume that at this
    # point that our symbol table is empty, as code such as if the parser has
    # seen code such as C<exists &Errno::EINVAL>, it will have created the
    # typeglob.
    # Doing this before defining @EXPORT_OK etc means that even if a platform is
    # crazy enough to define EXPORT_OK as an error constant, everything will
    # still work, because the parser will upgrade the PCS to a real typeglob.
    # We rely on the subroutine definitions below to update the internal caches.
    # Don't use %each, as we don't want a copy of the value.
    foreach my $name (keys %err) {
        if ($Errno::{$name}) {
            # We expect this to be reached fairly rarely, so take an approach
            # which uses the least compile time effort in the common case:
            eval "sub $name() { $err{$name} }; 1" or die $@;
        } else {
            $Errno::{$name} = \$err{$name};
        }
    }
}

our @EXPORT_OK = keys %err;

our %EXPORT_TAGS = (
    POSIX => [qw(
	E2BIG EACCES EADDRINUSE EADDRNOTAVAIL EAFNOSUPPORT EAGAIN EALREADY
	EBADF EBUSY ECHILD ECONNABORTED ECONNREFUSED ECONNRESET EDEADLK
	EDESTADDRREQ EDOM EDQUOT EEXIST EFAULT EFBIG EHOSTDOWN EHOSTUNREACH
	EINPROGRESS EINTR EINVAL EIO EISCONN EISDIR ELOOP EMFILE EMLINK
	EMSGSIZE ENAMETOOLONG ENETDOWN ENETRESET ENETUNREACH ENFILE ENOBUFS
	ENODEV ENOENT ENOEXEC ENOLCK ENOMEM ENOPROTOOPT ENOSPC ENOSYS ENOTBLK
	ENOTCONN ENOTDIR ENOTEMPTY ENOTSOCK ENOTTY ENXIO EOPNOTSUPP EPERM
	EPFNOSUPPORT EPIPE EPROCLIM EPROTONOSUPPORT EPROTOTYPE ERANGE EREMOTE
	EROFS ESHUTDOWN ESOCKTNOSUPPORT ESPIPE ESRCH ESTALE ETIMEDOUT
	ETOOMANYREFS ETXTBSY EUSERS EWOULDBLOCK EXDEV
    )]
);

sub TIEHASH { bless \%err }

sub FETCH {
    my (undef, $errname) = @_;
    return "" unless exists $err{$errname};
    my $errno = $err{$errname};
    return $errno == $! ? $errno : 0;
}

sub STORE {
    require Carp;
    Carp::confess("ERRNO hash is read only!");
}

*CLEAR = *DELETE = \*STORE; # Typeglob aliasing uses less space

sub NEXTKEY {
    each %err;
}

sub FIRSTKEY {
    my $s = scalar keys %err;	# initialize iterator
    each %err;
}

sub EXISTS {
    my (undef, $errname) = @_;
    exists $err{$errname};
}

tie %!, __PACKAGE__; # Returns an object, objects are true.

__END__

#line 275

# ex: set ro:
FILE   7abb2863/Fcntl.pm  ?#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Fcntl.pm"
package Fcntl;

#line 57

use strict;
our($VERSION, @ISA, @EXPORT, @EXPORT_OK, %EXPORT_TAGS);

require Exporter;
require XSLoader;
@ISA = qw(Exporter);
$VERSION = '1.11';

XSLoader::load();

# Named groups of exports
%EXPORT_TAGS = (
    'flock'   => [qw(LOCK_SH LOCK_EX LOCK_NB LOCK_UN)],
    'Fcompat' => [qw(FAPPEND FASYNC FCREAT FDEFER FDSYNC FEXCL FLARGEFILE
		     FNDELAY FNONBLOCK FRSYNC FSYNC FTRUNC)],
    'seek'    => [qw(SEEK_SET SEEK_CUR SEEK_END)],
    'mode'    => [qw(S_ISUID S_ISGID S_ISVTX S_ISTXT
		     _S_IFMT S_IFREG S_IFDIR S_IFLNK
		     S_IFSOCK S_IFBLK S_IFCHR S_IFIFO S_IFWHT S_ENFMT
		     S_IRUSR S_IWUSR S_IXUSR S_IRWXU
		     S_IRGRP S_IWGRP S_IXGRP S_IRWXG
		     S_IROTH S_IWOTH S_IXOTH S_IRWXO
		     S_IREAD S_IWRITE S_IEXEC
		     S_ISREG S_ISDIR S_ISLNK S_ISSOCK
		     S_ISBLK S_ISCHR S_ISFIFO
		     S_ISWHT S_ISENFMT		
		     S_IFMT S_IMODE
                  )],
);

# Items to export into callers namespace by default
# (move infrequently used names to @EXPORT_OK below)
@EXPORT =
  qw(
	FD_CLOEXEC
	F_ALLOCSP
	F_ALLOCSP64
	F_COMPAT
	F_DUP2FD
	F_DUPFD
	F_EXLCK
	F_FREESP
	F_FREESP64
	F_FSYNC
	F_FSYNC64
	F_GETFD
	F_GETFL
	F_GETLK
	F_GETLK64
	F_GETOWN
	F_NODNY
	F_POSIX
	F_RDACC
	F_RDDNY
	F_RDLCK
	F_RWACC
	F_RWDNY
	F_SETFD
	F_SETFL
	F_SETLK
	F_SETLK64
	F_SETLKW
	F_SETLKW64
	F_SETOWN
	F_SHARE
	F_SHLCK
	F_UNLCK
	F_UNSHARE
	F_WRACC
	F_WRDNY
	F_WRLCK
	O_ACCMODE
	O_ALIAS
	O_APPEND
	O_ASYNC
	O_BINARY
	O_CREAT
	O_DEFER
	O_DIRECT
	O_DIRECTORY
	O_DSYNC
	O_EXCL
	O_EXLOCK
	O_LARGEFILE
	O_NDELAY
	O_NOCTTY
	O_NOFOLLOW
	O_NOINHERIT
	O_NONBLOCK
	O_RANDOM
	O_RAW
	O_RDONLY
	O_RDWR
	O_RSRC
	O_RSYNC
	O_SEQUENTIAL
	O_SHLOCK
	O_SYNC
	O_TEMPORARY
	O_TEXT
	O_TRUNC
	O_WRONLY
     );

# Other items we are prepared to export if requested
@EXPORT_OK = (qw(
	DN_ACCESS
	DN_ATTRIB
	DN_CREATE
	DN_DELETE
	DN_MODIFY
	DN_MULTISHOT
	DN_RENAME
	F_GETLEASE
	F_GETSIG
	F_NOTIFY
	F_SETLEASE
	F_SETSIG
	LOCK_MAND
	LOCK_READ
	LOCK_RW
	LOCK_WRITE
	O_IGNORE_CTTY
	O_NOATIME
	O_NOLINK
	O_NOTRANS
), map {@{$_}} values %EXPORT_TAGS);

1;
FILE   5098017f/File/Glob.pm  #line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/File/Glob.pm"
package File::Glob;

use strict;
our($VERSION, @ISA, @EXPORT_OK, @EXPORT_FAIL, %EXPORT_TAGS, $DEFAULT_FLAGS);

require XSLoader;
use feature 'switch';

@ISA = qw(Exporter);

# NOTE: The glob() export is only here for compatibility with 5.6.0.
# csh_glob() should not be used directly, unless you know what you're doing.

%EXPORT_TAGS = (
    'glob' => [ qw(
        GLOB_ABEND
	GLOB_ALPHASORT
        GLOB_ALTDIRFUNC
        GLOB_BRACE
        GLOB_CSH
        GLOB_ERR
        GLOB_ERROR
        GLOB_LIMIT
        GLOB_MARK
        GLOB_NOCASE
        GLOB_NOCHECK
        GLOB_NOMAGIC
        GLOB_NOSORT
        GLOB_NOSPACE
        GLOB_QUOTE
        GLOB_TILDE
        glob
        bsd_glob
    ) ],
);

@EXPORT_OK   = (@{$EXPORT_TAGS{'glob'}}, 'csh_glob');

$VERSION = '1.13';

sub import {
    require Exporter;
    local $Exporter::ExportLevel = $Exporter::ExportLevel + 1;
    Exporter::import(grep {
	my $passthrough;
	given ($_) {
	    $DEFAULT_FLAGS &= ~GLOB_NOCASE() when ':case';
	    $DEFAULT_FLAGS |= GLOB_NOCASE() when ':nocase';
	    when (':globally') {
		no warnings 'redefine';
		*CORE::GLOBAL::glob = \&File::Glob::csh_glob;
	    }
	    $passthrough = 1;
	}
	$passthrough;
    } @_);
}

XSLoader::load();

$DEFAULT_FLAGS = GLOB_CSH();
if ($^O =~ /^(?:MSWin32|VMS|os2|dos|riscos)$/) {
    $DEFAULT_FLAGS |= GLOB_NOCASE();
}

# File::Glob::glob() is deprecated because its prototype is different from
# CORE::glob() (use bsd_glob() instead)
sub glob {
    splice @_, 1; # don't pass PL_glob_index as flags!
    goto &bsd_glob;
}

## borrowed heavily from gsar's File::DosGlob
my %iter;
my %entries;

sub csh_glob {
    my $pat = shift;
    my $cxix = shift;
    my @pat;

    # glob without args defaults to $_
    $pat = $_ unless defined $pat;

    # extract patterns
    $pat =~ s/^\s+//;	# Protect against empty elements in
    $pat =~ s/\s+$//;	# things like < *.c> and <*.c >.
			# These alone shouldn't trigger ParseWords.
    if ($pat =~ /\s/) {
        # XXX this is needed for compatibility with the csh
	# implementation in Perl.  Need to support a flag
	# to disable this behavior.
	require Text::ParseWords;
	@pat = Text::ParseWords::parse_line('\s+',0,$pat);
    }

    # assume global context if not provided one
    $cxix = '_G_' unless defined $cxix;
    $iter{$cxix} = 0 unless exists $iter{$cxix};

    # if we're just beginning, do it all first
    if ($iter{$cxix} == 0) {
	if (@pat) {
	    $entries{$cxix} = [ map { doglob($_, $DEFAULT_FLAGS) } @pat ];
	}
	else {
	    $entries{$cxix} = [ doglob($pat, $DEFAULT_FLAGS) ];
	}
    }

    # chuck it all out, quick or slow
    if (wantarray) {
        delete $iter{$cxix};
        return @{delete $entries{$cxix}};
    }
    else {
        if ($iter{$cxix} = scalar @{$entries{$cxix}}) {
            return shift @{$entries{$cxix}};
        }
        else {
            # return undef for EOL
            delete $iter{$cxix};
            delete $entries{$cxix};
            return undef;
        }
    }
}

1;
__END__

#line 438
FILE   0009d5b8/File/Spec.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/File/Spec.pm"
package File::Spec;

use strict;
use vars qw(@ISA $VERSION);

$VERSION = '3.33';
$VERSION = eval $VERSION;

my %module = (MacOS   => 'Mac',
	      MSWin32 => 'Win32',
	      os2     => 'OS2',
	      VMS     => 'VMS',
	      epoc    => 'Epoc',
	      NetWare => 'Win32', # Yes, File::Spec::Win32 works on NetWare.
	      symbian => 'Win32', # Yes, File::Spec::Win32 works on symbian.
	      dos     => 'OS2',   # Yes, File::Spec::OS2 works on DJGPP.
	      cygwin  => 'Cygwin');


my $module = $module{$^O} || 'Unix';

require "File/Spec/$module.pm";
@ISA = ("File::Spec::$module");

1;

__END__

#line 338
FILE   1404347b/File/Spec/Cygwin.pm  #line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/File/Spec/Cygwin.pm"
package File::Spec::Cygwin;

use strict;
use vars qw(@ISA $VERSION);
require File::Spec::Unix;

$VERSION = '3.33';
$VERSION = eval $VERSION;

@ISA = qw(File::Spec::Unix);

#line 29

#line 40

sub canonpath {
    my($self,$path) = @_;
    return unless defined $path;

    $path =~ s|\\|/|g;

    # Handle network path names beginning with double slash
    my $node = '';
    if ( $path =~ s@^(//[^/]+)(?:/|\z)@/@s ) {
        $node = $1;
    }
    return $node . $self->SUPER::canonpath($path);
}

sub catdir {
    my $self = shift;
    return unless @_;

    # Don't create something that looks like a //network/path
    if ($_[0] and ($_[0] eq '/' or $_[0] eq '\\')) {
        shift;
        return $self->SUPER::catdir('', @_);
    }

    $self->SUPER::catdir(@_);
}

#line 76


sub file_name_is_absolute {
    my ($self,$file) = @_;
    return 1 if $file =~ m{^([a-z]:)?[\\/]}is; # C:/test
    return $self->SUPER::file_name_is_absolute($file);
}

#line 99

my $tmpdir;
sub tmpdir {
    return $tmpdir if defined $tmpdir;
    $tmpdir = $_[0]->_tmpdir( $ENV{TMPDIR}, "/tmp", $ENV{'TMP'}, $ENV{'TEMP'}, 'C:/temp' );
}

#line 114

sub case_tolerant {
  return 1 unless $^O eq 'cygwin'
    and defined &Cygwin::mount_flags;

  my $drive = shift;
  if (! $drive) {
      my @flags = split(/,/, Cygwin::mount_flags('/cygwin'));
      my $prefix = pop(@flags);
      if (! $prefix || $prefix eq 'cygdrive') {
          $drive = '/cygdrive/c';
      } elsif ($prefix eq '/') {
          $drive = '/c';
      } else {
          $drive = "$prefix/c";
      }
  }
  my $mntopts = Cygwin::mount_flags($drive);
  if ($mntopts and ($mntopts =~ /,managed/)) {
    return 0;
  }
  eval { require Win32API::File; } or return 1;
  my $osFsType = "\0"x256;
  my $osVolName = "\0"x256;
  my $ouFsFlags = 0;
  Win32API::File::GetVolumeInformation($drive, $osVolName, 256, [], [], $ouFsFlags, $osFsType, 256 );
  if ($ouFsFlags & Win32API::File::FS_CASE_SENSITIVE()) { return 0; }
  else { return 1; }
}

#line 154

1;
FILE   c81f3d20/File/Spec/Unix.pm  s#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/File/Spec/Unix.pm"
package File::Spec::Unix;

use strict;
use vars qw($VERSION);

$VERSION = '3.33';
$VERSION = eval $VERSION;

#line 42

sub canonpath {
    my ($self,$path) = @_;
    return unless defined $path;
    
    # Handle POSIX-style node names beginning with double slash (qnx, nto)
    # (POSIX says: "a pathname that begins with two successive slashes
    # may be interpreted in an implementation-defined manner, although
    # more than two leading slashes shall be treated as a single slash.")
    my $node = '';
    my $double_slashes_special = $^O eq 'qnx' || $^O eq 'nto';


    if ( $double_slashes_special
         && ( $path =~ s{^(//[^/]+)/?\z}{}s || $path =~ s{^(//[^/]+)/}{/}s ) ) {
      $node = $1;
    }
    # This used to be
    # $path =~ s|/+|/|g unless ($^O eq 'cygwin');
    # but that made tests 29, 30, 35, 46, and 213 (as of #13272) to fail
    # (Mainly because trailing "" directories didn't get stripped).
    # Why would cygwin avoid collapsing multiple slashes into one? --jhi
    $path =~ s|/{2,}|/|g;                            # xx////xx  -> xx/xx
    $path =~ s{(?:/\.)+(?:/|\z)}{/}g;                # xx/././xx -> xx/xx
    $path =~ s|^(?:\./)+||s unless $path eq "./";    # ./xx      -> xx
    $path =~ s|^/(?:\.\./)+|/|;                      # /../../xx -> xx
    $path =~ s|^/\.\.$|/|;                         # /..       -> /
    $path =~ s|/\z|| unless $path eq "/";          # xx/       -> xx
    return "$node$path";
}

#line 82

sub catdir {
    my $self = shift;

    $self->canonpath(join('/', @_, '')); # '' because need a trailing '/'
}

#line 95

sub catfile {
    my $self = shift;
    my $file = $self->canonpath(pop @_);
    return $file unless @_;
    my $dir = $self->catdir(@_);
    $dir .= "/" unless substr($dir,-1) eq "/";
    return $dir.$file;
}

#line 110

sub curdir { '.' }

#line 118

sub devnull { '/dev/null' }

#line 126

sub rootdir { '/' }

#line 142

my $tmpdir;
sub _tmpdir {
    return $tmpdir if defined $tmpdir;
    my $self = shift;
    my @dirlist = @_;
    {
	no strict 'refs';
	if (${"\cTAINT"}) { # Check for taint mode on perl >= 5.8.0
            require Scalar::Util;
	    @dirlist = grep { ! Scalar::Util::tainted($_) } @dirlist;
	}
    }
    foreach (@dirlist) {
	next unless defined && -d && -w _;
	$tmpdir = $_;
	last;
    }
    $tmpdir = $self->curdir unless defined $tmpdir;
    $tmpdir = defined $tmpdir && $self->canonpath($tmpdir);
    return $tmpdir;
}

sub tmpdir {
    return $tmpdir if defined $tmpdir;
    $tmpdir = $_[0]->_tmpdir( $ENV{TMPDIR}, "/tmp" );
}

#line 175

sub updir { '..' }

#line 184

sub no_upwards {
    my $self = shift;
    return grep(!/^\.{1,2}\z/s, @_);
}

#line 196

sub case_tolerant { 0 }

#line 208

sub file_name_is_absolute {
    my ($self,$file) = @_;
    return scalar($file =~ m:^/:s);
}

#line 219

sub path {
    return () unless exists $ENV{PATH};
    my @path = split(':', $ENV{PATH});
    foreach (@path) { $_ = '.' if $_ eq '' }
    return @path;
}

#line 232

sub join {
    my $self = shift;
    return $self->catfile(@_);
}

#line 257

sub splitpath {
    my ($self,$path, $nofile) = @_;

    my ($volume,$directory,$file) = ('','','');

    if ( $nofile ) {
        $directory = $path;
    }
    else {
        $path =~ m|^ ( (?: .* / (?: \.\.?\z )? )? ) ([^/]*) |xs;
        $directory = $1;
        $file      = $2;
    }

    return ($volume,$directory,$file);
}


#line 299

sub splitdir {
    return split m|/|, $_[1], -1;  # Preserve trailing fields
}


#line 313

sub catpath {
    my ($self,$volume,$directory,$file) = @_;

    if ( $directory ne ''                && 
         $file ne ''                     && 
         substr( $directory, -1 ) ne '/' && 
         substr( $file, 0, 1 ) ne '/' 
    ) {
        $directory .= "/$file" ;
    }
    else {
        $directory .= $file ;
    }

    return $directory ;
}

#line 358

sub abs2rel {
    my($self,$path,$base) = @_;
    $base = $self->_cwd() unless defined $base and length $base;

    ($path, $base) = map $self->canonpath($_), $path, $base;

    if (grep $self->file_name_is_absolute($_), $path, $base) {
	($path, $base) = map $self->rel2abs($_), $path, $base;
    }
    else {
	# save a couple of cwd()s if both paths are relative
	($path, $base) = map $self->catdir('/', $_), $path, $base;
    }

    my ($path_volume) = $self->splitpath($path, 1);
    my ($base_volume) = $self->splitpath($base, 1);

    # Can't relativize across volumes
    return $path unless $path_volume eq $base_volume;

    my $path_directories = ($self->splitpath($path, 1))[1];
    my $base_directories = ($self->splitpath($base, 1))[1];

    # For UNC paths, the user might give a volume like //foo/bar that
    # strictly speaking has no directory portion.  Treat it as if it
    # had the root directory for that volume.
    if (!length($base_directories) and $self->file_name_is_absolute($base)) {
      $base_directories = $self->rootdir;
    }

    # Now, remove all leading components that are the same
    my @pathchunks = $self->splitdir( $path_directories );
    my @basechunks = $self->splitdir( $base_directories );

    if ($base_directories eq $self->rootdir) {
      shift @pathchunks;
      return $self->canonpath( $self->catpath('', $self->catdir( @pathchunks ), '') );
    }

    while (@pathchunks && @basechunks && $self->_same($pathchunks[0], $basechunks[0])) {
        shift @pathchunks ;
        shift @basechunks ;
    }
    return $self->curdir unless @pathchunks || @basechunks;

    # $base now contains the directories the resulting relative path 
    # must ascend out of before it can descend to $path_directory.
    my $result_dirs = $self->catdir( ($self->updir) x @basechunks, @pathchunks );
    return $self->canonpath( $self->catpath('', $result_dirs, '') );
}

sub _same {
  $_[1] eq $_[2];
}

#line 439

sub rel2abs {
    my ($self,$path,$base ) = @_;

    # Clean up $path
    if ( ! $self->file_name_is_absolute( $path ) ) {
        # Figure out the effective $base and clean it up.
        if ( !defined( $base ) || $base eq '' ) {
	    $base = $self->_cwd();
        }
        elsif ( ! $self->file_name_is_absolute( $base ) ) {
            $base = $self->rel2abs( $base ) ;
        }
        else {
            $base = $self->canonpath( $base ) ;
        }

        # Glom them together
        $path = $self->catdir( $base, $path ) ;
    }

    return $self->canonpath( $path ) ;
}

#line 477

# Internal routine to File::Spec, no point in making this public since
# it is the standard Cwd interface.  Most of the platform-specific
# File::Spec subclasses use this.
sub _cwd {
    require Cwd;
    Cwd::getcwd();
}


# Internal method to reduce xx\..\yy -> yy
sub _collapse {
    my($fs, $path) = @_;

    my $updir  = $fs->updir;
    my $curdir = $fs->curdir;

    my($vol, $dirs, $file) = $fs->splitpath($path);
    my @dirs = $fs->splitdir($dirs);
    pop @dirs if @dirs && $dirs[-1] eq '';

    my @collapsed;
    foreach my $dir (@dirs) {
        if( $dir eq $updir              and   # if we have an updir
            @collapsed                  and   # and something to collapse
            length $collapsed[-1]       and   # and its not the rootdir
            $collapsed[-1] ne $updir    and   # nor another updir
            $collapsed[-1] ne $curdir         # nor the curdir
          ) 
        {                                     # then
            pop @collapsed;                   # collapse
        }
        else {                                # else
            push @collapsed, $dir;            # just hang onto it
        }
    }

    return $fs->catpath($vol,
                        $fs->catdir(@collapsed),
                        $file
                       );
}


1;
FILE   c4023663/IO.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/IO.pm"
#

package IO;

use XSLoader ();
use Carp;
use strict;
use warnings;

our $VERSION = "1.25";
XSLoader::load 'IO', $VERSION;

sub import {
    shift;

    warnings::warnif('deprecated', qq{Parameterless "use IO" deprecated})
        if @_ == 0 ;
    
    my @l = @_ ? @_ : qw(Handle Seekable File Pipe Socket Dir);

    eval join("", map { "require IO::" . (/(\w+)/)[0] . ";\n" } @l)
	or croak $@;
}

1;

__END__

#line 68

FILE   af4df095/IO/File.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/IO/File.pm"
#

package IO::File;

#line 126

use 5.006_001;
use strict;
our($VERSION, @EXPORT, @EXPORT_OK, @ISA);
use Carp;
use Symbol;
use SelectSaver;
use IO::Seekable;
use File::Spec;

require Exporter;

@ISA = qw(IO::Handle IO::Seekable Exporter);

$VERSION = "1.14";

@EXPORT = @IO::Seekable::EXPORT;

eval {
    # Make all Fcntl O_XXX constants available for importing
    require Fcntl;
    my @O = grep /^O_/, @Fcntl::EXPORT;
    Fcntl->import(@O);  # first we import what we want to export
    push(@EXPORT, @O);
};

################################################
## Constructor
##

sub new {
    my $type = shift;
    my $class = ref($type) || $type || "IO::File";
    @_ >= 0 && @_ <= 3
	or croak "usage: new $class [FILENAME [,MODE [,PERMS]]]";
    my $fh = $class->SUPER::new();
    if (@_) {
	$fh->open(@_)
	    or return undef;
    }
    $fh;
}

################################################
## Open
##

sub open {
    @_ >= 2 && @_ <= 4 or croak 'usage: $fh->open(FILENAME [,MODE [,PERMS]])';
    my ($fh, $file) = @_;
    if (@_ > 2) {
	my ($mode, $perms) = @_[2, 3];
	if ($mode =~ /^\d+$/) {
	    defined $perms or $perms = 0666;
	    return sysopen($fh, $file, $mode, $perms);
	} elsif ($mode =~ /:/) {
	    return open($fh, $mode, $file) if @_ == 3;
	    croak 'usage: $fh->open(FILENAME, IOLAYERS)';
	} else {
            return open($fh, IO::Handle::_open_mode_string($mode), $file);
        }
    }
    open($fh, $file);
}

################################################
## Binmode
##

sub binmode {
    ( @_ == 1 or @_ == 2 ) or croak 'usage $fh->binmode([LAYER])';

    my($fh, $layer) = @_;

    return binmode $$fh unless $layer;
    return binmode $$fh, $layer;
}

1;
FILE   8ac00d1f/IO/Handle.pm  a#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/IO/Handle.pm"
package IO::Handle;

#line 259

use 5.006_001;
use strict;
our($VERSION, @EXPORT_OK, @ISA);
use Carp;
use Symbol;
use SelectSaver;
use IO ();	# Load the XS module

require Exporter;
@ISA = qw(Exporter);

$VERSION = "1.28";
$VERSION = eval $VERSION;

@EXPORT_OK = qw(
    autoflush
    output_field_separator
    output_record_separator
    input_record_separator
    input_line_number
    format_page_number
    format_lines_per_page
    format_lines_left
    format_name
    format_top_name
    format_line_break_characters
    format_formfeed
    format_write

    print
    printf
    say
    getline
    getlines

    printflush
    flush

    SEEK_SET
    SEEK_CUR
    SEEK_END
    _IOFBF
    _IOLBF
    _IONBF
);

################################################
## Constructors, destructors.
##

sub new {
    my $class = ref($_[0]) || $_[0] || "IO::Handle";
    @_ == 1 or croak "usage: new $class";
    my $io = gensym;
    bless $io, $class;
}

sub new_from_fd {
    my $class = ref($_[0]) || $_[0] || "IO::Handle";
    @_ == 3 or croak "usage: new_from_fd $class FD, MODE";
    my $io = gensym;
    shift;
    IO::Handle::fdopen($io, @_)
	or return undef;
    bless $io, $class;
}

#
# There is no need for DESTROY to do anything, because when the
# last reference to an IO object is gone, Perl automatically
# closes its associated files (if any).  However, to avoid any
# attempts to autoload DESTROY, we here define it to do nothing.
#
sub DESTROY {}


################################################
## Open and close.
##

sub _open_mode_string {
    my ($mode) = @_;
    $mode =~ /^\+?(<|>>?)$/
      or $mode =~ s/^r(\+?)$/$1</
      or $mode =~ s/^w(\+?)$/$1>/
      or $mode =~ s/^a(\+?)$/$1>>/
      or croak "IO::Handle: bad open mode: $mode";
    $mode;
}

sub fdopen {
    @_ == 3 or croak 'usage: $io->fdopen(FD, MODE)';
    my ($io, $fd, $mode) = @_;
    local(*GLOB);

    if (ref($fd) && "".$fd =~ /GLOB\(/o) {
	# It's a glob reference; Alias it as we cannot get name of anon GLOBs
	my $n = qualify(*GLOB);
	*GLOB = *{*$fd};
	$fd =  $n;
    } elsif ($fd =~ m#^\d+$#) {
	# It's an FD number; prefix with "=".
	$fd = "=$fd";
    }

    open($io, _open_mode_string($mode) . '&' . $fd)
	? $io : undef;
}

sub close {
    @_ == 1 or croak 'usage: $io->close()';
    my($io) = @_;

    close($io);
}

################################################
## Normal I/O functions.
##

# flock
# select

sub opened {
    @_ == 1 or croak 'usage: $io->opened()';
    defined fileno($_[0]);
}

sub fileno {
    @_ == 1 or croak 'usage: $io->fileno()';
    fileno($_[0]);
}

sub getc {
    @_ == 1 or croak 'usage: $io->getc()';
    getc($_[0]);
}

sub eof {
    @_ == 1 or croak 'usage: $io->eof()';
    eof($_[0]);
}

sub print {
    @_ or croak 'usage: $io->print(ARGS)';
    my $this = shift;
    print $this @_;
}

sub printf {
    @_ >= 2 or croak 'usage: $io->printf(FMT,[ARGS])';
    my $this = shift;
    printf $this @_;
}

sub say {
    @_ or croak 'usage: $io->say(ARGS)';
    my $this = shift;
    local $\ = "\n";
    print $this @_;
}

sub getline {
    @_ == 1 or croak 'usage: $io->getline()';
    my $this = shift;
    return scalar <$this>;
} 

*gets = \&getline;  # deprecated

sub getlines {
    @_ == 1 or croak 'usage: $io->getlines()';
    wantarray or
	croak 'Can\'t call $io->getlines in a scalar context, use $io->getline';
    my $this = shift;
    return <$this>;
}

sub truncate {
    @_ == 2 or croak 'usage: $io->truncate(LEN)';
    truncate($_[0], $_[1]);
}

sub read {
    @_ == 3 || @_ == 4 or croak 'usage: $io->read(BUF, LEN [, OFFSET])';
    read($_[0], $_[1], $_[2], $_[3] || 0);
}

sub sysread {
    @_ == 3 || @_ == 4 or croak 'usage: $io->sysread(BUF, LEN [, OFFSET])';
    sysread($_[0], $_[1], $_[2], $_[3] || 0);
}

sub write {
    @_ >= 2 && @_ <= 4 or croak 'usage: $io->write(BUF [, LEN [, OFFSET]])';
    local($\) = "";
    $_[2] = length($_[1]) unless defined $_[2];
    print { $_[0] } substr($_[1], $_[3] || 0, $_[2]);
}

sub syswrite {
    @_ >= 2 && @_ <= 4 or croak 'usage: $io->syswrite(BUF [, LEN [, OFFSET]])';
    if (defined($_[2])) {
	syswrite($_[0], $_[1], $_[2], $_[3] || 0);
    } else {
	syswrite($_[0], $_[1]);
    }
}

sub stat {
    @_ == 1 or croak 'usage: $io->stat()';
    stat($_[0]);
}

################################################
## State modification functions.
##

sub autoflush {
    my $old = new SelectSaver qualify($_[0], caller);
    my $prev = $|;
    $| = @_ > 1 ? $_[1] : 1;
    $prev;
}

sub output_field_separator {
    carp "output_field_separator is not supported on a per-handle basis"
	if ref($_[0]);
    my $prev = $,;
    $, = $_[1] if @_ > 1;
    $prev;
}

sub output_record_separator {
    carp "output_record_separator is not supported on a per-handle basis"
	if ref($_[0]);
    my $prev = $\;
    $\ = $_[1] if @_ > 1;
    $prev;
}

sub input_record_separator {
    carp "input_record_separator is not supported on a per-handle basis"
	if ref($_[0]);
    my $prev = $/;
    $/ = $_[1] if @_ > 1;
    $prev;
}

sub input_line_number {
    local $.;
    () = tell qualify($_[0], caller) if ref($_[0]);
    my $prev = $.;
    $. = $_[1] if @_ > 1;
    $prev;
}

sub format_page_number {
    my $old;
    $old = new SelectSaver qualify($_[0], caller) if ref($_[0]);
    my $prev = $%;
    $% = $_[1] if @_ > 1;
    $prev;
}

sub format_lines_per_page {
    my $old;
    $old = new SelectSaver qualify($_[0], caller) if ref($_[0]);
    my $prev = $=;
    $= = $_[1] if @_ > 1;
    $prev;
}

sub format_lines_left {
    my $old;
    $old = new SelectSaver qualify($_[0], caller) if ref($_[0]);
    my $prev = $-;
    $- = $_[1] if @_ > 1;
    $prev;
}

sub format_name {
    my $old;
    $old = new SelectSaver qualify($_[0], caller) if ref($_[0]);
    my $prev = $~;
    $~ = qualify($_[1], caller) if @_ > 1;
    $prev;
}

sub format_top_name {
    my $old;
    $old = new SelectSaver qualify($_[0], caller) if ref($_[0]);
    my $prev = $^;
    $^ = qualify($_[1], caller) if @_ > 1;
    $prev;
}

sub format_line_break_characters {
    carp "format_line_break_characters is not supported on a per-handle basis"
	if ref($_[0]);
    my $prev = $:;
    $: = $_[1] if @_ > 1;
    $prev;
}

sub format_formfeed {
    carp "format_formfeed is not supported on a per-handle basis"
	if ref($_[0]);
    my $prev = $^L;
    $^L = $_[1] if @_ > 1;
    $prev;
}

sub formline {
    my $io = shift;
    my $picture = shift;
    local($^A) = $^A;
    local($\) = "";
    formline($picture, @_);
    print $io $^A;
}

sub format_write {
    @_ < 3 || croak 'usage: $io->write( [FORMAT_NAME] )';
    if (@_ == 2) {
	my ($io, $fmt) = @_;
	my $oldfmt = $io->format_name(qualify($fmt,caller));
	CORE::write($io);
	$io->format_name($oldfmt);
    } else {
	CORE::write($_[0]);
    }
}

sub fcntl {
    @_ == 3 || croak 'usage: $io->fcntl( OP, VALUE );';
    my ($io, $op) = @_;
    return fcntl($io, $op, $_[2]);
}

sub ioctl {
    @_ == 3 || croak 'usage: $io->ioctl( OP, VALUE );';
    my ($io, $op) = @_;
    return ioctl($io, $op, $_[2]);
}

# this sub is for compatability with older releases of IO that used
# a sub called constant to detemine if a constant existed -- GMB
#
# The SEEK_* and _IO?BF constants were the only constants at that time
# any new code should just chech defined(&CONSTANT_NAME)

sub constant {
    no strict 'refs';
    my $name = shift;
    (($name =~ /^(SEEK_(SET|CUR|END)|_IO[FLN]BF)$/) && defined &{$name})
	? &{$name}() : undef;
}


# so that flush.pl can be deprecated

sub printflush {
    my $io = shift;
    my $old;
    $old = new SelectSaver qualify($io, caller) if ref($io);
    local $| = 1;
    if(ref($io)) {
        print $io @_;
    }
    else {
	print @_;
    }
}

1;
FILE   e20a597c/IO/Seekable.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/IO/Seekable.pm"
#

package IO::Seekable;

#line 96

use 5.006_001;
use Carp;
use strict;
our($VERSION, @EXPORT, @ISA);
use IO::Handle ();
# XXX we can't get these from IO::Handle or we'll get prototype
# mismatch warnings on C<use POSIX; use IO::File;> :-(
use Fcntl qw(SEEK_SET SEEK_CUR SEEK_END);
require Exporter;

@EXPORT = qw(SEEK_SET SEEK_CUR SEEK_END);
@ISA = qw(Exporter);

$VERSION = "1.10";
$VERSION = eval $VERSION;

sub seek {
    @_ == 3 or croak 'usage: $io->seek(POS, WHENCE)';
    seek($_[0], $_[1], $_[2]);
}

sub sysseek {
    @_ == 3 or croak 'usage: $io->sysseek(POS, WHENCE)';
    sysseek($_[0], $_[1], $_[2]);
}

sub tell {
    @_ == 1 or croak 'usage: $io->tell()';
    tell($_[0]);
}

1;
FILE   edd18cc0/List/Util.pm  z#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/List/Util.pm"
# List::Util.pm
#
# Copyright (c) 1997-2009 Graham Barr <gbarr@pobox.com>. All rights reserved.
# This program is free software; you can redistribute it and/or
# modify it under the same terms as Perl itself.
#
# This module is normally only loaded if the XS module is not available

package List::Util;

use strict;
use vars qw(@ISA @EXPORT_OK $VERSION $XS_VERSION $TESTING_PERL_ONLY);
require Exporter;

@ISA        = qw(Exporter);
@EXPORT_OK  = qw(first min max minstr maxstr reduce sum shuffle);
$VERSION    = "1.23";
$XS_VERSION = $VERSION;
$VERSION    = eval $VERSION;

eval {
  # PERL_DL_NONLAZY must be false, or any errors in loading will just
  # cause the perl code to be tested
  local $ENV{PERL_DL_NONLAZY} = 0 if $ENV{PERL_DL_NONLAZY};
  eval {
    require XSLoader;
    XSLoader::load('List::Util', $XS_VERSION);
    1;
  } or do {
    require DynaLoader;
    local @ISA = qw(DynaLoader);
    bootstrap List::Util $XS_VERSION;
  };
} unless $TESTING_PERL_ONLY;


if (!defined &sum) {
  require List::Util::PP;
  List::Util::PP->import;
}

1;

__END__

#line 234
FILE   e9830bb9/PerlIO/scalar.pm   �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/PerlIO/scalar.pm"
package PerlIO::scalar;
our $VERSION = '0.11_01';
require XSLoader;
XSLoader::load();
1;
__END__

#line 42
FILE   b463c74c/Scalar/Util.pm  �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Scalar/Util.pm"
# Scalar::Util.pm
#
# Copyright (c) 1997-2007 Graham Barr <gbarr@pobox.com>. All rights reserved.
# This program is free software; you can redistribute it and/or
# modify it under the same terms as Perl itself.

package Scalar::Util;

use strict;
use vars qw(@ISA @EXPORT_OK $VERSION @EXPORT_FAIL);
require Exporter;
require List::Util; # List::Util loads the XS

@ISA       = qw(Exporter);
@EXPORT_OK = qw(blessed dualvar reftype weaken isweak tainted readonly openhandle refaddr isvstring looks_like_number set_prototype);
$VERSION    = "1.23";
$VERSION   = eval $VERSION;

unless (defined &dualvar) {
  # Load Pure Perl version if XS not loaded
  require Scalar::Util::PP;
  Scalar::Util::PP->import;
  push @EXPORT_FAIL, qw(weaken isweak dualvar isvstring set_prototype);
}

sub export_fail {
  if (grep { /dualvar/ } @EXPORT_FAIL) { # no XS loaded
    my $pat = join("|", @EXPORT_FAIL);
    if (my ($err) = grep { /^($pat)$/ } @_ ) {
      require Carp;
      Carp::croak("$err is only available with the XS version of Scalar::Util");
    }
  }

  if (grep { /^(weaken|isweak)$/ } @_ ) {
    require Carp;
    Carp::croak("Weak references are not implemented in the version of perl");
  }

  if (grep { /^(isvstring)$/ } @_ ) {
    require Carp;
    Carp::croak("Vstrings are not implemented in the version of perl");
  }

  @_;
}

sub openhandle ($) {
  my $fh = shift;
  my $rt = reftype($fh) || '';

  return defined(fileno($fh)) ? $fh : undef
    if $rt eq 'IO';

  if (reftype(\$fh) eq 'GLOB') { # handle  openhandle(*DATA)
    $fh = \(my $tmp=$fh);
  }
  elsif ($rt ne 'GLOB') {
    return undef;
  }

  (tied(*$fh) or defined(fileno($fh)))
    ? $fh : undef;
}

1;

__END__

#line 284
FILE   !78989576/Tie/Hash/NamedCapture.pm   �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Tie/Hash/NamedCapture.pm"
use strict;
package Tie::Hash::NamedCapture;

our $VERSION = "0.08";

require XSLoader;
XSLoader::load(); # This returns true, which makes require happy.

__END__

#line 50
FILE   857c9658/Win32.pm  \�#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/Win32.pm"
package Win32;

# BEGIN {
    use strict;
    use vars qw|$VERSION $XS_VERSION @ISA @EXPORT @EXPORT_OK|;

    require Exporter;
    require DynaLoader;

    @ISA = qw|Exporter DynaLoader|;
    $VERSION = '0.44';
    $XS_VERSION = $VERSION;
    $VERSION = eval $VERSION;

    @EXPORT = qw(
	NULL
	WIN31_CLASS
	OWNER_SECURITY_INFORMATION
	GROUP_SECURITY_INFORMATION
	DACL_SECURITY_INFORMATION
	SACL_SECURITY_INFORMATION
	MB_ICONHAND
	MB_ICONQUESTION
	MB_ICONEXCLAMATION
	MB_ICONASTERISK
	MB_ICONWARNING
	MB_ICONERROR
	MB_ICONINFORMATION
	MB_ICONSTOP
    );
    @EXPORT_OK = qw(
        GetOSName
        SW_HIDE
        SW_SHOWNORMAL
        SW_SHOWMINIMIZED
        SW_SHOWMAXIMIZED
        SW_SHOWNOACTIVATE

        CSIDL_DESKTOP
        CSIDL_PROGRAMS
        CSIDL_PERSONAL
        CSIDL_FAVORITES
        CSIDL_STARTUP
        CSIDL_RECENT
        CSIDL_SENDTO
        CSIDL_STARTMENU
        CSIDL_MYMUSIC
        CSIDL_MYVIDEO
        CSIDL_DESKTOPDIRECTORY
        CSIDL_NETHOOD
        CSIDL_FONTS
        CSIDL_TEMPLATES
        CSIDL_COMMON_STARTMENU
        CSIDL_COMMON_PROGRAMS
        CSIDL_COMMON_STARTUP
        CSIDL_COMMON_DESKTOPDIRECTORY
        CSIDL_APPDATA
        CSIDL_PRINTHOOD
        CSIDL_LOCAL_APPDATA
        CSIDL_COMMON_FAVORITES
        CSIDL_INTERNET_CACHE
        CSIDL_COOKIES
        CSIDL_HISTORY
        CSIDL_COMMON_APPDATA
        CSIDL_WINDOWS
        CSIDL_SYSTEM
        CSIDL_PROGRAM_FILES
        CSIDL_MYPICTURES
        CSIDL_PROFILE
        CSIDL_PROGRAM_FILES_COMMON
        CSIDL_COMMON_TEMPLATES
        CSIDL_COMMON_DOCUMENTS
        CSIDL_COMMON_ADMINTOOLS
        CSIDL_ADMINTOOLS
        CSIDL_COMMON_MUSIC
        CSIDL_COMMON_PICTURES
        CSIDL_COMMON_VIDEO
        CSIDL_RESOURCES
        CSIDL_RESOURCES_LOCALIZED
        CSIDL_CDBURN_AREA
    );
# }

# We won't bother with the constant stuff, too much of a hassle.  Just hard
# code it here.

sub NULL 				{ 0 }
sub WIN31_CLASS 			{ &NULL }

sub OWNER_SECURITY_INFORMATION		{ 0x00000001 }
sub GROUP_SECURITY_INFORMATION		{ 0x00000002 }
sub DACL_SECURITY_INFORMATION		{ 0x00000004 }
sub SACL_SECURITY_INFORMATION		{ 0x00000008 }

sub MB_ICONHAND				{ 0x00000010 }
sub MB_ICONQUESTION			{ 0x00000020 }
sub MB_ICONEXCLAMATION			{ 0x00000030 }
sub MB_ICONASTERISK			{ 0x00000040 }
sub MB_ICONWARNING			{ 0x00000030 }
sub MB_ICONERROR			{ 0x00000010 }
sub MB_ICONINFORMATION			{ 0x00000040 }
sub MB_ICONSTOP				{ 0x00000010 }

#
# Newly added constants.  These have an empty prototype, unlike the
# the ones above, which aren't prototyped for compatibility reasons.
#
sub SW_HIDE           ()		{ 0 }
sub SW_SHOWNORMAL     ()		{ 1 }
sub SW_SHOWMINIMIZED  ()		{ 2 }
sub SW_SHOWMAXIMIZED  ()		{ 3 }
sub SW_SHOWNOACTIVATE ()		{ 4 }

sub CSIDL_DESKTOP              ()       { 0x0000 }     # <desktop>
sub CSIDL_PROGRAMS             ()       { 0x0002 }     # Start Menu\Programs
sub CSIDL_PERSONAL             ()       { 0x0005 }     # "My Documents" folder
sub CSIDL_FAVORITES            ()       { 0x0006 }     # <user name>\Favorites
sub CSIDL_STARTUP              ()       { 0x0007 }     # Start Menu\Programs\Startup
sub CSIDL_RECENT               ()       { 0x0008 }     # <user name>\Recent
sub CSIDL_SENDTO               ()       { 0x0009 }     # <user name>\SendTo
sub CSIDL_STARTMENU            ()       { 0x000B }     # <user name>\Start Menu
sub CSIDL_MYMUSIC              ()       { 0x000D }     # "My Music" folder
sub CSIDL_MYVIDEO              ()       { 0x000E }     # "My Videos" folder
sub CSIDL_DESKTOPDIRECTORY     ()       { 0x0010 }     # <user name>\Desktop
sub CSIDL_NETHOOD              ()       { 0x0013 }     # <user name>\nethood
sub CSIDL_FONTS                ()       { 0x0014 }     # windows\fonts
sub CSIDL_TEMPLATES            ()       { 0x0015 }
sub CSIDL_COMMON_STARTMENU     ()       { 0x0016 }     # All Users\Start Menu
sub CSIDL_COMMON_PROGRAMS      ()       { 0x0017 }     # All Users\Start Menu\Programs
sub CSIDL_COMMON_STARTUP       ()       { 0x0018 }     # All Users\Startup
sub CSIDL_COMMON_DESKTOPDIRECTORY ()    { 0x0019 }     # All Users\Desktop
sub CSIDL_APPDATA              ()       { 0x001A }     # Application Data, new for NT4
sub CSIDL_PRINTHOOD            ()       { 0x001B }     # <user name>\PrintHood
sub CSIDL_LOCAL_APPDATA        ()       { 0x001C }     # non roaming, user\Local Settings\Application Data
sub CSIDL_COMMON_FAVORITES     ()       { 0x001F }
sub CSIDL_INTERNET_CACHE       ()       { 0x0020 }
sub CSIDL_COOKIES              ()       { 0x0021 }
sub CSIDL_HISTORY              ()       { 0x0022 }
sub CSIDL_COMMON_APPDATA       ()       { 0x0023 }     # All Users\Application Data
sub CSIDL_WINDOWS              ()       { 0x0024 }     # GetWindowsDirectory()
sub CSIDL_SYSTEM               ()       { 0x0025 }     # GetSystemDirectory()
sub CSIDL_PROGRAM_FILES        ()       { 0x0026 }     # C:\Program Files
sub CSIDL_MYPICTURES           ()       { 0x0027 }     # "My Pictures", new for Win2K
sub CSIDL_PROFILE              ()       { 0x0028 }     # USERPROFILE
sub CSIDL_PROGRAM_FILES_COMMON ()       { 0x002B }     # C:\Program Files\Common
sub CSIDL_COMMON_TEMPLATES     ()       { 0x002D }     # All Users\Templates
sub CSIDL_COMMON_DOCUMENTS     ()       { 0x002E }     # All Users\Documents
sub CSIDL_COMMON_ADMINTOOLS    ()       { 0x002F }     # All Users\Start Menu\Programs\Administrative Tools
sub CSIDL_ADMINTOOLS           ()       { 0x0030 }     # <user name>\Start Menu\Programs\Administrative Tools
sub CSIDL_COMMON_MUSIC         ()       { 0x0035 }     # All Users\My Music
sub CSIDL_COMMON_PICTURES      ()       { 0x0036 }     # All Users\My Pictures
sub CSIDL_COMMON_VIDEO         ()       { 0x0037 }     # All Users\My Video
sub CSIDL_RESOURCES            ()       { 0x0038 }     # %windir%\Resources\, For theme and other windows resources.
sub CSIDL_RESOURCES_LOCALIZED  ()       { 0x0039 }     # %windir%\Resources\<LangID>, for theme and other windows specific resources.
sub CSIDL_CDBURN_AREA          ()       { 0x003B }     # <user name>\Local Settings\Application Data\Microsoft\CD Burning

sub VER_NT_DOMAIN_CONTROLLER () { 0x0000002 } # The system is a domain controller and the operating system is Windows Server 2008, Windows Server 2003, or Windows 2000 Server.
sub VER_NT_SERVER () { 0x0000003 } # The operating system is Windows Server 2008, Windows Server 2003, or Windows 2000 Server.
# Note that a server that is also a domain controller is reported as VER_NT_DOMAIN_CONTROLLER, not VER_NT_SERVER.
sub VER_NT_WORKSTATION () { 0x0000001 } # The operating system is Windows Vista, Windows XP Professional, Windows XP Home Edition, or Windows 2000 Professional.


sub VER_SUITE_BACKOFFICE               () { 0x00000004 } # Microsoft BackOffice components are installed.
sub VER_SUITE_BLADE                    () { 0x00000400 } # Windows Server 2003, Web Edition is installed.
sub VER_SUITE_COMPUTE_SERVER           () { 0x00004000 } # Windows Server 2003, Compute Cluster Edition is installed.
sub VER_SUITE_DATACENTER               () { 0x00000080 } # Windows Server 2008 Datacenter, Windows Server 2003, Datacenter Edition, or Windows 2000 Datacenter Server is installed.
sub VER_SUITE_ENTERPRISE               () { 0x00000002 } # Windows Server 2008 Enterprise, Windows Server 2003, Enterprise Edition, or Windows 2000 Advanced Server is installed. Refer to the Remarks section for more information about this bit flag.
sub VER_SUITE_EMBEDDEDNT               () { 0x00000040 } # Windows XP Embedded is installed.
sub VER_SUITE_PERSONAL                 () { 0x00000200 } # Windows Vista Home Premium, Windows Vista Home Basic, or Windows XP Home Edition is installed.
sub VER_SUITE_SINGLEUSERTS             () { 0x00000100 } # Remote Desktop is supported, but only one interactive session is supported. This value is set unless the system is running in application server mode.
sub VER_SUITE_SMALLBUSINESS            () { 0x00000001 } # Microsoft Small Business Server was once installed on the system, but may have been upgraded to another version of Windows. Refer to the Remarks section for more information about this bit flag.
sub VER_SUITE_SMALLBUSINESS_RESTRICTED () { 0x00000020 } # Microsoft Small Business Server is installed with the restrictive client license in force. Refer to the Remarks section for more information about this bit flag.
sub VER_SUITE_STORAGE_SERVER           () { 0x00002000 } # Windows Storage Server 2003 R2 or Windows Storage Server 2003 is installed.
sub VER_SUITE_TERMINAL                 () { 0x00000010 } # Terminal Services is installed. This value is always set.
# If VER_SUITE_TERMINAL is set but VER_SUITE_SINGLEUSERTS is not set, the system is running in application server mode.
sub VER_SUITE_WH_SERVER                () { 0x00008000 } # Windows Home Server is installed.


sub SM_TABLETPC                ()       { 86 }
sub SM_MEDIACENTER             ()       { 87 }
sub SM_STARTER                 ()       { 88 }
sub SM_SERVERR2                ()       { 89 }

sub PRODUCT_UNDEFINED                        () { 0x000 } # An unknown product
sub PRODUCT_ULTIMATE                         () { 0x001 } # Ultimate
sub PRODUCT_HOME_BASIC                       () { 0x002 } # Home Basic
sub PRODUCT_HOME_PREMIUM                     () { 0x003 } # Home Premium
sub PRODUCT_ENTERPRISE                       () { 0x004 } # Enterprise
sub PRODUCT_HOME_BASIC_N                     () { 0x005 } # Home Basic N
sub PRODUCT_BUSINESS                         () { 0x006 } # Business
sub PRODUCT_STANDARD_SERVER                  () { 0x007 } # Server Standard (full installation)
sub PRODUCT_DATACENTER_SERVER                () { 0x008 } # Server Datacenter (full installation)
sub PRODUCT_SMALLBUSINESS_SERVER             () { 0x009 } # Windows Small Business Server
sub PRODUCT_ENTERPRISE_SERVER                () { 0x00A } # Server Enterprise (full installation)
sub PRODUCT_STARTER                          () { 0x00B } # Starter
sub PRODUCT_DATACENTER_SERVER_CORE           () { 0x00C } # Server Datacenter (core installation)
sub PRODUCT_STANDARD_SERVER_CORE             () { 0x00D } # Server Standard (core installation)
sub PRODUCT_ENTERPRISE_SERVER_CORE           () { 0x00E } # Server Enterprise (core installation)
sub PRODUCT_ENTERPRISE_SERVER_IA64           () { 0x00F } # Server Enterprise for Itanium-based Systems
sub PRODUCT_BUSINESS_N                       () { 0x010 } # Business N
sub PRODUCT_WEB_SERVER                       () { 0x011 } # Web Server (full installation)
sub PRODUCT_CLUSTER_SERVER                   () { 0x012 } # HPC Edition
sub PRODUCT_HOME_SERVER                      () { 0x013 } # Home Server Edition
sub PRODUCT_STORAGE_EXPRESS_SERVER           () { 0x014 } # Storage Server Express
sub PRODUCT_STORAGE_STANDARD_SERVER          () { 0x015 } # Storage Server Standard
sub PRODUCT_STORAGE_WORKGROUP_SERVER         () { 0x016 } # Storage Server Workgroup
sub PRODUCT_STORAGE_ENTERPRISE_SERVER        () { 0x017 } # Storage Server Enterprise
sub PRODUCT_SERVER_FOR_SMALLBUSINESS         () { 0x018 } # Windows Server 2008 for Windows Essential Server Solutions
sub PRODUCT_SMALLBUSINESS_SERVER_PREMIUM     () { 0x019 } # Windows Small Business Server Premium
sub PRODUCT_HOME_PREMIUM_N                   () { 0x01A } # Home Premium N
sub PRODUCT_ENTERPRISE_N                     () { 0x01B } # Enterprise N
sub PRODUCT_ULTIMATE_N                       () { 0x01C } # Ultimate N
sub PRODUCT_WEB_SERVER_CORE                  () { 0x01D } # Web Server (core installation)
sub PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT () { 0x01E } # Windows Essential Business Server Management Server
sub PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY   () { 0x01F } # Windows Essential Business Server Security Server
sub PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING  () { 0x020 } # Windows Essential Business Server Messaging Server
sub PRODUCT_SERVER_FOUNDATION                () { 0x021 } # Server Foundation

sub PRODUCT_SERVER_FOR_SMALLBUSINESS_V       () { 0x023 } # Windows Server 2008 without Hyper-V for Windows Essential Server Solutions
sub PRODUCT_STANDARD_SERVER_V                () { 0x024 } # Server Standard without Hyper-V (full installation)
sub PRODUCT_DATACENTER_SERVER_V              () { 0x025 } # Server Datacenter without Hyper-V (full installation)
sub PRODUCT_ENTERPRISE_SERVER_V              () { 0x026 } # Server Enterprise without Hyper-V (full installation)
sub PRODUCT_DATACENTER_SERVER_CORE_V         () { 0x027 } # Server Datacenter without Hyper-V (core installation)
sub PRODUCT_STANDARD_SERVER_CORE_V           () { 0x028 } # Server Standard without Hyper-V (core installation)
sub PRODUCT_ENTERPRISE_SERVER_CORE_V         () { 0x029 } # Server Enterprise without Hyper-V (core installation)
sub PRODUCT_HYPERV                           () { 0x02A } # Microsoft Hyper-V Server

sub PRODUCT_STARTER_N                        () { 0x02F } # Starter N
sub PRODUCT_PROFESSIONAL                     () { 0x030 } # Professional
sub PRODUCT_PROFESSIONAL_N                   () { 0x031 } # Professional N

sub PRODUCT_STARTER_E                        () { 0x042 } # Starter E
sub PRODUCT_HOME_BASIC_E                     () { 0x043 } # Home Basic E
sub PRODUCT_HOME_PREMIUM_E                   () { 0x044 } # Home Premium E
sub PRODUCT_PROFESSIONAL_E                   () { 0x045 } # Professional E
sub PRODUCT_ENTERPRISE_E                     () { 0x046 } # Enterprise E
sub PRODUCT_ULTIMATE_E                       () { 0x047 } # Ultimate E

sub PRODUCT_UNLICENSED                       () { 0xABCDABCD } # product has not been activated and is no longer in the grace period

sub PROCESSOR_ARCHITECTURE_AMD64   ()   { 9 }      # x64 (AMD or Intel)
sub PROCESSOR_ARCHITECTURE_IA64    ()   { 6 }      # Intel Itanium Processor Family (IPF)
sub PROCESSOR_ARCHITECTURE_INTEL   ()   { 0 }      # x86
sub PROCESSOR_ARCHITECTURE_UNKNOWN ()   { 0xffff } # Unknown architecture.

sub _GetProcessorArchitecture {
    my $arch = {
	 386 => PROCESSOR_ARCHITECTURE_INTEL,
	 486 => PROCESSOR_ARCHITECTURE_INTEL,
	 586 => PROCESSOR_ARCHITECTURE_INTEL,
	2200 => PROCESSOR_ARCHITECTURE_IA64,
	8664 => PROCESSOR_ARCHITECTURE_AMD64,
    }->{Win32::GetChipName()};
    return defined($arch) ? $arch : PROCESSOR_ARCHITECTURE_UNKNOWN;
}

### This method is just a simple interface into GetOSVersion().  More
### specific or demanding situations should use that instead.

my ($cached_os, $cached_desc);

sub GetOSName {
    unless (defined $cached_os) {
	my($desc, $major, $minor, $build, $id, undef, undef, $suitemask, $producttype)
	    = Win32::GetOSVersion();
	my $arch = _GetProcessorArchitecture();
	my $productinfo = Win32::GetProductInfo(6, 0, 0, 0);
	($cached_os, $cached_desc) = _GetOSName($desc, $major, $minor, $build, $id,
						$suitemask, $producttype, $productinfo, $arch);
    }
    return wantarray ? ($cached_os, $cached_desc) : $cached_os;
}

sub GetOSDisplayName {
    # Calling GetOSDisplayName() with arguments is for the test suite only!
    my($name,$desc) = @_ ? @_ : GetOSName();
    $name =~ s/^Win//;
    if ($desc =~ /^Windows Home Server\b/ || $desc =~ /^Windows XP Professional x64 Edition\b/) {
	($name, $desc) = ($desc, "");
    }
    elsif ($desc =~ s/\s*(Windows (.*) Server( \d+)?)//) {
	$name = "$1 $name";
	$desc =~ s/^\s+//;
    }
    else {
	for ($name) {
	    s/^/Windows / unless /^Win32s$/;
	    s/\/.Net//;
	    s/NT(\d)/NT $1/;
	    if ($desc =~ s/\s*(HPC|Small Business|Web) Server//) {
		my $name = $1;
		$desc =~ s/^\s*//;
		s/(200.)/$name Server $1/;
	    }
	    s/^Windows (200[38])/Windows Server $1/;
	}
    }
    $name .= " $desc" if length $desc;
    return $name;
}

sub _GetSystemMetrics {
    my($index,$metrics) = @_;
    return Win32::GetSystemMetrics($index) unless ref $metrics;
    return $metrics->{$index} if ref $metrics eq "HASH" && defined $metrics->{$index};
    return 1 if ref $metrics eq "ARRAY" && grep $_ == $index, @$metrics;
    return 0;
}

sub _GetOSName {
    # The $metrics argument only exists for the benefit of t/GetOSName.t
    my($csd, $major, $minor, $build, $id, $suitemask, $producttype, $productinfo, $arch, $metrics) = @_;

    my($os,@tags);
    my $desc = "";
    if ($id == 0) {
	$os = "Win32s";
    }
    elsif ($id == 1) {
	if ($minor == 0) {
	    $os = "95";
	}
	elsif ($minor == 10) {
	    $os = "98";
	}
	elsif ($minor == 90) {
	    $os = "Me";
	}
    }
    elsif ($id == 2) {
	if ($major == 3) {
	    $os = "NT3.51";
	}
	elsif ($major == 4) {
	    $os = "NT4";
	}
	elsif ($major == 5) {
	    if ($minor == 0) {
		$os = "2000";
		if ($producttype == VER_NT_WORKSTATION) {
		    $desc = "Professional";
		}
		else {
		    if ($suitemask & VER_SUITE_DATACENTER) {
			$desc = "Datacenter Server";
		    }
		    elsif ($suitemask & VER_SUITE_ENTERPRISE) {
			$desc = "Advanced Server";
		    }
		    elsif ($suitemask & VER_SUITE_SMALLBUSINESS_RESTRICTED) {
			$desc = "Small Business Server";
		    }
		    else {
			$desc = "Server";
		    }
		}
		# XXX ignoring "Windows 2000 Advanced Server Limited Edition" for Itanium
		# XXX and "Windows 2000 Datacenter Server Limited Edition" for Itanium
	    }
	    elsif ($minor == 1) {
		$os = "XP/.Net";
		if (_GetSystemMetrics(SM_MEDIACENTER, $metrics)) {
		    $desc = "Media Center Edition";
		}
		elsif (_GetSystemMetrics(SM_TABLETPC, $metrics)) {
		    # Tablet PC Edition is based on XP Pro
		    $desc = "Tablet PC Edition";
		}
		elsif (_GetSystemMetrics(SM_STARTER, $metrics)) {
		    $desc = "Starter Edition";
		}
		elsif ($suitemask & VER_SUITE_PERSONAL) {
		    $desc = "Home Edition";
		}
		else {
		    $desc = "Professional";
		}
		# XXX ignoring all Windows XP Embedded and Fundamentals versions
	    }
	    elsif ($minor == 2) {
		$os = "2003";

		if (_GetSystemMetrics(SM_SERVERR2, $metrics)) {
		    # XXX R2 was released for all x86 and x64 versions,
		    # XXX but only Enterprise Edition for Itanium.
		    $desc = "R2";
		}

		if ($suitemask == VER_SUITE_STORAGE_SERVER) {
		    $desc .= " Windows Storage Server";
		}
		elsif ($suitemask == VER_SUITE_WH_SERVER) {
		    $desc .= " Windows Home Server";
		}
		elsif ($producttype == VER_NT_WORKSTATION && $arch == PROCESSOR_ARCHITECTURE_AMD64) {
		    $desc .= " Windows XP Professional x64 Edition";
		}

		# Test for the server type.
		if ($producttype != VER_NT_WORKSTATION) {
		    if ($arch == PROCESSOR_ARCHITECTURE_IA64) {
			if ($suitemask & VER_SUITE_DATACENTER) {
			    $desc .= " Datacenter Edition for Itanium-based Systems";
			}
			elsif ($suitemask & VER_SUITE_ENTERPRISE) {
			    $desc .= " Enterprise Edition for Itanium-based Systems";
			}
		    }
		    elsif ($arch == PROCESSOR_ARCHITECTURE_AMD64) {
			if ($suitemask & VER_SUITE_DATACENTER) {
			    $desc .= " Datacenter x64 Edition";
			}
			elsif ($suitemask & VER_SUITE_ENTERPRISE) {
			    $desc .= " Enterprise x64 Edition";
			}
			else {
			    $desc .= " Standard x64 Edition";
			}
		    }
		    else {
			if ($suitemask & VER_SUITE_COMPUTE_SERVER) {
			    $desc .= " Windows Compute Cluster Server";
			}
			elsif ($suitemask & VER_SUITE_DATACENTER) {
			    $desc .= " Datacenter Edition";
			}
			elsif ($suitemask & VER_SUITE_ENTERPRISE) {
			    $desc .= " Enterprise Edition";
			}
			elsif ($suitemask & VER_SUITE_BLADE) {
			    $desc .= " Web Edition";
			}
			elsif ($suitemask & VER_SUITE_SMALLBUSINESS_RESTRICTED) {
			    $desc .= " Small Business Server";
			}
			else {
			    if ($desc !~ /Windows (Home|Storage) Server/) {
				$desc .= " Standard Edition";
			    }
			}
		    }
		}
	    }
	}
	elsif ($major == 6) {
	    if ($minor == 0) {
		if ($producttype == VER_NT_WORKSTATION) {
		    $os = "Vista";
		}
		else {
		    $os = "2008";
		}
	    }
	    elsif ($minor == 1) {
		if ($producttype == VER_NT_WORKSTATION) {
		    $os = "7";
		}
		else {
		    $os = "2008";
		    $desc = "R2";
		}
	    }

            if ($productinfo == PRODUCT_ULTIMATE) {
		$desc .= " Ultimate";
	    }
            elsif ($productinfo == PRODUCT_HOME_PREMIUM) {
               $desc .= " Home Premium";
            }
            elsif ($productinfo == PRODUCT_HOME_BASIC) {
               $desc .= " Home Basic";
            }
            elsif ($productinfo == PRODUCT_ENTERPRISE) {
               $desc .= " Enterprise";
            }
            elsif ($productinfo == PRODUCT_BUSINESS) {
	       # "Windows 7 Business" had a name change to "Windows 7 Professional"
               $desc .= $minor == 0 ? " Business" : " Professional";
            }
            elsif ($productinfo == PRODUCT_STARTER) {
               $desc .= " Starter";
            }
            elsif ($productinfo == PRODUCT_CLUSTER_SERVER) {
               $desc .= " HPC Server";
            }
            elsif ($productinfo == PRODUCT_DATACENTER_SERVER) {
               $desc .= " Datacenter";
            }
            elsif ($productinfo == PRODUCT_DATACENTER_SERVER_CORE) {
               $desc .= " Datacenter Edition (core installation)";
            }
            elsif ($productinfo == PRODUCT_ENTERPRISE_SERVER) {
               $desc .= " Enterprise";
            }
            elsif ($productinfo == PRODUCT_ENTERPRISE_SERVER_CORE) {
               $desc .= " Enterprise Edition (core installation)";
            }
            elsif ($productinfo == PRODUCT_ENTERPRISE_SERVER_IA64) {
               $desc .= " Enterprise Edition for Itanium-based Systems";
            }
            elsif ($productinfo == PRODUCT_SMALLBUSINESS_SERVER) {
               $desc .= " Small Business Server";
            }
            elsif ($productinfo == PRODUCT_SMALLBUSINESS_SERVER_PREMIUM) {
               $desc .= " Small Business Server Premium Edition";
            }
            elsif ($productinfo == PRODUCT_STANDARD_SERVER) {
               $desc .= " Standard";
            }
            elsif ($productinfo == PRODUCT_STANDARD_SERVER_CORE) {
               $desc .= " Standard Edition (core installation)";
            }
            elsif ($productinfo == PRODUCT_WEB_SERVER) {
               $desc .= " Web Server";
            }
            elsif ($productinfo == PRODUCT_PROFESSIONAL) {
               $desc .= " Professional";
            }

	    if ($arch == PROCESSOR_ARCHITECTURE_INTEL) {
		$desc .= " (32-bit)";
	    }
	    elsif ($arch == PROCESSOR_ARCHITECTURE_AMD64) {
		$desc .= " (64-bit)";
	    }
	}
    }

    unless (defined $os) {
	warn "Unknown Windows version [$id:$major:$minor]";
	return;
    }

    for ($desc) {
	s/\s\s+/ /g;
	s/^\s//;
	s/\s$//;
    }

    # XXX What about "Small Business Server"? NT, 200, 2003, 2008 editions...

    if ($major >= 5) {
	# XXX XP, Vista, 7 all have starter editions
	#push(@tags, "Starter Edition") if _GetSystemMetrics(SM_STARTER, $metrics);
    }

    if (@tags) {
	unshift(@tags, $desc) if length $desc;
	$desc = join(" ", @tags);
    }

    if (length $csd) {
	$desc .= " " if length $desc;
	$desc .= $csd;
    }
    return ("Win$os", $desc);
}

# "no warnings 'redefine';" doesn't work for 5.8.7 and earlier
local $^W = 0;
bootstrap Win32;

1;

__END__

#line 1202
FILE   ab64d430/attributes.pm  
�#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/attributes.pm"
package attributes;

our $VERSION = 0.14;

@EXPORT_OK = qw(get reftype);
@EXPORT = ();
%EXPORT_TAGS = (ALL => [@EXPORT, @EXPORT_OK]);

use strict;

sub croak {
    require Carp;
    goto &Carp::croak;
}

sub carp {
    require Carp;
    goto &Carp::carp;
}

my %deprecated;
$deprecated{CODE} = qr/\A-?(locked)\z/;
$deprecated{ARRAY} = $deprecated{HASH} = $deprecated{SCALAR}
    = qr/\A-?(unique)\z/;

sub _modify_attrs_and_deprecate {
    my $svtype = shift;
    # Now that we've removed handling of locked from the XS code, we need to
    # remove it here, else it ends up in @badattrs. (If we do the deprecation in
    # XS, we can't control the warning based on *our* caller's lexical settings,
    # and the warned line is in this package)
    grep {
	$deprecated{$svtype} && /$deprecated{$svtype}/ ? do {
	    require warnings;
	    warnings::warnif('deprecated', "Attribute \"$1\" is deprecated");
	    0;
	} : 1
    } _modify_attrs(@_);
}

sub import {
    @_ > 2 && ref $_[2] or do {
	require Exporter;
	goto &Exporter::import;
    };
    my (undef,$home_stash,$svref,@attrs) = @_;

    my $svtype = uc reftype($svref);
    my $pkgmeth;
    $pkgmeth = UNIVERSAL::can($home_stash, "MODIFY_${svtype}_ATTRIBUTES")
	if defined $home_stash && $home_stash ne '';
    my @badattrs;
    if ($pkgmeth) {
	my @pkgattrs = _modify_attrs_and_deprecate($svtype, $svref, @attrs);
	@badattrs = $pkgmeth->($home_stash, $svref, @pkgattrs);
	if (!@badattrs && @pkgattrs) {
            require warnings;
	    return unless warnings::enabled('reserved');
	    @pkgattrs = grep { m/\A[[:lower:]]+(?:\z|\()/ } @pkgattrs;
	    if (@pkgattrs) {
		for my $attr (@pkgattrs) {
		    $attr =~ s/\(.+\z//s;
		}
		my $s = ((@pkgattrs == 1) ? '' : 's');
		carp "$svtype package attribute$s " .
		    "may clash with future reserved word$s: " .
		    join(' : ' , @pkgattrs);
	    }
	}
    }
    else {
	@badattrs = _modify_attrs_and_deprecate($svtype, $svref, @attrs);
    }
    if (@badattrs) {
	croak "Invalid $svtype attribute" .
	    (( @badattrs == 1 ) ? '' : 's') .
	    ": " .
	    join(' : ', @badattrs);
    }
}

sub get ($) {
    @_ == 1  && ref $_[0] or
	croak 'Usage: '.__PACKAGE__.'::get $ref';
    my $svref = shift;
    my $svtype = uc reftype($svref);
    my $stash = _guess_stash($svref);
    $stash = caller unless defined $stash;
    my $pkgmeth;
    $pkgmeth = UNIVERSAL::can($stash, "FETCH_${svtype}_ATTRIBUTES")
	if defined $stash && $stash ne '';
    return $pkgmeth ?
		(_fetch_attrs($svref), $pkgmeth->($stash, $svref)) :
		(_fetch_attrs($svref))
	;
}

sub require_version { goto &UNIVERSAL::VERSION }

require XSLoader;
XSLoader::load();

1;
__END__
#The POD goes here

#line 491
FILE   (2870f2ba/auto/Compress/Raw/Zlib/Zlib.dll ~�MZ�       ��  �       @                                   �   � �	�!�L�!This program cannot be run in DOS mode.
$       PE  L ��S  �  � # �      ��     �   3i                    p    �U                           (   @ �                           P �	                                                  LA �                           .text   ��     �                ` P`.data   d]   �  ^   �             @ `�/4                �             @ 0�.bss                            � `�.edata  (         �             @ 0@.idata  �   @                  @ 0�.reloc  �	   P  
   
             @ 0B/14         `                  @ 0@                                                                                                                                                                                                                                                                                                                                        U����$`�4i�� ���Ҹ    t�D$o�4i�$��� ����t�D$ 5i�$  5i�С`�4i��t:�$��4i�Æ ���Ҹ    t�D$��4i�$譆 ����t	�$`�4i���Ív U����$`�4i�~� ���Ҹ    t�D$��4i�$�h� ����t	�$  5i���Ð�������������U��WVS��<��A5i��$�ׁ �8��$�ˁ �PH�2���PH��$趁 ��    P)�������  ��$蔁 �@�@ �7  ��$�}� �$�͆ �ǋ���$�f� ��    �U�@���@	�]  ��$�B� �@����U��$�/� �U��D$Ġ4i�T$�$�P� ���!  ��$�� �@���@�@	��   ��$�� �@���@� �@�P�4�������U��$�Ā p��$跀 �U��D$    �|$�T$�$�,� �G
@t��$茀 �|$�$�8� �~��$�s� �Ƌ�$�g� �@E���<[^_]Í�&    ��$�F� �x��$�9� �@�@�<�������$�!� �@���P��U��$�� �U��D$   �T$�$��� ������$�� �D$Ġ4i�D$��4i�D$�4i�D$�4i�$�E� ��$� �U�D$��4i�T$�$�4� ��    ��    U��WVS��<��A5i��$�w �8��$�k �PH�2���PH��$�V ��    P)�������  ��$�4 �@�@ �7  ��$� �$�m� �ǋ���$� ��    �U�@���@	�]  ��$��~ �@����U��$��~ �U��D$Ġ4i�T$�$��� ���!  ��$�~ �@���@�@	��   ��$�~ �@���@� �@�P�4�������U��$�d~ p��$�W~ �U��D$    �|$�T$�$�̃ �G
@t��$�,~ �|$�$�؂ �~��$�~ �Ƌ�$�~ �@E���<[^_]Í�&    ��$��} �x��$��} �@�@�<�������$��} �@���P��U��$�} �U��D$   �T$�$蜂 ������$�} �D$Ġ4i�D$��4i�D$8�4i�D$�4i�$�� ��$�S} �U�D$��4i�T$�$�ԁ ��    ��    U��WVS��<��A5i��$�} �8��$�} �PH�2���PH��$��| ��    P)�������  ��$��| �@�@ �7  ��$�| �$�
@t��$��{ �|$�$�u� �~��$�{ �Ƌ�$�{ �@E���<[^_]Ít& ��$�{ �x��$�y{ �@�@�<�������$�a{ �@���P��U��$�K{ �U��D$   �T$�$�<� ������$�%{ �D$Ġ4i�D$��4i�D$h�4i�D$�4i�$� ��$��z �U�D$��4i�T$�$�t ��    ��    U��WVS��<��A5i��$�z �8��$�z �PH�2���PH��$�z ��    P)�������  ��$�tz �@�@ �G  ��$�]z �$� �ǋ���$�Fz ��    �U�@���@	�m  ��$�"z �@����U��$�z �U��D$Ġ4i�T$�$�0 ���1  ��$��y �@���@�@	��   ��$��y �@���@� �@���   1Ƀ��    ��4�������)ʉU��$�y p��$�y �U��D$    �|$�T$�$��~ �G
@t��$�[y �|$�$�~ �~��$�By �Ƌ�$�6y �@E���<[^_]Í�    ��$�y �x��$�	y �@�@�<�������$��x �@���P��U��$��x �U��D$   �T$�$��} ������$�x �D$Ġ4i�D$��4i�D$��4i�D$�4i�$�} ��$�x �U�D$��4i�T$�$�} ��    ��    U��WVS��<��A5i��$�Gx �8��$�;x �PH�2���PH��$�&x ��    P)�������  ��$�x �@�@ �7  ��$��w �$�=} �ǋ���$��w ��    �U�@���@	�]  ��$�w �@����U��$�w �U��D$Ġ4i�T$�$��| ���!  ��$�vw �@���@�@	��   ��$�Yw �@���@� �@�Pt�4�������U��$�4w p��$�'w �U��D$    �|$�T$�$�| �G
@t��$��v �|$�$�{ �~��$��v �Ƌ�$��v �@E���<[^_]Í�&    ��$�v �x��$�v �@�@�<�������$�v �@���P��U��$�{v �U��D$   �T$�$�l{ ������$�Uv �D$Ġ4i�D$��4i�D$�4i�D$�4i�$�z ��$�#v �U�D$��4i�T$�$�z ��    ��    U��WVS��<��A5i��$��u �8��$��u �PH�2���PH��$��u ��    P)�������  ��$�u �@�@ �7  ��$�u �$��z �ǋ���$�vu ��    �U�@���@	�]  ��$�Ru �@����U��$�?u �U��D$Ġ4i�T$�$�`z ���!  ��$�u �@���@�@	��   ��$��t �@���@� �@�Pp�4�������U��$��t p��$��t �U��D$    �|$�T$�$�<z �G
@t��$�t �|$�$�Hy �~��$�t �Ƌ�$�wt �@E���<[^_]Í�&    ��$�Vt �x��$�It �@�@�<�������$�1t �@���P��U��$�t �U��D$   �T$�$�y ������$��s �D$Ġ4i�D$��4i�D$�4i�D$�4i�$�Ux ��$��s �U�D$��4i�T$�$�Dx ��    ��    U��WVS��<��A5i��$�s �8��$�{s �PH�2���PH��$�fs ��    P)�������  ��$�Ds �@�@ �7  ��$�-s �$�}x �ǋ���$�s ��    �U�@���@	�]  ��$��r �@����U��$��r �U��D$Ġ4i�T$�$� x ���!  ��$�r �@���@�@	��   ��$�r �@���@� �@�Pl�4�������U��$�tr p��$�gr �U��D$    �|$�T$�$��w �G
@t��$�<r �|$�$��v �~��$�#r �Ƌ�$�r �@E���<[^_]Í�&    ��$��q �x��$��q �@�@�<�������$��q �@���P��U��$�q �U��D$   �T$�$�v ������$�q �D$Ġ4i�D$��4i�D$T�4i�D$�4i�$��u ��$�cq �U�D$��4i�T$�$��u ��    ��    U��WVS��<��A5i��$�'q �8��$�q �PH�2���PH��$�q ��    P)�������  ��$��p �@�@ �7  ��$��p �$�v �ǋ���$�p ��    �U�@���@	�]  ��$�p �@����U��$�p �U��D$Ġ4i�T$�$�u ���!  ��$�Vp �@���@�@	��   ��$�9p �@���@� �@���   �4�������U��$�p p��$�p �U��D$    �|$�T$�$�yu �G
@t��$��o �|$�$�t �~��$��o �Ƌ�$�o �@E���<[^_]Ít& ��$�o �x��$�o �@�@�<�������$�qo �@���P��U��$�[o �U��D$   �T$�$�Lt ������$�5o �D$Ġ4i�D$��4i�D$��4i�D$�4i�$�s ��$�o �U�D$��4i�T$�$�s ��    ��    U��WVS��<��A5i��$��n �8��$�n �PH�2���PH��$�n ��    P)�������  ��$�n �@�@ �7  ��$�mn �$�s �ǋ���$�Vn ��    �U�@���@	�]  ��$�2n �@����U��$�n �U��D$Ģ4i�T$�$�@s ���!  ��$��m �@���@�@	��   ��$��m �@���@� �@�PD�4�������U��$�m p��$�m �U��D$    �|$�T$�$�s �G
@t��$�|m �|$�$�(r �~��$�cm �Ƌ�$�Wm �@E���<[^_]Í�&    ��$�6m �x��$�)m �@�@�<�������$�m �@���P��U��$��l �U��D$   �T$�$��q ������$��l �D$Ģ4i�D$��4i�D$�4i�D$�4i�$�5q ��$�l �U�D$��4i�T$�$�$q ��    ��    U��WVS��<��A5i��$�gl �8��$�[l �PH�2���PH��$�Fl ��    P)�������  ��$�$l �@�@ �7  ��$�
@t��$�k �|$�$��o �~��$�k �Ƌ�$��j �@E���<[^_]Í�&    ��$��j �x��$��j �@�@�<�������$�j �@���P��U��$�j �U��D$   �T$�$�o ������$�uj �D$Ģ4i�D$��4i�D$�4i�D$�4i�$��n ��$�Cj �U�D$��4i�T$�$��n ��    ��    U��WVS��<��A5i��$�j �8��$��i �PH�2���PH��$��i ��    P)�������  ��$��i �@�@ �7  ��$�i �$��n �ǋ���$�i ��    �U�@���@	�]  ��$�ri �@����U��$�_i �U��D$Ģ4i�T$�$�n ���!  ��$�6i �@���@�@	��   ��$�i �@���@� �@�P�4�������U��$��h p��$��h �U��D$    �|$�T$�$�\n �G
@t��$�h �|$�$�hm �~��$�h �Ƌ�$�h �@E���<[^_]Í�&    ��$�vh �x��$�ih �@�@�<�������$�Qh �@���P��U��$�;h �U��D$   �T$�$�,m ������$�h �D$Ģ4i�D$��4i�D$H�4i�D$�4i�$�ul ��$��g �U�D$��4i�T$�$�dl ��    ��    U��WVS��<��A5i��$�g �8��$�g �PH�2���PH��$�g ��    P)�������  ��$�dg �@�@ �7  ��$�Mg �$�l �ǋ���$�6g ��    �U�@���@	�]  ��$�g �@����U��$��f �U��D$Ģ4i�T$�$� l ���!  ��$��f �@���@�@	��   ��$�f �@���@� �@�P�4�������U��$�f p��$�f �U��D$    �|$�T$�$��k �G
@t��$�\f �|$�$�k �~��$�Cf �Ƌ�$�7f �@E���<[^_]Í�&    ��$�f �x��$�	f �@�@�<�������$��e �@���P��U��$��e �U��D$   �T$�$��j ������$�e �D$Ģ4i�D$��4i�D$t�4i�D$�4i�$�j ��$�e �U�D$��4i�T$�$�j ��    ��    U��WVS��<��A5i��$�Ge �8��$�;e �PH�2���PH��$�&e ��    P)�������  ��$�e �@�@ �7  ��$��d �$�=j �ǋ���$��d ��    �U�@���@	�]  ��$�d �@����U��$�d �U��D$Ģ4i�T$�$��i ���!  ��$�vd �@���@�@	��   ��$�Yd �@���@� �@�PL�4�������U��$�4d p��$�'d �U��D$    �|$�T$�$�i �G
@t��$��c �|$�$�h �~��$��c �Ƌ�$��c �@E���<[^_]Í�&    ��$�c �x��$�c �@�@�<�������$�c �@���P��U��$�{c �U��D$   �T$�$�lh ������$�Uc �D$Ģ4i�D$��4i�D$��4i�D$�4i�$�g ��$�#c �U�D$��4i�T$�$�g ��    ��    U��WVS��<��A5i��$��b �8��$��b �PH�2���PH��$��b ��    P)�������  ��$�b �@�@ �7  ��$�b �$��g �ǋ���$�vb ��    �U�@���@	�]  ��$�Rb �@����U��$�?b �U��D$Ģ4i�T$�$�`g ���!  ��$�b �@���@�@	��   ��$��a �@���@� �@�P�4�������U��$��a p��$��a �U��D$    �|$�T$�$�<g �G
@t��$�a �|$�$�Hf �~��$�a �Ƌ�$�wa �@E���<[^_]Í�&    ��$�Va �x��$�Ia �@�@�<�������$�1a �@���P��U��$�a �U��D$   �T$�$�f ������$��` �D$Ģ4i�D$��4i�D$ԣ4i�D$�4i�$�Ue ��$��` �U�D$��4i�T$�$�De ��    ��    U��WVS��<��A5i��$�` �8��$�{` �PH�2���PH��$�f` ��    P)�������  ��$�D` �@�@ �7  ��$�-` �$�}e �ǋ���$�` ��    �U�@���@	�]  ��$��_ �@����U��$��_ �U��D$Ģ4i�T$�$� e ���!  ��$�_ �@���@�@	��   ��$�_ �@���@� �@�PP�4�������U��$�t_ p��$�g_ �U��D$    �|$�T$�$��d �G
@t��$�<_ �|$�$��c �~��$�#_ �Ƌ�$�_ �@E���<[^_]Í�&    ��$��^ �x��$��^ �@�@�<�������$��^ �@���P��U��$�^ �U��D$   �T$�$�c ������$�^ �D$Ģ4i�D$��4i�D$ �4i�D$�4i�$��b ��$�c^ �U�D$��4i�T$�$��b ��    ��    U��WVS��<��A5i��$�'^ �8��$�^ �PH�2���PH��$�^ ��    P)�������  ��$��] �@�@ �7  ��$��] �$�c �ǋ���$�] ��    �U�@���@	�]  ��$�] �@����U��$�] �U��D$Ģ4i�T$�$�b ���!  ��$�V] �@���@�@	��   ��$�9] �@���@� �@�Pt�4�������U��$�] p��$�] �U��D$    �|$�T$�$�|b �G
@t��$��\ �|$�$�a �~��$��\ �Ƌ�$�\ �@E���<[^_]Í�&    ��$�\ �x��$�\ �@�@�<�������$�q\ �@���P��U��$�[\ �U��D$   �T$�$�La ������$�5\ �D$Ģ4i�D$��4i�D$,�4i�D$�4i�$�` ��$�\ �U�D$��4i�T$�$�` ��    ��    U��WVS��<��A5i��$��[ �8��$�[ �PH�2���PH��$�[ ��    P)�������  ��$�[ �@�@ �7  ��$�m[ �$�` �ǋ���$�V[ ��    �U�@���@	�]  ��$�2[ �@����U��$�[ �U��D$Ģ4i�T$�$�@` ���!  ��$��Z �@���@�@	��   ��$��Z �@���@� �@�Pp�4�������U��$�Z p��$�Z �U��D$    �|$�T$�$�` �G
@t��$�|Z �|$�$�(_ �~��$�cZ �Ƌ�$�WZ �@E���<[^_]Í�&    ��$�6Z �x��$�)Z �@�@�<�������$�Z �@���P��U��$��Y �U��D$   �T$�$��^ ������$��Y �D$Ģ4i�D$��4i�D$d�4i�D$�4i�$�5^ ��$�Y �U�D$��4i�T$�$�$^ ��    ��    U��WVS��<��A5i��$�gY �8��$�[Y �PH�2���PH��$�FY ��    P)�������  ��$�$Y �@�@ �7  ��$�
@t��$�X �|$�$��\ �~��$�X �Ƌ�$��W �@E���<[^_]Í�&    ��$��W �x��$��W �@�@�<�������$�W �@���P��U��$�W �U��D$   �T$�$�\ ������$�uW �D$Ģ4i�D$��4i�D$��4i�D$�4i�$��[ ��$�CW �U�D$��4i�T$�$��[ ��    ��    U��WVS��<��A5i��$�W �8��$��V �PH�2���PH��$��V ��    P)�������  ��$��V �@�@ �7  ��$�V �$��[ �ǋ���$�V ��    �U�@���@	�]  ��$�rV �@����U��$�_V �U��D$̤4i�T$�$�[ ���!  ��$�6V �@���@�@	��   ��$�V �@���@� �@�P �4�������U��$��U p��$��U �U��D$    �|$�T$�$�\[ �G
@t��$�U �|$�$�hZ �~��$�U �Ƌ�$�U �@E���<[^_]Í�&    ��$�vU �x��$�iU �@�@�<�������$�QU �@���P��U��$�;U �U��D$   �T$�$�,Z ������$�U �D$̤4i�D$��4i�D$�4i�D$�4i�$�uY ��$��T �U�D$��4i�T$�$�dY ��    ��    U��WVS��<��A5i��$�T �8��$�T �PH�2���PH��$�T ��    P)�������  ��$�dT �@�@ �7  ��$�MT �$�Y �ǋ���$�6T ��    �U�@���@	�]  ��$�T �@����U��$��S �U��D$̤4i�T$�$� Y ���!  ��$��S �@���@�@	��   ��$�S �@���@� �@�P�4�������U��$�S p��$�S �U��D$    �|$�T$�$��X �G
@t��$�\S �|$�$�X �~��$�CS �Ƌ�$�7S �@E���<[^_]Í�&    ��$�S �x��$�	S �@�@�<�������$��R �@���P��U��$��R �U��D$   �T$�$��W ������$�R �D$̤4i�D$��4i�D$ �4i�D$�4i�$�W ��$�R �U�D$��4i�T$�$�W ��    ��    U��WVS��<��A5i��$�GR �8��$�;R �PH�2���PH��$�&R ��    P)�������  ��$�R �@�@ �7  ��$��Q �$�=W �ǋ���$��Q ��    �U�@���@	�]  ��$�Q �@����U��$�Q �U��D$̤4i�T$�$��V ���!  ��$�vQ �@���@�@	��   ��$�YQ �@���@� �@�Pt�4�������U��$�4Q p��$�'Q �U��D$    �|$�T$�$�V �G
@t��$��P �|$�$�U �~��$��P �Ƌ�$��P �@E���<[^_]Í�&    ��$�P �x��$�P �@�@�<�������$�P �@���P��U��$�{P �U��D$   �T$�$�lU ������$�UP �D$̤4i�D$��4i�D$P�4i�D$�4i�$�T ��$�#P �U�D$��4i�T$�$�T ��    ��    U��WVS��<��A5i��$��O �8��$��O �PH�2���PH��$��O ��    P)�������  ��$�O �@�@ �7  ��$�O �$��T �ǋ���$�vO ��    �U�@���@	�]  ��$�RO �@����U��$�?O �U��D$̤4i�T$�$�`T ���!  ��$�O �@���@�@	��   ��$��N �@���@� �@�Pp�4�������U��$��N p��$��N �U��D$    �|$�T$�$�<T �G
@t��$�N �|$�$�HS �~��$�N �Ƌ�$�wN �@E���<[^_]Í�&    ��$�VN �x��$�IN �@�@�<�������$�1N �@���P��U��$�N �U��D$   �T$�$�S ������$��M �D$̤4i�D$��4i�D$��4i�D$�4i�$�UR ��$��M �U�D$��4i�T$�$�DR ��    ��    U��WVS��<��A5i��$�M �8��$�{M �PH�2���PH��$�fM ��    P)�������  ��$�DM �@�@ �7  ��$�-M �$�}R �ǋ���$�M ��    �U�@���@	�]  ��$��L �@����U��$��L �U��D$̤4i�T$�$� R ���!  ��$�L �@���@�@	��   ��$�L �@���@� �@�P�4�������U��$�tL p��$�gL �U��D$    �|$�T$�$��Q �G
@t��$�<L �|$�$��P �~��$�#L �Ƌ�$�L �@E���<[^_]Í�&    ��$��K �x��$��K �@�@�<�������$��K �@���P��U��$�K �U��D$   �T$�$�P ������$�K �D$̤4i�D$��4i�D$��4i�D$�4i�$��O ��$�cK �U�D$��4i�T$�$��O ��    ��    U��WVS��<��A5i��$�'K �8��$�K �PH�2���PH��$�K ��    P)�������  ��$��J �@�@ �7  ��$��J �$�P �ǋ���$�J ��    �U�@���@	�]  ��$�J �@����U��$�J �U��D$̤4i�T$�$�O ���!  ��$�VJ �@���@�@	��   ��$�9J �@���@� �@�PL�4�������U��$�J p��$�J �U��D$    �|$�T$�$�|O �G
@t��$��I �|$�$�N �~��$��I �Ƌ�$�I �@E���<[^_]Í�&    ��$�I �x��$�I �@�@�<�������$�qI �@���P��U��$�[I �U��D$   �T$�$�LN ������$�5I �D$̤4i�D$��4i�D$�4i�D$�4i�$�M ��$�I �U�D$��4i�T$�$�M ��    ��    U��WVS��<��A5i��$��H �8��$�H �PH�2���PH��$�H ��    P)�������  ��$�H �@�@ �7  ��$�mH �$�M �ǋ���$�VH ��    �U�@���@	�]  ��$�2H �@����U��$�H �U��D$̤4i�T$�$�@M ���!  ��$��G �@���@�@	��   ��$��G �@���@� �@�P�4�������U��$�G p��$�G �U��D$    �|$�T$�$�M �G
@t��$�|G �|$�$�(L �~��$�cG �Ƌ�$�WG �@E���<[^_]Í�&    ��$�6G �x��$�)G �@�@�<�������$�G �@���P��U��$��F �U��D$   �T$�$��K ������$��F �D$̤4i�D$��4i�D$�4i�D$�4i�$�5K ��$�F �U�D$��4i�T$�$�$K ��    ��    U��WVS��<��A5i��$�gF �8��$�[F �PH�2���PH��$�FF ��    P)�������  ��$�$F �@�@ �7  ��$�
@t��$�E �|$�$��I �~��$�E �Ƌ�$��D �@E���<[^_]Í�    ��$��D �x��$��D �@�@�<�������$�D �@���P��U��$�D �U��D$   �T$�$�I ������$�uD �D$̤4i�D$��4i�D$D�4i�D$�4i�$��H ��$�CD �U�D$��4i�T$�$��H ��    ��    U��WVS��<��A5i��$�D �8��$��C �PH�2���PH��$��C ��    P)�������  ��$��C �@�@ �7  ��$�C �$��H �ǋ���$�C ��    �U�@���@	�]  ��$�rC �@����U��$�_C �U��D$̤4i�T$�$�H ���!  ��$�6C �@���@�@	��   ��$�C �@���@� �@�PD�4�������U��$��B p��$��B �U��D$    �|$�T$�$�\H �G
@t��$�B �|$�$�hG �~��$�B �Ƌ�$�B �@E���<[^_]Í�&    ��$�vB �x��$�iB �@�@�<�������$�QB �@���P��U��$�;B �U��D$   �T$�$�,G ������$�B �D$̤4i�D$��4i�D$p�4i�D$�4i�$�uF ��$��A �U�D$��4i�T$�$�dF ��    ��    U��WVS��<��A5i��$�A �8��$�A �PH�2���PH��$�A ��    P)�������  ��$�dA �@�@ �7  ��$�MA �$�F �ǋ���$�6A ��    �U�@���@	�]  ��$�A �@����U��$��@ �U��D$̤4i�T$�$� F ���!  ��$��@ �@���@�@	��   ��$�@ �@���@� �@�Ph�4�������U��$�@ p��$�@ �U��|$�����T$�L$�$��E �G
@t��$�[@ �|$�$�E �~��$�B@ �Ƌ�$�6@ �@E���<[^_]Í�    ��$�@ �x��$�	@ �@�@�<�������$��? �@���P��U��$��? �U��D$   �T$�$��D ������$�? �D$̤4i�D$��4i�D$��4i�D$�4i�$�D ��$�? �U�D$��4i�T$�$�D ��    ��    U��WVS��<��A5i��$�G? �8��$�;? �PH�2���PH��$�&? ��    P)�������  ��$�? �@�@ �7  ��$��> �$�=D �ǋ���$��> ��    �U�@���@	�]  ��$�> �@����U��$�> �U��D$̤4i�T$�$��C ���!  ��$�v> �@���@�@	��   ��$�Y> �@���@� �@�PX�4�������U��$�4> p��$�'> �U��|$�����T$�L$�$�sC �G
@t��$��= �|$�$�B �~��$��= �Ƌ�$��= �@E���<[^_]Í�    ��$�= �x��$�= �@�@�<�������$�= �@���P��U��$�{= �U��D$   �T$�$�lB ������$�U= �D$̤4i�D$��4i�D$Ԧ4i�D$�4i�$�A ��$�#= �U�D$��4i�T$�$�A ��    ��    U��WVS��,��A5i��$��< �0��$��< �PH�:���PH��$��< ��    P)�������   ��$�< �@�@ ��   ��$�< �$��A �Ƌ�$�z< �W���U�x��$�d< �D$P  �D$    �t$�$��A �F
@t��$�8< �t$�$��@ �w��$�< �Ƌ�$�< �M��    P���,[^_]Ë�$��; �p��$��; �@�@�4��S�����$��; �U�D$�4i�T$�$�N@ ��    U��WVS��<��A5i��$�; �8��$�; �PH�2���PH��$�v; ��    P)�������  ��$�T; �@�@ �(  ��$�=; �$�@ �ǋ���$�&; ��    �U�@���@	�N  ��$�; �@����U��$��: �U��D$Ģ4i�T$�$�@ ���  ��$��: �@���@�@	��   ��$�: �@���@� �@�P$�4�������U��$�: �U��|$�T$�$��? ��$�g: p�G
@t��$�T: �|$�$� ? �~��$�;: �Ƌ�$�/: �@E���<[^_]Ë�$�: �x��$�: �@�@�<��������$��9 �@���P��U��$��9 �U��D$   �T$�$��> �#�����$�9 �D$Ģ4i�D$��4i�D$�4i�D$�4i�$�> ��$�9 �U�D$��4i�T$�$�> �t& ��'    U��WVS��<��A5i��$�G9 �8��$�;9 �PH�2���PH��$�&9 ��    P)�������  ��$�9 �@�@ �(  ��$��8 �$�=> �ǋ���$��8 ��    �U�@���@	�N  ��$�8 �@����U��$�8 �U��D$̤4i�T$�$��= ���  ��$�v8 �@���@�@	��   ��$�Y8 �@���@� �@�P$�4�������U��$�48 �U��|$�T$�$�= ��$�8 p�G
@t��$�8 �|$�$�< �~��$��7 �Ƌ�$��7 �@E���<[^_]Ë�$��7 �x��$�7 �@�@�<��������$�7 �@���P��U��$�7 �U��D$   �T$�$�{< �#�����$�d7 �D$̤4i�D$��4i�D$,�4i�D$�4i�$��; ��$�27 �U�D$��4i�T$�$�; �t& ��'    U��WVS��,��A5i��$��6 �0��$��6 �PH�:���PH��$��6 ��    P)�������   ��$�6 �@�@ ��   ��$�6 �$��; ����4 ��U��$�6 �U��t$�T$�$��; ��$�c6 �W���U�x�F
@t��$�G6 �t$�$��: �w��$�.6 �Ƌ�$�"6 �M��    P���,[^_]Ë�$�6 �p��$��5 �@�@�4��R�����$��5 �U�D$�4i�T$�$�]: ��t& U��WVS��<��A5i��$�5 �8��$�5 �PH�2���PH��$�5 ��    P)������
@t��$�, �UԉT$�$�91 �EԉG��$�q, �Ƌ�$�e, �@Ẻ��<[^_]Ð�t& ��Uȉ$�C, �UȍM�L$�T$�$�M1 �E��q�����t& ��Uȉ$�, �Uȋ�l  �@ td�B�*�����$��+ �@���@�@ �  ���f�����$��+ �@�4���$��+ �D$   �t$�$��0 ������&    ��Uȉ$�+ �U��D$   �T$�$�1 �UȄ��m����$P�4i��/ ��$�[+ �U�D$3�4i�T$�$��/ �t& U��WVS��<��A5i��$�'+ �0��$�+ �PH�:���PH��$�+ ��    P)����F�����  ����$��* ��    �Ű@���EЋ�$��* �@�@ ��   ��$�* �$��/ �EԋEк��4i������@�    ��  %   =   �r  ��R�@�UЉE����   ��w�$�N* �@���x��  ��$�4* �@���@ �  ����t`��$�* �@���@	�|  ��$��) �@��� �@�K��$��) �P��Uȉ$��) �Uȋ@�@���E��"�����D$    �D$    �$    �d|  �U�<������$�T$�UЉT$�G|  �Ƌ�$�{) x��$�n) �Uԉt$�D$    �T$�$��. �E��@
@t��$�@) �UԉT$�$��- �EԉG��$�!) �Ƌ�$�) �@Ẻ��<[^_]Ð�t& ��Uȉ$��( �UȍM�L$�T$�$��- �E��q�����t& ��Uȉ$��( �Uȋ�l  �@ td�B�*�����$�( �@���@�@ �  ���f�����$�( �@�4���$�p( �D$   �t$�$�- ������&    ��Uȉ$�C( �U��D$   �T$�$��- �UȄ��m����$��4i�, ��$�( �U�D$}�4i�T$�$�, �t& U��WVS��,��A5i��$��' �8��$��' �PH�2���PH��$�' ��    P)������/  ����$�' �@���@	�3  ��$�w' �@���@�@	��   ��$�Z' �@���x��$�G' �|$�D$   �$�;, �ǍG�$�ƹ  �WH��t�B��th�����Bt~�G|��t�$��+ �<$��+ ��$��& �ǋ�$��& ������P���,[^_]Ë�$��& �@���@� �x낍v ��U�$�& �U�T$�$��+ �f���U�$�& �U�T$�$�+ �a�����$�e& �U�D$��4i�T$�$��* ��$�D& �D$��4i�D$Ш4i�D$ �4i�$�* �t& U��WVS��,��A5i��$�& �8��$��% �PH�2���PH��$��% ��    P)������/  ����$��% �@���@	�3  ��$�% �@���@�@	��   ��$�% �@���x��$�w% �|$�D$   �$�k* �ǍG�$���  �WH��t�B��th�����Bt~�G|��t�$�* �<$�* ��$�% �ǋ�$�% ������P���,[^_]Ë�$��$ �@���@� �x낍v ��U�$��$ �U�T$�$�* �f���U�$�$ �U�T$�$��) �a�����$�$ �U�D$��4i�T$�$�) ��$�t$ �D$��4i�D$�4i�D$ �4i�$��( �t& U��W��VS�Ã�,�5�A5i��$�3$ �C�    ��   �Ѓ���   �������t8�[�C�    �  �Ѓ��6  �J����<  ��������=  �   ��   ��v~��u8�C�   t��$�# �\$�$��( �C%� �_��D�C��,��[^_]Ë�@    뽋�$�u# �\$�$�( �C�Ѓ��A����K�A �  ���5�����M��$�;# �D$   �\$�$�( �M��X�����M��$�# ��l  �E��$��" �M��  9E�ug�S������&    ��$��" �\$�$�z' �C�Ѓ�������K�A �  ��������|$�$Ч4i�)' �|$�$ �4i�' �|$�$H�4i�	' ����'    U��WVS��L��A5i��$�W" �0��$�K" �PH�:���PH��$�6" ��    P)�������  ��w���$�" �@���@	�N  ��$��! �@�<���$��! �D$   �|$�$��& �E���~�$��! �@���@	��  ��$�! �@��� �@�E܋�~�$�! �@���@	�A  ��$�q! �@��� �@�E؋�~�$�V! �@���@	��  ��$�<! �@��� �@�Eԋ�$�$! �@�@ ��  ��$�
@t��$�� �|$�$�$ �~��$�� �Ƌ�$�� �@E���L[^_]Í�&    ��$� �@��� �@�E�������v ��$� �$��$ ��������&    ��$�f �@�<���$�V �D$   �|$�$�J$ �E�������$�0 �@�<���$�  �D$   �|$�$�$ �E������t& ��$�� �@�<���$�� �D$   �|$�$��# �E��2�����$�� �@���P��UЉ$� �U��D$   �T$�$�# �,�����$� �D$̤4i�D$��4i�D$��4i�D$�4i�$��" ��$�R �U�D$l�4i�T$�$��" �t& ��'    U��WVS��,��A5i��$� �8��$� �PH�2���PH��$�� ��    P)������  ����$�� �@���@	�#  ��$� �@���@�@	��   ��$� �@���x��$� �|$�D$   �$�{" �ǍG�$�v�  �WH��t�B��tX�����Btn�<$�." ��$�< �ǋ�$�0 ������P���,[^_]Ë�$� �@���@� �x�f���U�$�� �U�T$�$�$" �f���U�$�� �U�T$�$�" �q�����$� �U�D$��4i�T$�$�6! ��$� �D$��4i�D$Щ4i�D$ �4i�$��  �t& U��WVS��<��A5i��$�W �8��$�K �PH�2���PH��$�6 ��    P)������:  ����$� ��    �U�@���@	�n  ��$�� �@�<���$�� �D$   �|$�$��  �E���~�$� �@���@	��  ��$� �@��� �@�E܋�~�$� �@���@	�$  ��$�j �@��� �@�E؋�$�R �@�@ ��   ��$�; �x��$�. �@�@�<��E؍4������U܉D$�E��T$�$�Up  ��Uԉ$�� p��$�� �U��D$    �|$�T$�$�^  �G
@t��$� �|$�$�j �~��$� �Ƌ�$� �@E���<[^_]Ë�$� �@��� �@�E�������$�b �$� ���.����v ��$�F �@�<���$�6 �D$   �|$�$�J �E�������$� �@�<���$�  �D$   �|$�$� �E��S�����$�� �U�D$��4i�T$�$�[ �v U��WVS��<��A5i��$� �8��$� �PH�2���PH��$� ��    P)������:  ����$�a ��    �U�@���@	�n  ��$�= �@�<���$�- �D$   �|$�$�A �E���~�$�	 �@���@	��  ��$�� �@��� �@�E܋�~�$�� �@���@	�$  ��$� �@��� �@�E؋�$� �@�@ ��   ��$� �x��$�~ �@�@�<��E؍4������U܉D$�E��T$�$��s  ��Uԉ$�F p��$�9 �U��D$    �|$�T$�$� �G
@t��$� �|$�$� �~��$�� �Ƌ�$�� �@E���<[^_]Ë�$�� �@��� �@�E�������$� �$� ���.����v ��$� �@�<���$� �D$   �|$�$� �E�������$�` �@�<���$�P �D$   �|$�$�d �E��S�����$�* �U�D$�4i�T$�$� �v U��WVS��L��A5i��$�� � �Eԋ�$�� �PH�2���PH��$�� ��    P�E�)�������  �Eԃ��E̋�$� �@�@ ��   ��$� �$�� �Eȋ�$�y �@�|��G	��   ���p�}Љu����$�M ��vp�M��$��4i�L$� �Ƌ�$�) �t$�$�- �Uԉ��$� �Uԉ��L[^_]Ð��$�� �x��$�� �@�@���E��U����$�T�4i���$�� �U��D$   �T$�|$�$�� �u�E��F������4i�   �u���������������8��1�����$�o �@+Ẽ��#  �Ű���Uĉ$�M �M�H  �A��$�8 �Mȉt$�|$�L$�$� �EȋU��@
@t��$� �MȉL$�$� �UċMȍB�EԉJ������U��B<S�:  <T��������4i�   �u��f��uпp�4i�   �����1�1�8��&����R����M��A<S�{  <T�;����`�4i�   �  �t& �U��B<T��  <U�����A�4i�   �u��   ��&    �uп$�4i�
   ������    �u��   ����1�8��������������$�6 �U��B<U��  <X��  <F������s�4i�	   ���������M��$ܫ4i�L$�����M��A<R�1  <S������j�4i�   �u��   ����1�8��<����h�����$� �U��B��D<�L������$��4i�0�4i�   �u�������*�4i�   �u��   ����1�8�����������%�4i�   �u�������$�3 �M��D$   �L$�L$�$� �E������οP�4i�   ��������������8��q���������t& �ֿ2�4i�   ��������������8��A����m�����t& �οa�4i�   ������ֿ��4i�   �[����ֿ��4i�   ��������������8�����������v �ֿ}�4i�	   �����ο��4i�
   �   ����1�8������������uпZ�4i�   �   ����1�8�����������S�4i�   �Y����uпL�4i�   �   ����1�8��R����~����E�4i�   �W����>�4i�   �:����7�4i�   ������uп�4i�   ��<�����$�z �@+Ẽ���   ��ủ$�^ �M̃�H  �A��$�F �U��D$   �D$�4i�T$�$� �M��A
@t��$� �UȉT$�$� �EȍN�MԉF������uп
�4i�   ��������������8��T����������4i�   �Y�����4i�   �J�����4i�   �u�������֪4i�   ������$� �U��D$   �T$�T$�$�\ �E�������uп��4i�	   �   ����1�8������������v ���4i�
   �u��k����uпʪ4i�   ��������������8��~���������$�� �U�D$"�4i�T$�$�i �U��WVS��L��A5i��$� �8��$� �PH�2���PH��$� �M��R�UЍ�    P��)������g  ������}��$�^ �@���@	�Q  ��$�D �@�<���$�4 �D$   �|$�$�( �Eԋ�~�$� �@���@	�V  ��$��
 �U��T$�U��$�T$�5 �M���I "  �$�
 �@)�����   �E�����}���������}��Ġ4i�E�    �����������$�d
 �U��D$   �T$�T$�$�A �E��������4$�  뭍�    �M̋A%   =   ��   �A�D$�EȉT$�$��  ��������E��4$�� �E����^�����&    �D$    �D$    �$    �db  �F�������$�	 �t$�t$�D$   �$� ��������v ��$�	 ���ta�   )������4i�����$ �  �P �F|�Ġ4i�����v ��U��$�C	 �M��D$    �L$�$�L �U��
   � ���  �C�D$�$��4i� �C,�$��4i�D$� �C0�$̭4i�D$� �C4�$�4i�D$�w �C$����  �D$�$�4i�\ �C�$1�4i�D$�I �{��t^�$K�4i�6 �s��$O�4i�D$�  �F�$O�4i�D$� �F�$O�4i�D$�� �F�$O�4i�D$�� �$
   �� �C�$U�4i�D$�� �s��t^�$K�4i� �s��$O�4i�D$� �F�$O�4i�D$� �F�$O�4i�D$�t �F�$O�4i�D$�` �$
   �S�4i�[�4i�: �C�$o�4i�D$�7 �C�$��4i�D$�$ �C�$��4i�D$� �C �$��4i�D$�� �C<�$׮4i�D$�� �CD�$�4i�D$�� �CH�$�4i�D$�� �CL�$&�4i�D$� �CT�$B�4i�D$� �C�$[�4i�D$� �C�$v�4i�D$�x ��$��4i�D$�f ����$��4iDǉD$�N ����$ů4iDǉD$�6 ����$ޯ4iDǉD$� ����$��4iDǉD$� ��$�4iD��t$�� �C|�$)�4i�D$�� �$
   �� ��[^_]Ð�$�4i� �[����${�4i� ��[^_]Ít& ��'    U��WVS��<��A5i��$�W �8��$�K �PH�2���PH��$�6 ��    P)����G����  ����$� �@���@	��  ��$�� �@����U��$�� �U��D$Ġ4i�T$�$�
 ����  ��$� �@���@�@	��   ��$� �@���P��U��$� �U��D$   �T$�$�v	 �E�1҃�t=��~�$�W �@���x��   ��$�= �@���@ �  ��1҄�uW�E��?�����$� �ǋ�$�	 ������P���<[^_]Ð�t& ��$�� �@���@� �@�E��e�����$�� �@���@%   =   t5��$� �@�<���$� �D$    �|$�$� ���U�����$�r �@���P�=����t& ��$�V �@���@�@ �  ��������$�4 �D$Ġ4i�D$��4i�D$T�4i�D$�4i�$� ��$� �U�D$D�4i�T$�$� �t& ��'    U��WVS��<��A5i��$�� �8��$� �PH�2���PH��$� ��    P)����G����  ����$�~ �@���@	��  ��$�d �@����U��$�Q �U��D$Ģ4i�T$�$�r ����  ��$�( �@���@�@	��   ��$� �@���P��U��$�� �U��D$   �T$�$�� �E�1҃�t=��~�$�� �@���x��   ��$� �@���@ �  ��1҄�uW�E�������$� �ǋ�$�y ������P���<[^_]Ð�t& ��$�V �@���@� �@�E��e�����$�6 �@���@%   =   t5��$� �@�<���$� �D$    �|$�$� ���U�����$��  �@���P�=����t& ��$��  �@���@�@ �  ��������$�  �D$Ģ4i�D$��4i�D$��4i�D$�4i�$� ��$�r  �U�D$D�4i�T$�$�� �t& ��'    U��WVS��<��A5i��$�7  �8��$�+  �PH�2���PH��$�  ��    P)����G����  ����$���  �@���@	��  ��$���  �@����U��$���  �U��D$̤4i�T$�$�� ����  ��$��  �@���@�@	��   ��$�{�  �@���P��U��$�e�  �U��D$   �T$�$�V �E�1҃�t=��~�$�7�  �@���x��   ��$��  �@���@ �  ��1҄�uW�E�������$���  �ǋ�$���  ������P���<[^_]Ð�t& ��$���  �@���@� �@�E��e�����$��  �@���@%   =   t5��$��  �@�<���$�w�  �D$    �|$�$� ���U�����$�R�  �@���P�=����t& ��$�6�  �@���@�@ �  ��������$��  �D$̤4i�D$��4i�D$��4i�D$�4i�$�t ��$���  �U�D$D�4i�T$�$�c �t& ��'    U��WVS��<��A5i��$��  �8��$��  �PH�2���PH��$��  ��    P)������  ����$�a�  ��    �U��@���@	��  ��$�=�  �@�<���$�-�  �D$Ġ4i�|$�$�Q ����  ��$��  �@���@�@	�  ��$���  �@���x��$���  �D$   �|$�$�� �xP��$��  �U�P��U؉$��  �$�� �U؉��$��  �@����U؉$�y�  �U؉}�T$�E��\$�$�� ����   �E��4i��$�G�  �@�<���$�7�  �U܉|$�T$�$� ��$��  �@���H "  ��$��  �Ƌ�$���  �@E����<[^_]Í�&    ��$���  �@���@� �@� ����v ��$��  ���t�   )����4i�E��I�����  � �$��  �E��2�����$�y�  �D$Ġ4i�D$��4i�D$�4i�D$�4i�$���  ��$�G�  �U�D$��4i�T$�$���  U��WVS��|�5�A5i��$��  �8��$��  �PH����PH��$���  ��    P)����G����i  ��$���  �S�Uȋ@�D��E̋�$��  �MȋU����M��@���@	��  ��$��  �Mȋ@����$�~�  �D$Ġ4i�\$�$��  ����  ��$�X�  �Uȋ@���@�@	�v  ��$�8�  �Uȋ@���X��$�"�  �\$�D$   �$��  �����[  �E̺a�4i�F����E̋@�    �E  %  )=   ��  �M̋�@�E�A�C��S�Uċ@�Cl    �C�E��1� �  )Љ��   �C8��t�@��  �����   ����  �C���W  ���   � �  �M�)�C|�S�C�D$   �$�n  �}���~
@��  ��$��  �]�X��$��  �$���  ���$��  �Mȋ@����$��  �}ԉ\$��4i�E��\$�$���  ����   ��$�T�  �Uȋ@�<���$�A�  �\$�|$�$��  ��$�'�  �Mȋ@���H "  ��$�
���)�9�v��ʅ�t��u���}��Uĉ]��U��Eċ]�]���ڈE����
�:9EЍ� ���B�9�u��M��
�m�uË]��C�E�ǃ�    �  �����1��S�{Pǃ�       ���   �Ű
�SpQ)Sp�+����C1ɋS�{P���   �EЋẺ��   � �@�EċCpEċE�)Eă��EĉCp������4����C닍v ��$��  �M��D$   �L$�$��  ��������$p�4i���  ��$�a�  �U��D$    �T$�$�j�  �P����E��U��8���$�D$�  �D$�U�  �E������  �S�U��������)�9�r�А�t& ���)�9�v����m������  � �$���  ���{����M������  ���L$�D$�  �$���  �U�����K�M��{�����$��  �D$Ġ4i�D$��4i�D$4�4i�D$�4i�$���  ��$�h�  �U�D$�4i�T$�$���  �U��WVS��<��A5i��$�7�  �8��$�+�  �PH�2���PH��$��  ��    P)�������  ����$���  ��    �U��@���@	�b  ��$���  �@�<���$��  �D$Ġ4i�|$�$���  ���,  ��$��  �@���@�@	�(  ��$�z�  �@���x��$�g�  �D$   �|$�$�[�  �B�U܉$�Cb  �U܅����&  ��$�,�  �U�P��U܉$��  �$�i�  �U܉��$��  �@����U܉$���  �U܉}�T$�E��\$�$�F�  ����4i��   ��U܉$��  �@�<���$��  �U܉|$�T$�$��  ��$��  �@���H "  ��$�x�  �Ƌ�$�l�  �@E����<[^_]Ë�$�R�  �@���@� �P�������$�5�  �����   �   )������4i�N�����&    �B`�
�BP    �Bt    �����Bp    �BTu3��������U��D$    �D$    �$    �B  �U܉B�����U܉M��D$    �D$    �$    �6H  �U܉B�M����  � �$��  ��������$�k�  �D$Ġ4i�D$��4i�D$��4i�D$�4i�$���  ��$�9�  �U�D$��4i�T$�$��  f�U��WVS��<��A5i��$��  �8��$���  �PH�2���PH��$���  ��    P)�������  ����$���  ��    �U��@���@	�b  ��$��  �@�<���$��  �D$Ģ4i�|$�$��  ���,  ��$�g�  �@���@�@	�(  ��$�J�  �@���x��$�7�  �D$   �|$�$�+�  �B�U܉$�_  �U܅����&  ��$���  �U�P��U܉$���  �$�9�  �U܉��$���  �@����U܉$��  �U܉}�T$�E��\$�$��  ����4i��   ��U܉$��  �@�<���$�|�  �U܉|$�T$�$���  ��$�_�  �@���H "  ��$�H�  �Ƌ�$�<�  �@E����<[^_]Ë�$�"�  �@���@� �P�������$��  �����   �   )������4i�N�����&    �B`�
�BP    �Bt    �����Bp    �BTu3��������U��D$    �D$    �$    �S?  �U܉B�����U܉M��D$    �D$    �$    �E  �U܉B�M���^�  � �$�\�  ��������$�;�  �D$Ģ4i�D$��4i�D$��4i�D$�4i�$��  ��$�	�  �U�D$��4i�T$�$��  f�U��WVS��L��A5i��$���  �0��$���  �PH�:���PH��$��  ��    P)�������  ��w�$��  �@�D��E���$�}�  ��    �U܋@���@	�K  ��$�Y�  �@�<���$�I�  �D$Ģ4i�|$�$�m�  ���  ��$�#�  �@���@�@	��  ��$��  �@���x��$���  �|$�D$   �$���  �ǋE�,�4i� ����@�    ��  %   =   �p  �B�G��@�G    �G    �G�G�U؉$��~  �O�ɉGP�U؉E���Ht/�B�%   =   ��  �B�U؉L$�|$�$�}�  �U؋�J�@� �B
@�p  ��$�"�  �}�x��$��  �$�b�  ���$���  �@�<���$���  �|$��4i�E��\$�$�F�  �U�����   ��$���  �@����U؉$��  �U؉|$�T$�$��  ��$��  �@���H "  ��$�z�  �Ƌ�$�n�  �@E܉��L[^_]Ë�$�T�  �@���@� �x�d������U؉$�3�  �U��D$    �T$�$�<�  �U��g����t& ��$��  �}����   �   +}������4i������    ��U؉$���  �U؋�l  �@ t[�B�������U؉$��  �U؉T$�$�X�  �o�����MԉU؉$��  �U��D$    �T$�$��  �MԋU�������U؉$�\�  �U��D$   �T$�$���  �U؄��v����$h�4i��  �1�  � �$�/�  ���M�����$��  �D$Ģ4i�D$��4i�D$8�4i�D$�4i�$�n�  ��$���  �U�D$%�4i�T$�$�]�  ��t& U��WVS��l�5�A5i��$��  �8��$��  �PH����PH��$��  ��    P)����G����&
  ��$�a�  �S�U��@�D��EЋ�$�G�  �@�D��Eȋ�$�3�  �M��U����M��@���@	�{	  ��$�
@�T  ��t`�MЋ�P�A+U�%   =   ��  ��U��$�o�  �M��D$    �L$�$�x�  �U�E��T$�D$�C�$��;  �C��ta�UЋMЋ�P�A+U�%   =   �5  ��U��$��  �M��D$    �L$�$��  �U�E��T$�D$�C�$�5  �C��c  ��$���  �]�X��$��  �$��  ���$��  �M��@����$��  �}ԉ\$��4i�E��\$�$���  ���  ��$�^�  �U��@�<���$�K�  �\$�|$�$��  ��$�1�  �M��@���H "  ��$��  �Ë�$��  �@E����l[^_]Ë�$���  �M��D$    �L$�$���  �Q�����t& ��U��$���  �D$    �|$�$���  �U��h�����&    ���������	҉{P�a  ���������C�K�t�����@�    � �����$�X�  ��l  �@ �)  �M��A%  )=   ������U���׋@�E�B�������$��  �U��@���X��$���  �\$�D$   �$���  �����D�����$���  �}����@�����&�����$��  �@���@	��  ��$��  �@��� ��������@����������������$�n�  ������M��S��҉Pt)�A�[%   =   �V  �A�T$�\$�$�i�  �U���ыR�@� �A
@�J�����$��  �U��T$�$��  �,����C�E�    ������$���  ����$  �   )������4i�Y������$��  ��l  �@ �E��E��@ �������$��  ��l  �@ �{�����$�w�  �U��D$   �T$�$� �  ���R����$��4i���  ��$�B�  �UЉT$�$���  ������������t& ��$��  �U��D$    �D$   �T$�$��  �M��A
@�J���뜋A�����A������A�M�����$���  �@���@	�>�����$��  �@���@	�$�����$��  �@�<���$��  �D$   �|$�$�n�  �������$�_�  �}��M̉D�D$�EЉ$�D$��  �UЋ�@�k����v �ǉЃ���	�������$��  �}��D$   �|$�$��  ����   �G������U��$���  �M��D$    �L$�$���  �U������CP    1�1�������  � �$��  ���4�����$��  �D$Ģ4i�D$��4i�D$��4i�D$�4i�$���  ������������$��4i���  �$`�4i���  ��$�;�  �U�D$��4i�T$�$��  �S������    ��    U��WVS��L��A5i��$���  �0��$���  �PH�:���PH��$���  ��    P)������n  ��w���$��  �@���@	��  ��$��  �@�<���$��  �|$�D$   �$�x�  �ǋ�$�d�  �V�@���@	�  ��UЉ$�D�  �UЋ@��� �@�E܋�$�)�  �V�@���@	��  ��UЉ$�	�  �UЋ@��� �@�E؋�$���  �V�@���@	�  ��UЉ$���  �UЋ@��� �@�Eԋ�$��  ��    �U��@���@	�  ��$��  �@����UЉ$�|�  �U��D$̤4i�T$�$��  ����  ��$�S�  �@���@�@	�F  ��$�6�  �@���@� �P��   t�E܉BX��   t�E؉Bh��t�EԉBD�BV�B�Bh�B    �B   �D$�BX�UЉD$�B�$��  �UЉ�1���u�J�����BU��$��  �U�P��UЉ$��  �$���  �UЉ��$��  �@����UЉ$�t�  �UЉ}�T$�E��\$�$���  ����4i��  ��UЉ$�A�  �@�<���$�1�  �UЉ|$�T$�$��  ��$��  �@���H "  ��$���  �Ƌ�$���  �@E����L[^_]Ð��$���  �@��� �x�X�����    ��UЉ$��  �UЋ@����UЉ$��  �U��D$   �T$�$��  �E��������UЉ$�q�  �UЋ@����UЉ$�[�  �U��D$   �T$�$�L�  �E��D�����UЉ$�/�  �UЋ@����UЉ$��  �U��D$   �T$�$�
�  �E��������$���  �@���P��UЉ$���  �U��D$   �T$�$���  ��������$��  ���t�   )������4i�J�����  � �$��  ���4�����$�x�  �D$̤4i�D$��4i�D$�4i�D$�4i�$���  ��$�F�  �U�D$��4i�T$�$���  �
@�������$���  �M��L$�$��  �������    ��$��  �����   �   )������4i�������$��  ��l  �@ �������$�t�  �M��D$   �L$�$���  ��������$��4i���  ��$�?�  �@�<���$�/�  �D$   �|$�$�#�  �E��'�����  � �$��  ���`�����$���  �U�D$K�4i�T$�$�t�  ��$���  �D$̤4i�D$��4i�D$d�4i�D$�4i�$�2�  �v ��'    U��WVS��\�5�A5i��$��  �8��$�{�  �PH����PH��$�f�  ��    P)������y  ��$�D�  �S�U؋@�|���$�-�  �@�D��E���$��  �M؋U����Mċ@���@	�I  ��$���  �M؋@����$���  �D$̤4i�\$�$��  ���  ��$��  �U؋@���@�@	�  ��$��  �M؋@���@� �X�KD��4i���M��ǡ���E̋@�    �^  %   =   �  ��$�L�  �M��D$    �L$�$�U�  �M̉C��J��K���<  ���T  �E��4i趬���@ �E��[  �u�M���@    �U���@�EȋB%   =   �|  ��$���  �M��D$    �L$�$���  �UȋM���S��Mȋ@)Ȁ{U �E܉Ct�CV��C�CU �MȃC���C�S�}��MЉU���E��D$    �$�|�  ����   �C�S����  ��uӋU���P���U��$��  �U��M��T$�L$�$�U�  �U��E�EЋB%   =   u�BEЉ{�}���C�x�����&    ��$���  �M��D$    �L$�$���  �ËS�ǋC�UԋM�MЉ�+U�)�Sp�Ű�@+E�Ct���{Pu#�U��B%� �_��D�B�+K�H�B
@�  ��$�P�  �]�X��$�@�  �$��  ���$�,�  �U؋@����$��  �}�\$��4i�E��\$�$�n�  ����   ��$���  �M؋@�<���$���  �\$�|$�$�A�  ��$��  �U؋@���H "  ��$��  �Ë�$��  �@Eĉ��\[^_]Ë�$��  �U؋@���X��$�i�  �\$�D$   �$�]�  ��������B������    �ŰB�������v ��$�&�  ����2  �   )������4i������$���  ��l  �@ ��   �M̋A�}����v �L$�D$�C�$�m&  ����C������C�D$�C�D$�C�$�f   �C������$��  ��l  �@ �������$�z�  �U��D$   �T$�$��  ���b����$h�4i���  �E�    1��������$�6�  �M��L$�$���  �����f���$��  �U��D$   �T$�$��  ���	����$ �4i�{�  ���  � �$���  ���������$���  �U�D$ڴ4i�T$�$�L�  ��$��  �D$̤4i�D$��4i�D$��4i�D$�4i�$�
�  f�U��WVS��<��A5i��$�g�  �8��$�[�  �PH�2���PH��$�F�  ��    P)�������  ����$�!�  ��    �U��@���@	�b  ��$���  �@�<���$���  �D$̤4i�|$�$��  ���,  ��$���  �@���@�@	�(  ��$��  �@���x��$��  �D$   �|$�$��  �B�U܉$�Cy  �U܅����&  ��$�\�  �U�P��U܉$�I�  �$��  �U܉��$�2�  �@����U܉$��  �U܉}�T$�E��\$�$�v�  ����4i��   ��U܉$���  �@�<���$���  �U܉|$�T$�$�A�  ��$��  �@���H "  ��$��  �Ƌ�$��  �@E����<[^_]Ë�$��  �@���@� �P�������$�e�  �����   �   )������4i�N�����&    �B`�
�BP    �Bt    �����Bp    �BTu3��������U��D$    �D$    �$    �  �U܉B�����U܉M��D$    �D$    �$    �f"  �U܉B�M����  � �$��  ��������$��  �D$̤4i�D$��4i�D$��4i�D$�4i�$���  ��$�i�  �U�D$��4i�T$�$���  f�U��WVS��\��A5i��$�7�  � �E���$�(�  �PH�:���PH��$��  ��    P�E�)�������  ��w���$���  �@���@	�X  ��$���  �@�<���$��  �D$   �|$�$��  �Eċ�~�$��  �@���@	�`  ��$��  �@��� �@�Eԋ�~�$�e�  �@���@	�  ��$�K�  �@��� �@�EЋ�~�$�0�  �@���@	��  ��$��  �@��� �@�E؋�~�$���  �@���@	�1  ��$���  �@��� �@�E̋�~�$���  �@���@	��  ��$��  �@��� �@�Eȋ�~�$��  �@���@	�!  ��$�w�  �@��� �@�E���$�_�  �@���@	��  ��$�E�  �@�<���$�5�  �D$Ġ4i�|$�$�Y�  ����  ��$��  �@���@�@	��  ��$���  �@���@� �P�U��$�   ���  �,   �ƉE܉�1�������M܋U���t~�EԋMЉFX�E؉N\�M̉F`�EȉNd�N�M��M؉Fh�D$�ẺL$�MԉU��D$8   �D$�E��D$�4i�L$�D$�E��$�d�  �U������w  �4$�'�  �E�    ��u��$�+�  �� �@)����  ��V�U��$�
�  �$�Z�  ��U��$���  �M܋U��D$̤4i�L$�T$�$�]�  �F��$���  �@�@��  ��$��  �@�@����������  ��$��  �U����\[^_]�f���$�v�  �@��� �@�E������v ��$�V�  �@�<���$�F�  �D$   �|$�$�Z�  �E��������$� �  �@�<���$��  �D$   �|$�$��  �E������t& ��$���  �@�<���$���  �D$   �|$�$���  �E�������$��  �@�<���$��  �D$   �|$�$��  �E������t& ��$�v�  �@�<���$�f�  �D$   �|$�$�Z�  �E��B�����$�@�  �@�<���$�0�  �D$   �|$�$�$�  �E�������t& ��$��  �@���p��$���  �D$   �t$�$���  ���������$���  �$�f�  ������������$��  �����T$�|$�$�Y�  �Ƌ�$��  �t$�$��  �Ƌ�$�u�  �}�t$�E��\$�$���  ����4i��  ��U��$�E�  �U��t$�T$�$��  ��N "  �$�!�  �@+E����  �E��E��0�k����t& ��$���  �t$�t$�D$   �$���  ��������&    ���   �M��D$�B|�$�D$�$o  �ǋF<���FL�U��Z����E��M��FP    �Ft    �FD�E؉�Fp    �����FT��   �E�t%�U��D$    �D$    �$    �
  �F�U��B�F�B�F�B<�F<�B �F���   ����������   �T$�   )E��T$�$�q  �������$���  �M��D$   �L$�L$�$���  �E�������$���  ���tL�   )������4i�U�����&    �U��D$    �D$    �$    �!  �F�U�������    �s�  � �$�q�  ��������$�P�  �D$Ġ4i�D$-�4i�D$4�4i�D$�4i�$��  ��$��  �U�D$�4i�T$�$��  ��&    U��WVS��\��A5i��$���  � �E؋�$���  �PH�2���PH��$���  ��    P�E�)������o  ����$��  �@���@	��  ��$��  �@�<���$�q�  �D$   �|$�$�e�  �Eȋ�~�$�M�  �@���@	��  ��$�3�  �@��� �@�E���~�$��  �@���@	�  ��$���  �@��� �@�Eԋ�~�$��  �@���@	��  ��$�ɿ  �@��� �@�E܋�~�$访  �@���@	�D  ��$蔿  �@��� �@�EЋ�~�$�y�  �@���@	��  ��$�_�  �@��� �@�E̋�~�$�D�  �@���@	�h  ��$�*�  �@��� �@�E���$��  �@�D��$�   �E����  �,   ��1��������  �E��~�MԉFX�E܉N\�MЉF`�ẺNd�Fh�L$�M܉D$�E��D$8   �D$�4i�L$�M��D$�<$�u��L$螂  �U����E���  �Mċ�H��t{�Eċ@�    �E��  %   =   ��  ��U��M��$�@�  �D$    �$�EĉD$�I�  �M��U��EԉL$�MԉU��<$�L$�zj  �U��J<���JL�Q  �E��M��BP    �Bt    �BD�E܉
�Bp    �����BT��  �E�t%�U��D$    �D$    �$    �b  �U��B��}؉$菽  �� �$���  ��U��$�u�  �U��t$�D$̤4i�T$�$���  �E܋�$�M�  �@)�����  �E܍w�G��$�,�  �@�@��  ��$��  �@�@����������  ��$��  �0��\[^_]Ë�$�߼  �@��� �@�E��r�����$�¼  �@�<���$貼  �D$   �|$�$���  �E��z�����    ��$膼  �@�<���$�v�  �D$   �|$�$�j�  �E��	�����$�P�  �@�<���$�@�  �D$   �|$�$�4�  �E������t& ��$��  �@�<���$��  �D$   �|$�$���  �E��/�����$��  �@�<���$�л  �D$   �|$�$���  �E�������t& ��$覻  �@�<���$薻  �D$   �|$�$��  �E��U�����$�p�  �$��  �������]�����$�P�  �U������T$�L$�$���  �ǋ�$�,�  �|$�$�0�  �ǋ�$��  �|$�E��$�\$�q�  �E��4i���A  ��U��$��  �U��|$�T$�$�I�  ��O "  �$���  �@)�����   ���>�����E���t& �$1��~�  ��������$膺  �|$�|$�D$   �$�f�  ���(�����&    �Eċ@�E��2���f���$�F�  �U���l  �@ ��   �Mċ�H�Eċ@�E������E������u�����U��D$    �D$    �$    �  �U��B������    ��$�ֹ  �t$�t$�D$   �$趾  ��������&    ��$覹  �}��tQ�   +U������4i������U��$�}�  �M��D$   �L$�$��  �U����(����$��4i�߽  �R�  � �$�P�  ���H�����$�/�  �U�D$t�4i�T$�$谽  ���&    U��WVS��<��A5i��$���  �0��$��  �PH�:���PH��$�ָ  ��    ��P�)����$躸  ��    �U�@����U��$蝸  �U��D$   �D$	�4i�T$�$�.�  ��$�t�  �D$   �D$�4i�|$�t$�$��  ��$�J�  �D$�4i�D$�{3i�D$�4i�$�
�  ��$� �  �D$�4i�D$�[3i�D$<�4i�$��  ��$���  �D$�4i�D$�U3i�D$`�4i�$趼  ��$�̷  �D$�4i�D$�g3i�D$��4i�$茼  ��$袷  �D$�4i�D$@d3i�D$��4i�$�b�  ��$�x�  �D$�4i�D$y3i�D$��4i�$�8�  ��$�N�  �D$�4i�D$`v3i�D$�4i�$��  ��$�$�  �D$�4i�D$��3i�D$�4i�$��  ��$���  �D$�4i�D$ �3i�D$,�4i�$躻  � �@   ��$�Ƕ  �D$�4i�D$ �3i�D$T�4i�$臻  � �@    ��$蔶  �D$�4i�D$��3i�D$��4i�$�T�  ��$�j�  �D$�4i�D$P�3i�D$��4i�$�*�  ��$�@�  �D$�4i�D$0�3i�D$��4i�$� �  ��$��  �D$�4i�D$�t3i�D$Щ4i�$�ֺ  ��$��  �D$�4i�D$��3i�D$d�4i�$謺  ��$�µ  �D$�4i�D$��3i�D$�4i�$肺  ��$蘵  �D$�4i�D$pS3i�D$Ԧ4i�$�X�  ��$�n�  �D$�4i�D$Q3i�D$��4i�$�.�  ��$�D�  �D$�4i�D$�N3i�D$p�4i�$��  ��$��  �D$�4i�D$PL3i�D$D�4i�$�ڹ  ��$��  �D$�4i�D$�I3i�D$�4i�$谹  ��$�ƴ  �D$�4i�D$�G3i�D$�4i�$膹  ��$蜴  �D$�4i�D$0E3i�D$��4i�$�\�  ��$�r�  �D$�4i�D$�B3i�D$��4i�$�2�  ��$�H�  �D$�4i�D$p@3i�D$P�4i�$��  ��$��  �D$�4i�D$>3i�D$ �4i�$�޸  ��$���  �D$�4i�D$�;3i�D$�4i�$贸  ��$�ʳ  �D$�4i�D$pY3i�D$,�4i�$芸  ��$蠳  �D$�4i�D$`p3i�D$��4i�$�`�  ��$�v�  �D$�4i�D$��3i�D$��4i�$�6�  ��$�L�  �D$�4i�D$��3i�D$��4i�$��  ��$�"�  �D$�4i�D$�3i�D$��4i�$��  ��$���  �D$�4i�D$P93i�D$��4i�$踷  ��$�β  �D$�4i�D$�63i�D$d�4i�$获  ��$褲  �D$�4i�D$�43i�D$,�4i�$�d�  ��$�z�  �D$�4i�D$�3i�D$8�4i�$�:�  ��$�P�  �D$�4i�D$�l3i�D$�4i�$��  ��$�&�  �D$�4i�D$023i�D$ �4i�$��  ��$���  �D$�4i�D$�/3i�D$ԣ4i�$輶  ��$�ұ  �D$�4i�D$p-3i�D$��4i�$蒶  ��$許  �D$�4i�D$+3i�D$t�4i�$�h�  ��$�~�  �D$�4i�D$�(3i�D$H�4i�$�>�  ��$�T�  �D$�4i�D$P&3i�D$�4i�$��  ��$�*�  �D$�4i�D$ W3i�D$�4i�$��  ��$� �  �D$�4i�D$�#3i�D$�4i�$���  ��$�ְ  �D$�4i�D$�_3i�D$��4i�$薵  ��$謰  �D$�4i�D$�j3i�D$Ш4i�$�l�  ��$肰  �D$�4i�D$`�3i�D$T�4i�$�B�  ��$�X�  �D$�4i�D$��3i�D$��4i�$��  ��$�.�  �D$�4i�D$��3i�D$4�4i�$��  ��$��  �D$�4i�D$�!3i�D$��4i�$�Ĵ  ��$�گ  �D$�4i�D$03i�D$T�4i�$蚴  ��$谯  �D$�4i�D$�3i�D$�4i�$�p�  ��$膯  �D$�4i�D$p3i�D$�4i�$�F�  ��$�\�  �D$�4i�D$ 3i�D$��4i�$��  ��$�2�  �D$�4i�D$�3i�D$h�4i�$��  ��$��  �D$�4i�D$]3i�D$\�4i�$�ȳ  ��$�ޮ  �D$�4i�D$��3i�D$4�4i�$螳  ��$贮  �D$�4i�D$�3i�D$�4i�$�t�  ��$芮  �D$�4i�D$@3i�D$8�4i�$�J�  ��$�`�  �D$�4i�D$�3i�D$�4i�$� �  蛬  �81��   ��$�(�  �D$   �D$��4i�$踲  �Ƌ�$��  �D$   �D$    �t$�$�P�  ��$�ޭ  ���  ��t7��$�ʭ  ���  ��$躭  �p��$譭  �|$�t$�$��  ��$蓭  �u�p��$胭  x  ���$�r�  �Ƌ�$�f�  �@E���<[^_]��$x�4i�ڱ  ����������U��MWV�uS�ˁ���  ���}�K  ���   tI�}wJ�E��t�}1������9�u����  �؍� ���q��B�����i���  )���	߉�[^_]Á}�  �   �m�  ��[  �8��x���x���x���x���x���x���x���x���x	���x
���x���x���x
���H���H���H
˃��o����E��������tt�P�1��z�����9�u���  ��1����؉�1�����������U���ʍ�    U����$�]�t$�q���|$�������i���  )ˉ��]���������E�u����  �E��i���  ��������  )�)΁���  �>t����  ����  v����  �� ������ �t$CЋ|$����  �� ��B���	؋$��]Ít& ��'    U����$�]�t$�q���|$�������i���  )ˉ��]���������E�u����  �E��i���  ��������  )�)΁���  �>t����  ����  v����  �� ������ �t$CЋ|$����  �� ��B���	؋$��]Ð����������U��WVS��  �ɉ�������������������  ǅ���� ����   �   ����������҃� u�1���t�����1���t������t32��u�4����� u�f1��v ��1���t����v ���t32��u�4����� u�1���    ��1���t����v ���t32��u�4����� u�������t3��������t)��������1�ǅ����    ����t32��u�������������������tw1���1���t������t32��u�4����� u�������t3��������t)��������1�ǅ����    ����t32��u��������������������
�M��J�r��9���  ��u��u�M�L$�4$�U��tj�UԋMԋRL�IT�UЉM�덉ك��������|  ���'��    �u�ك�������u���׃��O  ��uۋE�u�t$�$�U��uǉ�1Ҹ����������
��������  �EȉA\�v  �}��l  �M�1ҋEȉ}��Ah    �ωEĉ���w0����   ��    �u�ك�������u�w\��UЋWh�u��uЃ�����4i����f�tOp�m�;UĉWhr����Ƌ}�w �M�����4i����f�DAp  v�EԉPh�UԋM��E��BT   �Jl�JL��p�M��U��U��D$�E��D$   �L$�T$�D$�$    �s  ���.  �U�   �M��Bt�4i�   �n����M�U�T$�$�U������������v �Ɖ���������  5��  9��=  �E�U��@3�4i�   �   ������&    �E�U��@P�4i�   �   ������v �ȉك�����    ��   ������t& �U�E��M̉r�B�E�J��UԋB(9B,�z8�Z<s
�щ�+U��Q,�u�D$�4$�i  �F�V�M�E̋�U��q�E�Eԋx8�X<� �f���f�u��ƉȀ}� �'  �MĉM����   �z  �M��E�   �M��ȉM�M��eċMă��M�!��Mĉ��mċEċM�E�����M��H�M��@f�E����M��9���  ���U��N�u�ك�������u�u����M�׋U�!���U����u��
�M��J�Rf�U����9��s  ��u��E�u�t$�$�U��������UԋRL�U�닉ƋUԉ�����B�������  ����  ����  �
�]��u���]Ã}8t�]�������u���]À:1u�۰�t֋K �C    ��tg�S$��tp�D$�  �D$   �C(�$�S �Ƹ������t��E�s�F4    �$�D$������t��t$�S(�E�$�S$�C    �E��a����C p�4i�C(    ��C$��4i뇍�    ��'    U��������(�U�]��]�u���u
�]��u���]Ã}8t�]�������u���]À:1u�۰�t֋C �C    ��tl�s$��tu�D$�  �D$   �C(�$�S �Ƹ������t��s�F4    �D$   �$�8������{����t$�S(�E�$�S$�C    �E��\����C p�4i�C(    ��C$��4i낐��&    U���������U�$�M�t$��u�$�t$��]ËR��t��y�B8    1��B<    �$�t$��]Ív ��ǋr<�1�� w��   �����#E�Z<��B81�롐�t& U����   �u��u�]�������}���u�؋u��]�}���]Ív �F���E�t�V�҉U�tڋ���e  �Mԋ����  �N�~�]ԉu��։M��Mԉ}��[8�y<�M��]̋]�M��Mԃ��]��]���0  �M��Mԉ]�1ۉ]��]Ё��  �M��Mԃ�l�M��Mԃ�T�M��Mԃ�p�M���v������C�����t& �$�0�4i�EԋP����  ��w���)  �����������E̋E̍O��Uԃ��B�U���Ѓ�����  ���t  ���%  �}��
f�����  ����������v�ŰE̋Ű}�������E���% �  E��E�    % �  ����U��G�B0�
   1��MԋA���H  �D$    �D$    �$    �q����UԋM��B�A0�   �}�������]Љ�]��u��E��M���ỦF�EЉN�MԉF�y<�y(�Q8��u
�}������۸����D��������������m̃�w2��������Ủ��	�����  ����������v�ỦϋE��U���5��  9���  �Mԃ}�Q@�   ��  1��E�    �E��    �UԋB@���  9E���FE�9�FЅ��V����M��T$�t$��p����$��  ��p����E�)U�U�)��)P@�W����MԋQH��t]9�s4���������׋U�����  ����������9�r�Ủ��ωѸ   ���M�)׃�#E�AD�ыE��m���  �U��   �E���������MԋU�+U��AD9��y  )�;A,�X  ���   �K  �E��@��4i�   �����EԋPH��t]9�s4���P������׋U�����U  ����������9�r�Ủ��ωѸ   ���M�)׃�#E�A@�ыE��m���  �UԋB@�   ���  �MԋEԋIP�MЋHX�   �U������E��E�#E�����M��P�@f�E���9ǉE�sS����������
f����J  ������U�����E̋E�#E̍���U��x�@��f�E���9�w��E����EȨ���   �MĉM��ʉM���   ���ыUЃ��E�#E���E�����M��P�@f�E���E�9�s\��������U������  ������������M�E�!���M�E�����M��H�@f�E���E�9�w��ỦʋE��M��m̋M���  +}��ыU��m�)���  �U���@��  �Mԃ��EĉQH�AD�   �9����Uԃ:�~���f��p   �Q  �EԋU��M��@T	   �Pl�PL�E��L$�U��M��$   �D$�E��T$�L$�D$�nW  ���E���  �UԋM��Bl�BX   �BP�Ѓ�X�D$�E��L$�D$�Bd�D$�B`�$   �DBp�D$�W  ���E���  �Eԃ}�    ������U��   ���  �UԸ   �JLǂ�      �MċJT�U������E��E�#E�����M��P�@f�E���9ǉE�sX���&��������&    ����  ������U�����E̋E�#E̍���U��x�@��f�E���9�w��E��π}� ��   �EШ���   �MȉM��ʉM���   ���ыUă��E�#E���E�����M��P�@f�E���E�9�s\���p����U�����2  ������������M�E�!���M�E�����M��H�@f�E���E�9�w��Ủ��M��m̋EԋM�+}����  �ыU�)��m���  �EȀ}� �B@�  �U��� �   ��@�  �Mԃ��QH�   �X����MԋA����   ��w1��������Ủ������
  ����������v�ỦϋU��E��M�+E�BA��t.�y �O  �U��D$)T$�A�$�q����MԋU��A�B0�MԋŨy u)�É� �  �M���������E���% �  ���E�;P�l  �}��E�    �}�1��E��    �UԋJ���%  �B���  ��w1��������Ủ������	  ����������v�ỦϋEԋM�9H�;  �U��B�4i�    ������
�����}�  �A\��  ����  �M��Ah    �   �UԋMԋBh�I\9��M�v\���Eă�w���X	  ���������E̋}ă��E̋U���?��4i��f�Dzp�EЃE��}��m�9ǉzhr��UĉωЃ���w!����4i���Mԃ�f�DAp  u��Ah   �EԋU��M��@T   �Pl�PL�E��L$�U��M��D$   �D$�T$�L$�$    �aR  ���E��J  �M��Ah    �   �EԋUԋ@`�щE�Bd�E��Ih9��M��Z����UԋEԋRL�HT�ẺUȺ   ��у�!ȉMċMȍ��P�@f�E���9ǉE�sW���h����Ủ����    ���i  ������}�����E�!Ѝ��x�@f�EЉ���9ȉE�wŉỦ���f�}���	  f�}��
�P��E���d���+E���t�����E���E�tC�E��MċU���	���x���E��M�1ɍB�Uȉ���;M�r�M�)�d���M�9�t���t"�U��MԍDQp��d����Mă�f������u�Eԋ�|������Ph�E�������]��   �����������w1��������Ủ��
f�����  ����������v�ŰMԋA ��t	�ŰMԉP�A�B	  �   1��E�    ���'����Ủ��
�@0�����Q����  �E̹   �U�����%��  �1������  �Ẽ����s  �M��E��Aw�4i�    �����������������U���������MԋU��A@����U��m��   �����MԋQ���D  ���s���1ɉ]ȉu��}ċ]��}�����G ����u�t�x��t�uԋ^@;X s	����^@;M�s�UЅ�u��Eԋ]ȋu��}��@��  �E�)�΅������MԋQ�E��@@    �    ���}  �������1ɉ]ȉu��}Đ�t& �]��}�����G ����u�t�x$��t�uԋ^@;X(s	����^@;M�s�]Ѕ�u��Eԋ]ȋu��}��@��  �U�)�΅��e����MԋQ�E��    ���N  ��w5���@������׋U�����E  ����������v�Ủ��ϋM��A;E���  �E��@��4i�   �#����]Љ�   �u�������]Љ������u�������MԋQ����   �MԋI@9ˉM�F˅�tx�Eԋ@ ���E�tX�@���E�tN�UЋB�R+Eȉ�p����t$�U���UЋU�)UȋU�9UЋU�F�EĉT$�$�w  �Eԋ�p����P����  �E�)��)H@�UԋJ@���#����R�E��@@    �    �����EԋP�@����EԋP�	����MԋQ�����]Љϋ]��Ủ�u�������   �^���]��������1������Mԉ��|$��F�D$�A��p����$�������p����2������]���1��[����~���`���f������MԋA ��������@$    �Q������]Љ�]��ϋu��B����MԋA ���-����@    �Q�����U�)A@��;E�GE�)E�)��EЋEԉH@�M����M����ɉM��S  �E��J�}�	�E�	Ш�� E��E���9���9M���	ȄE��  �]��]�1��u��uč�&    ������9�r�E�+EȋM�MȉEċE�U�9EЋ]��u�t�u��u�1��]�����9�u�]ȋu��UԋE�E��J@���p����   �e����MԋQ09���   )ЋQ()�Q4�MԋI@9�G������1��E�    �MԋA ��t��	���P,�A �@0   �D$    �D$    �$    ������UԋM��B�A0�   ������MԋA ���P����@    �Q�A����L$�t$�@��p����$�����UԉB��p���������L$�t$�@��p����$�����UԉB��p����-����M�)�Q4�����]Ћ]��Ủ�u��)���;}�s1���������}�����O  ����������;M�r�}̉ωыE��m̋U��M�+}�f�LBp�E��Mԃ��Bh�E�� ����Eԃ}�@L��4i�@T	   �@P �4i�@X   �    ������]Љ�]��u����m������E��}��@��4i�   �����}��   �x����E��ŰM��D$   ���E�E�U�D$�A�$�Y����}ԋW�G���E����E��U��@`�4i�   �1����E��D$   �E�E����E�E����E�E����E�E�D$�A�$������UԉщB�u����]Љ�]��}̋u����������a����D$    �D$    �$    �����MԍU��E��E勉A�D$   �T$�$�����}��E�    �   �G1��o����E�1��U��D$   �E�E����E�E�D$�B�$�@����M��E�    �Q�A�����ҍB9ǉE�s ������������������E�;}�r���+}��m̃}� �8  �Eԃ��M��LHn�E��m��M�����������Uԉ�������m��   �����UԉL$�t$�B��p����$�����UԉB��p��������U��   �u����E��U��@�4i�   �]����M��Aw�4i�   �H����U��   �:����M��UԉD$)��L$�B�$�?��������M��E��A��4i�    �����   ������E��U��@��4i�   ������ҍB9ǉE�s ������������������E�;}�r��Ẻ�+}��E�    ����������U�������M��E��AA�4i�    �s�����u�1��]��E�    �MԻ   �   �%����U��M��B��4i�   �7����U��M��B��4i�   �����M��A\�4i�   �
����M�ǁ�  �����   ������]Љ�]��u�1��E�    ������u��]��j����E��U��@(�4i�   �����E��U��@A�4i�   �����}�  ������M��E��U��Y�]ԉA�Q�1�M̉{<�u��}��K8�t$�<$�:  �G�MԋW�7�E��EԋI8�_�U��8�M̋x<�4���ǀ�  �����%�����u��]Ћ]��E���UЉ^�]̉F�V�Y8�   �y<�����E��U��@��4i�   ������E��U��@��4i�   ������MЉMċM������U��M��B�4i�M��    �����U��m��M̋B$������u[�J$�UԸ   ���B�D$    �D$    �$    �z����Mԋ}��A�G0�E��E�    %   ���1�����	��+���9�v��M����E��A��4i�    ������    ��    U�������S���]��u
ts�S���3�������   �F(9�v4�V4)�}�D$�|$�$�l  �F(�F,�F   1��]�u��}���]ËU)�F4�|$�T$�$�l  �~,�̓>
�h����D$    �D$    �$    �����U�|$�T$�$�۾���¸����9V�)����I����   �����������    U�������U�M��u]ËR��t��B�t��J 1��A0    ]�U��������$�U�]�u��}���u
J�J�{h������B�r����   ��0  �@    �B    �B    �B    �B0   �     �@    �@    �@ �  �@     �@(    �@,    �@0    �@8    �@<    �xl�xP�xLǀ�     ǀ�  �����J1��r�   ������Ch���������f�1�� �������'    U�������U��u]ËR��t�1��:
����t
�]��u���]Ës��t�K ��t�S$��t��C    �C    �C    �C,   �F�F    �F�F�� ��   ��   �Fq   �D$    �D$    �$    �[����C0�F(    �4$�	A  �F,�VL��F<�FDf�DP�  �FL�T ��FD�T$�D$    �$�Q  ���   �Fl    �F\    �Ft    �@���� �4i�� �4i�H�Fx   �F`   ���   �P�@���   �Fh    ���   �F|1��FH    ������؉F���F*   �����D$    �D$    �$    �S���������&    ��'    U�������U��u]ËR��t��zu�E�B1�]Ð��&    U�������US�M��u[]ËZ��t��   ���  ���R��f#Ef���  1�[]�U�������U��u]ËR��t��E���   �E���   �E���   �E�B|1�]Ív U��WVS���U�M�r�����\�B?���ɍ�E���   �I����   �A����   ��t����Ѓ��y0t|E���[^_]ËA�E�]�   ��t݋]�{��t�C���}�_��t���������u���}�_$��t�א�������u���}�,�X��EÃy0������yP�z����у����41���������[^_]Ãyl������
�C����E���[^_��]�U��������X�]�]�}��}�u���u�]�u��}���]Ð�t& ���s~�]�������u��}���]Å�t��x̋S����  ����m  ���F�E���  �K����  �V(��*��~(�U���  ��E��  ��I��  ��[�?  ��g�  �N���  ;}��C��u
�}� ��  �V���  �z  ��ta���   ���h  ���g  ���   �|$�4$�@���4i�P����U  ��t����   �{1���������F(����������Ft��u�1�����������  u�1��}� ������V������������  �S0�F�~������V���L�P�~�V�S0����  �����L�N�T���F�������F��~�؉F1��~ ���0������q������S
  �F01������   �� �����  	ыVl�ȃ� �Fq   �Һ�BEȉ���ȋN)���������)F���U�����U܋N�T�P�V�Vl��tO�K0�����U�ʋN���T�V�M�L�P�N�V�S0����  �U����T�N�U܈T���F�D$    �D$    �$    �W����C0�F�R������5����S��������C�}����V�J�UЅ���  �N �F�M��J�E�M؋M�9M���   �}؉ы}��)�I�V�9��~ ���V�F���~ �J9�v~��9FuҋR,��t	;E���  �C�x9{F{��u�~;~�}���  ���N�~ 똋S�@�|$�$�D$��J  �C{x�C{){)x�C�x��u��P�P몋}؋J,��t	9E���
  �B9F ��  �F�;�����������<�4i�C����������F�  �����V�B�х��  �V�U�Љ��#�N �R�
���N �N����҉Ft?�V9Fu؋R,��t	9E��(  �C�P9SFS��u;�N;N�M��;
  �V���1ɋV�R,��t	9E���  ���-  �F�~����K�@�T$�Uȉ$�D$�I  �C�U�SP�CS)S)P�C�H��u��P�P�� �4i�]؉}܉U�Ft�y�F`    �Vl����   �N8����  ���  f�J  ���  ���  �����f����   1ɋ��  ���  ��9Ft�Vl�������Ft�Vl���  =  w���������M܋Ft����  ����  ���F`    �Z����N8�Vl�]����N8�\��]��;�}��}��8_�>����9}��1����]Ћ}��[��9]�������  �]Љu��w�_9u���  �w�_9u���  �w�_9u���  �w�_9u���  �w�_9u���  �w�_9u��w  �w�_9u��g  �w�_9u��W  ;]��N  ���p����F\1�)�x�N8��T$�D$    �L$�4$��;  �Fl��F\�S�C�z9�F���u�������]؋}������R�C�|$�T$�$�|G  �C{x�C{){)x�C�P��u�P�P��@�+E�D$�E�F�D$�C0�$褟���C0�����{ ������0�4i�C������.����V�B$���   �F�E��#�N �R$�
���N �N����҉Ft?�V9Fu؋R,��t	9E��+  �C�P9SFS��u;�V;V�U���  �ЋV�1ɋV�R,��t	9E��K  ����   �F�'����K�@�T$�Uȉ$�D$�]F  �C�U�SP�CS)S)P�C�H��u��P�P낋V�B,��t`�F�H;N�M��A  �K0�V��H�V�N�K0���L�M܉N�D$    �D$    �$    �H����C0�Fq   �����Fq   �w����Fg   �V��F     �V�F[   �����]�}��Ft�j�V8�Fl���  �F`    ����  f�J  ���  ���  ������  f����   ���Ft�Nl�����  ����9ډFt�Nl��   ��u��������^t��u��}�1��]���!���1����V\��1ɉE�Fl)Ѕ�x�N8ыU�D$�L$�4$�T$��8  ��Fl�F\�J�M��I9JFJ���  �z ��  1������������    �F\1�)���x�V8T$�D$    �L$�4$�8  �Fl��F\�S�C�z9�F���u��������]�}��X����C�R�|$�$�T$�%D  �C{x�C{){)x�C�x u�P�P��@�+E�D$�E�F�D$�C0�$�N����C0�������   ���9������@�.���������   E������F     �FI   �$���+E�D$�E�F�D$�C0�$�����C0�����}؋V�E�����+E�D$�E�F�D$�C0�Mĉ$趛���C0�M��U���+E�D$�E�F�D$�C0�Mĉ$茛���C0�M������]؋}�����=  �����]�1��}������FI   �o����Fg   �������O����F�P;V�U��w�������1������D ������B�E�E��@�L$�UȉMĉD$�E�$�B  �UȋMċBJH�BJ)J)H�B�x u�P�P������F[   �8����D$    �D$    �$    虚���C0�N�V�M��
�V�D
��V�D
�V���M�N���A  �E�F�: �E����z,��у��z���у��z���у���z$��҃���M܈�UЋN�E���V�V�R�T�UЋN���V�V�R���T�UЋN���V�V�R�T�UЋN���V�V�R�T�EЋV���   ���	�U�   �Ft���   �   ��  �M��P�N�V�V�R�T�P�V�V�z t)�R�N�T�P�N�V�V�R���T�V���F�z, t�F�D$�F�D$�C0�$�����C0�V�F     �FE   ������F�K0�V��P�N�V�S0���T�P�V�V�K2�L�P�V�V�K3�L�P�V�V�K�L�P�N�V�S���T�P�V�V�K
�L�P�V�V�K�L���F�����+E�D$�E�F�D$�C0�$�R����C0�V�'���=�  �C��������E�   ������E�   �����4$�0  ������߻  �u�+]Ѝ9É^`��   ���������  ��1ɋ��  f�P ���  ���  ��ۋ}��� �4i�����  f����	  f����  ���  ��9ЋV`�Ft���F`    )�Vl�Ft�Vl������F`��뀋VL�FDf�DP�  �FL�T ��FD�T$�D$    �$��>  �~t ������Fl    �F\    ������V�E��D �V�D �V�D �V�D �V�D �EЋV���   ���	�U�   �Ft���   �   	�������M��V�D���F�Fq   ������������
�]��u���]ËS��t�r��Et��*��   ��    �B��t�D$�C(�$�S$�S�BD��t�D$�C(�$�S$�S�B@��t�D$�C(�$�S$�S�B8��t�D$�C(�$�S$�S�T$�C(�$�S$1���q���C    �D@��]��u���]Ã�[�q�����I�h�����q�_�����g�V������  �"����E�����t& U���H�U�E�]�M�u��]�U��U �E�E�}��u$�}�҉Eܸ����u
��t\����   ��@�Y  ���f�����   ��@�?  �   ����у�!��<9�M����J�z�����
)ޅ�u���E����M��M�9M�s�u�9u��@����u�ډ����}�])���)Ѻ   ���!F��E���G�E؋}��)��u܉C��  +E�G�E�P8�H<�Ĉ   [^_]É�������}�t,9�v�E��ً}�����Љ�   ��)��!�U�����\  �U��u�!��J�z��)��
��uG��@�x  ���	��@�i  �   ����M���!<:���J�z�����
)���tȉ������9މ}�v)�E��ً}������9�v���ى}������Љ�   ��)�z��U�!�����U��U�+U�9U���  �M�)�9M��M�s�u싖�  ���	  �űUȅ��1  �U��M�+U�U�9M���   )MЋu��M�+M�)΋M�+u�)�u����M����~�}��}�M�������  �M��}�	�E�	����� M��N9ϋM��E���9M���M��M��M���  �]��}�1ɋ]�E��D��D���9�r�u�}��M�u�+}�U�9M��E��]�t1ɉE��D
�D��9�u��E��u�u�U�+U��}�v1�M�ǋu��B���A�B���A������w߉M���uЋMЅ��)����E��J�}��R��}��������}��Q��������� t8�u��   ������u���V���u������K������~����t& �}�U��G��4i�   ������u�}��F��4i�   �����U�ǋM�u�+U��B���A�B���A������w߅����M�u��N������M�J�u��R��}��2������u��Q��!����M�9M���  �u�u���+M�MȉM��M�+M�9MЉM���  )MЋM�+M�)�+u���|���)�učN�M��M���M��M����M����ɉM��|  �}�	�M��E�M����� M��N9M��M���{�����9M����{�����{����M��6  1ɉE��U��D��U�D���;M�r�M�+M��u�u��M��M�M�9M��E��U�t"�]��]�1ɉE��D�D��9M�u�E��]��M�u�M�9ủM���  �M�)΋�|���+M̉uЋu�M��΃}� ��  �Mă}�	�E�	���� M��Mă�9��E��N9M���M��M��M��Y  �}�1ҋ�����;U�r�u��}̋M�u�+}�9M̋U�t1ɉE��D
�D��9�u��E�U�ỦU�+U�������U�+U�U�9M������)MЋűM�+M�)΋M�+u�)�u����M����~�}��}�M�������   �M��}�	�E�	����� M��N9ϋ}��E���9}���M��M��M���   �}�1ɉE��U��D��U�D���9�r�U��u�}��M�u�+}�U�9M��E��
�D��9�u�������U�������}��}�������}�G��4i�   �V����}̋u�������u��u��u�������}��u������}��u�뒐������U��WVS��   �M�E�    �E�    �E�    ���E�    �E�    �E�    �E�    �E�    t�]1��C��f�DU�9�w�E�}� f���E���   f�}� ��  f�}� ��  f�}� ��  f�}� ��  f�}� ��  f�}� ��  f�}� �"  f�}� �  f�}� �9  f�}� �  f�}� �  f�}� �J  f�}� �0  f�}� �  �U��P� @�@f�@  �@@���B�Mf�B  ��]1��   �Đ   [^_]�ǅ|���   ��t& f�}� ��  �   ��|����f�|U� �c  ��9�r��uֻ   �����)�x��4�]ظ����)�x��]ڍ46)�x��]܍46)�x��]ލ46)��t����]��46)��e����]��)��W����]��)��I����]��)��;����]��)��-����]��)������]��)������]��)������]��)���������)ރ� ������T  �E�f�E�  f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE�f�E�fE���f�E�t>�}1��U���&    �Gf��t�ҋ]�tU�f�s�\U���f�\U���9�wԋU��E����  �}ǅx���   ��h�����l����E�9�|���F�|����M9�CE��	�E�   �M����e��]����}��p�����g����}�S  ��  �}��f����}�O  ��  �}�E��U�1�ǅt��������}����E��E�    ��M�*M��M��3�E� ��9�x���"�    �E�`}��h�����l����A�M��4C�M��   �   +M��U����M����؉E���M��}�����)����M��M���M���}�)ٍ����]��t& ��U�f�q�QE�M��8��u�M��   �U������tf����u����B  �C�!Ѝ�]��D]ԃ�f��f�D]�u��|���9���  �M��]�A�C�E��E�9E���   ��p���!�;�t�����   �}��M���EM�)ȉ��;�|����M��E�sJ�M��   ���L]�)ȅ�~4�}��]��U����L��|�����\M����)؅�~��9�w�U��U��M��   ��E��}�S  ��  �}�O  ��  �}���    �]��<��]�}���]����}�]���\��E���t���)���f�D�E��]��������&    1������Eֻ   )������}�   ǅx�������E�x���ǅh��� �4iǅl���`�4i��x������4iE�h�����h������4iE�l�����l����������g��� �   �-����������f��� �   �.���������҉�t{�}��   �]�1Ʌ�t.��p���!�;�t�����   �E��� �]��]��E�    �E��C�����ً]����@�]�f�A  �Y�����Ӊ]�t�����u��M��M���u�}��U��    1��]��M��%����]���!ӋU���]����͋M��ǅ|���   ����ǅ|���
   ������|���u
�} ���������p���ǅ|���   �{���ǅ|���   �l���ǅ|���   �]���ǅ|���   �N���ǅ|���   �?���ǅ|���   �   �V���ǅ|���   ����ǅ|���   �
�����U����<  �f�E�f�>  �f�E�f�@  �f�E�f�B  �f�E�f�D  �f�E�f�F  �f�E�f�H  �f�E�f�J  �f�E�f�L  �f�E�f�N  �f�E�f�P  �f�E�f�R  �f�E�f�T  �f�E�f�V  �f�E�f�X  ��}��f�E�t?�}�1��T���t)�DU�1��ȃ�f�DUԉȃ�	����� u���f����;u�~ƃ�X[^_]ËE�����\  �}�f� ƄX   ���  ��P  �� ��1�1҃���P  ă���    ��붉E��!����M���P  �uĉ��E�    �]����`  �!�E�    �E��U���  ��;}�tE����A�D���9E�}�E��E�9U�f�A|ҋ]�f��C<  9U���]�+U����]�롋]������E�����������    U��WVS���U��R�M؋}؃������Ɂ�   �����U���  �����1��E�    �}��U�}��T��U��S9�}�}�9}���  9���   �]�֋��  ���  �Ü  �]��af��]��\��]���ًX	��xf���  ����  �x�L���}�X���  �   )�������f���  �L����  t3�]��T��   )�9�|��]��\����	߃�f���  ���  u͋M�����  �}�9}���  �M�   1ۉM�   �E��}�9}���  �}��}�������u����  �u�9u���  �򋈼  �   �  �|�)�9���  �T��U������  ��H	�pf���  ����  �p�T���U��H�   +��  ����f���  ���  �L����  ���
  �   )�9��  ���
  �U���H	�f���  f�u�p�U����  �p�T���U��H�   +��  ����f���  ���  �L����  ����  ���P�ۉ���	��x��f���  ����  �p�L���   �P���  )у����M�f���  ���  ���N�����E�1ۋU�   ��   �}�9}؉U��Y�����[^_]Í�    ��
�9  ���
  �   ���  )�9��;  ���
  �������  �]��X	�pf���  ����  �p�L���u��X���  �   )�����f���  �L���
  ���	�f���  ���  �(������
  �   ���  )�9��r  ���
  �������  �]��X	�pf���  ����  �p�L���u��X���  �   )�����f���  �L����  ��	��   �����P����H	�f���  ����  �X�L���  �   ���P)ك�	��f���  ���  �������t& �T����f��  ����  f���  �������  �����  ������&    ���
  ���f��  ���
  ���f��  ����  f���  ���������'    U��WVS�� ���  �U�M܅�u_���  ���  �U�   ��   �Z)�9���  �u���   ��ىu��u����  	�f���  �R���  �� [^_]Ë��  1����  �   �t& �u�4��   �u��v)�9��t  �]���]���ًX	��xf���  ��x���  �L���}��X���  �   )�����f���  �L3����  ;��  �������  �S�]싘�  �u�������]��X����� �4i�]�u荜�  �s�]ԉu�   +u�9���  ��p�]���H	߉�f���  ����  �x�\���  ���u��}��H�   )����L�f�u�f���  ���  �}��@�4i��tv�u�+4���4i�   )�9��  �������M��u�p	��xf���  �7���  �x�L7���  ��u���H�   )����L;�f�u�f���  ���  �}�����   �}��k  �� �4i�]܍<��_�]�   +]�9��#  �?�}�����}�	ϋHf�}��]�f���  �x����  �x�\���  ���]�H�   )���f�]�f���  �]��L;����  ��@�4i��uR���  ���  �������M�	�f�}�f���  ���  �����]�����	�f���  ���  �����f��}�+<���4i�   )�9��|   ���p�}�����xfM�f���  �7�x���  �L7���   �p���  �}�)��O����?�]���M�	�f�]�f���  ���  ���������� �4i���������f}����  f���  ���������f	u��u����  f���  �1����u���   �u���	��pf���  ��f�}�x�7���  �x�L7���  ��u����H�   )����L;�f���  ���  ��������&    U��W��VS�����  ����   �H���  �p����  �p�\���H�X���pfǀ�    ǀ�      ǀ�     ���f����pf�}��L�p���шL�M��p��f���L�����Xt�}�1����7���H���9u��Xu��[^_]Å��p����H���  �p����H�U�����&    U��U���   ��  ���	  ��$  ��|
  ��0  1�ǂ    �4iǂ,  �4iǂ8  (�4ifǂ�    ǂ�      ǂ�     fǄ��     ��=  u�f1�f�fǄ��	    ����u�0�fǄ�|
    ����u�fǂ�   ǂ�      ǂ�      ǂ�      ǂ�      ]Í�&    U����E�]�U�}��]�u��}���  ��
  ���e  ���  ��)σ���   ��
  ��y,�  ��  ���������$  ����������   ��  �U����   �E��Ƀ������   �����M�fǄ��   ����   �E�����1�1ɉ}��1f��|
  ��t`9���  �U�1��E�   �E�   ��9M�|X��}��;E��t�}9�t�;E�|����J  ;U�t	f���|
  f���
  ��u�1���9M�U��E�   �E��   }����	  ��(  �U����	  �E��Ƀ������   �����M�fǄ��	  ����   �E�����1�1ɉ}��2�f��|
  ��td9���  �U�1��E�   �E�   ��9M�|\��}��9E��t�~9�t�9E���ҍt& ��  ;U�t	f���|
  f���
  ��u�1���9M�U��E�   �E��   }��؍�0  �����   f���
   ��   f���
   ���   f���
   ���   f���
   ���   f���
   ���   f���
   �
   �umf���
   �uaf���
   �
uUf���
   �	uIf���
   �u=f���
   �u1f���
   �u%f���
   �uf���
   �u
  ������  ���E��@�D���  ���  ��
����
��9�CM��9���  9��x  ���  ��
    ����u�Efǃ�   ǃ�      ǃ�      ��ǃ�      ǃ�      tF���  ����  �C�S���  ��S���  �L���Cfǃ�    ǃ�      ��[^_]Ít& ��
%f���
  �Z����u�1��E�   �E�   �a���f���
  �5�����
%f���
  ������u�1��E�   �E�   ����f���
  ��������   �{������  ��
  ����K	�sf���  �1���  �S�L2���  �   ���s)у�
  ���	�;E�f���  ���  uËM썻�   �؉����	  ������M�����������������������M���1������  ��
�U ǀ�       ǀ�     �@,��4i�@0��4i�@ 5i�p(�@H��4i�@L��4i�@PЖ4i�@T��4i�@$5i��@�4i�@ �4i�@ �4i�@D��4i�����Px�Å��;���������v �B�\�4i�����v �B�X�4i�����v �B�T�4i�f����v �B�P�4i�H����v �B�L�4i�+����v �B�H�4i�
�3i�|3i�3i�|3i�|3i�|3iҁ3i�|3i�|3i�|3i��3iflags, windowBits, bufsize, dictionary Enabled Disabled DispStream 0x%p - %s 
     stream pointer is NULL     stream           0x%p
            zalloc    0x%p
            zfree     0x%p
            opaque    0x%p
            msg       %s
            msg                   next_in   0x%p  =>  %02x            next_out  0x%p            avail_in  %lu
            avail_out %lu
            total_in  %ld
            total_out %ld
            adler     %ld
     bufsize          %ld
     dictionary       0x%p
     dict_adler       0x%ld
     zip_mode         %d
     crc32            0x%x
     adler32          0x%x
     flags            0x%x
            APPEND    %s
            CRC32     %s
            ADLER32   %s
            CONSUME   %s
            LIMIT     %s
     window           0x%p
 s, message=NULL Compress::Raw::Zlib::inflateScanStream::DispStream  Compress::Raw::Zlib::inflateStream::DispStream  Compress::Raw::Zlib::deflateStream::DispStream  Compress::Raw::Zlib::inflateScanStream::status s, buf, out=NULL, eof=FALSE  Compress::Raw::Zlib::inflateScanStream::scan inflateScan    Wide character in Compress::Raw::Zlib::InflateScan::scan input parameter    Compress::Raw::Zlib::inflateScanStream::inflateReset    Compress::Raw::Zlib::inflateStream::inflateReset s, buf inflateSync Compress::Raw::Zlib::inflateStream::inflateSync Wide character in Compress::Raw::Zlib::Inflate::inflateSync s, buf, output, eof=FALSE   Compress::Raw::Zlib::inflateStream::inflate inflate Compress::Raw::Zlib::Inflate::inflate input parameter cannot be read-only when ConsumeInput is specified    Wide character in Compress::Raw::Zlib::Inflate::inflate input parameter Wide character in Compress::Raw::Zlib::Inflate::inflate output parameter    s, flags, level, strategy, bufsize  Compress::Raw::Zlib::deflateStream::_deflateParams s, output, f=Z_FINISH    Compress::Raw::Zlib::deflateStream::flush flush Wide character in Compress::Raw::Zlib::Deflate::flush input parameter s, buf, output deflate    Compress::Raw::Zlib::deflateStream::deflate Wide character in Compress::Raw::Zlib::Deflate::deflate input parameter Wide character in Compress::Raw::Zlib::Deflate::deflate output parameter    Compress::Raw::Zlib::deflateStream::deflateReset    inf_s, flags, level, method, windowBits, memLevel, strategy, bufsize inf_s  Compress::Raw::Zlib::inflateScanStream::_createDeflateStream    flags, level, method, windowBits, memLevel, strategy, bufsize, dictionary   Wide character in Compress::Raw::Zlib::Deflate::new dicrionary parameter v5.14.0 2.033 Zlib.c Compress::Raw::Zlib::constant Compress::Raw::Zlib::zlib_version   Compress::Raw::Zlib::ZLIB_VERNUM Compress::Raw::Zlib::adler32 Compress::Raw::Zlib::crc32    Compress::Raw::Zlib::crc32_combine  Compress::Raw::Zlib::adler32_combine    Compress::Raw::Zlib::_deflateInit   Compress::Raw::Zlib::_inflateScanInit   Compress::Raw::Zlib::_inflateInit   Compress::Raw::Zlib needs zlib version 1.x
 Compress::Raw::Zlib::gzip_os_code                           need dictionary                 stream end                                                      file error                      stream error                    data error                      insufficient memory             buffer error                    incompatible version                                                �0w,a�Q	��m��jp5�c飕d�2�����y�����җ+L�	�|�~-����d�� �jHq���A��}�����mQ���ǅӃV�l��kdz�b���e�O\�lcc=��
����5l��B�ɻ�@����l�2u\�E�
��|
��}D��ң�h���i]Wb��ge�q6l�knv���+ӉZz��J�go߹��ﾎC��Վ�`���~�ѡ���8R��O�g��gW����?K6�H�+
��J6`zA��`�U�g��n1y�iF��a��f���o%6�hR�w�G��"/&U�;��(���Z�+j�\����1�е���,��[��d�&�c윣ju
�m�	�?6�grW �J��z��+�{8���Ғ
���
Ζ�	 �\H1�E�b�n�S�wT]��l���?�����P�������������\�br�yk޵T@��OYX#�p8$�A#=�k�e�Z�|%	�Wd8�N���⟊!̧3`��*��$���?�-��l�	��$H��S�)F~�hwe��y?/�H$6t	5*�SK��HRp�ey1�~`�������|���=����6�����xT��9e��K��;
��"���	�ˮO]�_l�F�?�m��tCZ�#A��pl��Aw�G�6��-�ŵ �����Aq[�Zh��wC��lZO-_~6�-'� > ��S1���b���S�����W��Ĕ���Ֆ�������k�1�*�*��ykʬHp�o]�.*F��6�f��cT�T"e�M���©g��0&��)��������:���{��ϼk���Z��>	��8���$,�52F*sw1��pH��kQ6�Fzw�]cN������̵������J��#���p���A��F]#l8�?1�(B�Og�T~��yU��bL�8�^�#����ܖ� T�Z1O��bb��Sy�O�IV~�P�-�{��b��-R��4���٠��~^��eGn�Hl/�Su�6:�	#jT$+e?�y���H��f��'*�������b���#��ٽ��Ч
-�
=G\p�&G��w�)`�/�a��߫��i��5����&��LsZ<#0�z��M�z�FM8�,�9���;��:<�D?��>R:�<eP=X^6o}�76��5�4��W1�Օ0�k�2�3��k$���%�1�'�[-&LMb#{'�""�� �$!(�x*޺+F`�)q
>(�q-�v�,���.��7/���p��Xq�Ys�3�r%�w+OQvr�tE��ux܉~O�K
� ��i8P/_���Y����=ч�e��:�ZO��?(3w����wXR
�����y�K�i��w�\�¹9�~�����$6�6nQ��f��q�>,�o,I�Ӕ��	�渱{I
�k5���B��lۻ�֬��@2�l�E�\u��
L_�
 �'}����D����hi���bW]�eg�l6qnk���v��+��zZg�J����o������C`����֣�ѓ~8���O��Rѻg�Wg?��H�6K�
گ
L6J�Az`�`�èg�U1n��Fi�y�a���f�%oҠRh�6�w��G"�U&/ź;���(+�Z�\�j�������1,ٞ�[ޮ�d°�c�&uj��m�
�	��6?rg� W��J��z{�+��8�Ҏ��վ
�x�
������\� 	E�1Hn�b�w�Sʺ�]T��l��?֑���טP�̩������˓rb�\ky�@T��YO��X#$8p�=#A�e�k�|�Z�W�	%N�8d������3��!*��`�$᯴?���-�	�l�H$��S��~F)�ewh/?y�6$H�	t*5KS��RH��ye�p`~�1������¿�Б|�ˠ=��6������Tx��e9;��K"��
	����ˈ_�]OF�lm�?�t���ZC�A#�lp��wA��6�G�-�� �ż��qA�hZ�[Cw�Zl��-O6~_'-�> ݹ� ��1S��b���S�������W�§��ٖծ�����1�k�*�*�ky��pH��]o�F*.f�6���T�TcM�e"����¤0��g)��&�Ů��ޟ����:���{��k���Z���	>��8,$�5�*F21wsHp�Qk��zF�6c]�w���N����̵��ׄ���J��#��pȄ�A�#]F8l1?�(�gO�B~T�Uy��Lbˁ�8��#�^�������T �O1Z�bb��yS�I�OP�~V{�-�b��-��4��R�����^~��Ge­lH�nuS�/:6�#	�$Tj?e+��y䏼H���f��*'�˼��Ѝ����b���#��
�-
\G=&�p��G�w`)/��a����i��5������&�sL�<Z�0#��zz�M8MF�9�,�;ɒ�:��?D�<>��<�:R=Pe6^X7�}o5��64�1W��0�ճ2�k�3�$k�%���'�1�&-[�#bML"�'{ �"!$�*x�(+��)�`F(>
q-q�,�v�.�Ț/7��p���qX��sY�r�3�w�%vQO+t�ru՛E~��xK�O}
;g��� �/P8i��_�Y��=嗇e����:�ϏOZw3(?���RXw�@��
��K�y��i׫w��¡\~�9����$��6�6�Qn�f�>�q�,o�,�ӹI	��散
      
  `     	�     �  @  	�   X    	� ;  x  8  	�   h  (  	�    �  H  	�   T   � +  t  4  	� 
  �  J  	�   V   @  3  v  6  	�   f  &  	�    �  F  	� 	  ^    	� c  ~  >  	�   n  .  	�    �  N  	� `   Q   �   q  1  	� 
  a  !  	�    �  A  	�   Y    	� ;  y  9  	�   i  )  	�  	  �  I  	�   U   +  u  5  	� 
  `     	�     �  @  	�   X    	� ;  x  8  	�   h  (  	�    �  H  	�   T   � +  t  4  	� 
  �  J  	�   V   @  3  v  6  	�   f  &  	�    �  F  	� 	  ^    	� c  ~  >  	�   n  .  	�    �  N  	� `   Q   �   q  1  	� 
  a  !  	�    �  A  	�   Y    	� ;  y  9  	�   i  )  	�  	  �  I  	�   U   +  u  5  	� 
4i�4i@4i�4i�4i�4iu4ij4i�4i�4i�4iN
      
  `     	�     �  @  	�   X    	� ;  x  8  	�   h  (  	�    �  H  	�   T   � +  t  4  	� 
  �  J  	�   V   @  3  v  6  	�   f  &  	�    �  F  	� 	  ^    	� c  ~  >  	�   n  .  	�    �  N  	� `   Q   �   q  1  	� 
  a  !  	�    �  A  	�   Y    	� ;  y  9  	�   i  )  	�  	  �  I  	�   U   +  u  5  	� 
  `     	�     �  @  	�   X    	� ;  x  8  	�   h  (  	�    �  H  	�   T   � +  t  4  	� 
  �  J  	�   V   @  3  v  6  	�   f  &  	�    �  F  	� 	  ^    	� c  ~  >  	�   n  .  	�    �  N  	� `   Q   �   q  1  	� 
  a  !  	�    �  A  	�   Y    	� ;  y  9  	�   i  )  	�  	  �  I  	�   U   +  u  5  	� 
  


















  �  J  �  *  �  j  �    �  Z  �  :  �  z  �    �  F  �  &  �  f  �    �  V  �  6  �  v  �    �  N  �  .  �  n  �    �  ^  �  >  �  ~  �    �  A  �  !  �  a  �    �  Q  �  1  �  q  �  	  �  I  �  )  �  i  �    �  Y  �  9  �  y  �    �  E  �  %  �  e  �    �  U  �  5  �  u  �  
                	        

                         (   0   8   @   P   `   p   �   �   �   �                                                                                   	   	   
   
               
   
7+7�7 9(90989U9{98:p;x;�;�;�;�;�<=�=K?S?[?c?�?�?   `  D   �0�2�2�2�23+3�3�314K4�4b7|7�758�:�:�:r<�<�<�<�<B>`>h>p>�>   p  L   .0>0N0k0!2P4X4`4h4�4�4"6@6H6P6k6�89�;�;�<�<1= >>N>x>�>�>?[?�?�?�? �  �   B0G0Y0�0�0�0#141E1s1�1�1�1�122)2;2�2�2	33'393x3�3�3�34m56�7L8[9t9�9::?:N:a:t:�:�:�:�:�:�:;&;=;O;c;w;�;�;�;�;�;�;�; <<&<9<M<`<s<�<�<�<�<�<�<=3=D=k=�=�?�?�?�?�?�?   �  @   �00282@2H2e2�23�4�4�4�4�45�5{6*7[7c7k7s7�7�7V8�8W<'=\? �  T   :0B0J0R0o0�01�1�2i3q3y3�3�3�3G4-5�5�6�6�6�6�6�6�7�7�8�9�:�:�:�:�:�:;�;:<�< �  <   M03{3B5J5R5Z5v5�5�5�5[7x8/:\:d:l:t:�:�:f;�;=/?~?�? �  H   0
000;0�0K1�1�3�4x5�56*626:6B6[6�6�7�899A9I9Q9n9�9�;e<�<t?   �  �    1�1�1�1�1�1�1$4f5�7=9x9�9�9B:h:�:�:�:�:�:�:�:�:�:;;;2;:;B;\;d;l;�;�;�;�;�;�;�;�;�;
???4?<?D?^?f?n?�?�?�?�?�?�?�?�?�?   �    0000080@0Z0b0j0�0�0�0�0�0�0�0�0�01
11,141<1V1^1f1�1�1�1�1�1�1�1�1�1�122(20282R2Z2b2|2�2�2�2�2�2�2�2�2�23
3$3,343N3V3^3x3�3�3�3�3�3�3�3�3�3�34 4(404J4R4Z4t4|4�4�4}5b;�; <<<!<8<G<Q<X<o<~<�<�<�<�<�<�<�<�<�< ==&=0=7=N=]=j=q=�=�=�=�= >
>>>R>#?7?   �      d03I3�3494�7�7�78�:)=      �0�0R1�1�6�6r7�7�8�<     �2�3�3i9>;�?�?�?   ,   0022_2�2�2*3B3W3�3�3�4�4�4U5�;�<m= 0    l4�4*6�9�9{; @    �1�15m8p9�; P     �3K5x5�5�5�7�7c9q;};�;   `    	3#3n6�<�<�<�<   p    >�>�><?�?   �     &0�0�2�2�2X4o4�5�:�:D=�= �   �0�0121_1�1�1�1-2A2n2w2}2�2�2�2�2�2�2�2�2�23
333"3B3a3v3�3�3�3�34444 4)40474>4U4c4p4~4�4�4�4�4�4�4�4�4�4�4�4�455#5*515;5K5U5r5�5�5�5�5�5�5�5�5�5�5�56$646D6T6d6t6�6�6�6�6�67"7*7R7Z7b7j7r7z7�7�7�7�7�7�7�7�7�7�7�7�7�7�7�7�78
888"8*828:8B8J8R8Z8b8j8r8z8�8�8�8�8 � �    0000,0T<X<\<`<d<h<l<p<t<x<|<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�<�< ======== =$=(=   � 0   P;T;X;\;`;d;h;l;p;t;x;|;�;�;�;�;�;�;�;   � H   064686<6@6D6H6L6P6T6X6\6`6d6h6l6p6t6x6|6�6�6�6�6�6�6�6�6�6�6�6   � 0   00 0,080D0P0\0h0t0 =$=(=,=0=4=8=<=@=D=                                                                        Zlib.dbg    j*�"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1   `]          ?                                U   �       .text          �                .data                            .bss                            .rdata  `      _                     m           .jcr    `]          w   �                            �   @          �   �                       X  p
          �  �          �  0            �          @  �          s  P          �  �          �              p          5  �          b  0"          �  �$          �  �&             P)          4  �+          e  .          �  p0          �  �2            05          4  �7          f  �9          �  P<          �  �>          �  A          (  pC          Y  �E          }   G          �  pI          �  �K          �  M          6  �O      _deRef   S          h  @T          �  �W          �  �Z          �  �\      _deRef_l�^            ``          ;  �d          j  `f          �  i          �  �k          �   t          �  �          	  �y          	  `}          M	  �          	  ��          �	  �          �	  ��          
  ��          K
  ��          
  ��          �
  �          �
  ��            ��          D  0�          s  P�          �  ��          �  ��            ��      .text   �      ��  w            .data                            .bss                            .rdata  �      `  6             _adler32��                           &   �          7  ��      .text   ��                     .data                            .bss                                J  ��                           Z  `�          i         _crc32  p�          t  p�          �  ��      .text   ��       '             .data                            .bss                            .rdata                              �  ��                           �  ��          �  �;          �  `<          �  �;          �   �      .text   ��     �               .data                            .bss                            .rdata   :     @
                   �  P�                           �  ��          
             .data                            .bss                            .rdata  �O                                                       %  �          4   !         A  �#         O  �(         ]  �,         m  @/         �  �0         �   P          �  02         �  `2         �  �2         �  �2     _deflate 4         �  �D         �  �E         �  �F           �I           �K     .text        "0  N             .data                            .bss                            .rdata  �O     �   
                 &  0N                      .text   0N    y               .data                            .bss                            .rdata  �P     P                     4  �V                          C   Q          N  `Q          Z  �Q          e  �Q      .text   �V    ]               .data                            .bss                            .rdata  �P     >                    q  _                          }  P`         �  �f         �  �l         �  @Z          �  �Z          �  @[          �  �[          �  `q         �  Pr         �                             (           "  0s         4  �s         ?   U          M  0w         ^  �Y          l  Z          v  ��         �  @\      .text   _    "               .data          <                .bss                            .rdata   R     l
                    �  0�                          �  @�     _zError P�     _zcallocp�     _zcfree ��     .text   0�    r                .data   <                        .bss                            .rdata  �\     �   
                 �  ��                          �  $           �              �             �  ��         �  (             ��     .text   ��                   .data   <                        .bss                           .text   Ђ                    .text   ؂                    .text   ��                    .text   �                    .text   ��                    .text   ��                    .text    �                    .text   �                    .text   �                    .text   �                    .text    �                        !  0�                      .text   0�                     .data   <                        .bss    ,                        .text   @�                        -  P�                      _u.8973 @       .text   P�    3                .data   <                        .bss    @      �                     @  ��                      .text   ��      =             .data   @                       .bss                           .text   ��                    .text   ��     .idata$7�      .idata$5P      .idata$4T       .idata$6T          W  ��                      .text   ��                     .data   `                        .bss                               h  ��                      .text   ��                     .data   `                        .bss                               y  І                      .text   І                     .data   `                        .bss                               �  ��                      .text   ��                     .data   `                        .bss                           .text   ��                    .text   ��                    .text    �                        �  �                      .text   �                     .data   `                        .bss                           .text    �     .data   `       .bss          .idata$7�      .idata$5�      .idata$4�       .idata$60      .text   (�     .data   `       .bss          .idata$7�      .idata$5�      .idata$4�       .idata$6D      hname   �       fthunk  �      .text   0�                      .data   `                        .bss                           .idata$2                      .idata$4�       .idata$5�      .text   0�                      .data   `                        .bss                           .idata$4�                       .idata$5�                      .idata$7�     
  �                       5        __dll__     ��       I  �          U      ��       j    X��       y  $          �     ��       �  p          �  �O          �  P          �  �          �  H          �      ��       %    X��       C  �          V  �P          i  d]          v  ��         �  �          �            �  4          �  �          �  ��         �  �          �         __fmode                 ��       "  �          :            W      ��       �   T          �  �          �  l          �              �      ��       �     ��         �          #      ��       ]  h          k  �          }  �          �  ,          �     ��       �  `          �            �  �          �  L                         �          .  ��         <  t          J      ��            ��       �    X��       �     ��       �  T          �  �          �  L          �  \          �  @                 ��       L  �          \  d          h     ��       �      ��       �  |          �  x          �             �  �          �  �          	      ��       %      ��       =  �          T  X          e  @          �  <          �      ��       �    X��          �             �          2   (          K   �          d                s                �                �                �                �           i                __ZdlPv         i"                    �                �                �                !           __end__              !               (!          i                     =!               J!          i                     c!          i                    y!               �!          i                __Znaj          i#                    �!          i                __Znwj          i$                    �!               �!          i                 __ZdaPv         i!                �!  .eh_frame .gnu_debuglink ___EH_FRAME_BEGIN__ ___JCR_LIST__ ___gcc_register_frame ___gcc_deregister_frame .eh_frame _XS_Compress__Raw__Zlib__inflateScanStream_adler32 _XS_Compress__Raw__Zlib__inflateScanStream_crc32 _XS_Compress__Raw__Zlib__inflateScanStream_getLastBufferOffset _XS_Compress__Raw__Zlib__inflateScanStream_getLastBlockOffset _XS_Compress__Raw__Zlib__inflateScanStream_uncompressedBytes _XS_Compress__Raw__Zlib__inflateScanStream_compressedBytes _XS_Compress__Raw__Zlib__inflateScanStream_inflateCount _XS_Compress__Raw__Zlib__inflateScanStream_getEndOffset _XS_Compress__Raw__Zlib__inflateStream_get_Bufsize _XS_Compress__Raw__Zlib__inflateStream_total_out _XS_Compress__Raw__Zlib__inflateStream_adler32 _XS_Compress__Raw__Zlib__inflateStream_total_in _XS_Compress__Raw__Zlib__inflateStream_dict_adler _XS_Compress__Raw__Zlib__inflateStream_crc32 _XS_Compress__Raw__Zlib__inflateStream_status _XS_Compress__Raw__Zlib__inflateStream_uncompressedBytes _XS_Compress__Raw__Zlib__inflateStream_compressedBytes _XS_Compress__Raw__Zlib__inflateStream_inflateCount _XS_Compress__Raw__Zlib__deflateStream_total_out _XS_Compress__Raw__Zlib__deflateStream_total_in _XS_Compress__Raw__Zlib__deflateStream_uncompressedBytes _XS_Compress__Raw__Zlib__deflateStream_compressedBytes _XS_Compress__Raw__Zlib__deflateStream_adler32 _XS_Compress__Raw__Zlib__deflateStream_dict_adler _XS_Compress__Raw__Zlib__deflateStream_crc32 _XS_Compress__Raw__Zlib__deflateStream_status _XS_Compress__Raw__Zlib__deflateStream_get_Bufsize _XS_Compress__Raw__Zlib__deflateStream_get_Strategy _XS_Compress__Raw__Zlib__deflateStream_get_Level _XS_Compress__Raw__Zlib_ZLIB_VERNUM _XS_Compress__Raw__Zlib__inflateStream_msg _XS_Compress__Raw__Zlib__deflateStream_msg _XS_Compress__Raw__Zlib_zlib_version _XS_Compress__Raw__Zlib__inflateScanStream_resetLastBlockByte _XS_Compress__Raw__Zlib__inflateStream_set_Append _XS_Compress__Raw__Zlib_crc32 _XS_Compress__Raw__Zlib_adler32 _XS_Compress__Raw__Zlib__inflateScanStream_DESTROY _XS_Compress__Raw__Zlib__inflateStream_DESTROY _XS_Compress__Raw__Zlib__deflateStream_deflateTune _XS_Compress__Raw__Zlib__deflateStream_DESTROY _XS_Compress__Raw__Zlib_adler32_combine _XS_Compress__Raw__Zlib_crc32_combine _XS_Compress__Raw__Zlib_constant _XS_Compress__Raw__Zlib__inflateInit _my_z_errmsg _DispStream _XS_Compress__Raw__Zlib__inflateScanStream_DispStream _XS_Compress__Raw__Zlib__inflateStream_DispStream _XS_Compress__Raw__Zlib__deflateStream_DispStream _XS_Compress__Raw__Zlib__inflateScanStream_status _XS_Compress__Raw__Zlib__inflateScanStream_scan _XS_Compress__Raw__Zlib__inflateScanStream_inflateReset _XS_Compress__Raw__Zlib__inflateStream_inflateReset _XS_Compress__Raw__Zlib__inflateStream_inflateSync _XS_Compress__Raw__Zlib__inflateStream_inflate _XS_Compress__Raw__Zlib__deflateStream__deflateParams _XS_Compress__Raw__Zlib__deflateStream_flush _XS_Compress__Raw__Zlib__deflateStream_deflate _XS_Compress__Raw__Zlib__deflateStream_deflateReset _XS_Compress__Raw__Zlib__inflateScanStream__createDeflateStream _XS_Compress__Raw__Zlib__deflateInit _boot_Compress__Raw__Zlib _adler32_combine _adler32_combine64 _crc32_combine_ _get_crc_table _crc_table _crc32_combine _crc32_combine64 _inflateBackInit_ _inflateBack _order.2611 _lenfix.2589 _distfix.2590 _inflateBackEnd _updatewindow _inflateReset _inflateReset2 _inflateInit2_ _inflateInit_ _inflatePrime _order.2646 _lenfix.2617 _distfix.2618 _inflateEnd _inflateSetDictionary _inflateGetHeader _inflateSync _inflateSyncPoint _inflateCopy _inflateUndermine _inflateMark _uncompress _compress2 _compress _compressBound _longest_match _flush_pending _fill_window _deflate_slow _deflate_fast _deflate_stored _deflateSetDictionary _deflateReset _configuration_table _deflateSetHeader _deflatePrime _deflateTune _deflateBound _deflateParams _deflateEnd _deflateInit2_ _deflateInit_ _deflateCopy _inflate_fast _inflate_table _dext.2537 _dbase.2536 _lext.2535 _lbase.2534 _pqdownheap _build_tree _send_tree _compress_block _extra_lbits _base_length _extra_dbits _base_dist _copy_block.clone.0 __tr_init _static_l_desc _static_d_desc _static_bl_desc __tr_stored_block __tr_align _static_ltree __tr_flush_block _static_dtree _bl_order __tr_tally _extra_blbits _zlibVersion _zlibCompileFlags ___dllMain _storedPtr _storedReason _storedHandle __cygwin_dll_entry@12 _dll_index __cygwin_noncygwin_dll_entry@12 _DllMain@12 _cygwin_attach_dll __cygwin_crt0_common@8 _cygwin_premain0 _cygwin_premain1 _cygwin_premain2 _cygwin_premain3 __pei386_runtime_relocator ___FRAME_END__ ___JCR_END__ _register_frame_ctor _deregister_frame_dtor .eh_frame .ctors.65535 .dtors.65535 _Perl_croak _Perl_mg_set _Perl_sv_2uv_flags _Perl_sv_setpvn _Perl_sv_2pvbyte _Perl_croak_xs_usage _Perl_sv_setnv _Perl_newSVsv _Perl_xs_apiversion_bootcheck _Perl_sv_setref_pv _dll_dllcrt0 _GetModuleHandleA@4 _Perl_call_list _Perl_sv_upgrade _Perl_sv_setiv _GetProcAddress@8 _Perl_newSVpvf_nocontext _Perl_safesysmalloc _cygwin_internal _strerror _Perl_stack_grow _Perl_croak_nocontext _Perl_dowantarray _Perl_sv_2pv_flags _Perl_newSVpv _Perl_sv_setuv _Perl_mg_get _Perl_get_sv _Perl_sv_pvbyten_force _Perl_sv_derived_from _Perl_xs_version_bootcheck _Perl_sv_2bool_flags _Perl_sv_setpv _Perl_newXS _cygwin_detach_dll _Perl_sv_2mortal _pthread_getspecific _Perl_safesysfree _Perl_newSViv _Perl_sv_backoff _Perl_sv_2iv_flags _Perl_sv_newmortal _Perl_sv_utf8_downgrade _Perl_sv_free _Perl_sv_utf8_upgrade_flags_grow _Perl_sv_free2 _Perl_sv_grow ___RUNTIME_PSEUDO_RELOC_LIST__ __imp__Perl_croak_xs_usage __data_start__ ___DTOR_LIST__ _cygperl5_14_dll_iname __nm___impure_ptr __imp__Perl_sv_upgrade __imp__Perl_sv_2iv_flags .weak.__Jv_RegisterClasses.___gcc_register_frame __imp__Perl_sv_utf8_upgrade_flags_grow _z_errmsg __imp__Perl_stack_grow __imp__Perl_newSVsv __imp__Perl_croak_nocontext __imp__cygwin_detach_dll .weak.___register_frame_info.___gcc_register_frame .weak.___real__Znwj.__cygwin_crt0_common@8 __imp__Perl_sv_setiv __imp__Perl_sv_free2 __imp__Perl_dowantarray __imp__PL_thr_key __dll_characteristics__ __size_of_stack_commit__ .weak.___real__ZdlPv.__cygwin_crt0_common@8 __size_of_stack_reserve__ __major_subsystem_version__ ___dynamically_loaded __head_cygperl5_14_dll __imp__Perl_safesysmalloc __bss_start__ ___RUNTIME_PSEUDO_RELOC_LIST_END__ __size_of_heap_commit__ __imp__Perl_sv_2uv_flags __imp__GetProcAddress@8 __dist_code __imp__Perl_sv_newmortal __imp__putchar __imp__Perl_sv_derived_from __imp__Perl_sv_free __imp__puts __minor_os_version__ __image_base__ __imp__Perl_sv_setpvn __section_alignment__ __imp__memmove _deflate_copyright __imp___impure_ptr __imp__Perl_mg_get __IAT_end__ .weak.___real__ZdaPv.__cygwin_crt0_common@8 __RUNTIME_PSEUDO_RELOC_LIST__ __imp__Perl_mg_set _inflate_copyright __data_end__ __CTOR_LIST__ __imp__Perl_newXS __bss_end__ __imp__Perl_sv_utf8_downgrade __imp__Perl_sv_backoff ___CTOR_LIST__ __imp__Perl_newSVpv __imp__Perl_sv_setpv __rt_psrelocs_size __imp__Perl_safesysfree __imp__Perl_sv_pvbyten_force .weak.___real__ZnajRKSt9nothrow_t.__cygwin_crt0_common@8 __length_code _cygwin1_dll_iname __imp__memcpy __head_cygwin1_dll .weak.___real__Znaj.__cygwin_crt0_common@8 __file_alignment__ __imp__Perl_get_sv .weak.___real__ZdaPvRKSt9nothrow_t.__cygwin_crt0_common@8 __imp__malloc __imp__Perl_croak __imp__Perl_newSViv __imp__Perl_sv_setuv __major_os_version__ __imp__dll_dllcrt0 __imp__Perl_sv_setnv __imp__realloc __IAT_start__ __imp__Perl_sv_grow __imp__GetModuleHandleA@4 __DTOR_LIST__ __imp__memset .weak.___deregister_frame_info.___gcc_register_frame __size_of_heap_reserve__ ___ImageBase __subsystem__ __imp__calloc __imp__Perl_sv_2pvbyte __imp____errno __imp__cygwin_internal ___cygwin_cxx_malloc .weak.___real__ZnwjRKSt9nothrow_t.__cygwin_crt0_common@8 __imp__strerror __imp__free __major_image_version__ __loader_flags__ __imp__pthread_getspecific __imp__printf __head_libkernel32_a __imp__Perl_newSVpvf_nocontext __imp__Perl_sv_2bool_flags __minor_subsystem_version__ __minor_image_version__ __imp__Perl_sv_2mortal __nm__PL_thr_key __imp__Perl_xs_version_bootcheck __imp__Perl_xs_apiversion_bootcheck .weak.___real__ZdlPvRKSt9nothrow_t.__cygwin_crt0_common@8 __RUNTIME_PSEUDO_RELOC_LIST_END__ __libkernel32_a_iname __imp__Perl_call_list __imp__Perl_sv_setref_pv __imp__Perl_sv_2pv_flags ___tls_start__ __rt_psrelocs_start ___crt_xl_start__ ___crt_xi_start__ ___crt_xi_end__ ___register_frame_info ___crt_xp_start__ ___crt_xp_end__ ___crt_xc_end__ ___crt_xc_start__ ___crt_xt_start__ __Jv_RegisterClasses ___tls_end__ ___deregister_frame_info __ZdlPvRKSt9nothrow_t __rt_psrelocs_end __ZnajRKSt9nothrow_t __ZdaPvRKSt9nothrow_t ___crt_xt_end__ __ZnwjRKSt9nothrow_t FILE   ,546eb361/auto/Compress/Raw/Zlib/autosplit.ix   �#line 1 "/usr/lib/perl5/5.14/i686-cygwin-threads-64int/auto/Compress/Raw/Zlib/autosplit.ix"
# Index created by AutoSplit for ../../lib/Compress/Raw/Zlib.pm
#    (file acts as timestamp)
1;
FILE   769e8ba3/auto/Fcntl/Fcntl.dll  miMZ�       ��  �       @                                   �   � �	�!�L�!This program cannot be run in DOS mode.
$       PE  L 
@t�t$�$�W  �E܋U܃��r��]�u��}���]Ít& �U؉$�  �U؉��m����D$   �D$�$�_  �E�    % �  �E��x����v U��WVS��,�]�CH�;����CH����    C)�����    )ǋC�@ ��   �p�ҋC�4���   �C�T��B	tD��@%�  �D$�D$    �t$�$��  �F
@t�t$�$�f  �G�w���,[^_]��D$   �T$�$�  %�  뫍v �U�M��$�  �U�M��҉��s����C��H  )����f����|$�|$�U��D$   �$�+  �U���A�������'    U��WVS��l�]�M�sH�����sH��S)�����t�D$�0i�L$�$�y
  �E��7��&    ��	��   ���  �H   ���p�N   �����   �S�C�<$�T$�D$�
  ��D$    �D$    �D$   �M�ƋC�L$�<$�D$�E��D$�Q
  ����  �@�H�у��}  ����^����M���E��t$�<$�L$�D$�)
  ����g����D$   �D$�0i�<$��	  �D$    �D$    �D$0   �D$   �D$�0i�D$�<$�	  �E�    ����t� �@	�z  �@�E܍�x  ��4i�U��D$    �M��D$    �D$   �C�D$��L$�<$�D$�Z	  ������  �P�B�ȃ���  ���u	��	��  ��t	��P��tW�E�U��D$��T$�<$�D$�	  ��J��t�Q����  ���҉Q�Y  �f�b8���B     � �@    �F��P�D$   �L$ �M�L$�L	���D$�E܉T$�D$    �L$�D$�<$�}  ����  �����������U��<$�T$�q  �M�D$31i�D$�i�D$�1i�$�~  � �@ �  �E�D$31i�D$�i�D$�1i�$�R  �U� �@ @  �$�D$31i�D$�i�D$�1i�&  �M� �@ �  �$�D$31i�D$�i�D$�1i��  � �@ �  �E�D$31i�D$�i�D$�1i�$��  �U� �@ `  �$�D$31i�D$�i�D$2i�  �M� �@    �$�D$31i�D$�i�D$2i�v  � �@   �E�D$31i�D$�i�D$%2i�$�J  �U� �@   ���  ��t�D$�B�$�D$�  �M�A�ʋM؁�x  ���U�E�B���\[^_]ËP�B �  �|����D$�E��D$   �<$�  �E�������EЉL$�<$��  �EЋ����f��J�A �  �0����D$    �D$�0i�T$�<$�  �t����EЉL$�<$�  �E��D����D$   �<$�8  �ƉE܋�xv�p��H   �^����D$�D$   �<$�n  ��׋�D$�1i�<$�D$�  ��D$p1i�<$�D$�  �U��D$p1i�<$�T$�w  ���������������U����$Pi�D$� Pi�D$�Pi�$�  ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$�   ����tދ(Pi�E�$�   �E��(Pi����뾐�t& �M�E   �U��}   1������D$@i�$�Pi� Pi   �
�U ǀ�       ǀ�     �@,p"i�@0|"i�@ Qi�p(�@H� i�@L !i�@P!i�@T !i�@$Qi��@0!i�@8!i�@ @!i�@DH!i�����Px�Å��;���������v �B�0i�����v �B�0i�����v �B�0i�f����v �B�0i�H����v �B�0i�+����v �B�0i�
    %-p is not a valid Fcntl macro at %s line %d
 v5.14.0 1.11 Fcntl.c Fcntl::AUTOLOAD Fcntl::S_IMODE Fcntl::S_IFMT Fcntl:: Couldn't add key '%s' to %%Fcntl::  Couldn't add key '%s' to missing_hash Fcntl::S_ISREG Fcntl::S_ISDIR Fcntl::S_ISLNK Fcntl::S_ISSOCK Fcntl::S_ISBLK Fcntl::S_ISCHR Fcntl::S_ISFIFO Fcntl::S_ISENFMT DN_ACCESS DN_MODIFY DN_CREATE DN_DELETE DN_RENAME DN_ATTRIB DN_MULTISHOT FDSYNC FLARGEFILE FNONBLOCK FRSYNC F_ALLOCSP F_ALLOCSP64 F_COMPAT F_DUP2FD F_EXLCK F_FREESP F_FREESP64 F_FSYNC F_FSYNC64 F_GETLEASE F_GETLK64 F_GETSIG F_NODNY F_NOTIFY F_POSIX F_RDACC F_RDDNY F_RWACC F_RWDNY F_SETLEASE F_SETLK64 F_SETLKW64 F_SETSIG F_SHARE F_SHLCK F_UNSHARE F_WRACC F_WRDNY LOCK_MAND LOCK_READ LOCK_WRITE LOCK_RW O_ALIAS O_ASYNC O_DEFER O_EXLOCK O_IGNORE_CTTY O_LARGEFILE O_NOATIME O_NOLINK O_NOTRANS O_RANDOM O_RAW O_RSRC O_SEQUENTIAL O_SHLOCK O_TEMPORARY S_IFWHT S_ISTXT                        62i	   @2i	   J2i	   T2i	   ^2i	   h2i	   r2i   2i   �2i
   �2i	   �2i   �2i	   �2i   �2i   �2i   �2i   �2i   �2i
   �2i   �2i	   �2i
   3i	   
   Z3i	   d3i
   o3i   x3i   �3i   �3i	   �3i   �3i   �3i	   �3i	   �3i
   �3i   �3i   �3i   �3i   �3i   �3i
          �6i          �6i    @      �6i           �6i          �6i           �6i          �6i          �6i          �6i          �6i          �6i          �6i          �6i          �6i   	       �6i          7i          7i          7i	           7i          )7i          27i          :7i          C7i           O7i           W7i          ^7i    @      g7i    �      p7i
          {7i          �7i
    @      �7i           �7i          �7i           �7i           �7i          �7i          �7i          �7i          �7i   @       �7i    `      �7i           �7i    @      �7i          �7i    �      8i    �      	8i    �      8i          8i           "8i          *8i          28i   8       :8i          B8i   �      J8i          R8i          Z8i          b8i          j8i          r8i   �       {8i   �       �8i          �8i          �8i   @       �8i          �8i          �8i          �8i          �8i           �8i          �8i          �8i    �                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          �#�O    Z`           (`  <`  P`     �  �  Lr  �  d`  v`  �`  �`  �`        Fcntl.dll XS_Fcntl_AUTOLOAD XS_Fcntl_S_IFMT XS_Fcntl_S_IMODE _nm__PL_thr_key boot_Fcntl                                                                                                                                                                                                                                                                                                                                               Pp          �t  �p  xp          �t  q  �p          ,u  (q                      �q  �q  �q  �q  �q  �q  �q   r  r      $r  8r      Lr  \r  pr  �r  �r  �r  �r  �r  �r  �r  s  ,s  @s  Ps  ls  �s  �s  �s  �s  �s  �s  �s  t  t  (t  <t  \t      �q  �q  �q  �q  �q  �q  �q   r  r      $r  8r      Lr  \r  pr  �r  �r  �r  �r  �r  �r  �r  s  ,s  @s  Ps  ls  �s  �s  �s  �s  �s  �s  �s  t  t  (t  <t  \t      p_impure_ptr   �calloc    Ccygwin_detach_dll Ecygwin_internal   Zdll_dllcrt0   �free  malloc    �pthread_getspecific   �realloc   GetModuleHandleA  AGetProcAddress    D PL_thr_key    \Perl_call_list    �Perl_croak    �Perl_croak_sv �Perl_croak_xs_usage   Perl_get_hv   MPerl_hv_common    NPerl_hv_common_key_len    Perl_mg_set   &Perl_mro_method_changed_in    NPerl_newCONSTSUB  oPerl_newSV_type   qPerl_newSViv  vPerl_newSVpvf_nocontext   xPerl_newSVpvn_flags   �Perl_newXS    _Perl_stack_grow   lPerl_sv_2mortal   xPerl_sv_2uv_flags �Perl_sv_free  �Perl_sv_free2 �Perl_sv_newmortal �Perl_sv_setpvn    �Perl_sv_setuv �Perl_sv_upgrade   DPerl_xs_apiversion_bootcheck  EPerl_xs_version_bootcheck  p   p   p   p   p   p   p   p   p  cygwin1.dll p  p  KERNEL32.dll    (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  (p  cygperl5_14.dll                                                                                                                                                                                                        �   	0$0;0B0I0T0o0�0�0�0�0/4{4�4@5v5
6A6M6i6q6y6�6�6�6�6�6�6�6�6�78;8v9~9�9�9�9�9�9�9�9�9:
<!<G<P<Y<�<�<�<=
222"2*222:2B2J2R2Z2b2j2t2�2   0    �4�4�4�4�4�4�4�4�4�4�4�4�4�4�4�4 5555 5(50585@5H5P5X5`5h5p5x5�5�5�5�5�5�5�5�5�5�5�5�5�5�5�5�5 6666 6(60686@6H6P6X6�8�8 99 909@9P9`9p9�9�9�9�9�9�9�9�9 :: :0:@:P:`:p:�:�:�:�:�:�:�:�: ;; ;0;@;P;`;p;�;�;�;�;�;�;�;�; << <0<@<P<`<p<�<�<�<�<�<�<�<�< == =0=@=P=`=p=�=�=                                                                                                                                                                                                                                                                                                                                                                                                Fcntl.dbg   g=��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1   �
            #
  0          5
  X          I
  �          ^
     ��       s
            �
            �
  �           �
            �
  |          �
      ��             ��           eX��       &     ��       4  �           B  <          U             l              �      ��       �            �  P          �     ��       �      ��                   "             7  \          V      ��       r      ��       �  l          �  L          �  �          �  �          �      ��       1
$       PE  L	 
�������u鋍l����A@��  ��t����������T$��L����$�Q$��L���1���������l����@���d���t.��p���f�y�/t!������% �  = @  ��  = �  �C  ��l�����`�����h����B�I���������;�p����   �������p���f�  ��  �     ��h���f�8 ��  ��������������D�������h����;�D�����  �
�������u勍l����A@�Y  �<$�Q ��X�����X��� ��  ��l���1�ǅT����i�A���¨@��P���t	�I��T�����X����$��T�������  �x.ty;�p�����  ��p����P�σ�f��f�t�P���9���  �f���f��u�9���  ��P����ً�t����$��p���������u%��p���f�  �j�����t���f�:.�Z����r�����`����O���l����\$�4$�D$��\����T$��d����D$��h����I�������������l����A@�=  ��X����$�1  �������p�����9��������p�����p����������t����������T$��L����$��  ��L����r�����h����׋�t����9��������������u���d���@tr��t�����l����|$�$�R(���o���������% �  = @  �Y�����p����   ��9�������p���f� / f�@  �-�����p����g���1��������t����|$�$�=  됉<$�;  ��X�������f�  �   ����1�������X����$�Q�����������D$   �D$�@i�<$��  �:�����l���1��X���b����������މ�t����Ћ�h����M��9�tR��������u��x  ��t��������� �$�D$�օ�������l����B������׃���������  ����������U��WVSP�X@  �  �E��E�    �����f��~��  �B��������ȿ����п����̿�����Ŀ����ȿ���f��[tXf��*t1f��� f�
���f����   ��f��?uԁK   f�?����ځK   ;�п��tf�z�*�t�f�*��������!��Կ����   f��t�H������7f��]��  f��u�f�[ ����Կ��!�H�D��i���f�  f������ u|�;�Ŀ���>  �C� u=��0�pi�`iDȋ�Ŀ��C+�Ŀ���L$��C�D$   �T$�$�  ��̿��1��S��\@  [^_]Ít& ����?�����t& �M�L$�M�L$��п��������������\$�$�L$���n������J�����̿���s��\@  [^_]�f��B��������ȿ����п������̿���C����N�Ѝ~�U�f��u������9�v�f��tf��/u��  ��п���9 �}  �������$�  ����������  �@�����  f�ɍpf�������������������  f��f���9�u�f�   �S��п����������̿����ȿ��������������f�[��r��Կ��!tp� f%� f����f��-t!������]u�K   �ȍVf�]��W����A�Qf��]tf%� f�-�f�F���A��뼍v �Ѹ-   뚍�&    f�B!��r����f��C�u����   ��   �������̿���M�ډC��ȿ��������������&    ��9�������f���f��u������f��������$�@i�  ��������������  �$�  ���������d����C��п����̿�����ȿ���/�����п���U��WVS���E� �E܋E�U܋ �E������   �M������   �]܋5ܑi����������   �����   �����<��E��   �}�t�� �����D�t�� 9�t���U�M��� ��DU�U������D�t�� �U�)�u�M؋E܉M�E��[^_]�H  ��[^_]Ë5ܑif��������E�1���   �U�딍�&    �%   �E���E�    �t����5ܑi�U؋��   �M���E�    �Q����t& U��WVSP�H   �W  �E�������9��  ���������������
��f���9�u��Pf�  �������Hf��tm�������ˉ�ǅ����    1���f��{tqf��}tv���f��t=f��[u���f��]tgf��tb���f��]t���f��u�f��t��X�pf��uË�����������������M���L   1�[^_]Ã����됅�t��끐�t& �ډ��9�����������������wd�����������������    f��,t^��;�����w=�f��[��  v�f��{�m  f��}f�uԋ�������t2��������;�����vË]1��    ��L   [^_]Ë�������u�9����   �N�B)�������������ۉ������  ��������������	����Ä������[  1�������9�r������������	�9������t�B����f���9Ɖ�w�����G������1��Tf���f��u�f������{������u�   ��    ���f��{t%f��u���������������M�V�։������t܍U䋍�����؉$�������B�����u��E��˃������c������f��]tVf��tQ���f��]t���f��u��f��E��2���f������}�^���f������ �P������������p����]�����������������������������������    ��'    U�,   ��WVS��  �E�u�]����F�E�E   �    �F    �F    �F�F    ug����  ��f�������M⍕�������&    f���9�t
�����u썽����f�  �E�us���������,   [^_]Í�&    �����   ���������}����&    f�H���9�s������t�����\u���\@  ��t�ʃ���@f�P�����f������{��u�@f���v������f��{u���b����E��$�����\�����uڋE��,   [^_]�f������}u�f������ u��&����������������t& U��WVS���}�G��t;��W��t#�4�����������t�$��
  �@	tA�D$   �D$Ai�$�x
  � �P������    �D$   �D$�$��
  �]����D$   �D$Ai�$�7
  �D$   �$�D$�
  ���2����E�D$�@i�$�D$��	  ��    ��'    U���8�]�]�u��}��CH��8���CH�C��)�������   �
  �F
@t�t$�$�	  �w�M��    C��]�u��}���]ÉU��$��	  �U��ƋC떋E�D$�@i�$�D$�	  ��    ��    U��WVS��L�E�8�@H�Z�0���BH�D$   �D$Ai�N�M܋U����M؋M���$�D$�y	  �E܋U�D$   �D$&Ai�D$����Bi)����$�|$�N	  �M�D$+Ai�D$P(i�D$2Ai�$�  �E�D$    �D$IAi�D$+Ai�D$�%i�D$MAi�$�  �U�D$+Ai�D$ %i�D$`Ai�$�e  �M�D$uAi�D$�%i�D$}Ai�$�B  � �@   �E�D$   �D$ @i�$��  ���i� �$�g  �D$   �D$�Ai�ǉ$�  �E��0��	��   ���  �H   ���p�N   �����   �S�C�<$�T$�D$�  ��M��D$    �D$    �U��D$   �ƋC�T$�L$�<$�D$�1  ����   �@�H�у���   ����^����U���M��t$�<$�T$�L$�  ����g����E��<$�D$��  �U���  ��t�D$�B�$�D$�  �M�A�ʋM܁�x  ���U�E�B���L[^_]Ð�P�B �  �r����D$�E��D$   �<$�	  �E�������E��D$�Ai�<$�D$�"  �����������%đi��Q�L$=   r��   �	 -   =   w�)��	 ���̋�@PÐ��f�f�U����$pi�D$� pi�D$�pi�$�{  ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$�,  ����tދ(pi�E�$�$  �E��(pi����뾐�t& �M�E   �U���   1������D$P,i�$�pi� pi   �
�U ǀ�       ǀ�     �@,3i�@03i�@ qi�p(�@Hp1i�@L�1i�@P�1i�@T�1i�@$qi��@�1i�@�1i�@ �1i�@D�1i�����Px�Å��;���������v �B�<@i�����v �B�8@i�����v �B�4@i�f����v �B�0@i�H����v �B�,@i�+����v �B�(@i�
 pattern, ... File::Glob::DEFAULT_FLAGS v5.14.0 1.13 Glob.c File::Glob::GLOB_ERROR $;$ File::Glob::doglob File::Glob::AUTOLOAD Glob.xs File::Glob::bsd_glob File::Glob::  Couldn't add key '%s' to %%File::Glob:: GLOB_ABEND GLOB_ALPHASORT GLOB_ALTDIRFUNC GLOB_BRACE GLOB_ERR GLOB_LIMIT GLOB_MARK GLOB_NOCASE GLOB_NOCHECK GLOB_NOMAGIC GLOB_NOSORT GLOB_NOSPACE GLOB_QUOTE GLOB_TILDE GLOB_CSH        �Ai
   ���������Ai           �Ai   @       �Ai
   �       �Ai          Bi
    @      Bi	          Bi          'Bi          4Bi          ABi           MBi   ��������ZBi
          eBi
          pBi   �.                       2                                                                                                                                        ȑ  �      ȑ  <                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      �#�O    ��     	   	   (�  L�  p�   %  P(  �%  �  ��  ��  0)  #  �$  ��  ��  ��  Ѐ  �  �  	�  �  "�            Glob.dll XS_File__Glob_AUTOLOAD XS_File__Glob_GLOB_ERROR XS_File__Glob_doglob _nm__PL_charclass _nm__PL_thr_key _nm____stack_chk_guard boot_File__Glob bsd_glob bsd_globfree                                                                                                                                                                                                                  d�          ̖  `�  Ȑ          ��  đ  Ԑ          ��  Б  ��          ��  ܑ                      \�  h�  x�  ��  ��  ��  ��  ��  Ē  ؒ  �  ��  �  �  �  (�  4�  L�  T�  `�  l�  x�  ��  ��      ��  ��      ē  ؓ      �  ��  �   �  0�  @�  X�  h�  x�  ��  ��  Ĕ  ؔ  �  ��  �  0�  @�  T�  h�  ��  ��  ��  ��  ԕ  �  ��  �  �  0�  P�      \�  h�  x�  ��  ��  ��  ��  ��  Ē  ؒ  �  ��  �  �  �  (�  4�  L�  T�  `�  l�  x�  ��  ��      ��  ��      ē  ؓ      �  ��  �   �  0�  @�  X�  h�  x�  ��  ��  Ĕ  ؔ  �  ��  �  0�  @�  T�  h�  ��  ��  ��  ��  ԕ  �  ��  �  �  0�  P�       __errno   \_getpwuid32   c_getuid32 p_impure_ptr   �_lstat64  5_stat64   �calloc    closedir  Ccygwin_detach_dll Ecygwin_internal   Zdll_dllcrt0   �free  )getenv    Sgetpwnam  malloc    Oopendir   �pthread_getspecific   �qsort �readdir   �realloc   �strcmp    �strlcpy   �strlen    tolower    __stack_chk_fail  	 __stack_chk_guard GetModuleHandleA  AGetProcAddress     PL_charclass  D PL_thr_key    \Perl_call_list    �Perl_croak    �Perl_croak_sv �Perl_croak_xs_usage   Perl_get_hv   Perl_get_sv   NPerl_hv_common_key_len    Perl_mg_set   &Perl_mro_method_changed_in    0Perl_my_cxt_init  NPerl_newCONSTSUB  qPerl_newSViv  vPerl_newSVpvf_nocontext   xPerl_newSVpvn_flags   �Perl_newXS    �Perl_newXS_flags  Perl_safesysfree  Perl_safesysmalloc    Perl_safesysrealloc   _Perl_stack_grow   kPerl_sv_2iv_flags lPerl_sv_2mortal   qPerl_sv_2pv_flags �Perl_sv_magic �Perl_sv_newmortal �Perl_sv_setiv �Perl_sv_upgrade   DPerl_xs_apiversion_bootcheck  EPerl_xs_version_bootcheck  �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �  cygwin1.dll �  �  cygssp-0.dll    (�  (�  KERNEL32.dll    <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  <�  cygperl5_14.dll                                                                                                                4   	0$0;0B0I0T0o0�0�0�0�0�4<537�9};�;>�>J?�?      �   N5�56�6�7�7838�89Z9�9�9�9�9�9�9�9�9�9:::2:::B:f:s:�:�;<W<`<i<�<�<==='=,=9=r=z=�=�=�=�=�=�=�=�=�=�=�=�=�=�=>!>6>t>�>�>�>�>�>�>�>�>�>�>�>�>?#?0?>?L?Y?g?u?�?�?�?�?�?�?�?�?�?�?�?�?�?�? 0  �   0020o0v0}0�0�0�0�0�0�0�0�0�0�0�011$141D1S1b1�1�1�1�1�1�1�1"2*222:2B2J2R2Z2b2j2r2z2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�2333 @  (   �2�2�2�2�2�2�2�2 33 303@3P3`3                                                                                          Glob.dbg    �)q                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1   �          ?                                U   �       .text          �                .data                            .bss                            .rdata  @      _                     m           .jcr    �      _match  �                            w   �          �   �      _compare`      _glob2  �      _glob0  P
          �   p          �   �          �             �   �      .text   �        -             .data                            .bss                            .rdata  �                       _errfunc�                           �              �   �          �               �   P            0          $  �      .text   �       D             .data                           .bss                            .rdata  �      �               .text         .idata$7�      .idata$5�      .idata$4�       .idata$6�      .text         .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text        1                 .data                           .bss                                9  P                           D  $           O              ]             k  �          �  (           �  `      .text   P                    .data                           .bss                           .text   p                     .text   x                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                     .text   �                         �  �                       .text   �                      .data                           .bss    ,                        .text                             �                         _u.8973 @       .text        3                .data                           .bss    @      �                     �  P                       .text   P       =             .data                           .bss                           .text   `!                     .text   h!      .idata$7x      .idata$5l      .idata$4p       .idata$6�          �  p!                       .text   p!                      .data   @                        .bss                               �  �!                       .text   �!                      .data   @                        .bss                                 �!                       .text   �!                      .data   @                        .bss                                 �!                       .text   �!                      .data   @                        .bss                           .text   �!                     .text   �!                     .text   �!                     .text   �!                     .text   �!                         &  �!                       .text   �!                      .data   @                        .bss                           .text   �!      .data   @       .bss          .idata$7�      .idata$5�      .idata$4�       .idata$6�      .text   �!      .data   @       .bss          .idata$7�      .idata$5�      .idata$4�       .idata$6�      hname   �       fthunk  �      .text    "                       .data   @                        .bss                           .idata$2(                      .idata$4�       .idata$5�      .text    "                       .data   @                        .bss                           .idata$4�                       .idata$5�                      .idata$7�     
  \          
      ��       <
   PfX��       Z
  �          m
             �
  �          �
  �          �
  |          �
  �          �
  #          �
            �
        __alloca          �
  �          �
  @            #      __fmode           #  $   ��       6  $          N            p      ��       �  �          �              �      ��       �  �               ��         �          .      ��       h  d          {  �          �  �          �            �     ��       �            �  �          �  �          �  `          
$       PE  L g��S f �  � !     <     �'      0    �h                         �                         p  �   �  �                           �  �                                                  8�  �                           .text                           ` P`.data   D    0      $              @ `�.rdata  @   @      &              @ 0@/4      `   P      ,              @ 0@.bss       `                      � `�.edata  �   p      4              @ 0@.idata  �   �      6              @ 0�.reloc  �   �      >              @ 0B/14     p   �      @              @ B/29     :�   �   �   B              @ B/41     �   �                   @ B/55     H   �     0             @ B/67     �   �     D             @ B/78     r   �     F             @ B/89        �     b             @ B                                                U��WVS���5|��h�$ @�h�փ�������   �$ @�h����h����h�U���a�h�D$@�h�$�ҋU�����D$%@�h�$�҃�� 0�h��t�D$ `�h�$ P�h�ס@0�h��t8�$=@�h�ֺ    ����t�D$K@�h�$����h����t	�$@0�h���$��h�  �e�[^_]�� 0�h    �    념��&    U���� 0�h��t	�$ P�h�Сa�h��t�$�x��h���ÐUWVS���\$0�SH��J��KH�2�K���1)�����u�D1�~�$�D$�*  �h��tK�S�$��T$�J  �T$��C�D0�,$�D$�9  ��t
   �D$s@�h�C�D0�$�D$�
@t�|$�$�a  �~k�+��,[^_]É$�L$�T$�  �L$�T$�ɉ�u��n  ������    �����뒍t& �D$   �D$�$�L$�H  �L$���H����D$D�D$�@�h�$�D$�  ��t& UWVS��,�l$@�UH�E �J��MH�:�M���9)�������   �E�w�T9�@ t`�H�E���T$�,$��  ��t[� 1ҀHQ1�}�D$�T$�\$�,$��  �C
@t�\$�,$�L  �_u�u ��,[^_]Ív �,$�T$�  �T$����_
@t�\$�,$�V  �_u�u ��,[^_]É,$�T$�  �T$�҉�u��j  ������    �����뙋D$D�D$`@�h�,$�D$��  ���&    UWVS��,�l$@�UH�E �J��MH�:�M���9)�������   �D9�w�,$�D$��  �P�E�@ tW�H�ҋE��t`�T$�,$�0  1�1�}�D$�T$�\$�,$��  �C
@t�\$�,$�c  �_u�u ��,[^_]É,$�T$�  �T$�҉�u��w  ������    �����뙋D$D�D$`@�h�,$�D$��  ��t& UWVS��,�\$@�SH��J��KH�2�K���1)�������   �D1�~�$�D$�  � �@����   �D$�$�e  �S�$��T$���  �T$����t"��t+�l$���l$�C�D0�$�D$��  {�;��,[^_]��D$
   �D$s@�h�C�D0�$�D$��  {�;��,[^_]��k
  �    �s�$�  ��{�;��,[^_]ËD$D�D$`@�h�$�D$��  ����'    VS���\$ �SH��J��KH�2�K���1)Ѓ�~$�D1�$�D$��  � �@��u#s�3��[^ËD$$�D$�@�h�$�D$�^  �D$�@�h�$�@�h�B  �v ��'    UWVS��,�\$@�SH��J��KH�2�K���1)�������   �D1�~�$�D$�f  � �@����   �D$�$�
   �D$s@�h�C�D0�$�D$�  {�;��,[^_]���  �    �s�$��
   �D$s@�h�C�D0�$�D$��  {�;��<[^_]Él$���l$�C�D0�$�D$��  {�;�ӋD$T�D$A�h�$�D$��  �
  �D$    �D$�A�h�D$mA�h�D$��h�D$�A�h�$��
  �D$mA�h�D$@�h�D$�A�h�$�
  �D$mA�h�D$`�h�D$�A�h�$�
  �D$mA�h�D$P�h�D$ B�h�$�f
  �D$mA�h�D$p�h�D$B�h�$�F
  �D$mA�h�D$@�h�D$)B�h�$�&
  �D$mA�h�D$`�h�D$�@�h�$�
  �D$mA�h�D$0!�h�D$LA�h�$��	  �D$mA�h�D$��h�D$;B�h�$��	  �D$    �D$LB�h�D$mA�h�D$  �h�D$NB�h�$�	  �D$   �D$   �D$eB�h�$�6	  �D$   �D$    �$���T	  �t$�D$nB�h�$�D$�	  �D$   �D$    �$�$	  �t$�D$uB�h�$�D$��  �D$   �D$    �$��  �t$�D$}B�h�$�D$�  �D$   �D$    �$��  �t$�D$�B�h�$�D$�  �D$   �D$    �$�  �t$�D$�B�h�$�D$�\  �D$   �D$    �$�d  �t$�D$�B�h�$�D$�,  �D$   �D$    �$�4  �t$�D$�B�h�$�D$��  �D$   �D$    �$�  �t$�D$�B�h�$�D$��  �D$   �D$    �$��  �t$�D$�B�h�$�D$�  �D$    �D$    �$�  �t$�D$�B�h�$�D$�l  �D$   �D$
   �D$�B�h�$�4  �D$    �D$    �$���R  �D$�B�h�t$�$�D$�  �D$   �D$    �$�"  �D$�B�h�t$�$�D$��  �D$   �D$    �$��  �D$�B�h�t$�$�D$�  �D$    �D$    �$��  �D$�B�h�t$�$�D$�  �D$   �D$    �$�  �D$�B�h�t$�$�D$�Z  �D$   �D$    �$�b  �D$�B�h�t$�$�D$�*  ���  ��t�D$�C�$�D$��  �C��x  �T({�;��,[^_]Ð��U���� `�h�D$�$`�h�D$�(`�h�$�+  ���Í�    U����E�U�M��t|r;��t��f�u&�M�E   �U���   �M�E   �U���   �   �� �L$�D$    �$�   ����t�`�h�E�$�   �E��`�h�������t& 1������D$`'�h�$�(`�h�$`�h   �
�U �Mǀ�       ǀ�     �@, .�h�@0.�h�@a�h�H(�@H@,�h�@LP,�h�@P`,�h�@Tp,�h�@$ a�h��@�,�h�@�,�h�@ �,�h�@D�,�hǀ�   �,�h������Px�Å��1���������B�<0�h�����v �B�80�h�y����v �B�40�h�[����v �B�00�h�>����v �B�,0�h�!����v �B�(0�h�����v �B�$0�h������v �� 0�h����������%T��h���%P��h���Ð��������������Ð��������������Ð��������������Ð��������������%d��h���%X��h���%p��h���%H��h���%l��h��f�f�f�f��Ð��������������%|��h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%���h���%ā�h���%ȁ�h���%́�h���%Ё�h���%ԁ�h���%؁�h���%܁�h���%���h���%��h���%��h���%��h���%���h���%��h���%���h���%���h���% ��h���%��h���%��h���%��h���%��h���%��h���%��h��f�f�f�f�U��]����������������-�h    ����                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                cyggcc_s-1.dll __register_frame_info __deregister_frame_info cyggcj-14.dll _Jv_RegisterClasses  handle handle, pos 0 but true IO::File packname = "IO::File" +>& timeout, ... handle, blk=-1 handle, c handle, ... IO::Handle::setbuf   %s not implemented on this architecture sock    Usage: IO::Handle::setvbuf(handle, buf, type, size) IO::Handle::setvbuf v5.14.0 1.25 IO.c IO::Seekable::getpos IO::Seekable::setpos IO::File::new_tmpfile IO::Poll::_poll $;$ IO::Handle::blocking IO::Handle::ungetc IO::Handle::error IO::Handle::clearerr IO::Handle::untaint IO::Handle::flush IO::Handle::sync $ IO::Socket::sockatmark IO::Poll POLLIN POLLPRI POLLOUT POLLRDNORM POLLWRNORM POLLRDBAND POLLWRBAND POLLERR POLLHUP POLLNVAL IO::Handle _IOFBF _IOLBF _IONBF SEEK_SET SEEK_CUR SEEK_END    GCC: (GNU) 4.8.3    GCC: (GNU) 4.8.3    GCC: (GNU) 4.8.3    GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3 20140522 (Fedora Cygwin 4.8.3-5)   GCC: (GNU) 4.8.3    GCC: (GNU) 4.8.3                                                                                                                                                                                                           zR |�  ,      ����    A�B
�A�A�A�A    L   ����/    A�B
A�A�A�A�D h   \   (���   A�A�A�A�C@�
A�A�A�A�Dk
A�A�A�A�Ab
A�A�A�A�A �   �   ����   A�A�A�A�CP
A�A�A�A�Ai
A�A�A�A�AT
A�A�A�A�BI
A�A�A�A�A<   L  x���^   A�A�A�A�CP�
A�A�A�A�AT   �  �����   A�A�A�A�C@*
A�A�A�A�Ao
A�A�A�A�G  <   �  ����+   A�A�A�A�C@�
A�A�A�A�A <   $  �����    A�A�A�A�C@�
A�A�A�A�D <   d  �����    A�A�A�A�C@�
A�A�A�A�A <   �  @����    A�A�A�A�C@�
A�A�A�A�A h   �  ����   A�A�A�A�C@�
A�A�A�A�Ak
A�A�A�A�Ab
A�A�A�A�A ,   P  ����v    A�A�C B
A�A�A   h   �  ����   A�A�A�A�C@�
A�A�A�A�Ak
A�A�A�A�Ab
A�A�A�A�A T   �  ����!   A�A�A�A�CP�
A�A�A�A�BO
A�A�A�A�A     D  ����I    A�C    <   `  �����   A�A�A�A�C@�A�A�A�A�          zR |�        <���*    A�B
�E@N
�E@F
�C��       |   ����	    A�B
��C        zR |�        ����              zR |�        ����              zR |�        ����              zR |�        ����              zR |�        ����                  zR |�        ����	    A�B
   
==="=*=2=:=B=J=R=Z=b=j=r=z=�=�=�=�=�=�=�=�=�=�=�=�=�=>                                      �h=              �                   ��       `'�h              :�       �(�h               ُ       �(�h3               �       )�h              ��       @,�h               .�       P,�h               b�       `,�h               ��       p,�h               ��       �,�h               ��                                                                                                                                                               ��       GNU C 4.8.3 -mtune=generic -march=i686 -g -O3 -fno-strict-aliasing -fstack-protector IO.c /cygdrive/c/DOCUME~1/anmin.deng.ICATCHTEK/.cpan/build/IO-1.25-0TBSN4  �h=      signed char unsigned char short int short unsigned int __int32_t ?  int __uint32_t A%  unsigned int __int64_t YF  long long int __uint64_t [i  long long unsigned int long int _off64_t F  _ssize_t 7  sizetype size_t �%  long unsigned int clock_t u�  time_t z�  timespec �.  tv_sec ��   tv_nsec ��   4  char pid_t �  ssize_t ˡ  nlink_t ��   timer_t �  uint32_t "%  uintptr_t c%  timestruc_t �  off_t (�  dev_t 1  blksize_t 6�   blkcnt_t <5  uid_t N  gid_t T  ino_t \W  mode_t �%  	sigval 	�M  
sival_int 	�  
sival_ptr 	��   sigval_t 	�  	��  
_si_fd 	�  
_si_pipe_fhandler 	��  
_si_str 	�.   _sigcommune 	�  _si_code 	�   _si_read_handle 	��  _si_write_handle 	��  _si_process_handle 	��  ]   	�>  
si_sigval 	�M  
si_value 	�M   
__pad 	��  
_si_commune 	М  i  ~  
si_addr 	�     
d�    �  �  5 �  4  �   �  �   �    stat `�  st_dev �   st_ino    st_mode 
  0   �3   8   �3  a  �H  �   ��G  C   �%  	�   �%  F  �%     �%  s   �%  �   �%  �   ��1      ��1  cop_line �2  cop_stashpv �.  cop_file �.  cop_hints ��1   cop_seq ��1  $cop_warnings �;L  (cop_hints_hash �AL  , PMOP o	�
  pmop 8?%  0   @3   8   @3  a  @H  �   @�G  C   @%  	�   @%  F  @%     @%  s   @%  �   @%  �   @�1      @�1  u  A3  op_last B3  op_pmoffset D�   op_pmflags H�1  (op_pmreplrootu PH  ,op_pmstashstartu XIH  0 LOOP s	2  loop (�F
  HIorigargv 4  LIenvgv 3  PIincgv 
v&  HIsv_no v&  `Isv_yes v&  xIsubname @2  �Isubline �1  �Imin_intro_pending �1  �Imax_intro_pending �1  �Ipadix �1  �Ipadix_floor �1  �Ihints �1  �Idebug �Y  �Ibreakable_sub_gen �1  �Iamagic_generation �  �Icollation_name ".  �Icollxfrm_base #�  �Icollxfrm_mult $�  �Icollation_ix %�1  �Icollation_standard &4  �Iutf8cache 0�1  �Inumeric_standard 64  �Inumeric_local 84  �Inumeric_name :.  �Iutf8_alnum >@2  �Iutf8_ascii ?@2  �Iutf8_alpha @@2  �Iutf8_space A@2  �Iutf8_perl_space B@2  �Iutf8_perl_word C@2  �Iutf8_posix_digit D@2  �Iutf8_cntrl E@2  �Iutf8_graph F@2  �Iutf8_digit G@2  �Iutf8_upper H@2  �Iutf8_lower I@2  �Iutf8_print J@2   Iutf8_punct K@2  Iutf8_xdigit L@2  Iutf8_mark M@2  Iutf8_X_begin N@2  Iutf8_X_extend O@2  Iutf8_X_prepend P@2  Iutf8_X_non_hangul Q@2  Iutf8_X_L R@2   Iutf8_X_LV S@2  $Iutf8_X_LVT T@2  (Iutf8_X_T U@2  ,Iutf8_X_V V@2  0Iutf8_X_LV_LVT_V W@2  4Iutf8_toupper X@2  8Iutf8_totitle Y@2  <Iutf8_tolower Z@2  @Iutf8_tofold [@2  DIlast_swash_hv \�8  HIlast_swash_tmps ]@  LIlast_swash_slen ^�  PIlast_swash_key _�Y  TIlast_swash_klen `�1  ^Ipad_reset_pending f4  _Iglob_index h  `Iparser k�Y  dIpsig_ptr o4  hIpsig_name s4  lIptr_table ��Y  pIbeginav_save ��9  tIbody_arenas ��  xInumeric_radix_sv �@2  |Iregex_pad �4  �Iregex_padav ��9  �Ireentrant_buffer �Z  �Icustom_op_names ��8  �Icustom_op_descs ��8  �Iperlio �l2  �Iknown_layers �Z  �Idef_layerlist �Z  �Iencoding �@2  �Idebug_pad ��X  �Iutf8_idstart �@2  �Iutf8_idcont �@2  �Iutf8_xidstart �@2  �Iutf8_xidcont �@2  �Isort_RealCmp ��X   Icheckav_save ��9  Iunitcheckav_save ��9  Iclocktick ��  Isignals ��1  Ireentrant_retint �  Istashcache ��8  Isharehook �IW  Ilockhook �IW   Iunlockhook �IW  $Ithreadhook �tW  (Isignalhook ��W  ,Ihash_seed ��  0Irehash_seed ��  8Iisarev ��8  @Imy_cxt_size �  DImy_cxt_list �Z  HIdestroyhook  �W  LIregistered_mros 
�8  PIblockhooks 
mg_flags  �1  mg_len !�1  mg_obj "@2  mg_ptr #.   XPV �	�*  xpv �+  ~  ��8   3  �A9     ��  +  ��   XPVIV �	-+  xpviv ��+  ~  ��8   3  �A9     ��  +  ��  xiv_u ��8   XPVAV �	�+  xpvav �+  ~  �8   3  
ival ��1  
pval �.  
opval �3  
gvval �3  
p_tkval �.  
i_tkval ��1   �  �(  YYSTYPE �2  HE Y13  he p3  hent_next &4   hent_hek ;9  he_valu zK   HEK Z{3  hek �3  hek_hash �1   hek_len �1  hek_key �K   s4  !  s.  !$  s�  !�  s�  !R  s@2  !k  s4  !   s 4  !P   s,4  !�  s24   @2  &4  '3  �'  ^2  ��4  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   >,  � 5  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   �,  �g5  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   �+  ��5  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   �+  �56  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   �-  ��6  !  �.  !$  ��  !�  ��  !R  �@2  !k  �4  !   � 4  !P   �,4  !�  �24   regexp Xi8  ~  j�8   3  jA9     j�  +  j�  engine k
�9  xiou_dirp �9  xiou_any �   {  regnode #:  flags �1   type �1  next_off �1   regnode �9  reg_substr_datum $�:  min_offset %�1   max_offset &�1  substr '@2  utf8_substr (@2  end_shift )�1   reg_substr_data <+�:  data ,�:    2:  �:  �   %a   5;  start 6�1   end 7�1   &a   8�:  regexp_engine 0z
  �(  PAD �&  PADOFFSET �  �G  
xcv_start 3  
xcv_xsubany �/   �G  
xcv_root 3  
xcv_xsub �9   '3  H  42   �G  $IIH  op_pmreplroot J3  op_pmtargetoff L�G   $QH  op_pmreplstart R3  op_pmstashpv T.   passwd  3I  pw_name 4.   pw_passwd 5.  pw_uid 6�  pw_gid 7�  pw_comment 8.  pw_gecos 9.  pw_dir :.  pw_shell ;.   group 8bI  gr_name 9.   gr_passwd :.  gr_gid ;�  gr_mem <4   #� r_K  _asctime_buffer  t.   _asctime_size  u�  _ctime_buffer  .  _ctime_size  ��  _grent_struct  �I  _grent_buffer  �.   _grent_size  ��  $_grent_ptr  �_K  (_getlogin_buffer  �.  ,_getlogin_size  ��  0_pwent_struct  �H  4_pwent_buffer  �.  T_pwent_size  ��  X_pwent_ptr  �eK  \_readdir_struct  �u  `_readdir_size  ��  d_readdir_ptr  �u  h_strerror_buffer  	.  l_strerror_size  
�  p_ttyname_buffer  
hent_val @2  
hent_refcount �   4  �K  �    jmpenv � L  je_prev !L   je_buf "�  je_ret #  �je_mustcatch $4  � �K  JMPENV '�K  COPHH �,L   refcounted_he �  L  block_sub P�L  retop Q3   cv S�9  savearray U�9  argarray V�9  olddepth W�1    X�L   �G  block_format ]M  retop ^3   cv `�9  gv b3  dfoutgv c3   block_eval ��M  retop �3   old_namesv �@2  old_eval_root �3  cur_text �@2  cv ��9  cur_top_env ��M   L  $��M  svp �4  gv �3  )  ��L   #��M  ary ��9   ix ��   #�N  cur ��   end ��   #�/N  cur �@2   end �@2   $�cN  ary ��M  lazyiv ��M  lazysv �N   block_loop  ��N  resetsp ��1   my_op ��N  itervar_u ��M  state_u �/N   %  block_givwhen +�N  leave_op ,3    $ ?TO  blku_sub @GL  blku_format A�L  blku_eval BM  blku_loop CcN  blku_givwhen D�N   �	  subst 0~pP  sbu_type �1   sbu_rflags ��1  sbu_rxtainted ��1  sbu_iters ��1  sbu_maxiters ��1  sbu_oldsave ��1  sbu_orig �.  sbu_dstr �@2  sbu_targ �@2  sbu_s �.  sbu_m �.   sbu_strend �.  $sbu_rxres ��  (sbu_rx �<  , $8��P  cx_blk �e)  cx_subst �ZO   stackinfo  E>Q  si_stack F�9   si_cxstack G>Q  si_prev HDQ  si_next IDQ  si_cxix J�1  si_cxmax K�1  si_type L�1  si_markoff M�1   0)  �P  PERL_SI R�P  '  sQ  42  @2  x9   ZQ  '�1  �Q  42  @2  x9   yQ  '  �Q  42  @2  x9  @2  �  �1   �Q  '  �Q  42  x9  B>   �Q  &/  SUBLEXINFO �
  K�b  J
  B�  Lc  �
  6�  �d  Lc  �
  Lc  �
  Mi�h$   L-c  
u  8t s  7�$�h΀  0u  8t s 8tv 8t�B�h 7%�h�~  Eu  8t s  7$%�h΀  ku  8t s 8tv 8t�B�h 7<%�h�~  �u  8t s  7T%�h΀  �u  8t s 8tv 8t�B�h 7l%�h�~  �u  8t s  7�%�h΀  �u  8t s 8tv 8t�B�h 7�%�h�~  �u  8t s  7�%�h΀  v  8t s 8tv 8t�B�h 7�%�h�~  1v  8t s  7�%�h΀  Wv  8t s 8tv 8t�B�h 7&�h��  �v  8t s 8t�B�h8t:8t1 7&�h�~  �v  8t s  76&�h΀  �v  8t s 8tv 8t�B�h 7N&�h�~  �v  8t s  7f&�h΀  �v  8t s 8tv 8t�B�h 7~&�h�~  
@2  �}  42  �<   SPerl_sv_2mortal #�
@2  �~  42  �|   SPerl_sv_2iv_flags #�
@2    42  �[   SPerl_PerlIO_fileno #�  F  42  24   Wfcntl %�  c      X SPerlIO_ungetc    �  24     SPerl_mg_set #�  �  42  @2   SPerl_PerlIO_error #�  �  42  24   UPerl_PerlIO_clearerr #��  42  24   SPerl_PerlIO_flush #�  %�  42  24   UPerl_croak_nocontext #�I�  �  X Wfsync &M  `�     Wioctl 'B  }�      X UPerl_croak #H��  42  �  X SPerl_gv_stashpvn #��8  ΀  42  �  �1  �1   SPerl_newCONSTSUB #�	�9   �  42  �8  �  @2   UPerl_xs_apiversion_bootcheck #�:�  42  @2  �  �   UPerl_xs_version_bootcheck #�v�  42  �1  �1  �  �   SPerl_newXS #x
�9  ��  42  �  Y  �   SPerl_newXS_flags #~
�9  ށ  42  �  Y  �Y  �Y  �1   YPerl_call_list #�42  �1  �9    �   �  GNU C 4.8.3 -mtune=generic -march=i686 -g -O3 -fno-strict-aliasing -fstack-protector poll.c /cygdrive/c/DOCUME~1/anmin.deng.ICATCHTEK/.cpan/build/IO-1.25-0TBSN4   signed char unsigned char short int short unsigned int int unsigned int long long int long long unsigned int long int sizetype long unsigned int char double float long double  �   �  GNU C 4.8.3 20140522 (Fedora Cygwin 4.8.3-5) -mtune=generic -march=i686 -g -ggdb -O2 -fdebug-prefix-map=/home/corinna/src/cygwin/cygwin-1.7.32/cygwin-1.7.32-1.i686/build=/usr/src/debug/cygwin-1.7.32-1 -fdebug-prefix-map=/home/corinna/src/cygwin/cygwin-1.7.32/cygwin-1.7.32-1.i686/src/src=/usr/src/debug/cygwin-1.7.32-1 -fno-common -fbuiltin -fmessage-length=0 -fmerge-constants -ftracer /usr/src/debug/cygwin-1.7.32-1/winsup/cygwin/lib/dll_entry.c `'�h  6  int unsigned int short unsigned int char long int long long int   sizetype long unsigned int unsigned char DWORD |=  float signed char short int long long unsigned int PVOID �c  HINSTANCE__ ��  unused ��    HINSTANCE ��  �  HMODULE ��  tagCOINITBASE �:  	COINITBASE_MULTITHREADED   double 
VARENUM  �  	VT_EMPTY  	VT_NULL 	VT_I2 	VT_I4 	VT_R4 	VT_R8 	VT_CY 	VT_DATE 	VT_BSTR 	VT_DISPATCH 		VT_ERROR 
	VT_BOOL 	VT_VARIANT 	VT_UNKNOWN 
M   G�  __count I�   __value NS   _mbstate_t O�  _flock_t S�  __ULong Y  
M    
M   _atexit �[�  _next \�   _ind ]�  _fns _�  �  `  � j  	�  �  
M   �  
  _stdout @G
  _stderr @G
  _inc B�  _emergency C�  _current_category E�  0_current_locale F�  4__sdidinit H�  8__cleanup J
  �__sf �+
M   	n  P  
M    
@  G_lb 
  _next #A
   _niobs $�  _iobs %G
   
  �	  _rand48 =�
  _seed >�
   _mult ?�
  _add @   	  �
  
M   �XH  _unused_rand Z�   _strtok_last [A  _asctime_buf \H  _localtime_buf ]W  $_gamma_signgam ^�  H_rand_next _�  P_r48 `M
  X_mblen_state a�  h_mbtowc_state b�  p_wctomb_state c�  x_l64a_buf dX  �_signal_buf eh  �_getdate_err f�  �_mbrlen_state g�  �_mbrtowc_state h�  �_mbsrtowcs_state i�  �_wcrtomb_state j�  �_wcsrtombs_state k�  �_h_errno l�  � 	  X  
M   	  h  
M   	  x  
M   �q�  _nextf t�   _nmalloc u�  x 	�  �  
M   	�  �  
M   �V�  _reent m�
  _unused vx   	  �  
M   
M   
M   �  �  �  8  �   �
M   per_process_cxx_malloc �  	�  ,  
M   �  E  �
�  cygwin_attach_dll   �(�h3   ��   h v
_cygwin_crt0_common@8 �    K  �   &dll_dllcrt0 
  v
|  �  �   sizetype _mbstate_t O^  _flock_t S�  size_t Է  long unsigned int   char pid_t ��  int32_t �  uint32_t "�  uintptr_t c�  __gnuc_va_list (z  
�  CHAR 
  LONG 
  �  k  LPCSTR 
:�  
�  �  �   HANDLE 
|  
|  �  �   __ULong   _Bigint -=	  	_next /=	   	_k 0�  	_maxwds 0�  	_sign 0�  	_wds 0�  	_x 1C	   �  
  S	  �    __tm $5
  	__tm_sec 7�   	__tm_min 8�  	__tm_hour 9�  	__tm_mday :�  	__tm_mon ;�  	__tm_year <�  	__tm_wday =�  	__tm_yday >�  	__tm_isdst ?�    �  HV
  	_fnargs IV
   	_dso_handle JV
  �_fntypes L�   _is_cxa O�   
  f
  �   _atexit �[�
  	_next \�
   	_ind ]�  	_fns _�
  �  `
  � f
  
�
  �
  �   �
   __sbuf s�
  	_base tX   	_size u�   !?          �     "_reent @9�  #�W,
  H'_atexit0 |f
  L'_sig_func �   �'__sglue ��  �'__sf �  � �
  !?  �      �  �   �  !          �   �  !�         	  
|  3  �   
|  C  �    __sFILE64 p��  	_p �X   	_r �  	_w �  	_flags �  	_file �  	_bf ��
  	_lbfsize ��  	_data �  	_cookie �   	_read ��  $	_write ��  (%_seek    ,%_close   0%_ub �
  4%_up X  <%_ur �  @%_ubuf 	#  D%_nbuf 
3  G%_lb 
  H%_blksize �  P%_flags2 �  T%_offset   X%_seek64 �  `%_lock �  d%_mbstate �  h !.  �      .  �   �  __FILE C  (_glue !4  %_next #4   %_niobs $�  %_iobs %:   �  �  (_rand48 =~  %_seed >�   %_mult ?�  %_add @�   
  �  �   
  �  �   
  �  �   
X  �  �   
�  �  �   
  �  �   )�     �  =	  )   �     �  
C    �   
  ,  �   HINSTANCE__ 	�P  	unused 	��    HINSTANCE 	�a  ,  HMODULE 	�P  _SECURITY_ATTRIBUTES 
    �   (_KSYSTEM_TIME 
|    *�  � KUSER_SHARED_DATA 
,CW_GETPINFO_FULL ,CW_INIT_EXCEPTIONS ,CW_GET_CYGDRIVE_INFO 
  !  h  �  �   T  
~  ~  �   �  )�  �    �   k  per_process_cxx_malloc  g  	oper_new v   	oper_new__ v  	oper_delete   	oper_delete__   	oper_new_nt �  	oper_new___nt �  	oper_delete_nt �  	oper_delete___nt �   �  !�  �  �  �  �   m  -MTinterface ��  .concurrency ��   .threadcount �x  .pthread_prepare �$  .pthread_child �$  .pthread_parent �$  /Init �_ZN11MTinterface4InitEv B  H  �   /fixup_before_fork �_ZN11MTinterface17fixup_before_forkEv �  �  �   0fixup_after_fork �_ZN11MTinterface16fixup_after_forkEv �  �    �  thread_magic_t _C  -callback �$  .cb ��
   .next �$   �  MainFunc H  1std  g  2nothrow_t Csize_t ��  E   !  v  S   g  !  �  S  �   a  |  )�    �   �  3�  c�  7  4_vptr.verifyable_object B   magic f�  �  h     ^  �   5~verifyable_object i�  +  ^  �    !�  B   H  
  z   B Aisalpha_array  o   C_data_start__ '  C_data_end__ '  C_bss_start__ '  C_bss_end__ '  C__CTOR_LIST__ )�
  C__DTOR_LIST__ *�
  DCYGTLS_PADSIZE �  �1ESharedUserData 
X86_TUNE_USE_HIMODE_FIOP X86_TUNE_USE_SIMODE_FIOP X86_TUNE_USE_MOV0 
__CTOR_LIST__ �   .�h
__DTOR_LIST__ �  .�h                                                                                                                                                                                                                                                                                                                                                                                                                                                                       %  $ >   :;I     :;  

:;  'I   I  
! I/  :;  
I  ! I/   I  
4 :;I?                                                                                                                                                                                                                                                                                         b  �
.�f?�tL���~t����~��.�;//M�Yz�#Y�~����Yg��XLx[�~ �tYof�{t��tL���}t��<�}��.�;//M�Yz�#Y�}����Yg��tL���}.
�퇼� w��}f�J�;//K#Y~�Zt�'JL�~tY�}���tL
    )�h� t=��fg.n<w
t.t<s<��uv{@uuuux2uuuu� f 
 s� ##��   �   
 s� ##�           !    q v "�!   =    R�   �    R           #    p q v 4�#   5    s q v 4��   �    s q v 4�        A   �    U�   �    U        �   �   
 �               �   �    P�   	   s �  �   s         �   �    r �   �    r #��   	  
 s� ##��  �  
 s� ##�        �   �    q v "��      R�  �   R        �   �    p q v 4��   	   s q v 4��  �   s q v 4�          /   P�  �   P          /   R�  �   R        >  B   PB  Y   UY  o   �H}  �   U�  �   	��        p  }  
 �       �  �  
 �       �  �  
 �                     P  ?   s �  �   s �  �   s              r      R             q r "�  $   V$  ?   q r "��  �   q r "��     V          .   P.  ?   s q r 2&��  �   P�  �   s q r 2&��  �   P�  �  	 s v 2&�        ?  �   �\�  �   �\        I  M   PM  �   U     U�  �   U        d  f   Pf     W  �   W�  �   0��  �   W        Z  f   Pf     W  �   W�  �   P�  �   W�  �   W        �  �   W�  �   0�        f  m   W            
 �       y  �  
 �       �  �  
 �       �  �  
 �                  3   U3  >   s >  \  	 s u 2$��  �  	 s u 2$�V  b   s         &  +   r +  1   r #�1  \  
 s� ##��  �  
 s� ##�V  b  
 s� ##�        .  1   p v "�1  \   R�     RV  m   R        6  _   U_  a   u��     UV  n   U        \  <   �\1  Q   �\        a  �   U�  L   �H1  V   U        z  }   P}  �   �L  V   �L        �  �   P�  �   �T  1   �T1  5   P5  =   �L#=  V   �T        �  �   1��  �   s 1$#��  �   s1$��  �   s 1$#��  �   s1$#��  �   s 1$#��     s1$�     s1$#�>  N   1�N  U   u 1$#�U  u   u1$#�1  V   1�        �  �   0��     S>  N   0�N  U   UU  �   u��  �   U1  V   0�        <  D   PD  �   �\  1   �\Q  V   P        �  �   �L  #   �L#  .   Q.  1   �L        �  �  
 �               �  �   V�  �   s   
 s� ##�  
 s� ##�        �  �   r w "��  �   P�  �   r w "�     r w "�        �     V�  �   V�     V        �  �   Q�  �   Q�     �T        �  �   P        �  �   R�  �   �X�  �   R        �  �   Q�  �   �T�  �   Q        �  �   S�  �   � �  �   S�  �   �           2   P2  6   q ��6  �   �_���          T   P�  �   P          E   PE  m   V�  �   p ����  �   V        i  m   P        �  �  
 �               �  �  
 �               0  C   PC  Y   s �  �   V�  �   v�3	  ?	   s         6  ;   r ;  A   r #�A  Y  
 s� ##�3	  ?	  
 s� ##�        >  A   q v "�A  a   R3	  J	   R        >  C   p q v 4�C  Y   s q v 4�3	  ?	   s q v 4�        e  {   Q	  '	   Q'	  3	   �X        {  �   R�  �   R�  �   �X        �  �   P�  	   	��        �  �   W�  �   P�  	   W        �  �  
 �               a	  t	   Pt	  �	   u �	  �	   W�	  �	   w��	  �	   u 
  '
   u         g	  l	   r l	  r	   r #�r	  �	  
 u� ##��	  �	  
 u� ##�
  '
  
 u� ##�        o	  r	   q w "�r	  �	   R�	  �	   q w "��	  �	   uw "��	  �	   q w "�
  2
   R        o	  t	   p q w 4�t	  �	   u q w 4��	  �	  
  '
   u q w 4�        �	  �	   R�	  �	   R�	  
   �\        �	  �	   0�
  
   	��        �	  �	   S
  
   S        �	  �	   P
  
   P        �	  �	  
 �               Q
  d
   Pd
  z
   u �
  �
   W�
  �
   w�     u         W
  \
   r \
  b
   r #�b
  z
  
 u� ##�    
 u� ##�        _
  b
   q w "�b
  �
   R  '   R        _
  d
   p q w 4�d
  z
   u q w 4�     u q w 4�        �
  �
   R�
  �
   R�
  �
   �\        �
  �
   P�
     	��        �
  �
   S�
  �
   P�
     S        �
  �
  
 �               A  T   PT  j   u �  �   W�  �   w�     u         G  L   r L  R   r #�R  j  
 u� ##�    
 u� ##�        O  R   q w "�R  r   R     R        O  T   p q w 4�T  j   u q w 4�     u q w 4�        v  �   R�  �   R�  �   �\        �  �   0��     	��        �  �   S�  �   P�     S        �  �  
 �               0  C   PC  Y   s 
 s� ##�
 s� ##�        >  A   q v "�A  a   R
 �       �  �  
 �       
 �               N
 s� ##��
 s� ##�        \
 �                �
 s� ##��  �  
 s� ##�        �
 �       �  �  
 �       �  �  
 �               �     P     s �  �   s         �  �   r �     r #�    
 s� ##��  �  
 s� ##�        �     q v "�  !   R�      R        �     p q v 4�     s q v 4��  �   s q v 4�        "  %   p%  0   P        1  P   P        z  �   U�  �   U�  �   ��        b  o  
 �               �  �  
 �       �  �  
 �                 -   P-  =   r E  M   r         #  (   q #�(  Y   s�        #  (  
 q 2$r"�(  =  	 s 2$r"�E  M  	 s 2$r"�        #  (   p rq 2$4�(  -   p rs 2$4�-  =   r s 2$r4�E  M   r s 2$r4�        g  t   �X          �   r��  =   �\#�        �  �   v u "��  �   P�  �   v u "�        �  �   �Xv u 4�        �  �   P�  �   V�  �   P�  :   V        0   _    � e   r    � x      �         0   \    �e   o    �x      �        0   U    �e   h    �x      �        0   }    1�}   �    P�   �    1��   �    P�   �    �l�       1�      P                �   �   S�  �   P�     S           �    P�  �   P           �    0��  �   1�                                                                                                                                                      +   �   �   �           �   p  �  �  �  �          p  u  �  �  �  �                y  �  �  �  �          D  G  I  d          D  G  I  V  X  Z          �    m  y  �  �              y  ~  �  �  �  �          >  �  �  V          \  �    V          �  �    1          �  �  �  �  �            �  �  �  �  �  �          �  m  �  �            m  �  �          O  �  �  3	          �	  �	  �	  
          p
  �
  �
            `  �  �            O  �  �  �  �  
   �  �                                                                                                                                                                                                                                                    .file      ��  gcygming-crtbegin.c    g           _obj                {               �   @           �                                �   �       .text                         .data                           .bss                                �          h                .rdata         _                     �                         .jcr    @       .file   B   ��  gIO.c                  �                               �   �                      *  0          =  �          U  @	          k  p
          �  `          �  P          �  @
   �  �                +         �                    9         "                    D      	                        S                              a                            m      
   �                   +  �                          D      	                       a                            �   0                      .file   r   ��  gdll_entry.c           x  `                           �              �  $           �  (           �  �          �             �  p      .text   `                    .data                           .bss                                 ��  
   �                   +  �     �                    9  "     �                     D  8   	                        a  6                        �   D     2                     �        �                .file   v   ��  gfake              .text   �                     .file   z   ��  gfake              .text   �                     .file   ~   ��  gfake              .text   �                     .file   �   ��  gfake              .text   �                     .file   �   ��  gfake              .text   �                     .file   �   ��  gfake              .text   �                     .file   �   ��  gdll_main.cc           �  �                       .text   �                      .data                           .bss    ,                              :�  
   �                   +  \     �                     D  X   	                        a  8	     �                    �   x     2                     �   �     ,                .file   �   ��  gfake              .text   �                     .file   �   ��  g    
                �  �                       _u.25190@       .text   �     3                .data                           .bss    @      �                       ُ  
   2  
                 +  �     �                    D  x   	                        a  �	     D                   m  �  
   �"                   +  �     ?                    9  �     s                     D  �   	                        S                             a  *     e                   m  �  
   4                   +  
   4                   +  e
   4                   +  �
   4                   +       O                     D    	                        a  �     k                    �   �     2                     �   �     ,                .file   B  ��  gfake              .text   �                     .file   F  ��  gfake              .text   �                     .file   J  ��  gfake              .text   �                     .file   N  ��  gfake              .text   �                     .file   R  ��  gfake              .text   �                     .file   o  ��  g    �                �  �                       .text   �                      .data   @                        .bss                                 ��  
   
                   +  R     (                     D  8  	                        a  ;     s                    �   �     2                     �   �     ,                .text   �      .data   @       .bss          .idata$70      .idata$5|      .idata$4�       .idata$6�      .file   }  ��  gfake              hname   �       fthunk  x      .text   �                       .data   @                        .bss                           .idata$2                      .idata$4�       .idata$5x      .file   �  ��  gfake              .text   �                       .data   @                        .bss                           .idata$4�                       .idata$5�                      .idata$7<     
   f                   +  z     s                     D  X  	                       a  �     �                     �                         .file   �  ��  gcygming-crtend.c      �  $          �  @           �  �                       .text   �                       .data   @                        .bss                               �   $     <                .jcr    @                           �  �     	                    �                           �   ,                      .text   �      .idata$7L      .idata$5�      .idata$4�       .idata$6(      @feat.00   ��   .text   �      .idata$7P      .idata$5�      .idata$4�       .idata$68      @feat.00   ��   .text   �      .idata$7T      .idata$5�      .idata$4�       .idata$6H      @feat.00   ��   .text   �      .idata$7X      .idata$5�      .idata$4�       .idata$6\      @feat.00   ��   .text   �      .idata$7\      .idata$5�      .idata$4�       .idata$6l      @feat.00   ��   .text   �      .idata$7`      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   �      .idata$7d      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text          .idata$7h      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text         .idata$7l      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text         .idata$7p      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text         .idata$7t      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text          .idata$7x      .idata$5�      .idata$4�       .idata$6       @feat.00   ��   .text   (      .idata$7|      .idata$5�      .idata$4�       .idata$6      @feat.00   ��   .text   0      .idata$7�      .idata$5�      .idata$4�       .idata$6(      @feat.00   ��   .text   8      .idata$7�      .idata$5�      .idata$4�       .idata$6<      @feat.00   ��   .text   @      .idata$7�      .idata$5�      .idata$4�       .idata$6P      @feat.00   ��   .text   H      .idata$7�      .idata$5�      .idata$4�       .idata$6l      @feat.00   ��   .text   P      .idata$7�      .idata$5�      .idata$4�       .idata$6|      @feat.00   ��   .text   X      .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   `      .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   h      .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   p      .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   x      .idata$7�      .idata$5�      .idata$4�       .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4       .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4      .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4      .idata$6      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4      .idata$6      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4      .idata$6,      @feat.00   ��   .text   �      .idata$7�      .idata$5�      .idata$4      .idata$6@      @feat.00   ��   .text   �      .idata$7�      .idata$5       .idata$4      .idata$6P      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$4      .idata$6`      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$4       .idata$6p      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$4$      .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$4(      .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$4,      .idata$6�      @feat.00   ��   .text   �      .idata$7�      .idata$5      .idata$40      .idata$6�      @feat.00   ��   .text            .data   @       .bss          .idata$78      .idata$5�      .idata$4�       .idata$6      .text            .data   @       .bss          .idata$74      .idata$5�      .idata$4�       .idata$6      .text            .data   @       .bss          .idata$7,      .idata$5x      .idata$4�       .idata$6�      .idata$2        .idata$58      .idata$4P       .idata$2(       .idata$5�      .idata$4�       .idata$7�      .idata$58      .idata$4P       .idata$6       @feat.00   ��   .idata$7�      .idata$5<      .idata$4T       .idata$6,      @feat.00   ��   .idata$7�      .idata$5D      .idata$4\       .idata$6H      @feat.00   ��   .idata$7�      .idata$5H      .idata$4`       .idata$6T      @feat.00   ��   .idata$7�      .idata$5L      .idata$4d       .idata$6`      @feat.00   ��   .idata$7�      .idata$5P      .idata$4h       .idata$6t      @feat.00   ��   .idata$7       .idata$5T      .idata$4l       .idata$6�      @feat.00   ��   .idata$7      .idata$5X      .idata$4p       .idata$6�      @feat.00   ��   .idata$7      .idata$5\      .idata$4t       .idata$6�      @feat.00   ��   .idata$7      .idata$5`      .idata$4x       .idata$6�      @feat.00   ��   .idata$7      .idata$5d      .idata$4|       .idata$6�      @feat.00   ��   .idata$7      .idata$5h      .idata$4�       .idata$6�      @feat.00   ��   .idata$7      .idata$5l      .idata$4�       .idata$6�      @feat.00   ��   .idata$7      .idata$5p      .idata$4�       .idata$6�      @feat.00   ��   .idata$4�       .idata$5t      .idata$7       .idata$44      .idata$5      .idata$7�            `            @E�h��       8  P          J  �          e            q  H          ~              �      ��       �            �  �      _free   �          �  8          �             �  �          �  �            �          *             ?  �          L  �          j               y  �          �  �          �  �          �  @          �  @          �  L                ��       ,            A            V      ��       n     ��       �      ��       �      ��       �     ��       �               �  �          
  �                         +  �          I               Y             o  �          �  0          �  `          �  �          �          i                    �  �          �            �  (                           @      __ZdlPv         i$                    6     ��       N        _environ          ^  �          m  �          �      ��       �      ��       �               �            �             	               	  8      __dll__     ��       $	  �          7	      ��       L	  D          Z	    �h��       i	            	  �          �	  l          �	     ��       �	      ��       �	  �          �	  x          �	  @          

             
  8          '
      ��       S
  @E�h��       q
  �          �
  �          �
  D           �
             �
  �          �
            �
            �
               �
  h            �                         (  �          =  �          M  (      _poll   �          \  <          x             �  \      _calloc �          �  �      __fmode            �      ��       �      ��       �  �            h                       0              C      ��       n  �          ~     ��       �  �          �      ��       �  �      _realloc�          �  x          �  d          
$       PE  L 
   >              @ 0�.reloc     �      H              @ 0B/14         �      L              @ 0@                                                                                                                                                                                                                                                                                                                                        U����$ @�h�,  ���Ҹ    t�D$/@�h�$�,  ����t�D$ `�h�$ P�h�С C�h��t:�$E@�h�s,  ���Ҹ    t�D$S@�h�$�],  ����t	�$ C�h���Ív U����$ @�h�.,  ���Ҹ    t�D$g@�h�$�,  ����t	�$ P�h���Ð�������������U���(�E�u��]�}��HH�����HH�H�4�)�����u>��   �4���`  �J��x  ��  ���  �E׉X��]�u��}���]ËU�D$�@�h�$�T$��+  ��    ��    U��VS���]�SH��
���SH�S�4�)�����uz�A�4�    ���T��@	tW�@�H��
�I�L$�R�$�D$�T$�|,  s�3��[^]À廉Hs�3��[^]��$�@�h�+  �$�@�h�+  �E�D$�@�h�$�D$��*  ��&    U��WVS��<�]�SH��:���SH�S��)������  ����    �E�C���@ ��   �H�C�4��B��t
@t�t$�$�*  �w�E�C���<[^_]é    t��T$�U��$�g*  �U��f��U��$�
  �U����^�����H  �T$�U��D$	   �D$	   �D$�$��)  �U���t���C����t& �B�0����E�D$�@�h�$�D$�)  �U���8�]�]�u��}��CH��8���CH�C��)�������   �K����    �U���A tX�q�K�4��|���B�t$�$%   �����D$�T$�F*  �F
@t�t$�$�l)  �w�E�C��]�u��}���]ÉU��$��)  �U��ƋC뜋E�D$�@�h�$�D$��(  U���(�]�]�u��}��SH�{��
���SH��)�����uQ�4�   ���@  � u��`  �s�3�]�u��}���]Ív �D$V   �D$�$�(  ��tˍ�x  �ɋE�D$�@�h�$�D$�A(  ����'    U���8�]�]�u��}��SH��:���SH�S��)�������   ����    �E��C���@ �U���   �P�C�4��E�1�1��@  � uH�<�����{�T$�L$�t$�$��(  �F
@t�t$�$��'  �w�E�C��]�u��}���]ÉU܉M؉D$�$�(  �U܋M؄�t��   1�듍�&    �$�P(  ���m����E�D$�@�h�$�D$�2'  �v ��'    U���(�]�]�u��}��SH��
���SH�S�4�    �<2)�����u"�D��$�D$��'  s�3�]�u��}���]ËE�D$�@�h�$�D$�&  �U���8�]�]�u��}��SH��2���SH�S��)�������   ����    �E�C�<��@ ��   �P�C���G�  � u}��u(�C��H  ���E�C��]�u��}���]Í�&    �G�4�����s�T$�U��D$�D$    �$�@'  �U��B
@uO�V�E�C��]�u��}���]Í�&    �U��|$�$�&  �G�U��i�����    �$�&  ���J�����T$�$��%  �U�렋E�D$�@�h�$�D$�%  ���&    U���8�]�]�u��}��SH��2���SH�S��)�������   ����    �E��C�<��@ ��   �P�C���E�G�  � u��u%�C��H  ���E�C��]�u��}���]Ít& �D$    �G�4������$�D$��%  �$�D$�E�D$��%  �E�s�@
@uC�F�E�C��]�u��}���]É|$�$��$  �G�m�����&    �$�p%  �E��L����D$�$�$  �E�묋E�D$�@�h�$�D$�@$  ���&    U���8�]�]�u��}��SH�K��2���SH��)������  �C����    �<��@ ��   �H�C���G�  � ��   ��t	�G�@
u�C��H  ��S��]�u��}���]ÉU�4������M��D$   �D$�$�$  �M��$�L$�D$�$  �M�s�U��A
@uM�NS��]�u��}���]ÉU�M��|$�$�#  �G�M��U��\����U�$�%$  �U���9����v �L$�$�\#  �M��U�럋E�D$�@�h�$�D$��"  ��t& U��WVS��L�]�CH�����CH�C�4�)�������  �y��    �UЍ�2�uԋt��F	�0  ��@�E�F�ẺUȉ$�#  �Uȉ�C���x��   �U�D$�$�T$�ỦT$�#  �F t
��  �Uԋ�P�@�A�Q�C���H   ���   t�U��B  � ��   �F  � ��   �E�C���L[^_]ËS�������   ��@�Z�C���H "  뢍�&    �D$�D$   �$��"  �C���������E��D$   �D$�t$�$�"  �U�S�E�������&    �t$�$�t"  ���]������   �P����D$    �D$    �D$t   �D$    �C���$�D$��!  �����T$�$�"  ��u�������v �EԉU��D$   �$�D$�v!  �U������S�����
tS�Uԋ�P�@�A�Q�C���H   �C���H   ��|�����EԉM��D$   �$�D$�!  �M��F����EԉM��D$   �$�D$�!  �M�뙋E�D$�@�h�$�D$�   ���&    U��WVS��,�]��E؋CH�8���CH��    C)E��}����   �W�U�tu�}�~U}؋u�����t& �K  �u���}��E����E�f�E��m��]��m�S�E�E܍��:���C�8������U܋M؍D
���C���,[^_]É$��  �$��  ƃ�  �o����v ��B�hU��WVS��l�U��E؋BH����BH�B�K�M̍�)M؋M�}��u؋	���I�M��  �M̋u����Mȋ<��O����  ������  ��  ���  ���  ��E�    �i�}���  �Uԍ4�   �U��E�   ��   �K�A��   �E��]��U��D$    �D$   �\$�|$�$�8  �U����E��^  �H����  �����  �I���x  �EЅ�����t@�C��t�K�ߺ   �Au(��������:  %  �=  ���  ���1��h�E����]�9]���   �MԋA�ҋ0�7����C����������   %  �=  ��c  ��h��t*�ًG���  %  �=  ���  ��h���t& ������   �UЅ���1҄�E����ك��E��]�9]��g�������؋UԋB��؋]̉<��E�B���l[^_]Ëw�E�   �F�E��������������6  ��E�    �A� ������tn��@�3�����EЅ����e����v ���H  �MЅ����{�������   ��AA�����W����MЅ����R���f�����   ��@������E��]��U��D$   �\$�$�   �U��E��������v ��h�@���   �]��E�������t& ����   � �   �@��������E��e����C�8����t& �؋]̋u̍�    ��H  ��J�
��l[^_]�����   ���1��@������t& �M��]��D$   �|$�$�F  �E���������H�h����   �]��E�������@�80�������f��D$�E��]��U��D$   �$��  �U����E��������v ��B�h�������t& �Mԉ��D$   �\$�$�  1�������P�h��xa�]����E�1��������B�h�^�����i�I��xC�]��E��E�    ������$�U��D$   �|$�U  �U��E�    �B�������B�h���B�h뵍v U��WVS��\�]��{�EЋCH�MЋ���CH��)����ɉM��  ���U���׃}��Uȋ��  �U�T$�U��D$    �T$�D$�$�  ���E��O  � �@,�$�E��\  ��|  ���   �$�E��C8�D$�E  �C4�C8�C�D$�$�H  ��|  ƀ�   ��t  �p����  ��F�����F   � �@    ��p  ��E�+C���B��P�S�� �@���C�� �@������t  ��p  �F;F��  ���F��t  kp8p��+C���F��l  �F�CH+CD���F�C�F��8  �F�M̉F�N��@<�F    �F(��@<���  �C�@f%� f�F�E̋�B<�����B<~�ŰM���@<�L$�$�D$��  �D$!   ���  �   �$�D$��  �ŰM���P<�A���Ủ��  �@�C��@�EЋ��  �@�$�D$�  ��    ���  ���MЋ@��K�$��D  �� ���  �P����   ����  �R���  �M̋�B<�����B<�O  ��t  �Jk�8��B�J�P��l  �P��SD�SH�P�S�@���8  ��t  �@����  ��p  +S�	���Q��R�S��	�I���K��	�I������t  ��|  ��p  �U����   �$�  �Cuċ��Mĉ��E�C���\[^_]����  � �PP����������9u�������E̋�B<�����B<u�ŰB����  �����B��  ��t  �Jk�8��B�J�P��l  �P��SD�SH�P�S�@���8  ��t  �@����  ��p  +S�	���Q��R�S��	�I���K��	�I������t  ��|  ��p  �U����   �$�r  �C��H  ������t& ��t1� �   �@��������E��������������@�80��������D$   �D$�$�N  ����������H  ��CЉ��\[^_]Ð�t& �$�x  �F�L����A����   �����A������L$�$�3  ������    �A�D$   �L$�$�  �d����T$�$��  �l����D$#   �D$    �$�Z  �Ƌ�t  �F��t  �p�O����T$�$�  �*����Ẻ$�D$�  ������E�D$�@�h�$�D$�  �$�@�h�  ���  ��t�x	t(�$�Y  �D$A�h�$�A  �D$   �$�  �@�@��t΋ �@��u��Í�    ��    U��WVS��\�]��E̋CH�ű���CH�C��)������u��~  ���U����U��U��4��$��  �}�{�}��E���  �E�D$�E��D$    �D$�t$�$�l  ���E��;  � �@,�$�E��  ��|  ���   �$�E��C8�D$�  �C4�C8�C�D$�$�  ��|  ƀ�   ��t  �p����  ��F�����F   � �@    ��p  ��E�+C���B��P�S�� �@���C�� �@������t  ��p  �F;F��  ���F��t  kp8p��+C���F��l  �F�CH+CD���F�C�F��8  �F�F�EĉF� �@<�F    �F(��H<����  �C�M��@f%� f�F��P<�����P<~��uȋ@<�t$�$�D$�  �D$!   ���  �$�D$�  �UċMȋuċ�P<�A�����  �@�C��@�D$   �D$   �D$A�h�Eȉ$��  �D$   �D$   �D$!A�h�$����  �E̋F�$�D$�:  �ŰB�$�D$�(  �F�Mԉ�G9�t�D$   �D$�L$�$�  �}�tQ�   ��    �ŰMȋB����K�$��D  �� ;E�t�D$�E��D$   �$�D$�R  ��9u���Mċ�B<�����B<u�A����   �����A�   ��t  �Jk�8��B�J�P��l  �P��SD�SH�P�S�@���8  ��t  �@���9  ��p  +S�	���Q��R�S��	�I���K��	�I������t  ��|  ��p  �U����   �$�  �C�uԋM��4��E�C���\[^_]Í�H  ��E�C���\[^_]Í�&    �$�  �F�����uĉ$�t$��  ������B�D$   �T$�$�\  �U����L$�$�  ������D$#   �D$    �$�  �Ƌ�t  �F��t  �p�>����E�D$�@�h�$�D$�n  �$�@�h�Z  ���  ��t�x	t(�$�*  �D$A�h�$�  �D$   �$�  �@�@��t΋ �@��u��Í�    ��'    U��WVS��<�u��E��FH����FH�F�J��)]��]�}��}�����[��   �����}��M؍�]܋~^�   �u�<�   ���
  �UԋM����Uċ��B��t
  � �@   �D$    �D$0A�h�D$2A�h�D$��h�D$MA�h�$�>
  � �@    �D$    �D$0A�h�D$2A�h�D$/�h�D$]A�h�$�
  �D$    �D$0A�h�D$2A�h�D$ .�h�D$mA�h�$��	  � �@   �D$    �D$0A�h�D$2A�h�D$ .�h�D$�A�h�$�	  � �@    �D$    �D$�A�h�D$2A�h�D$�(�h�D$�A�h�$�c	  �D$    �D$�A�h�D$2A�h�D$�"�h�D$�A�h�$�3	  �D$    �D$0A�h�D$2A�h�D$ �h�D$�A�h�$�	  �D$    �D$�A�h�D$2A�h�D$@�h�D$�A�h�$��  �D$    �D$�A�h�D$2A�h�D$��h�D$�A�h�$�  �D$    �D$�A�h�D$2A�h�D$��h�D$ B�h�$�s  �D$    �D$�A�h�D$2A�h�D$p�h�D$B�h�$�C  �D$    �D$�A�h�D$2A�h�D$ �h�D$,B�h�$�  �D$    �D$�A�h�D$2A�h�D$��h�D$AB�h�$��  �D$    �D$�A�h�D$2A�h�D$��h�D$VB�h�$�  �D$    �D$�A�h�D$2A�h�D$��h�D$mB�h�$�  �D$    �D$�A�h�D$2A�h�D$P�h�D$�B�h�$�S  �D$    �D$�A�h�D$2A�h�D$0�h�D$�B�h�$�#  �D$    �D$�B�h�D$2A�h�D$p�h�D$�B�h�$��  �D$   �D$
   �D$�B�h�$�  �D$    �D$    �D$0   �D$   �D$�B�h�D$�$�E��l  �U�� �x	t.�D$�E��D$   �D$   �D$�B�h�T$�$�&  �E܋P�
��tV��x  �E��D$   �D$��$�D$�  ���  ��t�D$�C�$�D$�  �C�U��{�;��<[^_]��D$    �D$�$�  �P두��U����$`�h�D$� `�h�D$�`�h�$�  ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$�   ����tދ(`�h�E�$�   �E��(`�h����뾐�t& �M�E   �U��}   1������D$�7�h�$�`�h� `�h   �
�U ǀ�       ǀ�     �@,�>�h�@0�>�h�@ a�h�p(�@HP<�h�@L`<�h�@Pp<�h�@T�<�h�@$a�h��@�<�h�@�<�h�@ �<�h�@D�<�h�����Px�Å��;���������v �B�@�h�����v �B�@�h�����v �B�@�h�f����v �B�@�h�H����v �B�@�h�+����v �B�@�h�
 a b v5.14.0 1.23 @ ListUtil.c List::Util::max List::Util::min List::Util::sum List::Util::minstr List::Util::maxstr &@ List::Util::reduce List::Util::first List::Util::shuffle $$ Scalar::Util::dualvar $ Scalar::Util::blessed Scalar::Util::reftype Scalar::Util::refaddr Scalar::Util::weaken Scalar::Util::isweak Scalar::Util::readonly Scalar::Util::tainted Scalar::Util::isvstring  Scalar::Util::looks_like_number &$ Scalar::Util::set_prototype List::Util REAL_MULTICALL      �  �_                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    �#�O    �p           (p  pp  �p  �"  �   .  �(     /  �  @  P  �  0  �  p  �  p  �     `2  �p  �p  q   q  5q  Kq  ]q  uq  �q  �q  �q  �q  �q  r  )r  Gr  _r  vr            	 
   
==="=*=2=:=B=J=R=Z=b=j=r=z=�=�=�=�=�=�=�=�=�=�=�=�=�=�=�=�=>
>>>">*>2>:>B>J>R>Z>b>j>r>z>�>�>�>�>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          Util.dbg    ~��                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1              ?                                U   �       .text          �                .data                            .bss                            .rdata         _                     m           .jcr               w   �                            �   p          �   0          �   �          �   P            �                       7  p          P  �          i  �          �  @
          �   
             *
  �          >
            W
  <          r
  (           �
  �          �
              �
   PnX��       �
  �          �
     ��         �      _environ                       /  �          G             `            t  �          �  �      __dll__     ��       �      ��       �    nX��       �  8          �     ��       �  T          �  �            X            �          2      ��       ^   PnX��       |  �          �  �          �  �          �            �  �.          �            �  �          �            	
$       PE  L 
 t�D$�E�$�<  �C� �@+E�C    �C��[]ÐU��S���E�1��C	t�C�@
 u� �@��[]ÉD$�E�$��  �C��t& U��S���E�1��C	t�C�@
 u� �S�H1���)�9�GÃ�[]ÉD$�E�$�  �C�Ӎ�    U����E�u��u�]��1��C	t�C�P��    u��t(�@�]��u���]Ít& �D$�4$�D  �C�P��u؉4$�D$   �D$    �D$�E  �]��u���]Ív U1����X�U�}��}�]�u��2�F	��   �^�C
 �  �E�U��D$    �\$�$��  �Ű�@	 ��   ��B�9J�-  �K�E��E��}��E�    �U�E��E��E�    U�U�F�V�E��U�|$��T$�$�	  ��V;Pv�P�C��D�  @ �Cuf���]�u��}���]�f��F�V�}���E�    �E��E�E��UċU�UĉE؋A�U�1҃}� �EЉU�}G�K�E��U�E�UĉF�V�d����U�\$�$��
 ��   ����R�U���   ����   ��uh�_�w����   9]ԉ]�r#�p����   �H D  1�1��]�u��}���]Ë;Ywx�M��U�P+M��D$    �$�L$�   �G뼍v �_�w떉D$�E�M܉$�  �G�M܋���R�U��Z���_w�_�w�\����U�1����H����D$�E�\$�$�  �G�m�����t& ��Z���<����U�D$   �D$�$�  �G�����U�D$   �$�  ��t�M�D$ 1�h�D$   �$�Z  �=  ������    �������������&    U���(�]�E�]�u��}��$�\$�O
  �׋�Ɖ��b�������]�u��}���]ÐU��S���E�M��C��t�P��t���҉Pt�C    ��1�1�[]ÉD$�$�
  ��D$�$�
  �Ӎ�    ��    U��WVS��<�E�u�}���t�P���  ����  �D$    �D$1�h�<$�
  �C�P����  �E�M�D$    �t$�D$�L$�<$�y	  �E��C�U�P��t`���ug�D$�D$    �<$��	  �C� �@    ��P�C�� uI�C    �C    �@
@t�D$�<$�n	  �E��U��<[^_]ËP�B �  t���R��u��� t��D$�D$    �<$�Y	  �C��J�C    �K띐�t& �H�A �  ���������   �@�P��   t!�M��t��   	��   	t�U�:r��   �@�C�H��    ��   ����   �у��  �������������D$    �D$    �D$�<$�  �C�P�s�����&    �D$   �D$�<$�  �\�����&    ��t3�@�D$   �D$�<$�  ���!����@�����v �������D$   �D$    �D$�<$�  벉D$�<$��  �C�H�!����D$   �<$�  ��t!�Pq�h�D$1�h�D$   �<$�D$��  ��  �    �E������E����������H�A �  ���������&    ��'    U����E�u��u�]��1��C	u
�]��u���]ËC�P��    u<��t�@C�]��u���]Í�&    �D$   �D$    �D$�4$�  ��f��D$�4$��  �C�P밍�    ��    U���H�]�]�u��}��SH��{�E����SH�D$   �D$1�h�p��    �U����E܉$�T$��  �E܋U��D$   �D$"1�h��)����D$�t$�$�  �D$ 0�h�$�  ���  ��t�D$�C�$�D$�  �C��x  ���E�C��]�u��}���]Ð�������������U����$P�h�D$� P�h�D$�P�h�$�  ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$��   ����tދ(P�h�E�$��   �E��(P�h����뾐�t& �M�E   �U��   1������D$ �h�$�P�h� P�h   �
�U ǀ�       ǀ�     �@,`#�h�@0l#�h�@ Q�h�p(�@H�!�h�@L�!�h�@P�!�h�@T�!�h�@$Q�h��@ "�h�@"�h�@ "�h�@D"�h�����Px�Å��;���������v �B��0�h�����v �B��0�h�����v �B��0�h�f����v �B��0�h�H����v �B��0�h�+����v �B��0�h�
   
???"?)?0?9?@?G?N?e?s?�?�?�?�?�?�?�?�?�?�?�?�?      �   000,030:0A0K0[0e0�0�0�0�0�0�0�0�0�0�01
1141D1T1d1t1�1�1�1�12
222"2B2J2r2z2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�23
333"3*323:3B3J3R3Z3d3p3   0  8   00000 0$0(0,04080<0@0D0H0L0P0T0X0\0`0d0h0l0                                                                                                                                                                        scalar.dbg  a�1[                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1   4          ?                                U   �       .text          �                .data                            .bss                            .rdata  �      _                     m           .jcr    4          w   �                            �   �           �              �             �              �   p          �   �          �   �            �          +            A  `          X  �          l  �            p          �            �  P          �  �          �  �          �         .text   �        ;             .data          p                .bss                            .rdata        1                     �   
   @vX��       
  �          1
  �          F
  8          S
  `          a
            m
  `      __fmode           |
      ��       �
      ��       �
  4          �
  0          �
              �
      ��       '  d          =     ��       P  �          c      ��       �  ,          �     ��       �  $          �  <          �            �  �            `            D          4  l          B  8          P      ��       �     ��       �    vX��       �     ��       �            �            �             �  �           
$       PE  L ��S (  
�C�M��D��|$�t$�D$�$�ҋ3�~��ti�D$�$�
  �3�@�������0^ht����0^h��0^hEu�D$�$�t$�Z
  �$�J
  f�U��WVS��L�U�BH�:����BH�B����)ރ��  ���M����MċMԋ��@	��  �@�E���9��E�   �E�    ��   �UЉ��f�� �@�E����qtA��9���   ��@	uۋ}Ѝu�t$�U��D$"   �D$�<$��	  �U��ƋE��u���0^h�   �u��C����   �H����   �����   �I���  ��9��E�   �E�    �v����UЉ$�U��D$   �3	  �M��U��L$�$�ÉD$�#	  �uȋ}̋U��t$�|$�D$�$�n	  �U��}ԋB���B���$�U��D$�
  �D$�0^h�D$@^h�D$�0^h�$�z  �D$�0^h�D$@^h�D$�0^h�$�Z  � �@   �D$�0^h�D$@^h�D$1^h�$�1  � �@ �D$�0^h�D$@^h�D$01^h�$�  � �@ �D$�0^h�D$@^h�D$P1^h�$��  � �@  �D$�0^h�D$@^h�D$p1^h�$�  � �@  �D$�0^h�D$@^h�D$�1^h�$�  � �@ �D$�0^h�D$�^h�D$�1^h�$�d  � �@   �D$�0^h�D$�^h�D$�1^h�$�;  � �@    �D$�0^h�D$�^h�D$�1^h�$�  �E�-   � �@$� �x���$   �D$    �|$�����غ+   �|$�$   �D$    ��������  ��t�D$�C�$�D$�r  �C��x  ���E�C���<[^_]Ð�����U����$P^h�D$� P^h�D$�P^h�$��   ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$�   ����tދ(P^h�E�$�   �E��(P^h����뾐�t& �M�E   �U��m   1������D$0^h�$�P^h� P^h   �
�U ǀ�       ǀ�     �@,0!^h�@0<!^h�@ Q^h�p(�@H�^h�@L�^h�@P�^h�@T  ^h�@$Q^h��@ ^h�@ ^h�@   ^h�@D( ^h�����Px�Å��;���������v �B�0^h�����v �B�0^h�����v �B�0^h�f����v �B�0^h�H����v �B�0^h�+����v �B�0^h�
111"1*141@1                                                                                                                                                                                          NamedCapture.dbg    ��C                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       _obj                1             ?                                U   �       .text          �                .data                            .bss                            .rdata         _                     m           .jcr          _tie_it �                            w   �          �   �          �   @          �   @            �      .text   �      J
  R             .data                            .bss                            .rdata  �      �                    "  0                           -  $           8              F             T  `          j  (           u  @      .text   0                    .data                            .bss                               �  P                       .text   P                      .data                            .bss    ,                        .text   `                         �  p                       _u.8973 @       .text   p     3                .data                            .bss    @      �                     �  �                       .text   �       =             .data                           .bss                           .text   �                     .text   �      .idata$7�      .idata$5�       .idata$4P       .idata$6h          �  �                       .text   �                      .data                            .bss                               �  �                       .text   �                      .data                            .bss                               �  �                       .text   �                      .data                            .bss                               �                          .text                          .data                            .bss                           .text                        .text                        .text                         .text   (                     .text   0                           @                       .text   @                      .data                            .bss                           .text   P      .data           .bss          .idata$7�      .idata$5       .idata$4t       .idata$6�      .text   X      .data           .bss          .idata$7�      .idata$5      .idata$4x       .idata$6�      hname   t       fthunk         .text   `                       .data                            .bss                           .idata$2                      .idata$4t       .idata$5       .text   `                       .data                            .bss                           .idata$4|                       .idata$5                      .idata$7�     
  <          %
      ��       Z
     ��       s
    �X��       �
     ��       �
  �           �
  �           �
              �
      ��         �           
$       PE  L	 ��S �  �  � # v   �     �|      �    Ah                         �                         �  �   �                               �                                                  ��  �                          .text   �t      v                 ` P`.data      �      z              @ `�.rdata  �   �      �              @ 0@/4          �      �              @ 0�.bss       �                      � `�.edata  �   �      �              @ 0@.idata     �      �              @ 0�.reloc  �         �              @ 0B/14              �              @ 0@                                                                                                                                                                                                                                                                                                U����$��Ah�r  ���Ҹ    t�D$ϐAh�$��q  ����t�D$ �Ah�$ �Ah�С�Ah��t:�$�Ah��q  ���Ҹ    t�D$�Ah�$�q  ����t	�$�Ah���Ív U����$��Ah�~q  ���Ҹ    t�D$�Ah�$�hq  ����t	�$ �Ah���Ð�������������U����E�$�t$�PH��H  �X�
���PH���4���    P��$�t$��]��
���SH��)�����ul�<�   ���@	tE� �@�$�n  �����T$�$�D$�{p  �$�D$��p  �{�;�]�u��}���]��D$   �D$�$�p  ��D$|�Ah�$�p  ��t& U���(�]�]�u��}��CH��0���CH�C��)Ѓ��D$   �T$�T$�$�4p  �{��1��$�D$    ��=�Ah���D$��o  �$�D$�p  �s�3�]�u��}���]Ít& U���(�]�]�u��}��CH��0���CH�C��)Ѓ��D$   �T$�T$�$�o  �{��1��$�D$    ��=�Ah���D$�4o  �$�D$�o  �s�3�]�u��}���]Ít& U���(�]�]�u��}��CH��0���CH�C��)Ѓ��D$   �T$�T$�$�o  �{����l  �$�D$    ��D$�n  �$�D$��n  �s�3�]�u��}���]Ít& ��'    U���(�]�]�u��}��CH��0���CH�C��)Ѓ��D$   �T$�T$�$�n  �{���Yl  �$�D$    ��D$�n  �$�D$�on  �s�3�]�u��}���]Ít& ��'    U��WVS��<  �]��Ah�E�1��SH�s��:���SH��)������  ����    ���������@	tM�P�$�������g  ������=  vR��`  ��������C��E�3�Ah��  �e�[^_]Ít& �D$   �D$    �D$�$�m  �s��듍������$�������T$�f  �D�������ǅ����   ��t$�P��tU���t�Rǅ����    ����   �D��@	to�@�������D$�$�T$��j  ����u/�s�(�����&    ��tk��BB���҉�����묐�t& �C��x  ��������C�������t& �������D$   �D$    �D$�$�l  �������g����v ��t[� �@�   ��������D����������+����t& ��ǅ����   �����@�80�����������������&    �������D$   �D$�$�l  �s������������������������D$��Ah�$�rk  �c  ��t& U���(�]�]�u��}��SH��2���SH�S��)�����uW�����<�    �@	t.� �@�$�ii  �C��x  ��{���;�]�u��}���]��D$   �D$�$�Tk  ���D$ԑAh�$��j  �v ��'    U��WVS���]�CH�;�0���CH�C�@����t����  ��t�$�j  ����  �C)�����  ����Ah�
����������!�%����t�������  D��JD� ������Ah�T$�D$�Ah�$�Sj  �$�D$�j  ��C)����|$�|$�D$   �$�Sj  �ǡ�Ah�w�D$    �$�D$��i  �$�D$�Aj  �G�C)�����  ��Ah�~�D$    �$�D$�i  �$�D$�j  �F�C)�����  ��Ah�w�D$    �$�D$�{i  �$�D$��i  �G�C)����.  ��Ah�~�D$    �$�D$�Bi  �$�D$�i  �F���Ah����   �C)����  ���Ah�w�D$    �$�D$��h  �$�D$�Ni  �G�C)�����  ���Ah�~�D$    �$�D$�h  �$�D$�i  �F�C)����R  ���Ah�w�D$    �$�D$�h  �$�D$��h  �G�C)�����   ���Ah�~�D$    �$�D$�Ih  �$�D$�h  �F�;��[^_]Ét$�t$�D$   �$�`h  ��������|$�|$�D$   �$�@h  ���Z�����t$�t$�D$   �$� h  ��������|$�|$�D$   �$� h  ���&������Ah�4�   �{�$�D$    �D$�g  ��$�D$��g  �s�3��[^_]Ét$�t$�D$   �$�g  ���������|$�|$�D$   �$�g  ��������t$�t$�D$   �$�`g  ���4�����|$�|$�D$   �$�@g  ��������U���8�]�]�}��u��CH�3����CH���U��e  �ǋC)�����   ��uB�E���{�\$�4�    �$��f  �$�D$��f  �s�3�]�u��}���]Ð�t& �E�S�|$�$�4�    �U��D$    �Jf  �$�D$�f  �U��s�3�]�u��}���]Í�&    �D$   �t$�t$�$�Pf  �M����v U��WVS��L  �]��Ah�E�1��SH��2���SH�Sǅ����    ��)�������   �����<�    �@	ty� �@�������D$�������T$�������D$    �D$�  �D$    �D$�$   �c  ����������uE�C��H  ��{�;�E�3�Ahug�e�[^_]Ð�D$   �D$�$�te  �s�����&    �s�T$�D$    �$�	e  ��$�D$�Ke  �{�;��D$��Ah�$�d  ��\  ��    ��'    U��WVS��L  �]��Ah�E�1���������������CH�8���CH�D$   �t$�G�������������D$�������D$�D$    �D$    �D$    �$    �b  �� ����   �C�@����t��t����   �$��c  ����   �C+��������������D$   �$�D$�D$�$d  �������S�D$    �t$�<�    ��������$��c  �$�D$�d  �������{�;�
����������!�%����t�������  D��JD� ���)�T$�t$�$�+c  �$�D$�gc  ��C)����|$�|$�D$   �$�+c  �ǋ������w�D$    �$�D$��b  �$�D$�c  �G�C)����t$�t$�D$   �$��b  �Ƌ������~�D$    �$�D$�tb  �$�D$��b  �F�;������������D$   �$�D$�D$�b  ������������`Z  U��VS���]�SH��2���SH�S��)�����uE�����@	t#� �@�$�G`  ������C����e�[^]��D$   �D$�$�b  ���D$�Ah�$�a  �v U��WVS��\  �]��Ah�E�1���CH�������8���CH�$?�Ah��_  ��������t2�D$L�Ah�$�t_  ���������D$]�Ah�4$�[_  ���������C+��������x  ������������t��uk��ǅ����   t�4$�g_  ���������������D$�4$�4`  ������  �S��H  ��    ��C��E�3�Ah�  �e�[^_]Ë��������  �������D$�D$d   �$    ������������  �������B��t
f�8 ��   �������D$    �D$    �D$   �������D$�D$�����B�D$���D$    �$    �}^  �������� �$���������4$�X^  �s����������D$    �T$�$��_  �$�D$�"`  �{�;�������    �������D$   �$�D$�D$��_  �e���ǅ����   �w����������D$    �D$    �D$   �������T$�D$�����!����������D$�������D$�������������D$�������T$������ǅ����   ǅ����   �D$�t$�$    �E^  ���������������s���D$    ��������&    �4$�������]  ���������$��\  ���������V  �t& U���X�]�]��Ah�E�1��u��}��CH��0���CH�C�E�   ��)Ѓ�~I�EЍ}ԉD$�<$�\  ����uJ�S��H  ��    ��C��E�3�Ahu]�]�u��}���]��D$   �T$�T$�$�:^  띋S���EЉ|$�$�UĉD$��]  �$�D$�&^  �Uĉs�3���U  ��t& U���(�]�]�u��}��CH��0���CH�C��)Ѓ��D$   �T$�T$�$�]  �{����[  �$�D$    ��D$�K]  �$�D$�]  �s�3�]�u��}���]Ít& ��'    U��WVS��<�u�FH�����FH�m�Ah���E�F�]�)Ѓ�~r�C   �}��
���SH�S�4�)�������   �����<�    �@	ta�@�$�?Z  ������t.�D$��Ah�$��Y  ����t�Ѕ�tN�4$��    �Z  ���S��`  ��{�;�]�u��}���]��D$   �D$    �D$�$��[  넉4$��Y  �S��x  �����$t�Ah�7[  ��&    U���(�]�]�u��}��SH��
���SH�S�4�)�������   �����<�    �@	ta�@�$�OY  ������t.�D$ܒAh�$��X  ����t�Ѕ�tN�4$��    �#Y  ���S��`  ��{�;�]�u��}���]��D$   �D$    �D$�$��Z  넉4$��X  �S��x  �����$��Ah�GZ  ��&    U��WVS��,�]�KH�s�����KH��)�������   �D��z�@	tv�H�����@	tF� �@�L$�$�
���SH�S�4�)�����u�����4�    �@	tN�@�$�V  �{����$�D$�D$    �7X  �$�D$�X  �s�3�]�u��}���]Í�&    �D$   �D$    �D$�$�\X  ��$L�Ah��W  ��    U��WVS��   �]��Ah�E�1���CH�U��S�0���M��CH��)���������  ���<�    �}����@	��   �@�E��E��D$�E��$�O  ������u+�C��H  ���E�C��M�3
���SH�S�4�)������W  �����4�    �@	�  �@�E��iP  �U�T$�}��D$(   �$�Q  ����t]�E؉D$�E��D$
�Ah�$��P  �E��E�   �E�   ���D$    �D$    �D$    �|$�D$    �$��P  ���Eĉ$��P  �E�    �E���U��|$�D$    �D$    �D$    �D$    �$�zP  �E���$�O  �U��{��������T$�$�D$��P  �$�D$�0Q  �s�3�e�[^_]Ð�D$   �D$    �D$�$�Q  �E�������$ؓAh�P  U��WVS��\�]�SH��2���SH�S��)������W  �����<�    �@	�  �@�E���N  �U�T$�D$(   �$�O  ������  �C�T��B	�N  �R�U��T��B	�  ��R�U��T��B	��   ��J�D��@	��   � �@�U��D$�E��L$�T$�UĉD$�$�+O  �E�    ���ƍEԉD$�E��D$    �D$    �D$    �D$    �$��N  �E���$��M  �S������U����t$�$�D$�?O  �$�D$�O  �U��{�;�e�[^_]Ð�M��D$   �D$�$�aO  �M��5�����D$   �T$�$�DO  ���C����f��D$   �T$�$�$O  �E��C�������D$   �D$    �T$�$�O  �E��C�����E؉D$�E��D$
�Ah�$��M  �E�U��E�   �E�   ���D$    �D$    �D$    �T$�D$    �$�M  �������t& �D$   �D$    �D$�$�|N  �E�������$ �Ah��M  U��WVS��L  �]��Ah�E�1��KH�����KH�Kǅ����   ǅ����   �4�)�������  �D��r�@	�1  �P�$�������M  ����������u3�<�    �C��`  �8{�;�E�3�Ah�<  �e�[^_]Ít& �C�<�    ���@	��   �@�������L$�������L$�������L$�������L$�������L$�������T$�$�L  �����������e����L$�C�D��$�D$�bM  �������D$�C�D��$�D$�EM  �������D$    �L$�C�D��$�D$�M  �C��x  ��{�;����f��D$   �D$    �D$�$�L  �������������D$   �D$    �D$�$�L  ������������KD  �$��Ah��K  �
���SH�S�4�)�������  ǅP����  �qǅH���   �D��@	��   �@��@�����    ��D������@	��   �@��P����L$��@�����L����T$��H����T$��T����������T$��<����|$�L$�$�J  ��<���������   �C��`  ����D���C��M�3
J  �EB  ��t& U��WVS��   �]�CH�3�S�8���CH���E� �E� ��)��������E� �E� �E� �E��
���SH�S�4�)������
  ���=�Ah��   �<��4�    �G����  �G�D$    �D$�   �D$   �D$    �D$   �D$   @�$�JA  ������&  �$�A  �C��x  ��s���3�e�[^_]Ð�<��4�    �G�    �r������   ��O�@�E��E���  ���D$�EԉL$�M��D$    �D$    �D$    �$�@  ��� �$�U��B  �ŰM��D$    �T$�L$�ǉD$�E���D$�Eԉ$�`@  ���D$    �D$�   �D$   �D$    �D$   �D$   @�<$�=@  ���EЉ<$�A  �EЃ��������v �C��`  ��s�3�e�[^_]Í�    �D$   �D$    �|$�$�|A  �U����E��D$   �D$�|$�$�\A  �G ���E�������E�    �����D$ܖAh�$�@  ����'    U��WVS��L�]�SH��
���SH�S�4�)�������  ���=�AhwW�<��4�    �G���  �G�D$    �$�6?  ���K��`  ��x  ��D׉s�3�e�[^_]Í�    �<��4�    �G�    t�����   ��O�@�E��E���  ���D$�EԉL$�M��D$    �D$    �D$    �$�>  ��� �$�U���?  �ŰM��D$    �T$�L$�ǉD$�E���D$�Eԉ$�D>  ���D$    �<$�Y>  ���EЉ<$�?  �E������v �D$   �D$    �|$�$�?  ������E��D$   �D$�|$�$�?  �G ���E��
���SH�S�4�)�������  ���=�Ah��   ���4�    �@	tr�@������D$�{  �|$�$�}2  �S�Ћ�����D$    �|$�$�s3  �$�D$�3  ��Ћ���s�3�E�3�Ah�p  �e�[^_]Ív �D$   �D$    �D$�$�|3  �p�����    ��ԋ���4��V����   ��N�@��������    ������ց���  �L$��̋���4$�D$    �D$    �D$�D$    �Y1  ��� �$��Ћ����2  ��̋����Ћ���4$�������D$    �L$�T$�ǉD$���������D$�1  ���D$�{  �t$�<$�71  ��ԋ��S��Ћ�����t$�$�9�����Ћ����<$�92  ��ԋ��C������������D$   �D$�t$�$�C2  �V���������������)  �$��Ah�1  �v ��'    U��WVS1ۃ�L�$T�Ah��/  ��������   �D$��Ah�$�/  ��������  �0  �U��T$�D$
   �$�׃����`  �4$�/  ���$ΗAh�/  �����E�tB�D$ڗAh�$�2/  �ƋEЃ��D$�Ah�$�/  �����E�u(1ۋUЉ$�S/  ���E��$�}/  ���e��[^_]Å�tԋE��D$    �D$�E�$�փ���t��U�$�K*  �}�Eԉ�f�? ��   �Eԋ]���ۉE��E���E̍|Gf�? ��   �<$�
f��/��   f�����f�  �f��tf��/t�  f��\ux���f��u�U�������D$�$��$  �E�e�[^_]ËUf�������uf�������B��f����t f��\t�  f��\��  ���f��u덽�����5���f��/t���    f��.��   f���t���f��/t f��\u��f��\tf��/t���f��u�f�  �E�������T$�$�u)  �����������f��  �������$��#  �Ë�t���)���9��  �������<$�<_�D$��#  �������$�)  ��������V�.   f��tFf��\t+f��/t%f��.�R����V��f��t%f��\t
f��/�����.   �f�f��\tf��/t��f����f��u�f�  �f��\�N���f��������Ah����������Uf�z:������$���#  ��fǅ����: ������f�������E�X������"  �    �E    �E�e�[^_]Ë������$�(  ���"  � "   �E    �E�e�[^_]�f��/����f��/��   f��\������u�   f��/t��f����f������f��\u�f����t& ���f�������f��\t�f��/t��"�v f��/�������f����f�������f��\u�����f��/f��h�����\���������됍v ��'    U��WVS��\  �]��Ah�E�1��SH��2���SH�S��)������q  ���=�AhwQ���<�    �@	��  ��R�������@��  �o  ������C��E�3�Ah�,  �e�[^_]Í�    �������<��G	��  ��@�������O�W�L$�������D$    ��    ������ׁ���  �D$    �D$�D$    �<$��%  ��� �$�������<'  �������������D$    �<$�T$�L$�������D$���������D$�%  ���������$�`   =  �]   �������������<$�D$�e   �<$��������t7�������$�&  ������s�|$�$�����������C�����f��������$�r&  �����������$�������D$�  �������$�W������c����s�$�������  �������$��T$�D$��%  �$�D$�:&  �{�;�.����������T$�D$   �D$�$�&  �������������t& �������D$   �D$�|$�$��%  �������������D$�Ah�$�M%  �  U��WVS��L�]�E�    �$��  �D$    �D$    �D$    �D$    �\$�x�|$�D$   �$    �v#  �� �$�E��0%  �UЉ|$�D$    �\$�T$�D$   �ƍE�D$�t$�$    �0#  �}�� ��u
�e��[^_]��D$    �D$    �$�#  ������tՍ �$�$  �|$�$�EԉD$�]#  �X�Eԃ��\$�D$    �D$    �D$    �D$    �D$�D$   �$    �"  �� �ǉ4$�D$�W$  �|$�\$�D$    �D$    �D$   �ƉD$�E��$    �D$�N"  �Eԃ� �$� $  �e��[^_]Í�    U��WVS��  �]��Ah�E�1��CH�;�S�0���CH����)���������  ���=�Ah�#  ��    ���������@	�L  �@�������T$�������D$    �D$  �$�M"  ������ǅ����    ����=  ��  �=�Ah�  �Cǅ����   �@����t+��t&����  �������$�"  ����������  ������{�T$�D$    �$�"  �$�D$��"  ���������t�������$�"  �������D���C��M�3
��8\�L�����)с�  �<���f�@\ �1����$�D$\   ��������  ���������C)�����   ����   �A�<�   {�������������D$    �D$�$��  �$�D$�#   ��������A ������ǅ����   �����f��$�8����=�Ah����������������������T$�������D$   �D$�$��  �������J�������������!����������D$   �|$�|$�$�j  �������������
���SH�S�4�)������;  ����    �Eԋ4��V����   ��N�@�E��    ������ց���  �M̉L$�4$�D$    �D$    �D$�D$    �  ��� �UЉ$��  �M̋UЉ4$�D$    �L$�T$�ǉD$�E���D$�B  �U�S�UЃ��<$�����$����  �t$�$�D$�t  �$�D$�  �4$�E��u  �E̋UЉ�<$�e  �E�C��e�[^_]Ð�t& �E��D$   �D$�t$�$�m  �V���E�������D$��Ah�$��  ���&    U��WVS���  �]��Ah�E�1��SH��2���SH�S��)�������  ����    ��p������@	�0  �@��l����D��@	��  �@��h�����Ah��$�A  �=�Ah��  �������|$�$  �f  ���<$�  ���$�X  �|$��t����$�  ������1��׹   󫍅�����D$$��t����T$ ��h���ǅ����D   ǅ����   �D$��l����T$fǅ���� �D$    �D$    �D$    �D$    �D$    �$��  1�1Ƀ�(��tK�=�Ah�������;  �����T$�D$�C�D��$�D$��  �������$��  �   1Ƀ��5�Ah���d�����`����$��  ��t����$�8  ��d�����`�����p���s�T$�L$�$��  �$�D$�7  ���p���C��U�3�Ah��   �e�[^_]Ív ��~����|$�$  ��  ���<$�/�����t����v����t& �D$   �D$    �D$�$��  ��h���������D$   �D$    �D$�$�  �S��l�����������1�)������L  �D$��Ah�$��  ��    ��    U��VS���E�u�$�����$���p  �\$�4$�D$��  �4$�D$�$  �$����  ����[^]ÐU��WVS��|  �]��Ah�E�1��CH�3�8���CH�C��)����V����f  ����    ���������@	��  � �@��������ǅ����    ǅ����    ��  �$��Ah�  ��������   �=�AhvU�D$��Ah�$�  ����t>�������T$������������D$    �D$    �T$�$    �Ѓ�����  �D$��Ah�4$�  ����t>�������T$������������D$    �D$    �T$�$    �Ѓ����R  �4$��  ���$:�Ah�  ��������   �=�AhvI�D$ȚAh�$�=  ����t2�������$    �T$�������T$�������T$�Ѓ�����  �D$��Ah�4$��  ����t2�������$    �T$�������T$�������T$�Ѓ�����   �4$�  ���=�Ah��   ������;�2  �������4� �Ah���  �  ��������D$�D$   �D$    �D$x�Ah�$��  �����O  ������������   �$���Ah��    �4$�p  ������������s���D$    �D$�$��  �$�D$�6  �������C��E�3�Ah��  �e�[^_]�f��D$   �D$�$��  �������F�����C��H  ��������C�뮍�&    �C�D�����   �P����   ����   � �PP������   ǅ����   ǅ���� �  ������4$�  ������������s���$�����$���
�����D$   �D$�$��  �����Y����ؘAh�  ��'�����Ah�  �������Ah�  ��	����0�Ah�  �������ЙAh�  �������(�Ah�  ��������Ah�  ���������Ah�  �������Ah�  ������J�Ah�  ��������Ah�  ������h�Ah�  �������    �������D$�������D$�������������T$�������t$ǅ����  ǅ����    �D$    �$�6  �Ƌ��������$�+  �����������;����������.���������������f������ �����苵����fǄE����  s�T$�$�����������C��D����$h�Ah��  �,	  ��    ��    U��VS��0  ��Ah�E�1���Ah� �$�
  �=�AhwS�������t$�$  ��  ���4$�2
  ���$��  �t$�É$�!
  �U�3�Ah��u1�e�[^]Ð�t& �������\$�$  �  ���$����������v  ��    U���H�]�]�u��}���E��CH�8���CH�)�����������   �$�  �t$�$�E�D$�  ��Ah� �$��	  �4$�
  ���   uQ�C+E���~&�S��    �M��C��]�u��}���]Ít& �E��D$   �$�D$�D$��  뽍v �M��D$    �D$    �D$t   �D$    �L$�$��  �C+E����뤍t& �S��H  ��    ��C��]�u��}���]Ít& ��'    U��S����Ah�]� �$��  �]��[]�  �v ��'    U1���]É���'    U��]Ít& ��'    U��WVS��<�]��E�CH����CH�C�D$   �D$��Ah�r���<�    �E܉U��$�L$�*  �U��E��D$   �D$��Ah�t$���U�$)����D$��  �
  �$�D$��Ah�D$pAh�D$ɞAh��
  �$�D$��Ah�D$�Ah�D$؞Ah�
  �$�D$��Ah�D$Ah�D$�Ah�
  �$�D$��Ah�D$0iAh�D$��Ah�y
  �$�D$��Ah�D$ Ah�D$	�Ah�Y
  �$�D$��Ah�D$�KAh�D$�Ah�9
  �$�D$��Ah�D$P`Ah�D$5�Ah�
  �$�D$��Ah�D$�[Ah�D$L�Ah��	  �$�D$��Ah�D$�gAh�D$c�Ah��	  �$�D$��Ah�D$ Ah�D$z�Ah�	  �$�D$��Ah�D$�Ah�D$��Ah�	  �$�D$��Ah�D$p?Ah�D$��Ah�y	  �$�D$��Ah�D$�Ah�D$��Ah�Y	  �$�D$��Ah�D$ Ah�D$˟Ah�9	  �$�D$��Ah�D$PCAh�D$�Ah�	  �$�D$��Ah�D$ AAh�D$��Ah��  �$�D$��Ah�D$0Ah�D$�Ah��  �$�D$��Ah�D$0Ah�D$&�Ah�  �$�D$��Ah�D$�Ah�D$<�Ah�  �C��x  ��{�;�e�[^_]�� �Ah�   �$ �Ah�  �����������Ah    � �Ah�   �$ �Ah�_  �����������������������%��Ah���%��Ah���%��Ah���%��Ah���%��Ah���%��Ah���%�Ah��Q�L$=   r��   �	 -   =   w�)��	 ���̋�@PÐ��f�f�U����$�Ah�D$� �Ah�D$��Ah�$�[  ���Í�    U���(�E�U�M��tor����   ��tL�   �� �v �L$�D$    �$�  ����tދ(�Ah�E�$�  �E��(�Ah����뾐�t& �M�E   �U���   1������D$�|Ah�$��Ah� �Ah   �
�U ǀ�       ǀ�     �@,؄Ah�@0�Ah�@ �Ah�p(�@H��Ah�@L��Ah�@P��Ah�@T��Ah�@$�Ah��@��Ah�@ȁAh�@ ЁAh�@D؁Ah�����Px�Å��;���������v �B���Ah�����v �B���Ah�����v �B���Ah�f����v �B���Ah�H����v �B���Ah�+����v �B���Ah�
 DllUnregisterServer   usage: Win32::RegisterServer($libname)
 DllRegisterServer   usage: Win32::GetProcAddress($hinstance, $procname)
    usage: Win32::FreeLibrary($handle)
 usage: Win32::LoadLibrary($libname)
    usage: Win32::GetFileVersion($filename)
 \ %d.%d.%d.%d GetNativeSystemInfo PROCESSOR_ARCHITECTURE   usage: Win32::AbortSystemShutdown($machineName);
 SeShutdownPrivilege   usage: Win32::InitiateSystemShutdown($machineName, $message, $timeOut, $forceClose, $reboot);
  usage: Win32::LookupAccountSID($system, $sid, $account, $domain, $sidtype);
    usage: Win32::LookupAccountName($system, $account, $domain, $sid, $sidtype);
 usage: Win32::IsAdminUser() shell32.dll IsUserAnAdmin advapi32.dll    Cannot load advapi32.dll library OpenThreadToken OpenProcessToken GetTokenInformation AllocateAndInitializeSid EqualSid FreeSid Cannot load functions from advapi32.dll library Cannot open thread token or process token   Cannot allocate token information structure Cannot get token information    Cannot allocate administrators' SID usage: Win32::OutputDebugString($string)    usage: Win32::CreateFile($file) usage: Win32::CreateDirectory($dir) Perl    usage: Win32::MsgBox($message [, $flags [, $title]]);
  P e r l     usage: Win32::GetShortPathName($longPathName)   usage: Win32::ExpandEnvironmentStrings($String);
 userenv.dll CreateEnvironmentBlock DestroyEnvironmentBlock    usage: Win32::GetLongPathName($pathname)    usage: Win32::GetFullPathName($filename)  usage: Win32::SetCwd($cwd)    usage: Win32::GetANSIPathName($pathname)    usage: Win32::Spawn($cmdName, $args, $PID)  C o m m o n V i d e o   C o m m o n P i c t u r e s   C o m m o n M u s i c     C o m m o n   T e m p l a t e s   C o m m o n   S t a r t u p   C o m m o n   S t a r t   M e n u   C o m m o n   P r o g r a m s   C o m m o n   F a v o r i t e s     C o m m o n   D o c u m e n t s   C o m m o n   D e s k t o p   C o m m o n   A p p D a t a     C o m m o n   A d m i n i s t r a t i v e   T o o l s   usage: Win32::GetFolderPath($csidl [, $create])
 shfolder.dll SHGetFolderPathW SHGetFolderPathA SHGetSpecialFolderPathW SHGetSpecialFolderPathA A P P D A T A   U S E R P R O F I L E   P r o g r a m F i l e s     C o m m o n P r o g r a m F i l e s   S y s t e m R o o t   Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders    FqAh�oAh�oAh�oAh�oAh�oAh�oAh�oAh�oAh�oAh4qAh�oAh"qAh�oAhqAh�oAh�oAh�pAhrAhrAh�qAh�qAh�nAh�nAh�nAh�nAh�nAh�qAh�nAh�nAh�nAh�qAh�nAh�nAh�nAh�nAh�nAh�nAh�nAh�nAh�nAh�qAh�qAh�qAh�nAh�nAh�nAh�nAh�nAh�qAh�qAhvqAhv5.14.0 0.44 Win32.xs Win32::LookupAccountName Win32::LookupAccountSID Win32::InitiateSystemShutdown Win32::AbortSystemShutdown Win32::ExpandEnvironmentStrings Win32::MsgBox Win32::LoadLibrary Win32::FreeLibrary Win32::GetProcAddress Win32::RegisterServer Win32::UnregisterServer Win32::GetArchName Win32::GetChipName Win32::GuidGen Win32::GetFolderPath Win32::IsAdminUser Win32::GetFileVersion Win32::GetCwd Win32::SetCwd Win32::GetNextAvailDrive Win32::GetLastError Win32::SetLastError Win32::LoginName Win32::NodeName Win32::DomainName Win32::FsType Win32::GetOSVersion Win32::IsWinNT Win32::IsWin95 Win32::FormatMessage Win32::Spawn Win32::GetTickCount Win32::GetShortPathName Win32::GetFullPathName Win32::GetLongPathName Win32::GetANSIPathName Win32::CopyFile Win32::Sleep Win32::OutputDebugString Win32::GetCurrentProcessId Win32::GetCurrentThreadId Win32::CreateDirectory Win32::CreateFile Win32::GetSystemMetrics Win32::GetProductInfo Win32::SetChildShowWindow D e s k t o p   P r o g r a m s   P e r s o n a l   F a v o r i t e s   S t a r t u p   R e c e n t   S e n d T o   S t a r t   M e n u   M y   M u s i c   M y   V i d e o   N e t H o o d   F o n t s   T e m p l a t e s   A p p D a t a   P r i n t H o o d   L o c a l   A p p D a t a   C a c h e   C o o k i e s   H i s t o r y   M y   P i c t u r e s   A d m i n i s t r a t i v e   T o o l s   C D   B u r n i n g           V�Ah    f�Ah        x�Ah��Ah��Ah��Ah��Ah    ʠAh    �Ah�Ah    V�Ah        �Ah�Ah �Ah                4�AhD�AhX�Ah            t�Ah��Ah��Ah                ��Ah                                ��Ah                                        �Ah                ��                                                                                                                                                                                                                                                                         �  0      �  �      �         �  �      �  `      �  �       �  p"      �  T#      �  �%      �  �%      �  @,      �  �,      �  `/      �  �/      �   6      �  �6      �   8      �  9      �  �N      �  0O      �  $P      �  �P      �  aT      �  FU      �   \      �  u\      �  ``      �  �a      �  @i      �  Kk      �  pl      �  Lo      �  ,s      �  �s                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      �$�O    ��     B   B   (�  0�  8�  0I  �H  I  l�  |�  X�  Pu   u  @u   �  ��   s  0u  `R  �^  PT  0X  PI  �1     PC   A  `"  P    �*  P  �g  �1  �0  �     �s  0,  `l  P`  `&  �[  �&  0  �)  0  �K  0     P/  �3   :  �  p  p+  �N  8  6   E  �%  p?  �(  �  �e  �!  �  0i  �'  l  0J  ��  ��  ��  ��  ��  �  �  $�  2�  @�  H�  S�  `�  m�  }�  ��  ��  ��  ��  ��  ��  ��  ��  �  "�  4�  D�  O�  c�  s�  ��  ��  ��  ��  ��  ��  ��  �  �  1�  B�  U�  h�  }�  ��  ��  ��  ��  ��  ��  ��  �  �  &�  ;�  F�  S�  i�  |�  ��  ��  ��  ��  ��  ��  ��            	 
   
IsValidSid  LookupAccountNameA  LookupAccountSidA LookupPrivilegeValueA eOpenProcessToken  �RegCloseKey �RegOpenKeyExA �RegQueryValueExW  R CloseHandle p CopyFileA { CreateDirectoryA  � CreateDirectoryW  � CreateFileA � CreateFileW � CreateProcessA  ExpandEnvironmentStringsA ExpandEnvironmentStringsW ,FindClose 0FindFirstFileA  7FindFirstFileW  [FormatMessageA  `FreeLibrary �GetComputerNameA  �GetCurrentDirectoryA  �GetCurrentDirectoryW  �GetCurrentProcess �GetCurrentProcessId �GetCurrentThread  �GetCurrentThreadId  �GetDriveTypeA �GetFullPathNameA  �GetFullPathNameW  �GetLastError  GetModuleHandleA  AGetProcAddress  \GetShortPathNameA ]GetShortPathNameW oGetSystemInfo �GetTickCount  �GetVersionExA �GetVolumeInformationA 1LoadLibraryA  \MultiByteToWideChar |OutputDebugStringA  }OutputDebugStringW  CSetLastError  �Sleep �WideCharToMultiByte  CoCreateGuid  E CoTaskMemFree � StringFromCLSID � GetActiveWindow FGetSystemMetrics  �MessageBoxA �MessageBoxW   GetFileVersionInfoA  GetFileVersionInfoSizeA  VerQueryValueA   __errno   p_impure_ptr   �calloc    �chdir Ccygwin_detach_dll Ecygwin_internal   Zdll_dllcrt0   �free  )getenv    malloc    �pthread_getspecific   �realloc   �sprintf   �strcpy    �strlen    �strrchr   toupper   towupper  Vwcscpy    \wcslen     __stack_chk_fail  	 __stack_chk_guard  PL_charclass  D PL_thr_key    QPerl_block_gimme  �Perl_croak    �Perl_croak_nocontext  lPerl_newSV    qPerl_newSViv  rPerl_newSVnv  sPerl_newSVpv  wPerl_newSVpvn xPerl_newSVpvn_flags   �Perl_newXS    Perl_safesysfree  Perl_safesysmalloc    Perl_safesysrealloc   _Perl_stack_grow   gPerl_sv_2bool_flags   kPerl_sv_2iv_flags lPerl_sv_2mortal   qPerl_sv_2pv_flags �Perl_sv_grow  �Perl_sv_magic �Perl_sv_mortalcopy    �Perl_sv_newmortal �Perl_sv_setiv �Perl_sv_setpv �Perl_sv_setpvn    �Perl_sv_upgrade   =Perl_warn_nocontext   DPerl_xs_apiversion_bootcheck  EPerl_xs_version_bootcheck  �   �   �   �   �   �   �   �   �   �   �   �   �  ADVAPI32.DLL    �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  �  KERNEL32.dll    (�  (�  (�  OLE32.dll   <�  <�  <�  <�  USER32.dll  P�  P�  P�  VERSION.dll d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  d�  cygwin1.dll x�  x�  cygssp-0.dll    ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  ��  cygperl5_14.dll                                                                                                                                                                                                                                                T   	0$0;0B0I0T0o0�0�0�0�0d1w1"3�3;4�406�6z89�9�9�9:A:z:�:�:�:6;q;�;a< >�>2?`?    8   �0Q2p2�2�2�2T3�5�5
7>8�8.9�9�:[;!<@<�<l=�=:?`?�? 0  D   �01�1n2w3�56 6�6�7 89�9P:^:y:A;\;u;�;�;�;�;<X>i>�>�>.?[? @  ,   �021;3�35t5�5�6�8�8949�;�>�>�>0?   P  ,   $0S0�0M2n2�2�2�23a4|4F5�: </<u<�>   `  H   `0�01�1z4$5�5�5�79@9�9�9�:�:K;�;p<�<�<=]=�=�=�=(>z>�>�>�>L?   p  �  �0�01&181J1w1�1�1�1�1�1�1�1�1�1
222"2*222:2B2J2R2Z2b2j2r2z2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�2�23
333"3*323:3B3J3R3Z3b3j3r3z3�3�3�3�3�3�3�3�3�3�3�34
444"4*424:4B4J4R4Z4b4j4r4z4�4�4�4�4�4�4�4�4�4�4�4�4�4   �  p   �;�;�;�;�;�;�;�;�;�;�;�;�;�;�;�;�; <<<<<<<< <$<(<,<0<4<8<<<@<D<H<L<P<T<X<\<`<d<h<l<p<t<x<|<�<�<�< �  8    22222 2$2,24282@2L2P2T2h2l2p2�2�2�2�2�2�2                                                                                                                                                                                                                                                                              Win32.dbg   �_9�                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               _obj                1             ?                                U   �       .text          �                .data                            .bss                            .rdata  �      _                     m           .jcr              w   �                            �   0          �   0          �   �          �   p          �              �   �                         �            0	          /   
                 .text   (l      .data   �       .bss           .idata$7       .idata$5�      .idata$4�      .idata$6	      .text   0l      .data   �       .bss           .idata$7�
      .text   @l      .idata$7p      .idata$5      .idata$4      .idata$6D
      @feat.00   ��   .text   Hl     1                 .data   �                        .bss                                �  �l                           �  $           �              �             �  �l          �  (           �  �m      .text   �l                    .data   �                        .bss                           .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                     .text   �m                            n                       .text    n                      .data   �                        .bss    ,                        .text   n                            n                       _u.8973 @       .text    n     3                .data   �                        .bss    @      �                     "  `n                       .text   `n       =             .data   �                       .bss                           .text   pq                     .text   xq      .idata$7      .idata$5�      .idata$4�      .idata$6D	          9  �q                       .text   �q                      .data   �                        .bss                               J  �q                       .text   �q                      .data   �                        .bss                               [  �q                       .text   �q                      .data   �                        .bss                               l  �q                       .text   �q                      .data   �                        .bss                           .text   �q                     .text   �q                     .text   �q                     .text   �q                     .text   �q                         }  �q                       .text   �q                      .data   �                        .bss                           .text    r      .data   �       .bss          .idata$7�
      .text   �s      .idata$7�      .idata$5      .idata$4(      .idata$6|
      .text   �s      .idata$7�      .idata$5      .idata$4,      .idata$6�
      @feat.00   ��   .text   �s      .idata$7�      .idata$5       .idata$40      .idata$6�
      @feat.00   ��   .text    t      .idata$7�      .idata$5$      .idata$44      .idata$6�
      @feat.00   ��   .text   t      .idata$7�      .idata$5(      .idata$48      .idata$6�
      @feat.00   ��   .text   t      .idata$7�      .idata$5,      .idata$4<      .idata$6�
      @feat.00   ��   .text   t      .idata$7�      .idata$50      .idata$4@      .idata$6�
      @feat.00   ��   .text    t      .idata$7�      .idata$54      .idata$4D      .idata$6�
      @feat.00   ��   .text   (t      .idata$7�      .idata$58      .idata$4H      .idata$6      @feat.00   ��   .text   0t      .idata$7�      .idata$5<      .idata$4L      .idata$6      @feat.00   ��   .text   8t      .idata$7�      .idata$5@      .idata$4P      .idata$60      @feat.00   ��   .text   @t      .idata$7�      .idata$5D      .idata$4T      .idata$6@      @feat.00   ��   .text   Ht      .idata$7�      .idata$5H      .idata$4X      .idata$6T      @feat.00   ��   .text   Pt      .idata$7�      .idata$5L      .idata$4\      .idata$6l      @feat.00   ��   .text   Xt      .idata$7�      .idata$5P      .idata$4`      .idata$6�      @feat.00   ��   .text   `t      .idata$7�      .idata$5T      .idata$4d      .idata$6�      @feat.00   ��   .text   ht      .idata$7�      .idata$5X      .idata$4h      .idata$6�      @feat.00   ��   .text   pt      .idata$7�      .idata$5\      .idata$4l      .idata$6�      @feat.00   ��   .text   xt      .idata$7�      .idata$5`      .idata$4p      .idata$6�      @feat.00   ��   .text   �t      .idata$7�      .idata$5d      .idata$4t      .idata$6�      @feat.00   ��   .text   �t      .idata$7�      .idata$5h      .idata$4x      .idata$6�      @feat.00   ��   .text   �t      .idata$7�      .idata$5l      .idata$4|      .idata$6      @feat.00   ��   .text   �t      .idata$7�      .idata$5p      .idata$4�      .idata$6$      @feat.00   ��   .text   �t      .idata$7�      .idata$5t      .idata$4�      .idata$68      @feat.00   ��   .text   �t      .idata$7�      .idata$5x      .idata$4�      .idata$6H      @feat.00   ��   .text   �t      .idata$7�      .idata$5|      .idata$4�      .idata$6X      @feat.00   ��   .text   �t      .idata$7�      .idata$5�      .idata$4�      .idata$6l      @feat.00   ��   .text   �t      .idata$7�      .idata$5�      .idata$4�      .idata$6�      @feat.00   ��   .text   �t      .idata$7�      .idata$5�      .idata$4�      .idata$6�      @feat.00   ��   .text   �t      .idata$7       .idata$5�      .idata$4�      .idata$6�      @feat.00   ��   .rdata                                         <  $           X  0           t  <           �  H           �  T           �  `           �  l              x             �           8  �           T  �           p  �           �  �           �  �           �  �           �  �           �  �             �           4  �           P            l            �             �  ,          �  8          �  D          �  P          	  \          0	  h          L	  t          h	  �          �	  �          �	  �      .idata$2d       .idata$5�      .idata$4�      .idata$2x       .idata$5      .idata$4      .idata$2�       .idata$5      .idata$4$      .idata$7      .idata$5�      .idata$4�      .idata$68	      @feat.00   ��   .idata$7      .idata$5�      .idata$4�      .idata$6T	      @feat.00   ��   .idata$7       .idata$5�      .idata$4�      .idata$6`	      @feat.00   ��   .idata$7$      .idata$5�      .idata$4�      .idata$6h	      @feat.00   ��   .idata$7(      .idata$5�      .idata$4�      .idata$6|	      @feat.00   ��   .idata$7,      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$70      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$74      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$78      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7<      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7@      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7D      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7H      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7L      .idata$5�      .idata$4�      .idata$6�	      @feat.00   ��   .idata$7P      .idata$5�      .idata$4       .idata$6
      @feat.00   ��   .idata$7T      .idata$5�      .idata$4      .idata$6
      @feat.00   ��   .idata$7X      .idata$5�      .idata$4      .idata$6 
      @feat.00   ��   .idata$7\      .idata$5�      .idata$4      .idata$6,
      @feat.00   ��   .idata$7`      .idata$5       .idata$4      .idata$68
      @feat.00   ��   .idata$4      .idata$5      .idata$7d      .idata$4       .idata$5      .idata$7x      .idata$4�      .idata$5�      .idata$7          �	   s          �	  (l          �	  �r          �	  hr          
  �t          
  �s      _sprintf�m          )
  s          ;
  xr      _free   �q          J
  �t          Z
  �s          h
  0t          }
  l          �
  Hs          �
  �s          �
  �t          �
  r          �
  `s          �
  �r            pq             r          (  �r          >  ps          S  �s          m  @l      _toupper�m             l          �  r          �  Ps          �  (s          �  �t          �  �t          �  (r          �  hs            �r          +  Pt          @  �r          T  �r          d  xs          ~  Pr          �  Xs          �  Ht          �  pr          �  t          �  �r          �  �s          �  �q          
          j  �          �  8          �  �          �  4          �  �          �  p\          �               �c            �t          (  )          B  �          Q  p          j  @          |            �  �      __allocaHl          �  �          �            �  T          �  h            p            `          5  L_          O  �          k  �t          z  $          �  P          �              �  4          �  x          �  0?      __fmode             �  ��         �          /  D          G      ��       �  d          �  d           �      ��       �     ��       �  ,c          �      ��       8  �          G  �          U             g  ,          {  �          �     ��       �  �          �  �&          �  �          �            �  �            �          ,  d          @  �          a  `P          {  K[          �  @          �  L          �  �t          �  @Y          �      ��       '      ��       @   �          ]     �X��       j   (           |      ��       �   FE          �   �          �   �          �   \          �   �          �   �          !  d          &!  �          5!   &          O!   L          i!  �          �!  �          �!  �           �!  uL          �!  �          �!      ��       "  �          "  �          4"     ��       L"      ��       ]"  �
          =$  �          ^$  �          �$      ��       �$  ���X��       �$  �
          �%  �          �%  `          �%               �%               �%               
$       PE  L ��S "  �  � #         0           -h                    �     �3                         P  �    `  �                           p                                                    �`  t                           .text   �                       ` P`.data   L                        @ `�/4          0                    @ 0�.bss       @                      � `�.edata  �    P                    @ 0@.idata  �   `                    @ 0�.reloc     p                    @ 0B/14         �                     @ 0@                                                                                                                                                                                                                                                                                                                                        U����$  -h�  ���Ҹ    t�D$/ -h�$��
@t�|$�$��  �E�C���,[^_]ÉU��$��  �U����b����t& �t$�$�  �F<�u����V�B �  �n����E�D$� -h�$�D$�b  f�U��WVS��<�]�CH�3����CH��    C)ƋC���@ ��   �H�C�<����  �C�4�   ���8�B<��   �����   ����   �B�P��   u4�ʃ�	�(  ��
@t�|$�$��  s�3��<[^_]Ë 1��E�    ��AtF��@��   A��ɉM�t.�H�M܋M��}� �M�t	�	�ɉM�t�Q�M܅�uj�E����E�M�T$�|$�$�L$�  �l�����t& �U؉$�e  �U؉�������J�A �  �����E�D$� -h�$�D$��
  �t& � � ���E�u��E�    닋�B$��t�x	t_�J�������1��E�    �A�^��������� �  �� �  ������P��������P���R ��Eʋ	�A��������I뤋 �H��u�떍v U���(�]�]�}��u��CH�3�S����CH����)ǉ�������   �T��B<��   �����   ����   �B���x
���SH�S��)�������  ���M����MЋ�K���w  ����~  ���u  �[���]�u�M��]�D�����L[^_]Í�    �]����E��E�    �T��_��s�@�E�>-�E�u�m����Èx
�U ǀ�       ǀ�     �@,�-h�@0�-h�@ A-h�p(�@H�-h�@L�-h�@P�-h�@T�-h�@$A-h��@�-h�@�-h�@ �-h�@D�-h�����Px�Å��;���������v �B� -h�����v �B� -h�����v �B� -h�f����v �B� -h�H����v �B� -h�+����v �B� -h�
                           &  0	          <  (           G  
      .text    	                    .data                            .bss                               g   
                       .text    
                      .data                            .bss    ,                        .text   0
                         s  @
                       _u.8973 @       .text   @
     3                .data                            .bss    @      �                     �  �
                       .text   �
       =             .data                           .bss                           .text   �
      _malloc �
  �           
     ��       %
      ��       6
             M
             b
      ��       ~
      ��       �
  0          �
  ,          �
      ��          0X��       7  P          M            e  �           {            �               �               �               �               �               �          i                __ZdlPv         i                                                  $               4           __end__              F               X          i                     m               z          i                     �          i                     �               �          i                 __Znaj          i                    �          i                 __Znwj          i                     �               �          i                 __ZdaPv         i                 
package lib;

# THIS FILE IS AUTOMATICALLY GENERATED FROM lib_pm.PL.
# ANY CHANGES TO THIS FILE WILL BE OVERWRITTEN BY THE NEXT PERL BUILD.

use Config;

use strict;

my $archname         = $Config{archname};
my $version          = $Config{version};
my @inc_version_list = reverse split / /, $Config{inc_version_list};


our @ORIG_INC = @INC;	# take a handy copy of 'original' value
our $VERSION = '0.63';

sub import {
    shift;

    my %names;
    foreach (reverse @_) {
	my $path = $_;		# we'll be modifying it, so break the alias
	if ($path eq '') {
	    require Carp;
	    Carp::carp("Empty compile time value given to use lib");
	}

	if ($path !~ /\.par$/i && -e $path && ! -d _) {
	    require Carp;
	    Carp::carp("Parameter to use lib must be directory, not file");
	}
	unshift(@INC, $path);
	# Add any previous version directories we found at configure time
	foreach my $incver (@inc_version_list)
	{
	    my $dir = "$path/$incver";
	    unshift(@INC, $dir) if -d $dir;
	}
	# Put a corresponding archlib directory in front of $path if it
	# looks like $path has an archlib directory below it.
	my($arch_auto_dir, $arch_dir, $version_dir, $version_arch_dir)
	    = _get_dirs($path);
	unshift(@INC, $arch_dir)         if -d $arch_auto_dir;
	unshift(@INC, $version_dir)      if -d $version_dir;
	unshift(@INC, $version_arch_dir) if -d $version_arch_dir;
    }

    # remove trailing duplicates
    @INC = grep { ++$names{$_} == 1 } @INC;
    return;
}


sub unimport {
    shift;

    my %names;
    foreach my $path (@_) {
	my($arch_auto_dir, $arch_dir, $version_dir, $version_arch_dir)
	    = _get_dirs($path);
	++$names{$path};
	++$names{$arch_dir}         if -d $arch_auto_dir;
	++$names{$version_dir}      if -d $version_dir;
	++$names{$version_arch_dir} if -d $version_arch_dir;
    }

    # Remove ALL instances of each named directory.
    @INC = grep { !exists $names{$_} } @INC;
    return;
}

sub _get_dirs {
    my($dir) = @_;
    my($arch_auto_dir, $arch_dir, $version_dir, $version_arch_dir);

    $arch_auto_dir    = "$dir/$archname/auto";
    $arch_dir         = "$dir/$archname";
    $version_dir      = "$dir/$version";
    $version_arch_dir = "$dir/$version/$archname";

    return($arch_auto_dir, $arch_dir, $version_dir, $version_arch_dir);
}

1;
__END__

#line 212
FILE   c17458c6/integer.pm   �#line 1 "/usr/lib/perl5/5.14/integer.pm"
package integer;

our $VERSION = '1.00';

#line 82

$integer::hint_bits = 0x1;

sub import {
    $^H |= $integer::hint_bits;
}

sub unimport {
    $^H &= ~$integer::hint_bits;
}

1;
FILE   011a16c0/overload.pm  :#line 1 "/usr/lib/perl5/5.14/overload.pm"
package overload;

our $VERSION = '1.13';

sub nil {}

sub OVERLOAD {
  $package = shift;
  my %arg = @_;
  my ($sub, $fb);
  $ {$package . "::OVERLOAD"}{dummy}++; # Register with magic by touching.
  $fb = ${$package . "::()"}; # preserve old fallback value RT#68196
  *{$package . "::()"} = \&nil; # Make it findable via fetchmethod.
  for (keys %arg) {
    if ($_ eq 'fallback') {
      $fb = $arg{$_};
    } else {
      $sub = $arg{$_};
      if (not ref $sub and $sub !~ /::/) {
	$ {$package . "::(" . $_} = $sub;
	$sub = \&nil;
      }
      #print STDERR "Setting `$ {'package'}::\cO$_' to \\&`$sub'.\n";
      *{$package . "::(" . $_} = \&{ $sub };
    }
  }
  ${$package . "::()"} = $fb; # Make it findable too (fallback only).
}

sub import {
  $package = (caller())[0];
  # *{$package . "::OVERLOAD"} = \&OVERLOAD;
  shift;
  $package->overload::OVERLOAD(@_);
}

sub unimport {
  $package = (caller())[0];
  ${$package . "::OVERLOAD"}{dummy}++; # Upgrade the table
  shift;
  for (@_) {
    if ($_ eq 'fallback') {
      undef $ {$package . "::()"};
    } else {
      delete $ {$package . "::"}{"(" . $_};
    }
  }
}

sub Overloaded {
  my $package = shift;
  $package = ref $package if ref $package;
  $package->can('()');
}

sub ov_method {
  my $globref = shift;
  return undef unless $globref;
  my $sub = \&{*$globref};
  require Scalar::Util;
  return $sub
    if Scalar::Util::refaddr($sub) != Scalar::Util::refaddr(\&nil);
  return shift->can($ {*$globref});
}

sub OverloadedStringify {
  my $package = shift;
  $package = ref $package if ref $package;
  #$package->can('(""')
  ov_method mycan($package, '(""'), $package
    or ov_method mycan($package, '(0+'), $package
    or ov_method mycan($package, '(bool'), $package
    or ov_method mycan($package, '(nomethod'), $package;
}

sub Method {
  my $package = shift;
  if(ref $package) {
    local $@;
    local $!;
    require Scalar::Util;
    $package = Scalar::Util::blessed($package);
    return undef if !defined $package;
  }
  #my $meth = $package->can('(' . shift);
  ov_method mycan($package, '(' . shift), $package;
  #return $meth if $meth ne \&nil;
  #return $ {*{$meth}};
}

sub AddrRef {
  my $package = ref $_[0];
  return "$_[0]" unless $package;

  local $@;
  local $!;
  require Scalar::Util;
  my $class = Scalar::Util::blessed($_[0]);
  my $class_prefix = defined($class) ? "$class=" : "";
  my $type = Scalar::Util::reftype($_[0]);
  my $addr = Scalar::Util::refaddr($_[0]);
  return sprintf("%s%s(0x%x)", $class_prefix, $type, $addr);
}

*StrVal = *AddrRef;

sub mycan {				# Real can would leave stubs.
  my ($package, $meth) = @_;

  local $@;
  local $!;
  require mro;

  my $mro = mro::get_linear_isa($package);
  foreach my $p (@$mro) {
    my $fqmeth = $p . q{::} . $meth;
    return \*{$fqmeth} if defined &{$fqmeth};
  }

  return undef;
}

%constants = (
	      'integer'	  =>  0x1000, # HINT_NEW_INTEGER
	      'float'	  =>  0x2000, # HINT_NEW_FLOAT
	      'binary'	  =>  0x4000, # HINT_NEW_BINARY
	      'q'	  =>  0x8000, # HINT_NEW_STRING
	      'qr'	  => 0x10000, # HINT_NEW_RE
	     );

%ops = ( with_assign	  => "+ - * / % ** << >> x .",
	 assign		  => "+= -= *= /= %= **= <<= >>= x= .=",
	 num_comparison	  => "< <= >  >= == !=",
	 '3way_comparison'=> "<=> cmp",
	 str_comparison	  => "lt le gt ge eq ne",
	 binary		  => '& &= | |= ^ ^=',
	 unary		  => "neg ! ~",
	 mutators	  => '++ --',
	 func		  => "atan2 cos sin exp abs log sqrt int",
	 conversion	  => 'bool "" 0+ qr',
	 iterators	  => '<>',
         filetest         => "-X",
	 dereferencing	  => '${} @{} %{} &{} *{}',
	 matching	  => '~~',
	 special	  => 'nomethod fallback =');

use warnings::register;
sub constant {
  # Arguments: what, sub
  while (@_) {
    if (@_ == 1) {
        warnings::warnif ("Odd number of arguments for overload::constant");
        last;
    }
    elsif (!exists $constants {$_ [0]}) {
        warnings::warnif ("`$_[0]' is not an overloadable type");
    }
    elsif (!ref $_ [1] || "$_[1]" !~ /(^|=)CODE\(0x[0-9a-f]+\)$/) {
        # Can't use C<ref $_[1] eq "CODE"> above as code references can be
        # blessed, and C<ref> would return the package the ref is blessed into.
        if (warnings::enabled) {
            $_ [1] = "undef" unless defined $_ [1];
            warnings::warn ("`$_[1]' is not a code reference");
        }
    }
    else {
        $^H{$_[0]} = $_[1];
        $^H |= $constants{$_[0]};
    }
    shift, shift;
  }
}

sub remove_constant {
  # Arguments: what, sub
  while (@_) {
    delete $^H{$_[0]};
    $^H &= ~ $constants{$_[0]};
    shift, shift;
  }
}

1;

__END__

#line 1701

FILE   dc3a0166/strict.pm  �#line 1 "/usr/lib/perl5/5.14/strict.pm"
package strict;

$strict::VERSION = "1.04";

# Verify that we're called correctly so that strictures will work.
unless ( __FILE__ =~ /(^|[\/\\])\Q${\__PACKAGE__}\E\.pmc?$/ ) {
    # Can't use Carp, since Carp uses us!
    my (undef, $f, $l) = caller;
    die("Incorrect use of pragma '${\__PACKAGE__}' at $f line $l.\n");
}

my %bitmask = (
refs => 0x00000002,
subs => 0x00000200,
vars => 0x00000400
);

sub bits {
    my $bits = 0;
    my @wrong;
    foreach my $s (@_) {
	push @wrong, $s unless exists $bitmask{$s};
        $bits |= $bitmask{$s} || 0;
    }
    if (@wrong) {
        require Carp;
        Carp::croak("Unknown 'strict' tag(s) '@wrong'");
    }
    $bits;
}

my $default_bits = bits(qw(refs subs vars));

sub import {
    shift;
    $^H |= @_ ? bits(@_) : $default_bits;
}

sub unimport {
    shift;
    $^H &= ~ (@_ ? bits(@_) : $default_bits);
}

1;
__END__

#line 148
FILE   c0544bb1/utf8.pm  �#line 1 "/usr/lib/perl5/5.14/utf8.pm"
package utf8;

$utf8::hint_bits = 0x00800000;

our $VERSION = '1.09';

sub import {
    $^H |= $utf8::hint_bits;
    $enc{caller()} = $_[1] if $_[1];
}

sub unimport {
    $^H &= ~$utf8::hint_bits;
}

sub AUTOLOAD {
    require "utf8_heavy.pl";
    goto &$AUTOLOAD if defined &$AUTOLOAD;
    require Carp;
    Carp::croak("Undefined subroutine $AUTOLOAD called");
}

1;
__END__

#line 215
FILE   6a209f86/vars.pm  �#line 1 "/usr/lib/perl5/5.14/vars.pm"
package vars;

use 5.006;

our $VERSION = '1.02';

use warnings::register;
use strict qw(vars subs);

sub import {
    my $callpack = caller;
    my (undef, @imports) = @_;
    my ($sym, $ch);
    foreach (@imports) {
        if (($ch, $sym) = /^([\$\@\%\*\&])(.+)/) {
	    if ($sym =~ /\W/) {
		# time for a more-detailed check-up
		if ($sym =~ /^\w+[[{].*[]}]$/) {
		    require Carp;
		    Carp::croak("Can't declare individual elements of hash or array");
		} elsif (warnings::enabled() and length($sym) == 1 and $sym !~ tr/a-zA-Z//) {
		    warnings::warn("No need to declare built-in vars");
		} elsif  (($^H &= strict::bits('vars'))) {
		    require Carp;
		    Carp::croak("'$_' is not a valid variable name under strict vars");
		}
	    }
	    $sym = "${callpack}::$sym" unless $sym =~ /::/;
	    *$sym =
		(  $ch eq "\$" ? \$$sym
		 : $ch eq "\@" ? \@$sym
		 : $ch eq "\%" ? \%$sym
		 : $ch eq "\*" ? \*$sym
		 : $ch eq "\&" ? \&$sym 
		 : do {
		     require Carp;
		     Carp::croak("'$_' is not a valid variable name");
		 });
	} else {
	    require Carp;
	    Carp::croak("'$_' is not a valid variable name");
	}
    }
};

1;
__END__

#line 83
FILE   2fbc6902/warnings.pm  :�#line 1 "/usr/lib/perl5/5.14/warnings.pm"
# -*- buffer-read-only: t -*-
# !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
# This file is built by regen/warnings.pl.
# Any changes made here will be lost!

package warnings;

our $VERSION = '1.12';

# Verify that we're called correctly so that warnings will work.
# see also strict.pm.
unless ( __FILE__ =~ /(^|[\/\\])\Q${\__PACKAGE__}\E\.pmc?$/ ) {
    my (undef, $f, $l) = caller;
    die("Incorrect use of pragma '${\__PACKAGE__}' at $f line $l.\n");
}

#line 165

our %Offsets = (

    # Warnings Categories added in Perl 5.008

    'all'		=> 0,
    'closure'		=> 2,
    'deprecated'	=> 4,
    'exiting'		=> 6,
    'glob'		=> 8,
    'io'		=> 10,
    'closed'		=> 12,
    'exec'		=> 14,
    'layer'		=> 16,
    'newline'		=> 18,
    'pipe'		=> 20,
    'unopened'		=> 22,
    'misc'		=> 24,
    'numeric'		=> 26,
    'once'		=> 28,
    'overflow'		=> 30,
    'pack'		=> 32,
    'portable'		=> 34,
    'recursion'		=> 36,
    'redefine'		=> 38,
    'regexp'		=> 40,
    'severe'		=> 42,
    'debugging'		=> 44,
    'inplace'		=> 46,
    'internal'		=> 48,
    'malloc'		=> 50,
    'signal'		=> 52,
    'substr'		=> 54,
    'syntax'		=> 56,
    'ambiguous'		=> 58,
    'bareword'		=> 60,
    'digit'		=> 62,
    'parenthesis'	=> 64,
    'precedence'	=> 66,
    'printf'		=> 68,
    'prototype'		=> 70,
    'qw'		=> 72,
    'reserved'		=> 74,
    'semicolon'		=> 76,
    'taint'		=> 78,
    'threads'		=> 80,
    'uninitialized'	=> 82,
    'unpack'		=> 84,
    'untie'		=> 86,
    'utf8'		=> 88,
    'void'		=> 90,

    # Warnings Categories added in Perl 5.011

    'imprecision'	=> 92,
    'illegalproto'	=> 94,

    # Warnings Categories added in Perl 5.013

    'non_unicode'	=> 96,
    'nonchar'		=> 98,
    'surrogate'		=> 100,
  );

our %Bits = (
    'all'		=> "\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x55\x15", # [0..50]
    'ambiguous'		=> "\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00", # [29]
    'bareword'		=> "\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00", # [30]
    'closed'		=> "\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [6]
    'closure'		=> "\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [1]
    'debugging'		=> "\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00", # [22]
    'deprecated'	=> "\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [2]
    'digit'		=> "\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00", # [31]
    'exec'		=> "\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [7]
    'exiting'		=> "\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [3]
    'glob'		=> "\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [4]
    'illegalproto'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00", # [47]
    'imprecision'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00", # [46]
    'inplace'		=> "\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00", # [23]
    'internal'		=> "\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00", # [24]
    'io'		=> "\x00\x54\x55\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [5..11]
    'layer'		=> "\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [8]
    'malloc'		=> "\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00", # [25]
    'misc'		=> "\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [12]
    'newline'		=> "\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [9]
    'non_unicode'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01", # [48]
    'nonchar'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04", # [49]
    'numeric'		=> "\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [13]
    'once'		=> "\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [14]
    'overflow'		=> "\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [15]
    'pack'		=> "\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00", # [16]
    'parenthesis'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00", # [32]
    'pipe'		=> "\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [10]
    'portable'		=> "\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00", # [17]
    'precedence'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00", # [33]
    'printf'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00", # [34]
    'prototype'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00", # [35]
    'qw'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00", # [36]
    'recursion'		=> "\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00", # [18]
    'redefine'		=> "\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00", # [19]
    'regexp'		=> "\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00", # [20]
    'reserved'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00", # [37]
    'semicolon'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00", # [38]
    'severe'		=> "\x00\x00\x00\x00\x00\x54\x05\x00\x00\x00\x00\x00\x00", # [21..25]
    'signal'		=> "\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00", # [26]
    'substr'		=> "\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00", # [27]
    'surrogate'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10", # [50]
    'syntax'		=> "\x00\x00\x00\x00\x00\x00\x00\x55\x55\x15\x00\x40\x00", # [28..38,47]
    'taint'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00", # [39]
    'threads'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00", # [40]
    'uninitialized'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x00", # [41]
    'unopened'		=> "\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [11]
    'unpack'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00", # [42]
    'untie'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00", # [43]
    'utf8'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x15", # [44,48..50]
    'void'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00", # [45]
  );

our %DeadBits = (
    'all'		=> "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xaa\x2a", # [0..50]
    'ambiguous'		=> "\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00", # [29]
    'bareword'		=> "\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00", # [30]
    'closed'		=> "\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [6]
    'closure'		=> "\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [1]
    'debugging'		=> "\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00", # [22]
    'deprecated'	=> "\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [2]
    'digit'		=> "\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00", # [31]
    'exec'		=> "\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [7]
    'exiting'		=> "\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [3]
    'glob'		=> "\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [4]
    'illegalproto'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00", # [47]
    'imprecision'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00", # [46]
    'inplace'		=> "\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00", # [23]
    'internal'		=> "\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00", # [24]
    'io'		=> "\x00\xa8\xaa\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [5..11]
    'layer'		=> "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [8]
    'malloc'		=> "\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00", # [25]
    'misc'		=> "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [12]
    'newline'		=> "\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [9]
    'non_unicode'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02", # [48]
    'nonchar'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08", # [49]
    'numeric'		=> "\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [13]
    'once'		=> "\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [14]
    'overflow'		=> "\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [15]
    'pack'		=> "\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00", # [16]
    'parenthesis'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00", # [32]
    'pipe'		=> "\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [10]
    'portable'		=> "\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00", # [17]
    'precedence'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00", # [33]
    'printf'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00", # [34]
    'prototype'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00", # [35]
    'qw'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00", # [36]
    'recursion'		=> "\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00", # [18]
    'redefine'		=> "\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00", # [19]
    'regexp'		=> "\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00", # [20]
    'reserved'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00", # [37]
    'semicolon'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00", # [38]
    'severe'		=> "\x00\x00\x00\x00\x00\xa8\x0a\x00\x00\x00\x00\x00\x00", # [21..25]
    'signal'		=> "\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00", # [26]
    'substr'		=> "\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00\x00\x00\x00", # [27]
    'surrogate'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20", # [50]
    'syntax'		=> "\x00\x00\x00\x00\x00\x00\x00\xaa\xaa\x2a\x00\x80\x00", # [28..38,47]
    'taint'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00\x00", # [39]
    'threads'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00", # [40]
    'uninitialized'	=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00", # [41]
    'unopened'		=> "\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", # [11]
    'unpack'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00", # [42]
    'untie'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x00\x00", # [43]
    'utf8'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x2a", # [44,48..50]
    'void'		=> "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00", # [45]
  );

$NONE     = "\0\0\0\0\0\0\0\0\0\0\0\0\0";
$LAST_BIT = 102 ;
$BYTES    = 13 ;

$All = "" ; vec($All, $Offsets{'all'}, 2) = 3 ;

sub Croaker
{
    require Carp; # this initializes %CarpInternal
    local $Carp::CarpInternal{'warnings'};
    delete $Carp::CarpInternal{'warnings'};
    Carp::croak(@_);
}

sub _bits {
    my $mask = shift ;
    my $catmask ;
    my $fatal = 0 ;
    my $no_fatal = 0 ;

    foreach my $word ( @_ ) {
	if ($word eq 'FATAL') {
	    $fatal = 1;
	    $no_fatal = 0;
	}
	elsif ($word eq 'NONFATAL') {
	    $fatal = 0;
	    $no_fatal = 1;
	}
	elsif ($catmask = $Bits{$word}) {
	    $mask |= $catmask ;
	    $mask |= $DeadBits{$word} if $fatal ;
	    $mask &= ~($DeadBits{$word}|$All) if $no_fatal ;
	}
	else
          { Croaker("Unknown warnings category '$word'")}
    }

    return $mask ;
}

sub bits
{
    # called from B::Deparse.pm
    push @_, 'all' unless @_ ;
    return _bits(undef, @_) ;
}

sub import 
{
    shift;

    my $mask = ${^WARNING_BITS} ;

    if (vec($mask, $Offsets{'all'}, 1)) {
        $mask |= $Bits{'all'} ;
        $mask |= $DeadBits{'all'} if vec($mask, $Offsets{'all'}+1, 1);
    }
    
    # Empty @_ is equivalent to @_ = 'all' ;
    ${^WARNING_BITS} = @_ ? _bits($mask, @_) : $mask | $Bits{all} ;
}

sub unimport 
{
    shift;

    my $catmask ;
    my $mask = ${^WARNING_BITS} ;

    if (vec($mask, $Offsets{'all'}, 1)) {
        $mask |= $Bits{'all'} ;
        $mask |= $DeadBits{'all'} if vec($mask, $Offsets{'all'}+1, 1);
    }

    push @_, 'all' unless @_;

    foreach my $word ( @_ ) {
	if ($word eq 'FATAL') {
	    next; 
	}
	elsif ($catmask = $Bits{$word}) {
	    $mask &= ~($catmask | $DeadBits{$word} | $All);
	}
	else
          { Croaker("Unknown warnings category '$word'")}
    }

    ${^WARNING_BITS} = $mask ;
}

my %builtin_type; @builtin_type{qw(SCALAR ARRAY HASH CODE REF GLOB LVALUE Regexp)} = ();

sub MESSAGE () { 4 };
sub FATAL () { 2 };
sub NORMAL () { 1 };

sub __chk
{
    my $category ;
    my $offset ;
    my $isobj = 0 ;
    my $wanted = shift;
    my $has_message = $wanted & MESSAGE;

    unless (@_ == 1 || @_ == ($has_message ? 2 : 0)) {
	my $sub = (caller 1)[3];
	my $syntax = $has_message ? "[category,] 'message'" : '[category]';
	Croaker("Usage: $sub($syntax)");
    }

    my $message = pop if $has_message;

    if (@_) {
        # check the category supplied.
        $category = shift ;
        if (my $type = ref $category) {
            Croaker("not an object")
                if exists $builtin_type{$type};
	    $category = $type;
            $isobj = 1 ;
        }
        $offset = $Offsets{$category};
        Croaker("Unknown warnings category '$category'")
	    unless defined $offset;
    }
    else {
        $category = (caller(1))[0] ;
        $offset = $Offsets{$category};
        Croaker("package '$category' not registered for warnings")
	    unless defined $offset ;
    }

    my $i;

    if ($isobj) {
        my $pkg;
        $i = 2;
        while (do { { package DB; $pkg = (caller($i++))[0] } } ) {
            last unless @DB::args && $DB::args[0] =~ /^$category=/ ;
        }
	$i -= 2 ;
    }
    else {
        $i = _error_loc(); # see where Carp will allocate the error
    }

    # Defaulting this to 0 reduces complexity in code paths below.
    my $callers_bitmask = (caller($i))[9] || 0 ;

    my @results;
    foreach my $type (FATAL, NORMAL) {
	next unless $wanted & $type;

	push @results, (vec($callers_bitmask, $offset + $type - 1, 1) ||
			vec($callers_bitmask, $Offsets{'all'} + $type - 1, 1));
    }

    # &enabled and &fatal_enabled
    return $results[0] unless $has_message;

    # &warnif, and the category is neither enabled as warning nor as fatal
    return if $wanted == (NORMAL | FATAL | MESSAGE)
	&& !($results[0] || $results[1]);

    require Carp;
    Carp::croak($message) if $results[0];
    # will always get here for &warn. will only get here for &warnif if the
    # category is enabled
    Carp::carp($message);
}

sub _mkMask
{
    my ($bit) = @_;
    my $mask = "";

    vec($mask, $bit, 1) = 1;
    return $mask;
}

sub register_categories
{
    my @names = @_;

    for my $name (@names) {
	if (! defined $Bits{$name}) {
	    $Bits{$name}     = _mkMask($LAST_BIT);
	    vec($Bits{'all'}, $LAST_BIT, 1) = 1;
	    $Offsets{$name}  = $LAST_BIT ++;
	    foreach my $k (keys %Bits) {
		vec($Bits{$k}, $LAST_BIT, 1) = 0;
	    }
	    $DeadBits{$name} = _mkMask($LAST_BIT);
	    vec($DeadBits{'all'}, $LAST_BIT++, 1) = 1;
	}
    }
}

sub _error_loc {
    require Carp;
    goto &Carp::short_error_loc; # don't introduce another stack frame
}

sub enabled
{
    return __chk(NORMAL, @_);
}

sub fatal_enabled
{
    return __chk(FATAL, @_);
}

sub warn
{
    return __chk(FATAL | MESSAGE, @_);
}

sub warnif
{
    return __chk(NORMAL | FATAL | MESSAGE, @_);
}

# These are not part of any public interface, so we can delete them to save
# space.
delete $warnings::{$_} foreach qw(NORMAL FATAL MESSAGE);

1;

# ex: set ro:
FILE   9cc72e18/warnings/register.pm  #line 1 "/usr/lib/perl5/5.14/warnings/register.pm"
package warnings::register;

our $VERSION = '1.02';

#line 23

require warnings;

# left here as cruft in case other users were using this undocumented routine
# -- rjbs, 2010-09-08
sub mkMask
{
    my ($bit) = @_;
    my $mask = "";

    vec($mask, $bit, 1) = 1;
    return $mask;
}

sub import
{
    shift;
    my @categories = @_;

    my $package = (caller(0))[0];
    warnings::register_categories($package);

    warnings::register_categories($package . "::$_") for @categories;
}

1;
FILE   c0b6eea7/Archive/Zip.pm  D#line 1 "/usr/lib/perl5/site_perl/5.14/Archive/Zip.pm"
package Archive::Zip;

use 5.006;
use strict;
use Carp                ();
use Cwd                 ();
use IO::File            ();
use IO::Seekable        ();
use Compress::Raw::Zlib ();
use File::Spec          ();
use File::Temp          ();
use FileHandle          ();

use vars qw( $VERSION @ISA );
BEGIN {
    $VERSION = '1.37';

    require Exporter;
    @ISA = qw( Exporter );
}

use vars qw( $ChunkSize $ErrorHandler );
BEGIN {
    # This is the size we'll try to read, write, and (de)compress.
    # You could set it to something different if you had lots of memory
    # and needed more speed.
    $ChunkSize ||= 32768;

    $ErrorHandler = \&Carp::carp;
}

# BEGIN block is necessary here so that other modules can use the constants.
use vars qw( @EXPORT_OK %EXPORT_TAGS );
BEGIN {
    @EXPORT_OK   = ('computeCRC32');
    %EXPORT_TAGS = (
        CONSTANTS => [ qw(
            FA_MSDOS
            FA_UNIX
            GPBF_ENCRYPTED_MASK
            GPBF_DEFLATING_COMPRESSION_MASK
            GPBF_HAS_DATA_DESCRIPTOR_MASK
            COMPRESSION_STORED
            COMPRESSION_DEFLATED
            COMPRESSION_LEVEL_NONE
            COMPRESSION_LEVEL_DEFAULT
            COMPRESSION_LEVEL_FASTEST
            COMPRESSION_LEVEL_BEST_COMPRESSION
            IFA_TEXT_FILE_MASK
            IFA_TEXT_FILE
            IFA_BINARY_FILE
            ) ],

        MISC_CONSTANTS => [ qw(
            FA_AMIGA
            FA_VAX_VMS
            FA_VM_CMS
            FA_ATARI_ST
            FA_OS2_HPFS
            FA_MACINTOSH
            FA_Z_SYSTEM
            FA_CPM
            FA_TOPS20
            FA_WINDOWS_NTFS
            FA_QDOS
            FA_ACORN
            FA_VFAT
            FA_MVS
            FA_BEOS
            FA_TANDEM
            FA_THEOS
            GPBF_IMPLODING_8K_SLIDING_DICTIONARY_MASK
            GPBF_IMPLODING_3_SHANNON_FANO_TREES_MASK
            GPBF_IS_COMPRESSED_PATCHED_DATA_MASK
            COMPRESSION_SHRUNK
            DEFLATING_COMPRESSION_NORMAL
            DEFLATING_COMPRESSION_MAXIMUM
            DEFLATING_COMPRESSION_FAST
            DEFLATING_COMPRESSION_SUPER_FAST
            COMPRESSION_REDUCED_1
            COMPRESSION_REDUCED_2
            COMPRESSION_REDUCED_3
            COMPRESSION_REDUCED_4
            COMPRESSION_IMPLODED
            COMPRESSION_TOKENIZED
            COMPRESSION_DEFLATED_ENHANCED
            COMPRESSION_PKWARE_DATA_COMPRESSION_LIBRARY_IMPLODED
            ) ],

        ERROR_CODES => [ qw(
            AZ_OK
            AZ_STREAM_END
            AZ_ERROR
            AZ_FORMAT_ERROR
            AZ_IO_ERROR
            ) ],

        # For Internal Use Only
        PKZIP_CONSTANTS => [ qw(
            SIGNATURE_FORMAT
            SIGNATURE_LENGTH
            LOCAL_FILE_HEADER_SIGNATURE
            LOCAL_FILE_HEADER_FORMAT
            LOCAL_FILE_HEADER_LENGTH
            CENTRAL_DIRECTORY_FILE_HEADER_SIGNATURE
            DATA_DESCRIPTOR_FORMAT
            DATA_DESCRIPTOR_LENGTH
            DATA_DESCRIPTOR_SIGNATURE
            DATA_DESCRIPTOR_FORMAT_NO_SIG
            DATA_DESCRIPTOR_LENGTH_NO_SIG
            CENTRAL_DIRECTORY_FILE_HEADER_FORMAT
            CENTRAL_DIRECTORY_FILE_HEADER_LENGTH
            END_OF_CENTRAL_DIRECTORY_SIGNATURE
            END_OF_CENTRAL_DIRECTORY_SIGNATURE_STRING
            END_OF_CENTRAL_DIRECTORY_FORMAT
            END_OF_CENTRAL_DIRECTORY_LENGTH
            ) ],

        # For Internal Use Only
        UTILITY_METHODS => [ qw(
            _error
            _printError
            _ioError
            _formatError
            _subclassResponsibility
            _binmode
            _isSeekable
            _newFileHandle
            _readSignature
            _asZipDirName
            ) ],
    );

    # Add all the constant names and error code names to @EXPORT_OK
    Exporter::export_ok_tags( qw(
        CONSTANTS
        ERROR_CODES
        PKZIP_CONSTANTS
        UTILITY_METHODS
        MISC_CONSTANTS
        ) );

}

# Error codes
use constant AZ_OK           => 0;
use constant AZ_STREAM_END   => 1;
use constant AZ_ERROR        => 2;
use constant AZ_FORMAT_ERROR => 3;
use constant AZ_IO_ERROR     => 4;

# File types
# Values of Archive::Zip::Member->fileAttributeFormat()

use constant FA_MSDOS        => 0;
use constant FA_AMIGA        => 1;
use constant FA_VAX_VMS      => 2;
use constant FA_UNIX         => 3;
use constant FA_VM_CMS       => 4;
use constant FA_ATARI_ST     => 5;
use constant FA_OS2_HPFS     => 6;
use constant FA_MACINTOSH    => 7;
use constant FA_Z_SYSTEM     => 8;
use constant FA_CPM          => 9;
use constant FA_TOPS20       => 10;
use constant FA_WINDOWS_NTFS => 11;
use constant FA_QDOS         => 12;
use constant FA_ACORN        => 13;
use constant FA_VFAT         => 14;
use constant FA_MVS          => 15;
use constant FA_BEOS         => 16;
use constant FA_TANDEM       => 17;
use constant FA_THEOS        => 18;

# general-purpose bit flag masks
# Found in Archive::Zip::Member->bitFlag()

use constant GPBF_ENCRYPTED_MASK             => 1 << 0;
use constant GPBF_DEFLATING_COMPRESSION_MASK => 3 << 1;
use constant GPBF_HAS_DATA_DESCRIPTOR_MASK   => 1 << 3;

# deflating compression types, if compressionMethod == COMPRESSION_DEFLATED
# ( Archive::Zip::Member->bitFlag() & GPBF_DEFLATING_COMPRESSION_MASK )

use constant DEFLATING_COMPRESSION_NORMAL     => 0 << 1;
use constant DEFLATING_COMPRESSION_MAXIMUM    => 1 << 1;
use constant DEFLATING_COMPRESSION_FAST       => 2 << 1;
use constant DEFLATING_COMPRESSION_SUPER_FAST => 3 << 1;

# compression method

# these two are the only ones supported in this module
use constant COMPRESSION_STORED                 => 0; # file is stored (no compression)
use constant COMPRESSION_DEFLATED               => 8; # file is Deflated
use constant COMPRESSION_LEVEL_NONE             => 0;
use constant COMPRESSION_LEVEL_DEFAULT          => -1;
use constant COMPRESSION_LEVEL_FASTEST          => 1;
use constant COMPRESSION_LEVEL_BEST_COMPRESSION => 9;

# internal file attribute bits
# Found in Archive::Zip::Member::internalFileAttributes()

use constant IFA_TEXT_FILE_MASK => 1;
use constant IFA_TEXT_FILE      => 1;
use constant IFA_BINARY_FILE    => 0;

# PKZIP file format miscellaneous constants (for internal use only)
use constant SIGNATURE_FORMAT   => "V";
use constant SIGNATURE_LENGTH   => 4;

# these lengths are without the signature.
use constant LOCAL_FILE_HEADER_SIGNATURE   => 0x04034b50;
use constant LOCAL_FILE_HEADER_FORMAT      => "v3 V4 v2";
use constant LOCAL_FILE_HEADER_LENGTH      => 26;

# PKZIP docs don't mention the signature, but Info-Zip writes it.
use constant DATA_DESCRIPTOR_SIGNATURE     => 0x08074b50;
use constant DATA_DESCRIPTOR_FORMAT        => "V3";
use constant DATA_DESCRIPTOR_LENGTH        => 12;

# but the signature is apparently optional.
use constant DATA_DESCRIPTOR_FORMAT_NO_SIG => "V2";
use constant DATA_DESCRIPTOR_LENGTH_NO_SIG => 8;

use constant CENTRAL_DIRECTORY_FILE_HEADER_SIGNATURE  => 0x02014b50;
use constant CENTRAL_DIRECTORY_FILE_HEADER_FORMAT     => "C2 v3 V4 v5 V2";
use constant CENTRAL_DIRECTORY_FILE_HEADER_LENGTH     => 42;

use constant END_OF_CENTRAL_DIRECTORY_SIGNATURE        => 0x06054b50;
use constant END_OF_CENTRAL_DIRECTORY_SIGNATURE_STRING =>
    pack( "V", END_OF_CENTRAL_DIRECTORY_SIGNATURE );
use constant END_OF_CENTRAL_DIRECTORY_FORMAT           => "v4 V2 v";
use constant END_OF_CENTRAL_DIRECTORY_LENGTH           => 18;

use constant GPBF_IMPLODING_8K_SLIDING_DICTIONARY_MASK => 1 << 1;
use constant GPBF_IMPLODING_3_SHANNON_FANO_TREES_MASK  => 1 << 2;
use constant GPBF_IS_COMPRESSED_PATCHED_DATA_MASK      => 1 << 5;

# the rest of these are not supported in this module
use constant COMPRESSION_SHRUNK    => 1;    # file is Shrunk
use constant COMPRESSION_REDUCED_1 => 2;    # file is Reduced CF=1
use constant COMPRESSION_REDUCED_2 => 3;    # file is Reduced CF=2
use constant COMPRESSION_REDUCED_3 => 4;    # file is Reduced CF=3
use constant COMPRESSION_REDUCED_4 => 5;    # file is Reduced CF=4
use constant COMPRESSION_IMPLODED  => 6;    # file is Imploded
use constant COMPRESSION_TOKENIZED => 7;    # reserved for Tokenizing compr.
use constant COMPRESSION_DEFLATED_ENHANCED => 9;   # reserved for enh. Deflating
use constant COMPRESSION_PKWARE_DATA_COMPRESSION_LIBRARY_IMPLODED => 10;

# Load the various required classes
require Archive::Zip::Archive;
require Archive::Zip::Member;
require Archive::Zip::FileMember;
require Archive::Zip::DirectoryMember;
require Archive::Zip::ZipFileMember;
require Archive::Zip::NewFileMember;
require Archive::Zip::StringMember;

use constant ZIPARCHIVECLASS => 'Archive::Zip::Archive';
use constant ZIPMEMBERCLASS  => 'Archive::Zip::Member';

# Convenience functions

sub _ISA ($$) {
    # Can't rely on Scalar::Util, so use the next best way
    local $@;
    !! eval { ref $_[0] and $_[0]->isa($_[1]) };
}

sub _CAN ($$) {
    local $@;
    !! eval { ref $_[0] and $_[0]->can($_[1]) };
}





#####################################################################
# Methods

sub new {
    my $class = shift;
    return $class->ZIPARCHIVECLASS->new(@_);
}

sub computeCRC32 {
    my ( $data, $crc );

    if ( ref( $_[0] ) eq 'HASH' ) {
        $data = $_[0]->{string};
        $crc  = $_[0]->{checksum};
    }
    else {
        $data = shift;
        $data = shift if ref($data);
        $crc  = shift;
    }

	return Compress::Raw::Zlib::crc32( $data, $crc );
}

# Report or change chunk size used for reading and writing.
# Also sets Zlib's default buffer size (eventually).
sub setChunkSize {
    shift if ref( $_[0] ) eq 'Archive::Zip::Archive';
}

}

}