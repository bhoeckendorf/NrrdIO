/*
  NrrdIO: stand-alone code for basic nrrd functionality
  Copyright (C) 2005  Gordon Kindlmann
  Copyright (C) 2004, 2003, 2002, 2001, 2000, 1999, 1998  University of Utah
 
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

/*
** Rules of thumb for editing these things.  The airEnum definitions are
** unfortunately EXTREMELY sensitive to small typo errors, and there is
** no good way to detect the errors.  So:
**
** 1) Be awake and undistracted.  Turn down the music.
** 2) When editing the char arrays, make sure that you put commas
**    where you mean them to be.  C's automatic string concatenation 
**    is not your friend here.
** 3) When editing the *StrEqv and *ValEqv arrays, make absolutely
**    sure that both are changed in parallel.  Use only one enum value
**    per line; putting all equivalents on that line, and make sure that
**    there is one line in both *StrEqv and *ValEqv for all the possible
**    enum values, and that there are as many elements in each line.
** 4) Make sure that additions here are reflected in nrrdEnums.h and
**    vice versa.
*/

/* ------------------------ nrrdFormat ------------------------- */

char
_nrrdFormatTypeStr[NRRD_FORMAT_TYPE_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_format)",
  "nrrd",
  "pnm",
  "png",
  "vtk",
  "text",
  "eps"
};

char
_nrrdFormatTypeDesc[NRRD_FORMAT_TYPE_MAX+1][AIR_STRLEN_MED] = {
  "unknown_format",
  "native format for nearly raw raster data",
  "Portable aNy Map: includes PGM for grayscale and PPM for color",
  "Portable Network Graphics: lossless compression of 8- and 16-bit data",
  "Visualization ToolKit STRUCTURED_POINTS data",
  "white-space-delimited plain text encoding of 2-D float array",
  "Encapsulated PostScript images"
};

char
_nrrdFormatTypeStrEqv[][AIR_STRLEN_SMALL] = {
  "nrrd",
  "pnm",
  "png",
  "vtk",
  "table", "text", "txt",
  "eps",
  ""
};

int
_nrrdFormatTypeValEqv[] = {
  nrrdFormatTypeNRRD,
  nrrdFormatTypePNM,
  nrrdFormatTypePNG,
  nrrdFormatTypeVTK,
  nrrdFormatTypeText, nrrdFormatTypeText, nrrdFormatTypeText,
  nrrdFormatTypeEPS
};

airEnum
_nrrdFormatType = {
  "format",
  NRRD_FORMAT_TYPE_MAX,
  _nrrdFormatTypeStr,  NULL,
  _nrrdFormatTypeDesc,
  _nrrdFormatTypeStrEqv, _nrrdFormatTypeValEqv,
  AIR_FALSE
};
airEnum *
nrrdFormatType = &_nrrdFormatType;

/* ------------------------ nrrdType ------------------------- */

char 
_nrrdTypeStr[NRRD_TYPE_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_type)",
  "signed char",
  "unsigned char",
  "short",
  "unsigned short",
  "int",
  "unsigned int",
  "long long int",
  "unsigned long long int",
  "float",
  "double",
  "block"
};

char 
_nrrdTypeDesc[NRRD_TYPE_MAX+1][AIR_STRLEN_MED] = {
  "unknown type",
  "signed 1-byte integer",
  "unsigned 1-byte integer",
  "signed 2-byte integer",
  "unsigned 2-byte integer",
  "signed 4-byte integer",
  "unsigned 4-byte integer",
  "signed 8-byte integer",
  "unsigned 8-byte integer",
  "4-byte floating point",
  "8-byte floating point",
  "size user-defined at run-time"
};

#define ntCH nrrdTypeChar
#define ntUC nrrdTypeUChar
#define ntSH nrrdTypeShort
#define ntUS nrrdTypeUShort
#define ntIN nrrdTypeInt
#define ntUI nrrdTypeUInt
#define ntLL nrrdTypeLLong
#define ntUL nrrdTypeULLong
#define ntFL nrrdTypeFloat
#define ntDB nrrdTypeDouble
#define ntBL nrrdTypeBlock

