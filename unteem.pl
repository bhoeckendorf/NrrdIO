#
# This helps in converting teem source files into NrrdIO source files,
# by changing the way #includes are done, and by excluding the lines
# delimited by "BEGIN non-NrrdIO" and "END non-NrrdIO", as well as doing
# some other hacks
#

if (exists $ENV{"ITK_NRRDIO"}) {
    $ITK = 1;
} else {
    $ITK = 0;
}

$printing = 1;
while (<>) {
    $printing = 0 if (m/BEGIN non-NrrdIO/);
    s|#include "air.h"|#include "NrrdIO.h"|g;
    s|#include "biff.h"|#include "NrrdIO.h"|g;
    s|#include "nrrd.h"|#include "NrrdIO.h"|g;
    s|#include <teem(.*)>|#include "teem$1"|g;
    if ($ITK) {
	s|\/\* NrrdIO-hack-000 \*\/|\/\* THE FOLLOWING INCLUDE IS ONLY FOR THE ITK DISTRIBUTION.\n   This header mangles the symbols in the NrrdIO library, preventing\n   conflicts in applications linked against two versions of NrrdIO. \*\/\n#include "itk_NrrdIO_mangle.h"|g;
    }
    s|.* \/\* NrrdIO-hack-001 \*\/|#if 1|g;
    print if $printing;
    $printing = 1 if (m/END non-NrrdIO/);
}
