/*
  NrrdIO: stand-alone code for basic nrrd functionality
  Copyright (C) 2004, 2003, 2002, 2001, 2000, 1999, 1998 University of Utah
 
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

char
nrrdTypePrintfStr[NRRD_TYPE_MAX+1][AIR_STRLEN_SMALL] = {
  "%*d",  /* what else? sscanf: skip, printf: use "minimum precision" */
  "%d",
  "%u",
  "%hd",
  "%hu",
  "%d",
  "%u",
  AIR_LLONG_FMT,
  AIR_ULLONG_FMT,
  "%f",
  "%lf",
  "%*d"  /* what else? */
};

/*
** the setting of NRRD_TYPE_BIGGEST has to be in accordance with this
*/
int 
nrrdTypeSize[NRRD_TYPE_MAX+1] = {
  0,  /* unknown */
  1,  /* char */
  1,  /* unsigned char */
  2,  /* short */
  2,  /* unsigned short */
  4,  /* int */
  4,  /* unsigned int */
  8,  /* long long */
  8,  /* unsigned long long */
  4,  /* float */
  8,  /* double */
  0   /* effectively unknown; user has to set explicitly */
};

int 
nrrdTypeIsIntegral[NRRD_TYPE_MAX+1] = {
  0,  /* unknown */
  1,  /* char */
  1,  /* unsigned char */
  1,  /* short */
  1,  /* unsigned short */
  1,  /* int */
  1,  /* unsigned int */
  1,  /* long long */
  1,  /* unsigned long long */
  0,  /* float */
  0,  /* double */
  1   /* for some reason we pretend that blocks are integers */
};

int 
nrrdTypeIsUnsigned[NRRD_TYPE_MAX+1] = {
  0,  /* unknown */
  0,  /* char */
  1,  /* unsigned char */
  0,  /* short */
  1,  /* unsigned short */
  0,  /* int */
  1,  /* unsigned int */
  0,  /* long long */
  1,  /* unsigned long long */
  0,  /* float */
  0,  /* double */
  0   /* for some reason we pretend that blocks are signed */
};

/*
******** nrrdTypeMin[]
******** nrrdTypeMax[]
**
** only intended for small (<= 32 bits) integral types,
** so that we know how to "unquantize" integral values.
** A 64-bit double can correctly store the 32-bit integral
** mins and maxs, but gets the last few places wrong in the
** 64-bit mins and max.
*/
double
nrrdTypeMin[NRRD_TYPE_MAX+1] = {
  0,               /* unknown */
  SCHAR_MIN,       /* char */
  0,               /* unsigned char */
  SHRT_MIN,        /* short */
  0,               /* unsigned short */
  INT_MIN,         /* int */
  0,               /* unsigned int */
  NRRD_LLONG_MIN,  /* long long */
  0,               /* unsigned long long */
  0,               /* float */
  0,               /* double */
  0                /* punt */
},
nrrdTypeMax[NRRD_TYPE_MAX+1] = {
  0,               /* unknown */
  SCHAR_MAX,       /* char */
  UCHAR_MAX,       /* unsigned char */
  SHRT_MAX,        /* short */
  USHRT_MAX,       /* unsigned short */
  INT_MAX,         /* int */
  UINT_MAX,        /* unsigned int */
  NRRD_LLONG_MAX,  /* long long */
  NRRD_ULLONG_MAX, /* unsigned long long */
  0,               /* float */
  0,               /* double */
  0                /* punt */
};

/*
******** nrrdTypeNumberOfValues[]
**
** only meaningful for integral values, and only correct for
** 32-bit values; tells the number of different integral values that
** can be represented by the type
*/
double
nrrdTypeNumberOfValues[NRRD_TYPE_MAX+1] = {
  0,                         /* unknown */
  UCHAR_MAX+1,               /* char */
  UCHAR_MAX+1,               /* unsigned char */
  USHRT_MAX+1,               /* short */
  USHRT_MAX+1,               /* unsigned short */
  (double)UINT_MAX+1,        /* int */
  (double)UINT_MAX+1,        /* unsigned int */
  (double)NRRD_ULLONG_MAX+1, /* long long */
  (double)NRRD_ULLONG_MAX+1, /* unsigned long long */
  0,                         /* float */
  0,                         /* double */
  0                          /* punt */
};