char
_nrrdTypeStrEqv[][AIR_STRLEN_SMALL] = {
  "signed char", /* but NOT just "char" */ "int8", "int8_t",
  "uchar", "unsigned char", "uint8", "uint8_t", 
  "short", "short int", "signed short", "signed short int", "int16", "int16_t",
  "ushort", "unsigned short", "unsigned short int", "uint16", "uint16_t", 
  "int", "signed int", "int32", "int32_t", 
  "uint", "unsigned int", "uint32", "uint32_t",
  "longlong", "long long", "long long int", "signed long long",
               "signed long long int", "int64", "int64_t", 
  "ulonglong", "unsigned long long", "unsigned long long int",
               "uint64", "uint64_t", 
  "float",
  "double",
  "block",
  ""
};

int
_nrrdTypeValEqv[] = {
  ntCH, ntCH, ntCH,
  ntUC, ntUC, ntUC, ntUC,
  ntSH, ntSH, ntSH, ntSH, ntSH, ntSH,
  ntUS, ntUS, ntUS, ntUS, ntUS,
  ntIN, ntIN, ntIN, ntIN,
  ntUI, ntUI, ntUI, ntUI, 
  ntLL, ntLL, ntLL, ntLL, ntLL, ntLL, ntLL, 
  ntUL, ntUL, ntUL, ntUL, ntUL, 
  ntFL,
  ntDB,
  ntBL
};

airEnum
_nrrdType = {
  "type",
  NRRD_TYPE_MAX,
  _nrrdTypeStr, NULL,
  _nrrdTypeDesc,
  _nrrdTypeStrEqv, _nrrdTypeValEqv,
  AIR_FALSE
};
airEnum *
nrrdType = &_nrrdType;

/* ------------------------ nrrdEncodingType ------------------------- */

char
_nrrdEncodingTypeStr[NRRD_ENCODING_TYPE_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_encoding)",
  "raw",
  "ascii",
  "hex",
  "gz",
  "bz2"
};

char
_nrrdEncodingTypeDesc[NRRD_ENCODING_TYPE_MAX+1][AIR_STRLEN_MED] = {
  "unknown encoding",
  "file is byte-for-byte same as memory representation",
  "values written out in ASCII",
  "case-insenstive hexadecimal encoding (2 chars / byte)",
  "gzip compression of binary encoding",
  "bzip2 compression of binary encoding"
};

char
_nrrdEncodingTypeStrEqv[][AIR_STRLEN_SMALL] = {
  "(unknown_encoding)",
  "raw",
  "txt", "text", "ascii",
  "hex",
  "gz", "gzip",
  "bz2", "bzip2",
  ""
};

int
_nrrdEncodingTypeValEqv[] = {
  nrrdEncodingTypeUnknown,
  nrrdEncodingTypeRaw,
  nrrdEncodingTypeAscii, nrrdEncodingTypeAscii, nrrdEncodingTypeAscii,
  nrrdEncodingTypeHex,
  nrrdEncodingTypeGzip, nrrdEncodingTypeGzip,
  nrrdEncodingTypeBzip2, nrrdEncodingTypeBzip2
};

airEnum
_nrrdEncodingType = {
  "encoding",
  NRRD_ENCODING_TYPE_MAX,
  _nrrdEncodingTypeStr, NULL,
  _nrrdEncodingTypeDesc,
  _nrrdEncodingTypeStrEqv, _nrrdEncodingTypeValEqv,
  AIR_FALSE
};
airEnum *
nrrdEncodingType = &_nrrdEncodingType;

/* ------------------------ nrrdCenter ------------------------- */

char
_nrrdCenterStr[NRRD_CENTER_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_center)",
  "node",
  "cell"
};

char
_nrrdCenterDesc[NRRD_CENTER_MAX+1][AIR_STRLEN_MED] = {
  "unknown centering",
  "samples are at boundaries between elements along axis",
  "samples are at centers of elements along axis",
};

