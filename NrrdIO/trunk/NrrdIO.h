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

#ifndef AIR_HAS_BEEN_INCLUDED
#define AIR_HAS_BEEN_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) && !defined(TEEM_STATIC) && !defined(__CYGWIN__)
#define air_export __declspec(dllimport)
#else
#define air_export
#endif

#ifdef _WIN32
#if _MSC_VER < 1300 || !defined(_USE_MATH_DEFINES)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_E
#define M_E  2.71828182845904523536
#endif
#endif
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
typedef signed __int64 airLLong;
typedef unsigned __int64 airULLong;
#define AIR_LLONG_FMT "%I64d"
#define AIR_ULLONG_FMT "%I64u"
#define AIR_LLONG(x) x##i64
#define AIR_ULLONG(x) x##ui64
#else
typedef signed long long airLLong;
typedef unsigned long long airULLong;
#define AIR_LLONG_FMT "%lld"
#define AIR_ULLONG_FMT "%llu"
#define AIR_LLONG(x) x##ll
#define AIR_ULLONG(x) x##ull
#endif

#define AIR_STRLEN_SMALL (128+1)
#define AIR_STRLEN_MED   (256+1)
#define AIR_STRLEN_LARGE (512+1)
#define AIR_STRLEN_HUGE  (1024+1)

/* enum.c: enum value <--> string conversion utility */  
typedef struct {
  char name[AIR_STRLEN_SMALL];
               /* what are these things? */
  int M;       /* If "val" is NULL, the the valid enum values are from 1 to M
 	          (represented by strings str[1] through str[M]), and the
 	          unknown/invalid value is 0.  If "val" is non-NULL, the
 	          valid enum values are from val[1] to val[M] (but again, 
		  represented by strings str[1] through str[M]), and the
		  unknown/invalid value is val[0].  In both cases, str[0]
		  is the string to represent an unknown/invalid value */
  char (*str)[AIR_STRLEN_SMALL]; 
               /* "canonical" textual representation of the enum values */
  int *val;    /* non-NULL iff valid values in the enum are not [1..M], and/or
		  if value for unknown/invalid is not zero */
  char (*desc)[AIR_STRLEN_MED];
               /* desc[i] is a short description of the enum values represented
		  by str[i] (thereby starting with the unknown value), to be
		  used to by things like hest */
  char (*strEqv)[AIR_STRLEN_SMALL];  
               /* All the variations in strings recognized in mapping from
		  string to value (the values in valEqv).  This **MUST** be
		  terminated by a zero-length string ("") so as to signify
		  the end of the list.  This should not contain the string
		  for unknown/invalid.  If "strEqv" is NULL, then mapping
		  from string to value is done by traversing "str", and 
		  "valEqv" is ignored. */
  int *valEqv; /* The values corresponding to the strings in strEqv; there
		  should be one integer for each non-zero-length string in
		  strEqv: strEqv[i] is a valid string representation for
		  value valEqv[i]. This should not contain the value for
		  unknown/invalid.  This "valEqv" is ignored if "strEqv" is
		  NULL. */
  int sense;   /* require case matching on strings */
} airEnum;
extern int airEnumUnknown(airEnum *enm);
extern int airEnumValCheck(airEnum *enm, int val);
extern char *airEnumStr(airEnum *enm, int val);
extern char *airEnumDesc(airEnum *enm, int val);
extern int airEnumVal(airEnum *enm, const char *str);
extern char *airEnumFmtDesc(airEnum *enm, int val, int canon, const char *fmt);

/*
******** airEndian enum
**
** for identifying how a file was written to disk, for those encodings
** where the raw data on disk is dependent on the endianness of the
** architecture.
*/
enum {
  airEndianUnknown,         /* 0: nobody knows */
  airEndianLittle = 1234,   /* 1234: Intel and friends */
  airEndianBig = 4321,      /* 4321: the rest */
  airEndianLast
};
/* endianAir.c */
extern air_export airEnum *airEndian;
extern air_export const int airMyEndian;

/* array.c: poor-man's dynamically resizable arrays */
typedef struct {
  void *data,         /* where the data is */
    **dataP;          /* (possibly NULL) address of user's data variable,
			 kept in sync with internal "data" variable */
  int len,            /* length of array: # units for which there is
		         considered to be data (which is <= total # units
		         allocated).  The # bytes which contain data is
		         len*unit.  Always updated (unlike "*lenP") */
    *lenP,            /* (possibly NULL) address of user's length variable,
			 kept in sync with internal "len" variable */
    incr,             /* the granularity of the changes in amount of space
			 allocated: when the length reaches a multiple of
			 "incr", then the array is resized */
    size;             /* array is allocated to have "size" increments, or,
			 size*incr elements, or, 
			 size*incr*unit bytes */
  size_t unit;        /* the size in bytes of one element in the array */

  /* the following are all callbacks useful for maintaining either an array
     of pointers (allocCB and freeCB) or array of structs (initCB and
     doneCB).  allocCB or initCB is called when the array length increases,
     and freeCB or doneCB when it decreases.  Any of them can be NULL if no
     such activity is desired.  allocCB sets values in the array (as in
     storing the return from malloc(); freeCB is called on values in the
     array (as in calling free()), and the values are cast to void*.  allocCB
     and freeCB don't care about the value of "unit" (though perhaps they
     should).  initCB and doneCB are called on the _addresses_ of elements in
     the array.  allocCB and initCB are called for the elements in ascending
     order in the array, and freeCB and doneCB are called in descending
     order.  allocCB and initCB are mutually exclusive- they can't both be
     non-NULL. Same goes for freeCB and doneCB */
  void *(*allocCB)(void);  /* values of new elements set to return of this */
  void *(*freeCB)(void *); /* called on the values of invalidated elements */
  void (*initCB)(void *);  /* called on addresses of new elements */
  void (*doneCB)(void *);  /* called on addresses of invalidated elements */

} airArray;
extern airArray *airArrayNew(void **dataP, int *lenP, size_t unit, int incr);
extern void airArrayStructCB(airArray *a, 
			     void (*initCB)(void *), void (*doneCB)(void *));
extern void airArrayPointerCB(airArray *a, 
			      void *(*allocCB)(void), void *(*freeCB)(void *));
extern int airArraySetLen(airArray *a, int newlen);
extern int airArrayIncrLen(airArray *a, int delta);
extern airArray *airArrayNix(airArray *a);
extern airArray *airArrayNuke(airArray *a);


/*
******** airFP enum
**
** the different kinds of floating point number afforded by IEEE 754,
** and the values returned by airFPClass_f().
**
** The values probably won't agree with those in #include's like
** ieee.h, ieeefp.h, fp_class.h.  This is because IEEE 754 hasn't
** defined standard values for these, so everyone does it differently.
** 
** This enum uses underscores (against teem convention) to help
** legibility while also conforming to the spirit of the somewhat
** standard naming conventions
*/
enum {
  airFP_Unknown,               /*  0: nobody knows */
  airFP_SNAN,                  /*  1: signalling NaN */
  airFP_QNAN,                  /*  2: quiet NaN */
  airFP_POS_INF,               /*  3: positive infinity */
  airFP_NEG_INF,               /*  4: negative infinity */
  airFP_POS_NORM,              /*  5: positive normalized non-zero */
  airFP_NEG_NORM,              /*  6: negative normalized non-zero */
  airFP_POS_DENORM,            /*  7: positive denormalized non-zero */
  airFP_NEG_DENORM,            /*  8: negative denormalized non-zero */
  airFP_POS_ZERO,              /*  9: +0.0, positive zero */
  airFP_NEG_ZERO,              /* 10: -0.0, negative zero */
  airFP_Last                   /* after the last valid one */
};
/* 754.c: IEEE-754 related stuff values */
typedef union {
  unsigned int i;
  float f;
} airFloat;
typedef union {
  airULLong i;
  double d;
} airDouble;
extern air_export const int airMyQNaNHiBit;
extern air_export const int airMyBigBitField;
extern float airFPPartsToVal_f(int sign, int exp, int frac);
extern void airFPValToParts_f(int *signP, int *expP, int *fracP, float v);
extern double airFPPartsToVal_d(int sign, int exp, airULLong frac);
extern void airFPValToParts_d(int *signP, int *expP, airULLong *fracP,
			      double v);