/*
** _nrrdFieldValidInImage[]
**
** these fields are valid embedded in PNM and PNG comments
** This does NOT include the fields who's values are constrained
** the image format (and in the case of PNM, magic) itself.
*/
int
_nrrdFieldValidInImage[NRRD_FIELD_MAX+1] = {
  0, /* nrrdField_unknown */
  1, /* nrrdField_comment */
  1, /* nrrdField_content */
  0, /* nrrdField_number */
  0, /* nrrdField_type */
  0, /* nrrdField_block_size */
  0, /* nrrdField_dimension */
  0, /* nrrdField_sizes */
  1, /* nrrdField_spacings */
  1, /* nrrdField_axis_mins */
  1, /* nrrdField_axis_maxs */
  1, /* nrrdField_centers */
  1, /* nrrdField_kinds */
  1, /* nrrdField_labels */
  1, /* nrrdField_units */
  0, /* nrrdField_min */
  0, /* nrrdField_max */
  1, /* nrrdField_old_min */
  1, /* nrrdField_old_max */
  0, /* nrrdField_data_file */
  0, /* nrrdField_endian */
  0, /* nrrdField_encoding */
  0, /* nrrdField_line_skip */
  0  /* nrrdField_byte_skip */
};

/*
** _nrrdFieldOnePerAxis
** 
** whether or not you need one value per axis, like labels and spacings
*/
int
_nrrdFieldOnePerAxis[NRRD_FIELD_MAX+1] = {
  0, /* nrrdField_unknown */
  0, /* nrrdField_comment */
  0, /* nrrdField_content */
  0, /* nrrdField_number */
  0, /* nrrdField_type */
  0, /* nrrdField_block_size */
  0, /* nrrdField_dimension */
  1, /* nrrdField_sizes */
  1, /* nrrdField_spacings */
  1, /* nrrdField_axis_mins */
  1, /* nrrdField_axis_maxs */
  1, /* nrrdField_centers */
  1, /* nrrdField_kinds */
  1, /* nrrdField_labels */
  1, /* nrrdField_units */
  0, /* nrrdField_min */
  0, /* nrrdField_max */
  0, /* nrrdField_old_min */
  0, /* nrrdField_old_max */
  0, /* nrrdField_data_file */
  0, /* nrrdField_endian */
  0, /* nrrdField_encoding */
  0, /* nrrdField_line_skip */
  0  /* nrrdField_byte_skip */
};

/*
** _nrrdFieldValidInText[]
** 
** these fields are valid embedded in plain text comments
** This does NOT include the fields who's values are constrained
** the plain text format itself.
*/
int
_nrrdFieldValidInText[NRRD_FIELD_MAX+1] = {
  0, /* nrrdField_unknown */
  1, /* nrrdField_comment */
  1, /* nrrdField_content */
  0, /* nrrdField_number */
  0, /* nrrdField_type: decided AGAINST plain text holding general type 
	(but I forget why ...) */
  0, /* nrrdField_block_size */
  1, /* nrrdField_dimension: but can only be 1 or 2 */
  0, /* nrrdField_sizes */
  1, /* nrrdField_spacings */
  1, /* nrrdField_axis_mins */
  1, /* nrrdField_axis_maxs */
  1, /* nrrdField_centers */
  1, /* nrrdField_kinds */
  1, /* nrrdField_labels */
  1, /* nrrdField_units */
  0, /* nrrdField_min */
  0, /* nrrdField_max */
  1, /* nrrdField_old_min */
  1, /* nrrdField_old_max */
  0, /* nrrdField_data_file */
  0, /* nrrdField_endian */
  0, /* nrrdField_encoding */
  0, /* nrrdField_line_skip */
  0  /* nrrdField_byte_skip */
};

/*
** _nrrdFieldRequired[]
**
** regardless of whether its a nrrd, PNM, or plain text, these things
** need to be conveyed, either explicity or implicitly
*/
int
_nrrdFieldRequired[NRRD_FIELD_MAX+1] = {
  0, /* "Ernesto \"Che\" Guevara" */
  0, /* "#" */
  0, /* nrrdField_content */
  0, /* nrrdField_number */
  1, /* nrrdField_type */
  0, /* nrrdField_block size */
  1, /* nrrdField_dimension */
  1, /* nrrdField_sizes */
  0, /* nrrdField_spacings */
  0, /* nrrdField_axis mins */
  0, /* nrrdField_axis maxs */
  0, /* nrrdField_centers */
  0, /* nrrdField_kinds */
  0, /* nrrdField_labels */
  0, /* nrrdField_units */
  0, /* nrrdField_min */
  0, /* nrrdField_max */
  0, /* nrrdField_old min */
  0, /* nrrdField_old max */
  0, /* nrrdField_data file */
  0, /* nrrdField_endian */
  1, /* nrrdField_encoding */
  0, /* nrrdField_line skip */
  0  /* nrrdField_byte skip */
};

