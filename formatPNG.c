/*
  NrrdIO: stand-alone code for basic nrrd functionality
  Copyright (C) 2003, 2002, 2001, 2000, 1999, 1998 University of Utah
 
  These source files have been copied and/or modified from teem,
  Gordon Kindlmann's research software; <http://teem.sourceforge.net>.
  Teem is licensed under the GNU Lesser Public License. The
  non-copyleft licensing defined here applies to only the source files
  in the NrrdIO distribution (not the rest of teem), and only to the
  files originating with NrrdIO (not analogous files in teem).
 
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.
 
  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:
 
  1. The origin of this software must not be misrepresented; you must
     not claim that you wrote the original software. If you use this
     software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
 
  2. Altered source versions must be plainly marked as such, and must
     not be misrepresented as being the original software.
 
  3. This notice may not be removed or altered from any source distribution.
*/

#include "NrrdIO.h"
#include "privateNrrd.h"

int
_nrrdFormatPNG_available(void) {
  
  return AIR_FALSE;
}

int
_nrrdFormatPNG_nameLooksLike(const char *filename) {
  
  return airEndsWith(filename, NRRD_EXT_PNG);
}

int
_nrrdFormatPNG_fitsInto(const Nrrd *nrrd, const NrrdEncoding *encoding,
			int useBiff) {
  char me[]="_nrrdFormatPNG_fitsInto", err[AIR_STRLEN_MED];
  
  sprintf(err, "%s: Sorry, %s format not available in NrrdIO",
	  me, nrrdFormatPNG->name);
  biffMaybeAdd(NRRD, err, useBiff); 
  return AIR_FALSE;

}

int
_nrrdFormatPNG_contentStartsLike(NrrdIoState *nio) {

  return AIR_FALSE;

}

int
_nrrdFormatPNG_read(FILE *file, Nrrd *nrrd, NrrdIoState *nio) {
  char me[]="_nrrdReadPNG", err[AIR_STRLEN_MED];

  sprintf(err, "%s: Sorry, %s format not available in NrrdIO",
	  me, nrrdFormatPNG->name);
  biffAdd(NRRD, err);
  return 1;
}

/* this strongly assumes that nrrdFitsInFormat() was true */
int
_nrrdFormatPNG_write(FILE *file, const Nrrd *_nrrd, NrrdIoState *nio) {
  char me[]="_nrrdFormatPNG_write", err[AIR_STRLEN_MED];

  sprintf(err, "%s: Sorry, %s format not available in NrrdIO",
	  me, nrrdFormatPNG->name);
  biffAdd(NRRD, err);
  return 1;
}

const NrrdFormat
_nrrdFormatPNG = {
  "PNG",
  AIR_FALSE,  /* isImage */
  AIR_FALSE,  /* readable */
  AIR_FALSE,  /* usesDIO */
  _nrrdFormatPNG_available,
  _nrrdFormatPNG_nameLooksLike,
  _nrrdFormatPNG_fitsInto,
  _nrrdFormatPNG_contentStartsLike,
  _nrrdFormatPNG_read,
  _nrrdFormatPNG_write
};

const NrrdFormat *const
nrrdFormatPNG = &_nrrdFormatPNG;