extern float airFPGen_f(int cls);
extern double airFPGen_d(int cls);
extern int airFPClass_f(float val);
extern int airFPClass_d(double val);
extern void airFPFprintf_f(FILE *file, float val);
extern void airFPFprintf_d(FILE *file, double val);
extern air_export const airFloat airFloatQNaN;
extern air_export const airFloat airFloatSNaN;
extern air_export const airFloat airFloatPosInf;
extern air_export const airFloat airFloatNegInf;
extern float airNaN();
extern int airIsNaN(float f);
extern int airIsInf_f(float f);
extern int airIsInf_d(double d);
extern int airExists_f(float f);
extern int airExists_d(double d);

/*
******** airType
**
** Different types which air cares about.
** Currently only used in the command-line parsing, but perhaps will
** be used elsewhere in air later
*/
enum {
  airTypeUnknown,   /* 0 */
  airTypeBool,      /* 1 */
  airTypeInt,       /* 2 */
  airTypeFloat,     /* 3 */
  airTypeDouble,    /* 4 */
  airTypeChar,      /* 5 */
  airTypeString,    /* 6 */
  airTypeEnum,      /* 7 */
  airTypeOther,     /* 8 */
  airTypeLast
};
#define AIR_TYPE_MAX   8
/* parseAir.c */
extern double airAtod(const char *str);
extern int airSingleSscanf(const char *str, const char *fmt, void *ptr);
extern airEnum *airBool;
extern int airParseStrB(int *out, const char *s,
			const char *ct, int n, ... /* nothing used */);
extern int airParseStrI(int *out, const char *s,
			const char *ct, int n, ... /* nothing used */);
extern int airParseStrF(float *out, const char *s,
			const char *ct, int n, ... /* nothing used */);
extern int airParseStrD(double *out, const char *s,
			const char *ct, int n, ... /* nothing used */);
extern int airParseStrC(char *out, const char *s,
			const char *ct, int n, ... /* nothing used */);
extern int airParseStrS(char **out, const char *s,
			const char *ct, int n, ... /* REQUIRED, even if n>1:
						      int greedy */);
extern int airParseStrE(int *out, const char *s,
			const char *ct, int n, ... /* REQUIRED: airEnum *e */);
extern air_export int (*airParseStr[AIR_TYPE_MAX+1])(void *, const char *,
					  const char *, int, ...);

/* string.c */
extern char *airStrdup(const char *s);
extern size_t airStrlen(const char *s);
extern air_export int airStrtokQuoting;
extern char *airStrtok(char *s, const char *ct, char **last);
extern int airStrntok(const char *s, const char *ct);
extern char *airStrtrans(char *s, char from, char to);
extern int airEndsWith(const char *s, const char *suff);
extern char *airUnescape(char *s);
extern char *airOneLinify(char *s);
extern char *airToLower(char *str);
extern char *airToUpper(char *str);
extern int airOneLine(FILE *file, char *line, int size);

/* sane.c */
/*
******** airInsane enum
** 
** reasons for why airSanity() failed (specifically, the possible
** return values for airSanity()
*/
enum {
  airInsane_not,           /*  0: actually, all sanity checks passed */
  airInsane_endian,        /*  1: airMyEndian is wrong */
  airInsane_pInfExists,    /*  2: AIR_EXISTS(positive infinity) was true */
  airInsane_nInfExists,    /*  3: AIR_EXISTS(negative infinity) was true */
  airInsane_NaNExists,     /*  4: AIR_EXISTS(NaN) was true */
  airInsane_FltDblFPClass, /*  5: double -> float assignment messed up the
			       airFPClass_f() of the value */
  airInsane_QNaNHiBit,     /*  6: airMyQNaNHiBit is wrong */
  airInsane_dio,           /*  7: airMyDio set to something invalid */
  airInsane_32Bit,         /*  8: airMy32Bit is wrong */
  airInsane_FISize,        /*  9: sizeof(float), sizeof(int) not 4 */
  airInsane_DLSize         /* 10: sizeof(double), sizeof(airLLong) not 8 */
};
#define AIR_INSANE_MAX        10
extern const char *airInsaneErr(int insane);
extern int airSanity();

/* miscAir.c */
extern air_export const char *airTeemVersion;
extern air_export const char *airTeemReleaseDate;
extern void *airNull(void);
extern void *airSetNull(void **ptrP);
extern void *airFree(void *ptr);
extern void *airFreeP(void *_ptrP);
extern FILE *airFopen(const char *name, FILE *std, const char *mode);
extern FILE *airFclose(FILE *file);
extern int airSinglePrintf(FILE *file, char *str, const char *fmt, ...);
extern air_export const int airMy32Bit;

/* dio.c */
/*
******** airNoDio enum
**
** reasons for why direct I/O won't be used with a particular 
** file/pointer combination
*/
enum {
  airNoDio_okay,    /*  0: actually, you CAN do direct I/O */
  airNoDio_arch,    /*  1: teem thinks this architecture can't do it */
  airNoDio_format,  /*  2: teem thinks given data file format can't use it */
  airNoDio_file,    /*  3: given NULL file */
  airNoDio_std,     /*  4: DIO isn't possible for std{in|out|err} */
  airNoDio_fd,      /*  5: couldn't get underlying file descriptor */
  airNoDio_fcntl,   /*  6: the required fcntl() call failed */
  airNoDio_small,   /*  7: requested size is too small */
  airNoDio_size,    /*  8: requested size not a multiple of d_miniosz */
  airNoDio_ptr,     /*  9: pointer not multiple of d_mem */
  airNoDio_fpos,    /* 10: current file position not multiple of d_miniosz */
  airNoDio_test,    /* 11: couldn't memalign() even a small bit of memory */
  airNoDio_disable  /* 12: someone disabled it with airDisableDio */
};
#define AIR_NODIO_MAX  12
extern const char *airNoDioErr(int noDio);
extern air_export const int airMyDio;
extern air_export int airDisableDio;
extern int airDioInfo(int *mem, int *min, int *max, FILE *file);
extern int airDioTest(size_t size, FILE *file, void *ptr);
extern size_t airDioRead(FILE *file, void **ptrP, size_t size);
extern size_t airDioWrite(FILE *file, void *ptr, size_t size);

/* mop.c: clean-up utilities */
enum {
  airMopNever,
  airMopOnError,
  airMopOnOkay,
  airMopAlways
};
typedef void *(*airMopper)(void *);
typedef struct {
  void *ptr;         /* the thing to be processed */
  airMopper mop;     /* the function to which does the processing */
  int when;          /* from the airMopWhen enum */
} airMop;
extern airArray *airMopNew(void);
extern void airMopAdd(airArray *arr,
		      void *ptr, airMopper mop, int when);
extern void airMopSub(airArray *arr, void *ptr, airMopper mop);
extern void airMopMem(airArray *arr, void *_ptrP, int when);
extern void airMopUnMem(airArray *arr, void *_ptrP);
extern void airMopPrint(airArray *arr, void *_str, int when);
extern void airMopDone(airArray *arr, int error);
extern void airMopError(airArray *arr);
extern void airMopOkay(airArray *arr);
extern void airMopDebug(airArray *arr);

/*******     the interminable sea of defines and macros     *******/

#define AIR_TRUE 1
#define AIR_FALSE 0
#define AIR_WHITESPACE " \t\n\r\v\f"       /* K+R pg. 157 */

/*
******** AIR_ENDIAN, AIR_QNANHIBIT, AIR_DIO, AIR_BIGBITFIELD
**
** These reflect particulars of hardware which we're running on.
** The reason to have these in addition to TEEM_ENDIAN, TEEM_DIO, etc.,
** is that those are not by default defined for every source-file
** compilation: the teem library has to define NEED_ENDIAN, NEED_DIO, etc,
** and these in turn generate appropriate compile command-line flags
** by Common.mk. By having these defined here, they become available
** to anyone who simply links against the air library (and includes air.h),
** with no command-line flags required, and no usage of Common.mk required.
*/
#define AIR_ENDIAN (airMyEndian)
#define AIR_QNANHIBIT (airMyQNaNHiBit)
#define AIR_DIO (airMyDio)
#define AIR_32BIT (airMy32Bit)
#define AIR_BIGBITFIELD (airMyBigBitField)