airEnum
_nrrdCenter_enum = {
  "centering",
  NRRD_CENTER_MAX,
  _nrrdCenterStr, NULL,
  _nrrdCenterDesc,
  NULL, NULL,
  AIR_FALSE
};
airEnum *
nrrdCenter = &_nrrdCenter_enum;

/* ------------------------ nrrdKind ------------------------- */

/*
  nrrdKindDomain,              1: "Yes, you can resample me" 
  nrrdKindList,                2: "No, it is goofy to resample me" 
  nrrdKindStub,                3: axis with one sample (a placeholder) 
  nrrdKindScalar,              4: effectively, same as a stub 
  nrrdKindComplex,             5: real and imaginary components 
  nrrdKind2Vector,             6: 2 component vector 
  nrrdKind3Color,              7: ANY 3-component color value 
  nrrdKind4Color,              8: ANY 4-component color value 
  nrrdKind3Vector,             9: 3 component vector 
  nrrdKind3Normal,            10: 3 component vector, assumed normalized 
  nrrdKind4Vector,            11: 4 component vector 
  nrrdKind2DSymMatrix,        12: Mxx Mxy Myy 
  nrrdKind2DMaskedSymMatrix,  13: mask Mxx Mxy Myy 
  nrrdKind2DMatrix,           14: Mxx Mxy Myx Myy 
  nrrdKind2DMaskedMatrix,     15: mask Mxx Mxy Myx Myy 
  nrrdKind3DSymMatrix,        16: Mxx Mxy Mxz Myy Myz Mzz 
  nrrdKind3DMaskedSymMatrix,  17: mask Mxx Mxy Mxz Myy Myz Mzz 
  nrrdKind3DMatrix,           18: Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz 
  nrrdKind3DMaskedMatrix,     19: mask Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz 
*/

char
_nrrdKindStr[NRRD_KIND_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_kind)",
  "domain",
  "list",
  "stub",
  "scalar",
  "complex",
  "2-vector",
  "3-color",
  "4-color",
  "3-vector",
  "3-normal",
  "4-vector",
  "2D-symmetric-matrix",
  "2D-masked-symmetric-matrix",
  "2D-matrix",
  "2D-masked-matrix",
  "3D-symmetric-matrix",
  "3D-masked-symmetric-matrix",
  "3D-matrix",
  "3D-masked-matrix"
};

char
_nrrdKindDesc[NRRD_KIND_MAX+1][AIR_STRLEN_MED] = {
  "unknown kind",
  "a domain variable of the function which the nrrd samples",
  "some list of attributes; it makes no sense to resample along these",
  "a place-holder axis with a single sample",
  "axis used to indicate that the nrrd contains a scalar value",
  "real and imaginary parts of a value",
  "a 2-component vector",
  "any 3-component color value",
  "any 4-component color value",
  "a 3-element vector",
  "a 3-element vector which is assumed normalized",
  "a 4-element vector",
  "3 elements of 2D symmetric matrix: Mxx Mxy Myy",
  "mask plus 3 elements of 2D symmetric matrix: mask Mxx Mxy Myy",
  "4 elements of general 2D matrix: Mxx Mxy Myx Myy",
  "mask plus 4 elements of general 2D matrix: mask Mxx Mxy Myx Myy",
  "6 elements of 3D symmetric matrix: Mxx Mxy Mxz Myy Myz Mzz",
  "mask plus 6 elements of 3D symmetric matrix: mask Mxx Mxy Mxz Myy Myz Mzz",
  "9 elements of general 3D matrix: Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz",
  "mask plus 9 elements of general 3D matrix: mask Mxx Mxy Mxz Myx Myy Myz Mzx Mzy Mzz"
};

