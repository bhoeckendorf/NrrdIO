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
  nrrdEncodingTypeBzip2, nrrdEncodingTypeBzip2,
  nrrdEncodingTypeLast
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

/* ------------------------ nrrdAxisInfo ------------------------- */

char
_nrrdAxisInfoStr[NRRD_AXIS_INFO_MAX+1][AIR_STRLEN_SMALL] = {
  "(unknown_axis_info)",
  "size",
  "spacing",
  "min",
  "max",
  "center",
  "label",
  "unit"
};

char
_nrrdAxisInfoDesc[NRRD_AXIS_INFO_MAX+1][AIR_STRLEN_MED] = {
  "unknown axis info",
  "number of samples along axis",
  "spacing between samples",
  "minimum position in \"world\" space associated with axis",
  "maximum position in \"world\" space associated with axis",
  "centering style for samples along this axis",
  "short description of space or value spanned by axis",
  "units in which sample spacing is measured"
};

airEnum
_nrrdAxisInfo = {
  "axis_info",
  NRRD_AXIS_INFO_MAX,
  _nrrdAxisInfoStr, NULL,
  _nrrdAxisInfoDesc,
  NULL, NULL,
  AIR_TRUE
};
airEnum *
nrrdAxisInfo = &_nrrdAxisInfo;
  
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
  "sizes",
  "spacings",
  "axis mins",
  "axis maxs",
  "centers",
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
  "key/value"
};

char
_nrrdFieldDesc[NRRD_FIELD_MAX+1][AIR_STRLEN_MED] = {
  "unknown field identifier",
  "comment",
  "short description of whole array and/or its provinance",
  "total number of samples in array",
  "type of sample value",
  "number of bytes in one block (for block-type)",
  "number of axes in array",
  "list of number of samples along each axis, aka \"dimensions\" of the array",
  "list of sample spacings along each axis",
  "list of minimum positions associated with each axis",
  "list of maximum positions associated with each axis",
  "list of sample centerings for each axis",
  "list of short descriptions for each axis",
  "list of units in which each axes' spacing is measured",
  "supposed minimum array value",
  "supposed maximum array value",
  "minimum array value prior to quantization",
  "maximum array value prior to quantization",
  "with detached headers, where is data to be found",
  "endiannes of data as written in file",
  "encoding of data written in file",
  "number of lines to skip prior to byte skip and reading data",
  "number of bytes to skip after line skip and prior to reading data",
  "string-based key/value pairs"
};

char
_nrrdFieldStrEqv[][AIR_STRLEN_SMALL]  = {
  "#",
  "content",
  "number",
  "type",
  "block size", "blocksize",
  "dimension",
  "sizes",
  "spacings",
  "axis mins", "axismins",
  "axis maxs", "axismaxs",
  "centers",
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
  nrrdField_sizes,
  nrrdField_spacings,
  nrrdField_axis_mins, nrrdField_axis_mins,
  nrrdField_axis_maxs, nrrdField_axis_maxs,
  nrrdField_centers,
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
};

airEnum
_nrrdField = {
  "nrrd_field",
  NRRD_FIELD_MAX,
  _nrrdFieldStr, NULL,
  _nrrdFieldDesc,
  _nrrdFieldStrEqv, _nrrdFieldValEqv, 
  AIR_FALSE
};
airEnum *
nrrdField = &_nrrdField;