/*
******** AIR_NAN, AIR_QNAN, AIR_SNAN, AIR_POS_INF, AIR_NEG_INF
**
** its nice to have these values available without the cost of a 
** function call.
**
** NOTE: AIR_POS_INF and AIR_NEG_INF correspond to the _unique_
** bit-patterns which signify positive and negative infinity.  With
** the NaNs, however, they are only one of many possible
** representations.
*/
#define AIR_NAN  (airFloatQNaN.f)
#define AIR_QNAN (airFloatQNaN.f)
#define AIR_SNAN (airFloatSNaN.f)
#define AIR_POS_INF (airFloatPosInf.f)
#define AIR_NEG_INF (airFloatNegInf.f)

/* 
******** AIR_EXISTS
**
** is non-zero (true) only for values which are not NaN or +/-infinity
** 
** You'd think that (x == x) might work, but no no no, some optimizing
** compilers (e.g. SGI's cc) say "well of course they're equal, for all
** possible values".  Bastards!
**
** One of the benefits of IEEE 754 floating point numbers is that
** gradual underflow means that x = y <==> x - y = 0 for any (positive
** or negative) normalized or denormalized float.  Otherwise this
** macro could not be valid; some floating point conventions say that
** a zero-valued exponent means zero, regardless of the mantissa.
**
** However, there MAY be problems on machines which use extended
** (80-bit) floating point registers, such as Intel chips- where the
** same initial value 1) directly read from the register, versus 2)
** saved to memory and loaded back, may end up being different.  I
** have yet to produce this behavior, or convince myself it can't
** happen.  If you have problems, then use the version of the macro
** which is a function call to airExists_d(), and please email me:
** gk@cs.utah.edu
**
** The reason to #define AIR_EXISTS as airExists_d is that on some
** optimizing compilers, the !((x) - (x)) doesn't work.  This has been
** the case on Windows and 64-bit irix6 (64 bit) with -Ofast.  If
** airSanity fails because a special value "exists", then use the
** first version of AIR_EXISTS.
**
** There are two performance consequences of using airExists_d(x):
** 1) Its a function call (but WIN32 can __inline it)
** 2) (via AIR_EXISTS_G) It requires bit-wise operations on 64-bit
** ints, which might be terribly slow.
**
** The reason for using airExists_d and not airExists_f is for
** doubles > FLT_MAX: airExists_f would say these are infinity.
*/
#ifdef _WIN32
#define AIR_EXISTS(x) (airExists_d(x))
#else
#define AIR_EXISTS(x) (!((x) - (x)))
#endif

/*
******** AIR_EXISTS_F(x)
**
** This is another way to check for non-specialness (not NaN, not
** +inf, not -inf) of a _float_, by making sure the exponent field
** isn't all ones.
**
** Unlike !((x) - (x)) or airExists(x), the argument to this macro
** MUST MUST MUST be a float, and the float must be of the standard
** 32-bit size, which must also be the size of an int.  The reason for
** this constraint is that macros are not functions, so there is no
** implicit cast or conversion to a single type.  Casting the address
** of the macro arg to an int* only works when the arg has the same
** size as an int.
**
** No cross-platform comparitive timings have been done to compare the
** speed of !((x) - (x)) versus airExists() versus AIR_EXISTS_F()
** 
** This macro is endian-safe.
*/
#define AIR_EXISTS_F(x) ((*(unsigned int*)&(x) & 0x7f800000) != 0x7f800000)

/*
******** AIR_EXISTS_D(x)
**
** like AIR_EXISTS_F(), but the argument here MUST be a double
*/
#define AIR_EXISTS_D(x) (                               \
  (*(airULLong*)&(x) & AIR_ULLONG(0x7ff0000000000000))  \
    != AIR_ULLONG(0x7ff0000000000000))

/*
******** AIR_ISNAN_F(x)
**
** detects if a float is NaN by looking at the bits, without relying on
** any of its arithmetic properties.  As with AIR_EXISTS_F(), this only
** works when the argument really is a float, and when floats are 4-bytes
*/
#define AIR_ISNAN_F(x) (((*(unsigned int*)&(x) & 0x7f800000)==0x7f800000) && \
			 (*(unsigned int*)&(x) & 0x007fffff))

/*
******** AIR_MAX(a,b), AIR_MIN(a,b), AIR_ABS(a)
**
** the usual
*/
#define AIR_MAX(a,b) ((a) > (b) ? (a) : (b))
#define AIR_MIN(a,b) ((a) < (b) ? (a) : (b))
#define AIR_ABS(a) ((a) > 0 ? (a) : -(a))

/*
******** AIR_COMPARE(a,b)
**
** the sort of compare that qsort() wants for ascending sort
*/
#define AIR_COMPARE(a,b) ((a) < (b)     \
		          ? -1          \
                          : ((a) > (b) \
                             ? 1        \
                             : 0))

/*
******** AIR_FREE, AIR_FCLOSE
**
** just to help you set all pointers to free'd data and fclose'd files to NULL
*/
#define AIR_FREE(x) (x) = airFree((x))
#define AIR_FCLOSE(f) (f) = airFclose((f))


/*
******** AIR_IN_OP(a,b,c), AIR_IN_CL(a,b,c)
**
** is true if the middle argument is in the open/closed interval
** defined by the first and third arguments
** 
** AIR_IN_OP is new name for old AIR_BETWEEN
** AIR_IN_CL is new name for odl AIR_INSIDE
*/
#define AIR_IN_OP(a,b,c) ((a) < (b) && (b) < (c))     /* closed interval */
#define AIR_IN_CL(a,b,c) ((a) <= (b) && (b) <= (c))   /* open interval */

/*
******** AIR_CLAMP(a,b,c)
**
** returns the middle argument, after being clamped to the closed
** interval defined by the first and third arguments
*/
#define AIR_CLAMP(a,b,c) ((b) < (a)        \
		 	   ? (a)           \
			   : ((b) > (c)    \
			      ? (c)        \
			      : (b)))

/*
******** AIR_MOD(i, N)
**
** returns that integer in [0, N-1] which is i plus a multiple of N. It
** may be unfortunate that the expression (i)%(N) appears three times;
** this should be inlined.  Or perhaps the compiler's optimizations
** (common sub-expression elimination) will save us.
**
** Note: integer divisions are not very fast on some modern chips;
** don't go silly using this one.
*/
#define AIR_MOD(i, N) ((i)%(N) >= 0 ? (i)%(N) : N + (i)%(N))

/*
******** AIR_LERP(w, a, b)
**
** returns a when w=0, and b when w=1, and linearly varies in between
*/
#define AIR_LERP(w, a, b) ((w)*((b) - (a)) + (a))

/*
******** AIR_AFFINE(i,x,I,o,O)
**
** given intervals [i,I], [o,O] and a value x which may or may not be
** inside [i,I], return the value y such that y stands in the same
** relationship to [o,O] that x does with [i,I].  Or:
**
**    y - o         x - i     
**   -------   =   -------
**    O - o         I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both non-zero.  Strictly speaking, real problems arise only when
** when I-i is zero: division by zero generates either NaN or infinity
*/
#define AIR_AFFINE(i,x,I,o,O) ( \
((double)(O)-(o))*((double)(x)-(i)) / ((double)(I)-(i)) + (o))

/*
******** AIR_DELTA(i,x,I,o,O)
**
** given intervals [i,I] and [o,O], calculates the number y such that
** a change of x within [i,I] is proportional to a change of y within
** [o,O].  Or:
**
**      y             x     
**   -------   =   -------
**    O - o         I - i
**
** It is the callers responsibility to make sure I-i and O-o are 
** both non-zero
*/
#define AIR_DELTA(i,x,I,o,O) ( \
((double)(O)-(o))*((double)(x)) / ((double)(I)-(i)) )

