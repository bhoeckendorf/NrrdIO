#
# generates (to stdout) a header file intended to be included into
# source files where there is a concern of name-space collision induced
# by linking to two different version of NrrdIO
#
# You probably shouldn't run this on a mac: all symbols are preceeded by _
#

if (0 != $#ARGV) {
    die "usage: perl mangle.pl <prefix>\n";
}
$prefix = $ARGV[0];

print "#ifndef ${prefix}_NrrdIO_mangle_h\n";
print "#define ${prefix}_NrrdIO_mangle_h\n";
print "\n";
print "/*\n";
print "\n";
print "This header file mangles all symbols exported from the\n";
print "NrrdIO library. It is included in all files while building\n";
print "the NrrdIO library.  Due to namespace pollution, no NrrdIO\n";
print "headers should be included in .h files in ITK.\n";
print "\n";
print "This file was created via the mangle.pl script in the\n";
print "NrrdIO distribution:\n";
print "\n";
print "  perl mangle.pl ${prefix} > ${prefix}_NrrdIO_mangle.h\n";
print "\n";
print "*/\n";
print "\n";
open(NM, "nm libNrrdIO.a |");
while (<NM>) {
    if (m/ [TD] /) {
	s|.* [TD] (.*)|$1|g;
	chop;
	$sym = $_;
	print "#define ${sym} ${prefix}_${sym}\n";
    }
}
close(NM);
print "#endif\n";