char
_nrrdKindStr_Eqv[][AIR_STRLEN_SMALL] = {
  "domain",
  "list",
  "stub",
  "scalar",
  "complex",
  "2-vector",
  "3-color",
  "4-color",
  "3-vector",
  "3-normal",
  "4-vector",
  "2D-symmetric-matrix", "2D-sym-matrix", 
        "2D-symmetric-tensor", "2D-sym-tensor",
  "2D-masked-symmetric-matrix", "2D-masked-sym-matrix",
        "2D-masked-symmetric-tensor", "2D-masked-sym-tensor",
  "2D-matrix",
        "2D-tensor",
  "2D-masked-matrix",
        "2D-masked-tensor",
  "3D-symmetric-matrix", "3D-sym-matrix",
        "3D-symmetric-tensor", "3D-sym-tensor",
  "3D-masked-symmetric-matrix", "3D-masked-sym-matrix",
        "3D-masked-symmetric-tensor", "3D-masked-sym-tensor",
  "3D-matrix",
        "3D-tensor",
  "3D-masked-matrix",
        "3D-masked-tensor",
  ""
};

int
_nrrdKindVal_Eqv[] = {
  nrrdKindDomain,
  nrrdKindList,
  nrrdKindStub,
  nrrdKindScalar,
  nrrdKindComplex,
  nrrdKind2Vector,
  nrrdKind3Color,
  nrrdKind4Color,
  nrrdKind3Vector,
  nrrdKind3Normal,
  nrrdKind4Vector,
  nrrdKind2DSymMatrix, nrrdKind2DSymMatrix,
        nrrdKind2DSymMatrix, nrrdKind2DSymMatrix,
  nrrdKind2DMaskedSymMatrix, nrrdKind2DMaskedSymMatrix,
        nrrdKind2DMaskedSymMatrix, nrrdKind2DMaskedSymMatrix,
  nrrdKind2DMatrix,
        nrrdKind2DMatrix,
  nrrdKind2DMaskedMatrix,
        nrrdKind2DMaskedMatrix,
  nrrdKind3DSymMatrix, nrrdKind3DSymMatrix,
        nrrdKind3DSymMatrix, nrrdKind3DSymMatrix,
  nrrdKind3DMaskedSymMatrix, nrrdKind3DMaskedSymMatrix,
        nrrdKind3DMaskedSymMatrix, nrrdKind3DMaskedSymMatrix,
  nrrdKind3DMatrix,
        nrrdKind3DMatrix,
  nrrdKind3DMaskedMatrix,
        nrrdKind3DMaskedMatrix,
};

airEnum
_nrrdKind_enum = {
  "kind",
  NRRD_KIND_MAX,
  _nrrdKindStr, NULL,
  _nrrdKindDesc,
  _nrrdKindStr_Eqv, _nrrdKindVal_Eqv,
  AIR_FALSE
};
airEnum *
nrrdKind = &_nrrdKind_enum;

/* ------------------------ nrrdField ------------------------- */

char
_nrrdFieldStr[NRRD_FIELD_MAX+1][AIR_STRLEN_SMALL] = {
  "Ernesto \"Che\" Guevara",
  "#",
  "content",
  "number",
  "type",
  "block size",
  "dimension",
  "space",
  "space dimension",
  "sizes",
  "spacings",
  "thicknesses",
  "axis mins",
  "axis maxs",
  "space directions",
  "centers",
  "kinds",
  "labels",
  "units",
  "min",
  "max",
  "old min",
  "old max",
  "data file",
  "endian",
  "encoding",
  "line skip",
  "byte skip",
  "key/value",
  "sample units",
  "space units",
  "space origin"
};

char
_nrrdFieldDesc[NRRD_FIELD_MAX+1][AIR_STRLEN_MED] = {
  "unknown field identifier",
  "comment",
  "short description of whole array and/or its provenance",
  "total number of samples in array",
  "type of sample value",
  "number of bytes in one block (for block-type)",
  "number of axes in array",
  "identifier for space in which array grid lies",
  "dimension of space in which array grid lies",
  "list of number of samples along each axis, aka \"dimensions\" of the array",
  "list of sample spacings along each axis",
  "list of sample thicknesses along each axis",
  "list of minimum positions associated with each axis",
  "list of maximum positions associated with each axis",
  "list of direction inter-sample vectors for each axis",
  "list of sample centerings for each axis",
  "list of kinds for each axis",
  "list of short descriptions for each axis",
  "list of units in which each axes' spacing and thickness is measured",
  "supposed minimum array value",
  "supposed maximum array value",
  "minimum array value prior to quantization",
  "maximum array value prior to quantization",
  "with detached headers, where is data to be found",
  "endiannes of data as written in file",
  "encoding of data written in file",
  "number of lines to skip prior to byte skip and reading data",
  "number of bytes to skip after line skip and prior to reading data",
  "string-based key/value pairs",
  "units of measurement of (scalar) values inside array itself",
  "list of units for measuring origin and direct vectors' coefficients",
  "location in space of center of first (lowest memory address) sample"
};