/*
******** AIR_INDEX(i,x,I,L,t)
**
** READ CAREFULLY!!
**
** Utility for mapping a floating point x in given range [i,I] to the
** index of an array with L elements, AND SAVES THE INDEX INTO GIVEN
** VARIABLE t, WHICH MUST BE OF SOME INTEGER TYPE because this relies
** on the implicit cast of an assignment to truncate away the
** fractional part.  ALSO, t must be of a type large enough to hold
** ONE GREATER than L.  So you can't pass a variable of type unsigned
** char if L is 256
**
** DOES NOT DO BOUNDS CHECKING: given an x which is not inside [i,I],
** this may produce an index not inside [0,L-1] (but it won't always
** do so: the output being outside range [0,L-1] is not a reliable
** test of the input being outside range [i, I]).  The mapping is
** accomplished by dividing the range from i to I into L intervals,
** all but the last of which is half-open; the last one is closed.
** For example, the number line from 0 to 3 would be divided as
** follows for a call with i = 0, I = 4, L = 4:
**
** index:       0    1    2    3 = L-1
** intervals: [   )[   )[   )[    ]
**            |----|----|----|----|
** value:     0    1    2    3    4
**
** The main point of the diagram above is to show how I made the
** arbitrary decision to orient the half-open interval, and which
** end has the closed interval.
**
** Note that AIR_INDEX(0,3,4,4,t) and AIR_INDEX(0,4,4,4,t) both set t = 3
**
** The reason that this macro requires a argument for saving the
** result is that this is the easiest way to avoid extra conditionals.
** Otherwise, we'd have to do some check to see if x is close enough
** to I so that the generated index would be L and not L-1.  "Close
** enough" because due to precision problems you can have an x < I
** such that (x-i)/(I-i) == 1, which was a bug with the previous version
** of this macro.  It is far simpler to just do the index generation
** and then do the sneaky check to see if the index is too large by 1.
** We are relying on the fact that C _defines_ boolean true to be exactly 1.
**
** Note also that we are never explicity casting to one kind of int or
** another-- the given t can be any integral type, including long long.
*/
#define AIR_INDEX(i,x,I,L,t) ( \
(t) = (L) * ((double)(x)-(i)) / ((double)(I)-(i)), \
(t) -= ((t) == (L)) )

/*
******** AIR_ROUNDUP, AIR_ROUNDDOWN
**
** rounds integers up or down; just wrappers around floor and ceil
*/
#define AIR_ROUNDUP(x)   ((int)(floor((x)+0.5)))
#define AIR_ROUNDDOWN(x) ((int)(ceil((x)-0.5)))

/*
******** _AIR_SIZE_T_FMT
**
** This is the format string to use when printf/fprintf/sprintf-ing 
** a value of type size_t.  In C99, "%z" serves this purpose.
**
** This is not a useful macro for the world at large- only for teem
** source files.  Why: we need to leave this as a bare string, so that
** we can exploit C's implicit string concatenation in forming a
** format string.  Therefore, unlike the definition of AIR_ENDIAN,
** AIR_DIO, etc, AIR_SIZE_T_FMT can NOT just refer to a const variable
** (like airMyEndian).  Therefore, TEEM_32BIT has to be defined for
** ALL source files which want to use AIR_SIZE_T_FMT, and to be
** conservative, that's all teem files.  The converse is, since there is
** no expectation that other projects which use teem will be defining
** TEEM_32BIT, this is not useful outside teem, thus the leading _.
*/
#ifdef __APPLE__
#  define _AIR_SIZE_T_FMT "%lu"
#else
#  if TEEM_32BIT == 0
#    define _AIR_SIZE_T_FMT "%lu"
#  elif TEEM_32BIT == 1
#    define _AIR_SIZE_T_FMT "%u"
#  else
#    define _AIR_SIZE_T_FMT "(no _AIR_SIZE_T_FMT w/out TEEM_32BIT %*d)"
#  endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* AIR_HAS_BEEN_INCLUDED */

#ifndef BIFF_HAS_BEEN_INCLUDED
#define BIFF_HAS_BEEN_INCLUDED



#ifdef __cplusplus
extern "C" {
#endif

#define BIFF_MAXKEYLEN 128  /* maximum allowed key length (not counting 
			       the null termination) */

extern void biffSet(const char *key, const char *err);
extern void biffAdd(const char *key, const char *err);
extern void biffMaybeAdd(const char *key, const char *err, int useBiff);
extern int biffCheck(const char *key);
extern void biffDone(const char *key);
extern void biffMove(const char *destKey, const char *err, const char *srcKey);
extern char *biffGet(const char *key);
extern char *biffGetDone(const char *key);

#ifdef __cplusplus
}
#endif

#endif /* BIFF_HAS_BEEN_INCLUDED */

#ifndef NRRD_DEFINES_HAS_BEEN_INCLUDED
#define NRRD_DEFINES_HAS_BEEN_INCLUDED

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif


#define NRRD_DIM_MAX 10            /* Maximum dimension which we can handle */

#define NRRD_EXT_NRRD   ".nrrd"
#define NRRD_EXT_NHDR   ".nhdr"
#define NRRD_EXT_PGM    ".pgm"
#define NRRD_EXT_PPM    ".ppm"
#define NRRD_EXT_PNG    ".png"
#define NRRD_EXT_VTK    ".vtk"
#define NRRD_EXT_TEXT   ".txt"
#define NRRD_EXT_EPS    ".eps"

#define NRRD_KERNEL_PARMS_NUM 8    /* max # arguments to a kernel-
				      this is weird: it isn't the max
				      of any of the NrrdKernels
				      defined by the nrrd library
				      (that is more like 3), but is
				      the max number of parms of any
				      NrrdKernel used by anyone using
				      teem, such as in gage.
				      Enforcing one global max
				      simplifies implementation. */

/* 
** For the 64-bit integer types (not standard except in C99), we try
** to use the names for the _MIN and _MAX values which are used in C99
** (as well as gcc) such as LLONG_MAX.
** 
** If these aren't defined, we try the ones used on SGI such as
** LONGLONG_MAX.
**
** If these aren't defined either, we go wild and define something
** ourselves (which just happen to be the values defined in C99), with
** total disregard to what the architecture and compiler actually
** support.  These values are tested, however, by nrrdSanity().
*/

#ifdef LLONG_MAX
#  define NRRD_LLONG_MAX LLONG_MAX
#else
#  ifdef LONGLONG_MAX
#    define NRRD_LLONG_MAX LONGLONG_MAX
#  else
#    define NRRD_LLONG_MAX AIR_LLONG(9223372036854775807)
#  endif
#endif

#ifdef LLONG_MIN
#  define NRRD_LLONG_MIN LLONG_MIN
#else
#  ifdef LONGLONG_MIN
#    define NRRD_LLONG_MIN LONGLONG_MIN
#  else
#    define NRRD_LLONG_MIN (-NRRD_LLONG_MAX-AIR_LLONG(1))
#  endif
#endif

#ifdef ULLONG_MAX
#  define NRRD_ULLONG_MAX ULLONG_MAX
#else
#  ifdef ULONGLONG_MAX
#    define NRRD_ULLONG_MAX ULONGLONG_MAX
#  else
#    define NRRD_ULLONG_MAX AIR_ULLONG(18446744073709551615)
#  endif
#endif

/*
** Chances are, you shouldn't mess with these
*/

#define NRRD_COMMENT_CHAR '#'
#define NRRD_COMMENT_INCR 16
#define NRRD_KEYVALUE_INCR 32
#define NRRD_PNM_COMMENT "# NRRD>"    /* this is designed to be robust against
				         the mungling that xv does, but no
				         promises for any other image
					 programs */

#define NRRD_PNG_FIELD_KEY "NRRD"     /* this is the key used for getting nrrd
				         fields into/out of png comments */
#define NRRD_PNG_COMMENT_KEY "NRRD#"  /* this is the key used for getting nrrd
				         comments into/out of png comments */

#define NRRD_UNKNOWN  "???"           /* how to represent something unknown in
				         a field of the nrrd header, when it
				         being unknown is not an error */

#ifdef __cplusplus
}
#endif

#endif /* NRRD_DEFINES_HAS_BEEN_INCLUDED */

#ifndef NRRD_ENUMS_HAS_BEEN_INCLUDED
#define NRRD_ENUMS_HAS_BEEN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*******
******** NONE of these enums should have values set explicitly in their
******** definition.  The values should simply start at 0 (for Unknown)
******** and increase one integer per value.  The _nrrdCheckEnums()
******** sanity check assumes this, and there is no reason to use 
******** explicit values for any of the enums.
*******/

/*
******** nrrdIoState* enum
** 
** the various things it makes sense to get and set in nrrdIoState struct
** via nrrdIoStateGet and nrrdIoStateSet
*/
enum {
  nrrdIoStateUnknown,
  nrrdIoStateDetachedHeader,
  nrrdIoStateBareText,
  nrrdIoStateCharsPerLine,
  nrrdIoStateValsPerLine,
  nrrdIoStateSkipData,
  nrrdIoStateKeepNrrdDataFileOpen,
  nrrdIoStateZlibLevel,
  nrrdIoStateZlibStrategy,
  nrrdIoStateBzip2BlockSize,
  nrrdIoStateLast
};

/*
******** nrrdFormatType enum
**
** the different file formats which nrrd supports
*/
enum {
  nrrdFormatTypeUnknown,
  nrrdFormatTypeNRRD,   /* 1: basic nrrd format (associated with both
			   magic nrrdMagicOldNRRD and nrrdMagicNRRD0001 */
  nrrdFormatTypePNM,    /* 2: PNM image */
  nrrdFormatTypePNG,    /* 3: PNG image */
  nrrdFormatTypeVTK,    /* 4: VTK Structured Points datasets (v1.0 and 2.0) */
  nrrdFormatTypeText,   /* 5: bare ASCII text for 2D arrays */
  nrrdFormatTypeEPS,    /* 6: Encapsulated PostScript (write-only) */
  nrrdFormatTypeLast
};
#define NRRD_FORMAT_TYPE_MAX    6

/*
******** nrrdBoundary enum
**
** when resampling, how to deal with the ends of a scanline
*/
enum {
  nrrdBoundaryUnknown,
  nrrdBoundaryPad,      /* 1: fill with some user-specified value */
  nrrdBoundaryBleed,    /* 2: copy the last/first value out as needed */
  nrrdBoundaryWrap,     /* 3: wrap-around */
  nrrdBoundaryWeight,   /* 4: normalize the weighting on the existing samples;
			   ONLY sensible for a strictly positive kernel
			   which integrates to unity (as in blurring) */
  nrrdBoundaryLast
};
#define NRRD_BOUNDARY_MAX  4

/*
******** nrrdType enum
**
** all the different types, identified by integer
**
** 18 July 03: After some consternation, I decided to set
** nrrdTypeUnknown and nrrdTypeDefault to the same thing, with the
** reasoning that the only times that nrrdTypeDefault is used is when
** controlling an *output* type (the type of "nout"), or rather,
** choosing not to control an output type.  As output types must be
** known, there is no confusion between being unset/unknown (invalid)
** and being simply default.
*/
enum {
  nrrdTypeUnknown=0,     /*  0: signifies "type is unset/unknown" */
  nrrdTypeDefault=0,     /*  0: signifies "determine output type for me" */
  nrrdTypeChar,          /*  1:   signed 1-byte integer */
  nrrdTypeUChar,         /*  2: unsigned 1-byte integer */
  nrrdTypeShort,         /*  3:   signed 2-byte integer */
  nrrdTypeUShort,        /*  4: unsigned 2-byte integer */
  nrrdTypeInt,           /*  5:   signed 4-byte integer */
  nrrdTypeUInt,          /*  6: unsigned 4-byte integer */
  nrrdTypeLLong,         /*  7:   signed 8-byte integer */
  nrrdTypeULLong,        /*  8: unsigned 8-byte integer */
  nrrdTypeFloat,         /*  9:          4-byte floating point */
  nrrdTypeDouble,        /* 10:          8-byte floating point */
  nrrdTypeBlock,         /* 11: size user defined at run time; MUST BE LAST */
  nrrdTypeLast
};
#define NRRD_TYPE_MAX       11
#define NRRD_TYPE_SIZE_MAX   8    /* max(sizeof()) over all scalar types */
#define NRRD_TYPE_BIGGEST double  /* this should be a basic C type which
				     requires for storage the maximum size
				     of all the basic C types */

/*
******** nrrdEncodingType enum
**
** how data might be encoded into a bytestream
*/
enum {
  nrrdEncodingTypeUnknown,
  nrrdEncodingTypeRaw,      /* 1: same as memory layout (modulo endianness) */
  nrrdEncodingTypeAscii,    /* 2: decimal values are spelled out in ascii */
  nrrdEncodingTypeHex,      /* 3: hexidecimal (two chars per byte) */
  nrrdEncodingTypeGzip,     /* 4: gzip'ed raw data */
  nrrdEncodingTypeBzip2,    /* 5: bzip2'ed raw data */
  nrrdEncodingTypeLast
};
#define NRRD_ENCODING_TYPE_MAX 5

/*
******** nrrdZlibStrategy enum
**
** how gzipped data is compressed
*/
enum {
  nrrdZlibStrategyUnknown,
  nrrdZlibStrategyDefault,   /* 1: default (Huffman + string match) */
  nrrdZlibStrategyHuffman,   /* 2: Huffman only */
  nrrdZlibStrategyFiltered,  /* 3: specialized for filtered data */
  nrrdZlibStrategyLast
};
#define NRRD_ZLIB_STRATEGY_MAX  3

/*
******** nrrdCenter enum
**
** node-centered vs. cell-centered
*/
enum {
  nrrdCenterUnknown,
  nrrdCenterNode,            /* 1: samples at corners of things
				(how "voxels" are usually imagined)
				|\______/|\______/|\______/|
				X        X        X        X   */
  nrrdCenterCell,            /* 2: samples at middles of things
				(characteristic of histogram bins)
				 \___|___/\___|___/\___|___/
				     X        X        X       */
  nrrdCenterLast
};
#define NRRD_CENTER_MAX         2

/*
******** nrrdAxisInfo enum
**
** the different pieces of per-axis information recorded in a nrrd
*/
enum {
  nrrdAxisInfoUnknown,
  nrrdAxisInfoSize,                 /* 1: number of samples along axis */
#define NRRD_AXIS_INFO_SIZE_BIT    (1<<1)
  nrrdAxisInfoSpacing,              /* 2: spacing between samples */
#define NRRD_AXIS_INFO_SPACING_BIT (1<<2)
  nrrdAxisInfoMin,                  /* 3: minimum pos. assoc. w/ 1st sample */
#define NRRD_AXIS_INFO_MIN_BIT     (1<<3) 
  nrrdAxisInfoMax,                  /* 4: maximum pos. assoc. w/ last sample */
#define NRRD_AXIS_INFO_MAX_BIT     (1<<4)
  nrrdAxisInfoCenter,               /* 5: cell vs. node */
#define NRRD_AXIS_INFO_CENTER_BIT  (1<<5)
  nrrdAxisInfoLabel,                /* 6: string describing the axis */
#define NRRD_AXIS_INFO_LABEL_BIT   (1<<6)
  nrrdAxisInfoUnit,                 /* 7: string identifying units */
#define NRRD_AXIS_INFO_UNIT_BIT    (1<<7)
  nrrdAxisInfoLast
};
#define NRRD_AXIS_INFO_MAX             7
#define NRRD_AXIS_INFO_ALL  ((1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5)|(1<<6)|(1<<7))
#define NRRD_AXIS_INFO_NONE 0

/*
** the "endian" enum is actually in the air library, but it is very
** convenient to have it incorporated into the nrrd enum framework for
** the purposes of string<-->int conversion.  Unfortunately, the
** little and big values are 1234 and 4321 respectively, so
** NRRD_ENDIAN_MAX is not actually the highest valid value, but only
** an indicator of how many valid values there are.
*/
#define NRRD_ENDIAN_MAX 2