char
_nrrdFieldStrEqv[][AIR_STRLEN_SMALL]  = {
  "#",
  "content",
  "number",
  "type",
  "block size", "blocksize",
  "dimension",
  "space",
  "space dimension", "spacedimension",
  "sizes",
  "spacings",
  "thicknesses",
  "axis mins", "axismins",
  "axis maxs", "axismaxs",
  "space directions", "spacedirections",
  "centers",
  "kinds",
  "labels",
  "units",
  "min",
  "max",
  "old min", "oldmin",
  "old max", "oldmax",
  "data file", "datafile",
  "endian",
  "encoding",
  "line skip", "lineskip",
  "byte skip", "byteskip",
  /* nothing for keyvalue */
  "sample units", "sampleunits",
  "space units", "spaceunits",
  "space origin", "spaceorigin",
  ""
};

int
_nrrdFieldValEqv[] = {
  nrrdField_comment,
  nrrdField_content,
  nrrdField_number,
  nrrdField_type,
  nrrdField_block_size, nrrdField_block_size,
  nrrdField_dimension,
  nrrdField_space,
  nrrdField_space_dimension, nrrdField_space_dimension,
  nrrdField_sizes,
  nrrdField_spacings,
  nrrdField_thicknesses,
  nrrdField_axis_mins, nrrdField_axis_mins,
  nrrdField_axis_maxs, nrrdField_axis_maxs,
  nrrdField_space_directions, nrrdField_space_directions,
  nrrdField_centers,
  nrrdField_kinds,
  nrrdField_labels,
  nrrdField_units,
  nrrdField_min,
  nrrdField_max,
  nrrdField_old_min, nrrdField_old_min,
  nrrdField_old_max, nrrdField_old_max,
  nrrdField_data_file, nrrdField_data_file,
  nrrdField_endian,
  nrrdField_encoding,
  nrrdField_line_skip, nrrdField_line_skip,
  nrrdField_byte_skip, nrrdField_byte_skip,
  /* nothing for keyvalue */
  nrrdField_sample_units, nrrdField_sample_units,
  nrrdField_space_units, nrrdField_space_units,
  nrrdField_space_origin, nrrdField_space_origin,
};

airEnum
_nrrdField = {
  "nrrd_field",
  NRRD_FIELD_MAX,
  _nrrdFieldStr, NULL,
  _nrrdFieldDesc,
  _nrrdFieldStrEqv, _nrrdFieldValEqv, 
  AIR_FALSE  /* field identifiers not case sensitive */
};
airEnum *
nrrdField = &_nrrdField;

/* ------------------------ nrrdSpace ------------------------- */

/*
  nrrdSpaceRightAnteriorSuperior,     *  1: NIFTI-1 *
  nrrdSpaceLeftAnteriorSuperior,      *  2: standard Analyze *
  nrrdSpaceLeftPosteriorSuperior,     *  3: DICOM *
  nrrdSpaceRightAnteriorSuperiorTime, *  4: *
  nrrdSpaceLeftAnteriorSuperiorTime,  *  5: *
  nrrdSpaceLeftPosteriorSuperiorTime, *  6: *
  nrrdSpace3DRightHanded,             *  7: *
  nrrdSpace3DLeftHanded,              *  8: *
  nrrdSpace3DRightHandedTime,         *  9: *
  nrrdSpace3DLeftHandedTime,          * 10: *
*/

char
_nrrdSpaceStr[NRRD_SPACE_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_space)",
  "right-anterior-superior",
  "left-anterior-superior",
  "left-posterior-superior",
  "right-anterior-superior-time",
  "left-anterior-superior-time",
  "left-posterior-superior-time",
  "3D-right-handed",
  "3D-left-handed",
  "3D-right-handed-time",
  "3D-left-handed-time"
};

char
_nrrdSpaceDesc[NRRD_SPACE_MAX+1][AIR_STRLEN_MED] = {
  "unknown space",
  "right-anterior-superior (used in NIFTI-1 and Slicer)",
  "left-anterior-superior (used in Analyze 7.5)",
  "left-posterior-superior (used in DICOM)",
  "right-anterior-superior-time",
  "left-anterior-superior-time",
  "left-posterior-superior-time",
  "3D-right-handed",
  "3D-left-handed",
  "3D-right-handed-time",
  "3D-left-handed-time"
};

char
_nrrdSpaceStrEqv[][AIR_STRLEN_SMALL] = {
  "(unknown_space)",
  "right-anterior-superior", "right anterior superior",
      "rightanteriorsuperior", "RAS",
  "left-anterior-superior", "left anterior superior",
      "leftanteriorsuperior", "LAS",
  "left-posterior-superior", "left posterior superior",
      "leftposteriorsuperior", "LPS",
  "right-anterior-superior-time", "right anterior superior time",
      "rightanteriorsuperiortime", "RAST",
  "left-anterior-superior-time", "left anterior superior time",
      "leftanteriorsuperiortime", "LAST",
  "left-posterior-superior-time", "left posterior superior time",
      "leftposteriorsuperiortime", "LPST",
  "3D-right-handed", "3D right handed", "3Drighthanded"
  "3D-left-handed", "3D left handed", "3Dlefthanded",
  "3D-right-handed-time", "3D right handed time",
      "3Drighthandedtime",
  "3D-left-handed-time", "3D left handed time",
      "3Dlefthandedtime",
  ""
};

int
_nrrdSpaceValEqv[] = {
  nrrdSpaceUnknown,
  nrrdSpaceRightAnteriorSuperior, nrrdSpaceRightAnteriorSuperior,
     nrrdSpaceRightAnteriorSuperior, nrrdSpaceRightAnteriorSuperior,
  nrrdSpaceLeftAnteriorSuperior, nrrdSpaceLeftAnteriorSuperior,
     nrrdSpaceLeftAnteriorSuperior, nrrdSpaceLeftAnteriorSuperior,
  nrrdSpaceLeftPosteriorSuperior, nrrdSpaceLeftPosteriorSuperior,
     nrrdSpaceLeftPosteriorSuperior, nrrdSpaceLeftPosteriorSuperior,
  nrrdSpaceRightAnteriorSuperiorTime, nrrdSpaceRightAnteriorSuperiorTime,
     nrrdSpaceRightAnteriorSuperiorTime, nrrdSpaceRightAnteriorSuperiorTime,
  nrrdSpaceLeftAnteriorSuperiorTime, nrrdSpaceLeftAnteriorSuperiorTime,
     nrrdSpaceLeftAnteriorSuperiorTime, nrrdSpaceLeftAnteriorSuperiorTime,
  nrrdSpaceLeftPosteriorSuperiorTime, nrrdSpaceLeftPosteriorSuperiorTime,
     nrrdSpaceLeftPosteriorSuperiorTime, nrrdSpaceLeftPosteriorSuperiorTime,
  nrrdSpace3DRightHanded, nrrdSpace3DRightHanded, nrrdSpace3DRightHanded,
  nrrdSpace3DLeftHanded, nrrdSpace3DLeftHanded, nrrdSpace3DLeftHanded,
  nrrdSpace3DRightHandedTime, nrrdSpace3DRightHandedTime, 
     nrrdSpace3DRightHandedTime,
  nrrdSpace3DLeftHandedTime, nrrdSpace3DLeftHandedTime,
     nrrdSpace3DLeftHandedTime
};

airEnum
_nrrdSpace = {
  "space",
  NRRD_SPACE_MAX,
  _nrrdSpaceStr, NULL,
  _nrrdSpaceDesc,
  _nrrdSpaceStrEqv, _nrrdSpaceValEqv,
  AIR_FALSE
};
airEnum *
nrrdSpace = &_nrrdSpace;