/*
******** nrrdField enum
**
** the various fields we can parse in a NRRD header
*/
enum {
  nrrdField_unknown,
  nrrdField_comment,         /*  1 */
  nrrdField_content,         /*  2 */
  nrrdField_number,          /*  3 */
  nrrdField_type,            /*  4 */
  nrrdField_block_size,      /*  5 */
  nrrdField_dimension,       /*  6 */
  nrrdField_sizes,           /*  7 */
  nrrdField_spacings,        /*  8 */
  nrrdField_axis_mins,       /*  9 */
  nrrdField_axis_maxs,       /* 10 */
  nrrdField_centers,         /* 11 */
  nrrdField_labels,          /* 12 */
  nrrdField_units,           /* 13 */
  nrrdField_min,             /* 14 */
  nrrdField_max,             /* 15 */
  nrrdField_old_min,         /* 16 */
  nrrdField_old_max,         /* 17 */
  nrrdField_data_file,       /* 18 */
  nrrdField_endian,          /* 19 */
  nrrdField_encoding,        /* 20 */
  nrrdField_line_skip,       /* 21 */
  nrrdField_byte_skip,       /* 22 */
  nrrdField_keyvalue,        /* 23 */
  nrrdField_last
};
#define NRRD_FIELD_MAX          23

/* 
******** nrrdHasNonExist* enum
**
** oh look, I'm violating my rules outline above for how the enum values
** should be ordered.  The reason for this is that its just too bizarro to
** have the logical value of both nrrdHasNonExistFalse and nrrdHasNonExistTrue
** to be (in C) true.  For instance, nrrdHasNonExist() should be able to 
** return a value from this enum which also functions in a C expressions as
** the expected boolean value.  If for some reason (outide the action of
** nrrdHasNonExist(), nrrdHasNonExistUnknown is interpreted as true, that's
** probably harmlessly conservative.  Time will tell.
*/
enum {
  nrrdHasNonExistFalse,     /* 0: no non-existent values were seen */
  nrrdHasNonExistTrue,      /* 1: some non-existent values were seen */
  nrrdHasNonExistOnly,      /* 2: NOTHING BUT non-existant values were seen */
  nrrdHasNonExistUnknown,   /* 3 */
  nrrdHasNonExistLast
};
#define NRRD_HAS_NON_EXIST_MAX 3


#ifdef __cplusplus
}
#endif

#endif /* NRRD_ENUMS_HAS_BEEN_INCLUDED */

#ifndef NRRD_MACROS_HAS_BEEN_INCLUDED
#define NRRD_MACROS_HAS_BEEN_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/*
******** NRRD_CELL_POS, NRRD_NODE_POS, NRRD_POS
******** NRRD_CELL_IDX, NRRD_NODE_IDX, NRRD_IDX
**
** the guts of nrrdAxisPos() and nrrdAxisIdx(), for converting
** between "index space" location and "position" or "world space" location,
** given the centering, min and max "position", and number of samples.
**
** Unlike nrrdAxisPos() and nrrdAxisIdx(), this assumes that center
** is either nrrdCenterCell or nrrdCenterNode, but not nrrdCenterUnknown.
*/
/* index to position, cell centering */
#define NRRD_CELL_POS(min, max, size, idx)       \
  AIR_AFFINE(0, (idx) + 0.5, (size), (min), (max))

/* index to position, node centering */
#define NRRD_NODE_POS(min, max, size, idx)       \
  AIR_AFFINE(0, (idx), (size)-1, (min), (max))

/* index to position, either centering */
#define NRRD_POS(center, min, max, size, idx)    \
  (nrrdCenterCell == (center)                         \
   ? NRRD_CELL_POS((min), (max), (size), (idx))  \
   : NRRD_NODE_POS((min), (max), (size), (idx)))

/* position to index, cell centering */
#define NRRD_CELL_IDX(min, max, size, pos)       \
  (AIR_AFFINE((min), (pos), (max), 0, (size)) - 0.5)

/* position to index, node centering */
#define NRRD_NODE_IDX(min, max, size, pos)       \
  AIR_AFFINE((min), (pos), (max), 0, (size)-1)

/* position to index, either centering */
#define NRRD_IDX(center, min, max, size, pos)    \
  (nrrdCenterCell == (center)                         \
   ? NRRD_CELL_IDX((min), (max), (size), (pos))  \
   : NRRD_NODE_IDX((min), (max), (size), (pos)))

/*
******** NRRD_SPACING
**
** the guts of nrrdAxisSpacing(), determines the inter-sample
** spacing, given centering, min and max "position", and number of samples
**
** Unlike nrrdAxisSpacing, this assumes that center is either
** nrrdCenterCell or nrrdCenterNode, but not nrrdCenterUnknown.
*/
#define NRRD_SPACING(center, min, max, size)  \
  (nrrdCenterCell == center                        \
   ? ((max) - (min))/(size)                        \
   : ((max) - (min))/((size) - 1))                 \


#ifdef __cplusplus
}
#endif

#endif /* NRRD_MACROS_HAS_BEEN_INCLUDED */

#ifndef NRRD_HAS_BEEN_INCLUDED
#define NRRD_HAS_BEEN_INCLUDED

#include <errno.h>


#if defined(_WIN32) && !defined(TEEM_STATIC) && !defined(__CYGWIN__)
#define nrrd_export __declspec(dllimport)
#else
#define nrrd_export
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NRRD nrrdBiffKey

/*
******** NrrdAxis struct
**
** all the information which can sensibly be associated with
** one axis of a nrrd.  The only member which MUST be explicitly
** set to something meaningful is "size".
**
** The min and max values give the range of positions "represented"
** by the samples along this axis.  In node-centering, "min" IS the
** position at the lowest index.  In cell-centering, the position at
** the lowest index is between min and max (a touch bigger than min,
** assuming min < max).
*/
typedef struct {
  int size;                      /* number of elements along each axis */
  double spacing;                /* if non-NaN, distance between samples */
  double min, max;               /* if non-NaN, range of positions spanned
				    by the samples on this axis.  Obviously,
				    one can set "spacing" to something
				    incompatible with min and max: the idea
				    is that only one (min and max, or
				    spacing) should be taken to be significant
				    at any time. */
  int center;                    /* cell vs. node centering */
  char *label;                   /* short info string for each axis */
  char *unit;                    /* short string for identifying units */
} NrrdAxis;

/*
******** Nrrd struct
**
** The struct used to wrap around the raw data array
*/
typedef struct {
  /* 
  ** NECESSARY information describing the main array.  This is
  ** generally set at the same time that either the nrrd is created,
  ** or at the time that the nrrd is wrapped around an existing array 
  */
  void *data;                    /* the data in memory */
  int type;                      /* a value from the nrrdType enum */
  int dim;                       /* what is dimension of data */

  /* 
  ** All per-axis specific information
  */
  NrrdAxis axis[NRRD_DIM_MAX];   /* axis[0] is the fastest axis in the scan-
				    line ordering, the one who's coordinates
				    change the fastest as the elements are
				    accessed in the order in which they appear
				    in memory */

  /* 
  ** Information of dubious standing- descriptive of whole array, but
  ** not necessary (meaningful only for some uses of a nrrd), but basic
  ** enough to be part of the basic nrrd type
  */
  char *content;                 /* briefly, just what the hell is this data */
  int blockSize;                 /* for nrrdTypeBlock array, block byte size */
  double oldMin, oldMax;         /* if non-NaN, and if nrrd is of integral
				    type, extremal values for the array
				    BEFORE it was quantized */
  void *ptr;                     /* never read or set by nrrd; use/abuse
				    as you see fit */

  /* 
  ** Comments.  Read from, and written to, header.
  ** The comment array "cmt" is NOT NULL-terminated.
  ** The number of comments is cmtArr->len.
  */
  char **cmt;
  airArray *cmtArr;

  /*
  ** Key-value pairs.
  */
  char **kvp;
  airArray *kvpArr;
} Nrrd;

struct NrrdIoState_t;
struct NrrdEncoding_t;

/*
******** NrrdFormat
**
** All information and behavior relevent to one datafile format
*/
typedef struct {
  char name[AIR_STRLEN_SMALL];    /* short identifying string */
  int isImage,    /* this format is intended solely for "2D" images, which
		     controls the invocation of _nrrdReshapeUpGrayscale()
		     if nrrdStateGrayscaleImage3D */
    readable,     /* we can read as well as write this format */
    usesDIO;      /* this format can use Direct IO */

  /* tests if this format is currently available in this build */
  int (*available)(void);

  /* (for writing) returns non-zero if a given filename could likely be
     represented by this format */
  int (*nameLooksLike)(const char *filename);

  /* (for writing) returns non-zero if a given nrrd/encoding pair will fit
     in this format */
  int (*fitsInto)(const Nrrd *nrrd, const struct NrrdEncoding_t *encoding, 
		   int useBiff);

  /* (for reading) returns non-zero if what has been read in so far 
     is recognized as the beginning of this format */
  int (*contentStartsLike)(struct NrrdIoState_t *nio);

  /* reader and writer */
  int (*read)(FILE *file, Nrrd *nrrd, struct NrrdIoState_t *nio);
  int (*write)(FILE *file, const Nrrd *nrrd, struct NrrdIoState_t *nio);
} NrrdFormat;

/*
******** NrrdEncoding
**
** All information and behavior relevent to one way of encoding data
**
** The data readers are responsible for memory allocation.
** This is necessitated by the memory restrictions of direct I/O
*/
typedef struct NrrdEncoding_t {
  char name[AIR_STRLEN_SMALL],    /* short identifying string */
    suffix[AIR_STRLEN_SMALL];     /* costumary filename suffix */
  int endianMatters,
    isCompression;
  int (*available)(void);
  int (*read)(Nrrd *nrrd, struct NrrdIoState_t *nio);
  int (*write)(const Nrrd *nrrd, struct NrrdIoState_t *nio);
} NrrdEncoding;

/*
******** NrrdIoState struct
**
** Everything transient relating to how the nrrd is read and written.
** Once the nrrd has been read or written, this information is moot,
** except that after reading, it is a potentially useful record of what
** it took to read in a nrrd, and it is the mechanism for hacks like
** keepNrrdDataFileOpen
*/
typedef struct NrrdIoState_t {
  char *path,               /* allows us to remember the directory
			       from whence this nrrd was "load"ed, or
			       to whence this nrrd is "save"ed, MINUS the
			       trailing "/", so as to facilitate games with
			       header-relative data files */
    *base,                  /* when "save"ing a nrrd into seperate
			       header and data, the name of the header
			       file (e.g. "output.nhdr") MINUS the ".nhdr".
			       This is  massaged to produce a header-
			       relative data filename.  */
    *dataFN,                /* ON READ: no semantics 
			       ON WRITE: name to be saved in the "data file"
			       field of the nrrd, either verbatim from
			       "unu make -h", or, internally, created based
			       on detached header path and name */
    *line;                  /* buffer for saving one line from file */
  
  int lineLen,              /* allocated size of line, including the
			       last character for \0 */
    pos;                    /* line[pos] is beginning of stuff which
			       still has yet to be parsed */

  FILE *dataFile;           /* if non-NULL, where the data is to be
			       read from or written to.  If NULL, data
			       will be read from current file */

  int endian,               /* endian-ness of the data in file, for
			       those encoding/type combinations for
			       which it matters (from nrrdEndian) */
    lineSkip,               /* if dataFile non-NULL, the number of
			       lines in dataFile that should be
			       skipped over (so as to bypass another
			       form of ASCII header preceeding raw
			       data) */
    byteSkip,               /* exactly like lineSkip, but bytes
			       instead of lines.  First the lines are
			       skipped, then the bytes */
    seen[NRRD_FIELD_MAX+1], /* for error checking in header parsing */
    detachedHeader,         /* ON READ+WRITE: nrrd is split into distinct
			       header and data (for nrrd format only) */
    bareText,               /* when writing a plain text file, is there any
			       effort made to record the nrrd struct
			       info in the text file */
    charsPerLine,           /* when writing ASCII data in which we
			       intend only to write a huge long list
			       of numbers whose text formatting
			       implies nothing, then how many
			       characters do we limit ourselves to per
			       line */
    valsPerLine,            /* when writing ASCII data in which we DO
			       intend to sigify (or at least hint at)
			       something with the formatting, then
			       what is the max number of values to
			       write on a line */
    skipData,               /* if non-zero (all formats):
			       ON READ: don't allocate memory for, and don't
			       read in, the data portion of the file (but we
			       do verify that for nrrds, detached datafiles
			       can be opened).  Note: Does NOT imply 
			       keepNrrdDataFileOpen.  Warning: resulting
			       nrrd struct will have "data" pointer NULL.
			       ON WRITE: don't write data portion of file
			       (for nrrds, don't even try to open detached
			       datafiles).  Warning: can result in broken
			       noncomformant files.
			       (be careful with this) */
    keepNrrdDataFileOpen,   /* ON READ: don't close nio->dataFile when
			       you otherwise would, when reading the
			       nrrd format Probably used in conjunction with
			       skipData.  (currently for "unu data")
			       ON WRITE: no semantics */
    zlibLevel,              /* zlib compression level (0-9, -1 for
			       default[6], 0 for no compression). */
    zlibStrategy,           /* zlib compression strategy, can be one
			       of the nrrdZlibStrategy enums, default is
			       nrrdZlibStrategyDefault. */
    bzip2BlockSize;         /* block size used for compression, 
			       roughly equivalent to better but slower
			       (1-9, -1 for default[9]). */
  void *oldData;            /* ON READ: data pointer that may have already been
			       allocated for the right size to hold the data */
  size_t oldDataSize;       /* ON READ: size of data pointed to by oldData */
  /* format and encoding.  These are initialized to nrrdFormatUnknown
     and nrrdEncodingUnknown, respectively. USE THESE VALUES for 
     any kind of initialization or flagging; DO NOT USE NULL */
  const NrrdFormat *format;
  const NrrdEncoding *encoding;
} NrrdIoState;


/******** defaults (nrrdDef..) and state (nrrdState..) */
/* defaultsNrrd.c */
extern nrrd_export const NrrdEncoding *nrrdDefWriteEncoding;
extern nrrd_export int nrrdDefWriteBareText;
extern nrrd_export int nrrdDefWriteCharsPerLine;
extern nrrd_export int nrrdDefWriteValsPerLine;
extern nrrd_export int nrrdDefCenter;
extern nrrd_export double nrrdDefSpacing;
extern nrrd_export int nrrdStateVerboseIO;
extern nrrd_export int nrrdStateAlwaysSetContent;
extern nrrd_export int nrrdStateDisableContent;
extern nrrd_export char *nrrdStateUnknownContent;
extern nrrd_export int nrrdStateGrayscaleImage3D;
extern nrrd_export int nrrdStateKeyValueReturnInternalPointers;

/******** all the airEnums used through-out nrrd */
/* (the actual C enums are in nrrdEnums.h) */
/* enumsNrrd.c */
extern nrrd_export airEnum *nrrdFormatType;
extern nrrd_export airEnum *nrrdType;
extern nrrd_export airEnum *nrrdEncodingType;
extern nrrd_export airEnum *nrrdCenter;
extern nrrd_export airEnum *nrrdAxisInfo;
extern nrrd_export airEnum *nrrdField;

/******** arrays of things (poor-man's functions/predicates) */
/* arraysNrrd.c */
extern nrrd_export char nrrdTypePrintfStr[][AIR_STRLEN_SMALL];
extern nrrd_export int nrrdTypeSize[];
extern nrrd_export double nrrdTypeMin[];
extern nrrd_export double nrrdTypeMax[];
extern nrrd_export int nrrdTypeIsIntegral[];
extern nrrd_export int nrrdTypeIsUnsigned[];
extern nrrd_export double nrrdTypeNumberOfValues[];

/******** pseudo-constructors, pseudo-destructors, and such */
/* methodsNrrd.c */
extern NrrdIoState *nrrdIoStateNew(void);
extern void nrrdIoStateInit(NrrdIoState *io);
extern NrrdIoState *nrrdIoStateNix(NrrdIoState *io);
extern void nrrdInit(Nrrd *nrrd);
extern Nrrd *nrrdNew(void);
extern Nrrd *nrrdNix(Nrrd *nrrd);
extern Nrrd *nrrdEmpty(Nrrd *nrrd);
extern Nrrd *nrrdNuke(Nrrd *nrrd);
extern int nrrdWrap_nva(Nrrd *nrrd, void *data, int type,
			int dim, const int *size);
extern int nrrdWrap(Nrrd *nrrd, void *data, int type, int dim,
		    ... /* sx, sy, .., axis(dim-1) size */);
extern int nrrdCopy(Nrrd *nout, const Nrrd *nin);
extern int nrrdAlloc_nva(Nrrd *nrrd, int type, int dim, const int *size);
extern int nrrdAlloc(Nrrd *nrrd, int type, int dim,
		     ... /* sx, sy, .., axis(dim-1) size */);
extern int nrrdMaybeAlloc_nva(Nrrd *nrrd, int type, int dim, const int *size);
extern int nrrdMaybeAlloc(Nrrd *nrrd, int type, int dim,
			  ... /* sx, sy, .., axis(dim-1) size */);
extern int nrrdPPM(Nrrd *, int sx, int sy);
extern int nrrdPGM(Nrrd *, int sx, int sy);

/******** axis info related */
/* axis.c */
extern int nrrdAxisInfoCopy(Nrrd *nout, const Nrrd *nin,
			    const int *axmap, int bitflag);
extern void nrrdAxisInfoSet_nva(Nrrd *nin, int axInfo, const void *info);
extern void nrrdAxisInfoSet(Nrrd *nin, int axInfo,
			    ... /* const void* */);
extern void nrrdAxisInfoGet_nva(const Nrrd *nrrd, int axInfo, void *info);
extern void nrrdAxisInfoGet(const Nrrd *nrrd, int axInfo,
			    ... /* void* */);
extern double nrrdAxisPos(const Nrrd *nrrd, int ax, double idx);
extern double nrrdAxisIdx(const Nrrd *nrrd, int ax, double pos);
extern void nrrdAxisPosRange(double *loP, double *hiP,
			     const Nrrd *nrrd, int ax,
			     double loIdx, double hiIdx);
extern void nrrdAxisIdxRange(double *loP, double *hiP,
			     const Nrrd *nrrd, int ax,
			     double loPos, double hiPos);
extern void nrrdAxisSpacingSet(Nrrd *nrrd, int ax);
extern void nrrdAxisMinMaxSet(Nrrd *nrrd, int ax, int defCenter);

/******** simple things */
/* simple.c */
extern nrrd_export const char *nrrdBiffKey;
extern const Nrrd **nrrdCNPP(Nrrd **nin, int N);
extern int nrrdPeripheralInit(Nrrd *nrrd);
extern int nrrdPeripheralCopy(Nrrd *nout, const Nrrd *nin);
extern int nrrdContentSet(Nrrd *nout, const char *func,
			  const Nrrd *nin, const char *format,
			  ... /* printf-style arg list */ );
extern void nrrdDescribe(FILE *file, const Nrrd *nrrd);
extern int nrrdCheck(const Nrrd *nrrd);
extern int nrrdElementSize(const Nrrd *nrrd);
extern size_t nrrdElementNumber(const Nrrd *nrrd);
extern int nrrdSanity(void);
extern int nrrdSameSize(const Nrrd *n1, const Nrrd *n2, int useBiff);

/******** comments related */
/* comment.c */
extern int nrrdCommentAdd(Nrrd *nrrd, const char *str);
extern void nrrdCommentClear(Nrrd *nrrd);
extern int nrrdCommentCopy(Nrrd *nout, const Nrrd *nin);

/******** key/value pairs */
/* keyvalue.c */
extern int nrrdKeyValueSize(const Nrrd *nrrd);
extern int nrrdKeyValueAdd(Nrrd *nrrd, const char *key, const char *value);
extern char *nrrdKeyValueGet(const Nrrd *nrrd, const char *key);
extern void nrrdKeyValueIndex(const Nrrd *nrrd, 
			      char **keyP, char **valueP, int ki);
extern int nrrdKeyValueErase(Nrrd *nrrd, const char *key);
extern void nrrdKeyValueClear(Nrrd *nrrd);
extern int nrrdKeyValueCopy(Nrrd *nout, const Nrrd *nin);

/******** endian related */
/* endianNrrd.c */
extern void nrrdSwapEndian(Nrrd *nrrd);

/******** getting information to and from files */
/* formatXXX.c */
extern nrrd_export const NrrdFormat *const nrrdFormatNRRD;
extern nrrd_export const NrrdFormat *const nrrdFormatPNM;
extern nrrd_export const NrrdFormat *const nrrdFormatPNG;
extern nrrd_export const NrrdFormat *const nrrdFormatVTK;
extern nrrd_export const NrrdFormat *const nrrdFormatText;
extern nrrd_export const NrrdFormat *const nrrdFormatEPS;
/* format.c */
extern nrrd_export const NrrdFormat *const nrrdFormatUnknown;
extern nrrd_export const NrrdFormat *
  const nrrdFormatArray[NRRD_FORMAT_TYPE_MAX+1];
/* encodingXXX.c */
extern nrrd_export const NrrdEncoding *const nrrdEncodingRaw;
extern nrrd_export const NrrdEncoding *const nrrdEncodingAscii;
extern nrrd_export const NrrdEncoding *const nrrdEncodingHex;
extern nrrd_export const NrrdEncoding *const nrrdEncodingGzip;
extern nrrd_export const NrrdEncoding *const nrrdEncodingBzip2;
/* encoding.c */
extern nrrd_export const NrrdEncoding *const nrrdEncodingUnknown;
extern nrrd_export const NrrdEncoding *
  const nrrdEncodingArray[NRRD_ENCODING_TYPE_MAX+1];
/* read.c */
extern int nrrdLineSkip(NrrdIoState *io);
extern int nrrdByteSkip(Nrrd *nrrd, NrrdIoState *io);
extern int nrrdLoad(Nrrd *nrrd, const char *filename, NrrdIoState *io);
extern int nrrdRead(Nrrd *nrrd, FILE *file, NrrdIoState *io);
/* write.c */
extern int nrrdIoStateSet(NrrdIoState *io, int parm, int value);
extern int nrrdIoStateSetEncoding(NrrdIoState *io,
				  const NrrdEncoding *encoding);
extern int nrrdIoStateSetFormat(NrrdIoState *io, 
				const NrrdFormat *format);
extern int nrrdIoStateGet(NrrdIoState *io, int parm);
extern const NrrdEncoding *nrrdIoStateGetEncoding(NrrdIoState *io);
extern const NrrdFormat *nrrdIoStateBetFormat(NrrdIoState *io);
extern int nrrdSave(const char *filename, const Nrrd *nrrd, NrrdIoState *io);
extern int nrrdWrite(FILE *file, const Nrrd *nrrd, NrrdIoState *io);

/******** getting value into and out of an array of general type, and
   all other simplistic functionality pseudo-parameterized by type */
/* accessors.c */
extern nrrd_export int    (*nrrdILoad[NRRD_TYPE_MAX+1])(const void *v);
extern nrrd_export float  (*nrrdFLoad[NRRD_TYPE_MAX+1])(const void *v);
extern nrrd_export double (*nrrdDLoad[NRRD_TYPE_MAX+1])(const void *v);
extern nrrd_export int    (*nrrdIStore[NRRD_TYPE_MAX+1])(void *v, int j);
extern nrrd_export float  (*nrrdFStore[NRRD_TYPE_MAX+1])(void *v, float f);
extern nrrd_export double (*nrrdDStore[NRRD_TYPE_MAX+1])(void *v, double d);
extern nrrd_export int    (*nrrdILookup[NRRD_TYPE_MAX+1])(const void *v, 
							  size_t I);
extern nrrd_export float  (*nrrdFLookup[NRRD_TYPE_MAX+1])(const void *v, 
							  size_t I);
extern nrrd_export double (*nrrdDLookup[NRRD_TYPE_MAX+1])(const void *v, 
							  size_t I);
extern nrrd_export int    (*nrrdIInsert[NRRD_TYPE_MAX+1])(void *v,
							  size_t I, int j);
extern nrrd_export float  (*nrrdFInsert[NRRD_TYPE_MAX+1])(void *v,
							  size_t I, float f);
extern nrrd_export double (*nrrdDInsert[NRRD_TYPE_MAX+1])(void *v,
							  size_t I, double d);
extern nrrd_export int    (*nrrdSprint[NRRD_TYPE_MAX+1])(char *, const void *);


/******** permuting, shuffling, and all flavors of reshaping */
/* reorder.c */
extern int nrrdAxesInsert(Nrrd *nout, const Nrrd *nin, int ax);

#ifdef __cplusplus
}
#endif

#endif /* NRRD_HAS_BEEN_INCLUDED */
