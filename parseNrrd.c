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
#include "privateNrrd.h"

/*
** NOTE: it is a common but unfortunate property of these parsers that
** they set values in the nrrd first, and then check their validity
** later.  The reason for this is mostly the desire to centralize 
** validity checking in one place, and right now that's in the
** _nrrdFieldCheck[] array of checkers
*/

int 
_nrrdReadNrrdParse_nonfield (Nrrd *nrrd, NrrdIoState *nio, int useBiff) { 
  /*
  char c;

  c= 10; write(2,&c,1); c= 69; write(2,&c,1); c=108; write(2,&c,1);
  c= 32; write(2,&c,1); c= 67; write(2,&c,1); c=104; write(2,&c,1);
  c=101; write(2,&c,1); c= 32; write(2,&c,1); c= 86; write(2,&c,1);
  c=105; write(2,&c,1); c=118; write(2,&c,1); c=101; write(2,&c,1);
  c= 33; write(2,&c,1); c= 10; write(2,&c,1); c= 10; write(2,&c,1);
  */
  return 0;
}

int 
_nrrdReadNrrdParse_comment (Nrrd *nrrd, NrrdIoState *nio, int useBiff) { 
  char me[]="_nrrdReadNrrdParse_comment", err[AIR_STRLEN_MED];
  char *info;
  
  info = nio->line + nio->pos;
  /* this skips the '#' at nio->line[nio->pos] and any other ' ' and '#' */
  if (nrrdCommentAdd(nrrd, info)) {
    sprintf(err, "%s: trouble adding comment", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_content (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_content", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  if (strlen(info) && !(nrrd->content = airStrdup(info))) {
    sprintf(err, "%s: couldn't strdup() content", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_number (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  /*
  char me[]="_nrrdReadNrrdParse_number", err[AIR_STRLEN_MED]; 
  char *info;

  info = nio->line + nio->pos;
  if (1 != sscanf(info, NRRD_BIG_INT_PRINTF, &(nrrd->num))) {
    sprintf(err, "%s: couldn't parse number \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  } 
  */

  /* It was decided to just completely ignore this field.  "number" is
  ** entirely redundant with the (required) sizes field, and there no
  ** need to save it to, or learn it from, the header.  In fact the "num"
  ** field was eliminated from the Nrrd struct some time ago, in favor of
  ** the nrrdElementNumber() function.  It may seem odd or unfortunate that
  ** 
  **   number: Hank Hill sells propane and propane accessories
  **
  ** is a valid field specification, but at least Peggy is proud ...
  */

  return 0;
}

int 
_nrrdReadNrrdParse_type (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_type", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  if (!(nrrd->type = airEnumVal(nrrdType, info))) {
    sprintf(err, "%s: couldn't parse type \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_type](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

#define _PARSE_ONE_VAL(FIELD, CONV, TYPE) \
  if (1 != sscanf(info, CONV, &(FIELD))) { \
    sprintf(err, "%s: couldn't parse " TYPE " from \"%s\"", me, info); \
    biffMaybeAdd(NRRD, err, useBiff); return 1; \
  }

int
_nrrdReadNrrdParse_block_size (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_block_size", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nrrd->blockSize, "%d", "int");
  /* because blockSize and type fields may appear in any order,
     we can't use _nrrdFieldCheck[], but we can do something */
  if (!( nrrd->blockSize > 0 )) {
    sprintf(err, "%s: block size %d not > 0", me, nrrd->blockSize);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_dimension (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_dimension", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nrrd->dim, "%d", "int");
  if (_nrrdFieldCheck[nrrdField_dimension](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

/* 
** checking nrrd->dim against zero is valid because it is initialized
** to zero, and, _nrrdReadNrrdParse_dimension() won't allow it to be
** set to anything outside the range [1, NRRD_DIM_MAX] 
*/
#define _CHECK_HAVE_DIM \
  if (0 == nrrd->dim) { \
    sprintf(err, "%s: don't yet have a valid dimension", me); \
    biffMaybeAdd(NRRD, err, useBiff); return 1; \
  }

#define _CHECK_HAVE_SPACE_DIM \
  if (0 == nrrd->spaceDim) { \
    sprintf(err, "%s: don't yet have a valid space dimension", me); \
    biffMaybeAdd(NRRD, err, useBiff); return 1; \
  }

#define _CHECK_GOT_ALL_VALUES \
  if (nrrd->dim != ret) { \
    sprintf(err, "%s: parsed %d values, but dimension is %d",  \
            me, ret, nrrd->dim); \
    biffMaybeAdd(NRRD, err, useBiff); return 1; \
  }

int
_nrrdReadNrrdParse_sizes (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_sizes", err[AIR_STRLEN_MED];
  int ret, val[NRRD_DIM_MAX];
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  ret = airParseStrI(val, info, _nrrdFieldSep, nrrd->dim);
  _CHECK_GOT_ALL_VALUES;
  nrrdAxisInfoSet_nva(nrrd, nrrdAxisInfoSize, val);
  /* HEY: this is a very imperfect check of excess info */
  if (nrrd->dim+1 == airParseStrI(val, info, _nrrdFieldSep, nrrd->dim+1)) {
    sprintf(err, "%s: seem to have more than expected %d sizes",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_sizes](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_spacings (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_spacings", err[AIR_STRLEN_MED];
  int ret;
  double val[NRRD_DIM_MAX];
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  ret = airParseStrD(val, info, _nrrdFieldSep, nrrd->dim);
  _CHECK_GOT_ALL_VALUES;
  nrrdAxisInfoSet_nva(nrrd, nrrdAxisInfoSpacing, val);
  /* HEY: this is a very imperfect check of excess info */
  if (nrrd->dim+1 == airParseStrD(val, info, _nrrdFieldSep, nrrd->dim+1)) {
    sprintf(err, "%s: seem to have more than expected %d spacings",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_spacings](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_thicknesses (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_thicknesses", err[AIR_STRLEN_MED];
  int ret;
  double val[NRRD_DIM_MAX];
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  ret = airParseStrD(val, info, _nrrdFieldSep, nrrd->dim);
  _CHECK_GOT_ALL_VALUES;
  nrrdAxisInfoSet_nva(nrrd, nrrdAxisInfoThickness, val);
  /* HEY: this is a very imperfect check of excess info */
  if (nrrd->dim+1 == airParseStrD(val, info, _nrrdFieldSep, nrrd->dim+1)) {
    sprintf(err, "%s: seem to have more than expected %d thicknesses",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_thicknesses](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_axis_mins (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_axis_mins", err[AIR_STRLEN_MED];
  int ret;
  double val[NRRD_DIM_MAX];
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  ret = airParseStrD(val, info, _nrrdFieldSep, nrrd->dim);
  _CHECK_GOT_ALL_VALUES;
  nrrdAxisInfoSet_nva(nrrd, nrrdAxisInfoMin, val);
  /* HEY: this is a very imperfect check of excess info */
  if (nrrd->dim+1 == airParseStrD(val, info, _nrrdFieldSep, nrrd->dim+1)) {
    sprintf(err, "%s: seem to have more than expected %d axis mins",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_axis_mins](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_axis_maxs (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_axis_maxs", err[AIR_STRLEN_MED];
  int ret;
  double val[NRRD_DIM_MAX];
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  ret = airParseStrD(val, info, _nrrdFieldSep, nrrd->dim);
  _CHECK_GOT_ALL_VALUES;
  nrrdAxisInfoSet_nva(nrrd, nrrdAxisInfoMax, val);
  /* HEY: this is a very imperfect check of excess info */
  if (nrrd->dim+1 == airParseStrD(val, info, _nrrdFieldSep, nrrd->dim+1)) {
    sprintf(err, "%s: seem to have more than expected %d axis maxs",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_axis_maxs](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdGetSpaceVector(double val[NRRD_DIM_MAX],
                    char **hhP, int spaceDim, int useBiff) {
  char me[]="_nrrdGetSpaceVector", err[AIR_STRLEN_MED], *hh, *buff, sep[]=",)";
  airArray *mop;
  int ret, dd, length;
  
  mop = airMopNew();

  hh = *hhP;
  /* skip past space */
  length = strspn(hh, _nrrdFieldSep);
  hh += length;

  /* make sure we have something */
  if (!*hh) {
    sprintf(err, "%s: hit end of string before seeing (", me);
    biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
  }
  /* first, see if we're getting the non-vector */
  if ( (strstr(hh, _nrrdNoSpaceVector) == hh) ) {
    if (!hh[strlen(_nrrdNoSpaceVector)] 
        || strchr(_nrrdFieldSep, hh[strlen(_nrrdNoSpaceVector)])) {
      /* yes, we got the non-vector */
      for (dd=0; dd<spaceDim; dd++) {
        val[dd] = AIR_NAN;
      }
      length += strlen(_nrrdNoSpaceVector);
    } else {
      /* we got something that started out looking like the non-vector */
      sprintf(err, "%s: couldn't parse non-vector \"%s\"", me, hh);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  } else {
    /* this isn't a non-vector */
    /* make sure we have an open paren */
    if ('(' != *hh) {
      sprintf(err, "%s: first vector in \"%s\" didn't start with '('", me, hh);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    /* copy string (including open paren) for local fiddling */
    if (!(buff = airStrdup(hh))) {
      sprintf(err, "%s: couldn't allocate local buffer", me);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    airMopAdd(mop, buff, airFree, airMopAlways);
    /* scan for close paren */
    hh = buff+1;
    while (*hh) {
      if (')' == *hh) {
        break;
      } else {
        hh++;
      }
    }
    if (')' != *hh) {
      sprintf(err, "%s: didn't see ')' at end of first vector in \"%s\"",
              me, hh);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    /* terminate at end paren */
    *(hh+1) = 0;
    length += strlen(buff);
    /* see if we have too many fields */
    ret = airStrntok(buff+1, sep);
    if (ret > spaceDim) {
      sprintf(err, "%s: space dimension is %d, but seem to have %d "
              "coefficients", me, spaceDim, ret);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    /* try to parse the values */
    ret = airParseStrD(val, buff+1, ",", spaceDim);
    if (spaceDim != ret) {
      sprintf(err, "%s: parsed %d values, but space dimension is %d",
              me, ret, spaceDim);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  }
  /* probably not useful */
  for (dd=spaceDim; dd<NRRD_DIM_MAX; dd++) {
    val[dd] = AIR_NAN;
  }
  /* make sure all coefficients exist or not together */
  for (dd=1; dd<spaceDim; dd++) {
    if (!!AIR_EXISTS(val[0]) ^ !!AIR_EXISTS(val[dd])) {
      sprintf(err, "%s: existance of all space vector coefficients must "
              "be consistent (val[0] not like val[%d])", me, dd);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  }
  for (dd=0; dd<spaceDim; dd++) {
    if (airIsInf_d(val[dd])) {
      sprintf(err, "%s: vector coefficient %d can't be infinite", me, dd);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  }
  *hhP += length;
  airMopOkay(mop); 
  return 0;
}

int
_nrrdReadNrrdParse_space_directions (Nrrd *nrrd, NrrdIoState *nio, 
                                     int useBiff) {
  char me[]="_nrrdReadNrrdParse_space_directions", err[AIR_STRLEN_MED];
  int dd;
  char *info;

  info = nio->line + nio->pos;
  _CHECK_HAVE_DIM;
  _CHECK_HAVE_SPACE_DIM;

  for (dd=0; dd<nrrd->dim; dd++) {
    if (_nrrdGetSpaceVector(nrrd->axis[dd].spaceDirection,
                            &info, nrrd->spaceDim, useBiff)) {
      sprintf(err, "%s: trouble getting space vector %d of %d", 
              me, dd+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); return 1;
    }
  }
  if (strlen(info) != strspn(info, _nrrdFieldSep)) {
    sprintf(err, "%s: seem to have more than expected %d directions",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_space_directions](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_centers (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_centers", err[AIR_STRLEN_MED];
  int i;
  char *tok, *info, *last;
  airArray *mop;

  mop = airMopNew();
  info = airStrdup(nio->line + nio->pos);
  airMopAdd(mop, info, airFree, airMopAlways);
  _CHECK_HAVE_DIM;
  for (i=0; i<nrrd->dim; i++) {
    tok = airStrtok(!i ? info : NULL, _nrrdFieldSep, &last);
    if (!tok) {
      sprintf(err, "%s: couldn't extract string for center %d of %d",
              me, i+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    if (!strcmp(tok, NRRD_UNKNOWN)) {
      nrrd->axis[i].center = nrrdCenterUnknown;
      continue;
    }
    if (!(nrrd->axis[i].center = airEnumVal(nrrdCenter, tok))) {
      sprintf(err, "%s: couldn't parse center \"%s\" for axis %d",
              me, tok, i);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  }
  if (airStrtok(!i ? info : NULL, _nrrdFieldSep, &last)) {
    sprintf(err, "%s: seem to have more than expected %d centers",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_centers](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
  }
  airMopOkay(mop); 
  return 0;
}

int
_nrrdReadNrrdParse_kinds (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_kinds", err[AIR_STRLEN_MED];
  int i;
  char *info, *tok, *last;
  airArray *mop;

  mop = airMopNew();
  info = airStrdup(nio->line + nio->pos);
  airMopAdd(mop, info, airFree, airMopAlways);
  _CHECK_HAVE_DIM;
  for (i=0; i<nrrd->dim; i++) {
    tok = airStrtok(!i ? info : NULL, _nrrdFieldSep, &last);
    if (!tok) {
      sprintf(err, "%s: couldn't extract string for kind %d of %d",
              me, i+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
    if (!strcmp(tok, NRRD_UNKNOWN)) {
      nrrd->axis[i].kind = nrrdKindUnknown;
      continue;
    }
    if (!(nrrd->axis[i].kind = airEnumVal(nrrdKind, tok))) {
      sprintf(err, "%s: couldn't parse \"%s\" kind %d of %d",
              me, tok, i+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
    }
  }
  if (airStrtok(!i ? info : NULL, _nrrdFieldSep, &last)) {
    sprintf(err, "%s: seem to have more than expected %d kinds",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_kinds](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); airMopError(mop); return 1;
  }
  airMopOkay(mop); 
  return 0;
}

char *
_nrrdGetQuotedString(char **hP, int useBiff) {
  char me[]="_nrrdGetQuotedString", err[AIR_STRLEN_MED], *h, *buff, *ret;
  airArray *buffArr;
  int pos;
  
  h = *hP;
  /* skip past space */
  /* printf("!%s: h |%s|\n", me, h);*/
  h += strspn(h, _nrrdFieldSep);
  /* printf("!%s: h |%s|\n", me, h);*/

  /* make sure we have something */
  if (!*h) {
    sprintf(err, "%s: hit end of string before seeing opening \"", me);
    biffMaybeAdd(NRRD, err, useBiff); return NULL;
  }
  /* make sure we have a starting quote */
  if ('"' != *h) {
    sprintf(err, "%s: didn't start with \"", me);
    biffMaybeAdd(NRRD, err, useBiff); return NULL;
  }
  h++;
    
  /* parse string until end quote */
  buff = NULL;
  buffArr = airArrayNew((void**)(&buff), NULL, sizeof(char), 2);
  if (!buffArr) {
    sprintf(err, "%s: couldn't create airArray", me);
      biffMaybeAdd(NRRD, err, useBiff); return NULL;
  }
  pos = airArrayIncrLen(buffArr, 1);  /* pos should get 0 */
  while (h[pos]) {
    /* printf("!%s: h+%d |%s|\n", me, pos, h+pos); */
    if ('\"' == h[pos]) {
      break;
    }
    if ('\\' == h[pos] && '\"' == h[pos+1]) {
      h += 1;
    }
    buff[pos] = h[pos];
    pos = airArrayIncrLen(buffArr, 1);
  }
  if ('\"' != h[pos]) {
    sprintf(err, "%s: didn't see ending \" soon enough", me);
    biffMaybeAdd(NRRD, err, useBiff); return NULL;
  }
  h += pos + 1;
  buff[pos] = 0;

  ret = airStrdup(buff);
  airArrayNuke(buffArr);
  *hP = h;
  
  return ret;
}

int
_nrrdReadNrrdParse_labels (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_labels", err[AIR_STRLEN_MED];
  char *h;  /* this is the "here" pointer which gradually progresses
               through all the labels (for all axes) */
  int i;
  char *info;

  /* because we have to correctly interpret quote marks, we
     can't simply rely on airParseStrS */
  info = nio->line + nio->pos;
  /* printf("!%s: info |%s|\n", me, info); */
  _CHECK_HAVE_DIM;
  h = info;
  for (i=0; i<nrrd->dim; i++) {
    if (!( nrrd->axis[i].label = _nrrdGetQuotedString(&h, useBiff) )) {
      sprintf(err, "%s: couldn't get get label %d of %d\n",
              me, i+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); return 1;
    }
  }
  if (strlen(h) != strspn(h, _nrrdFieldSep)) {
    sprintf(err, "%s: seem to have more than expected %d labels",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_labels](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_units (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_units", err[AIR_STRLEN_MED];
  char *h;  /* this is the "here" pointer which gradually progresses
               through all the units (for all axes) */
  int i;
  char *info;

  /* because we have to correctly interpret quote marks, we
     can't simply rely on airParseStrS */
  info = nio->line + nio->pos;
  /* printf("!%s: info |%s|\n", me, info); */
  _CHECK_HAVE_DIM;
  h = info;
  for (i=0; i<nrrd->dim; i++) {
    if (!( nrrd->axis[i].units = _nrrdGetQuotedString(&h, useBiff) )) {
      sprintf(err, "%s: couldn't get get unit %d of %d\n",
              me, i+1, nrrd->dim);
      biffMaybeAdd(NRRD, err, useBiff); return 1;
    }
  }
  if (strlen(h) != strspn(h, _nrrdFieldSep)) {
    sprintf(err, "%s: seem to have more than expected %d units",
            me, nrrd->dim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_units](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_min (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {

  /* This field is no longer assumed to be anything meaningful,
     because nrrd->min no longer exists with the advent of NrrdRange.
     But, having the field is not an error, to not trip on older
     NRRD00.01 and NRRD0001 files which (legitimately) used it */

  return 0;
}

int
_nrrdReadNrrdParse_max (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {

  /* nrrd->max no longer exists, see above */

  return 0;
}

int
_nrrdReadNrrdParse_old_min (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_old_min", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nrrd->oldMin, "%lg", "double");
  if (_nrrdFieldCheck[nrrdField_old_min](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_old_max (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_old_max", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nrrd->oldMax, "%lg", "double");
  if (_nrrdFieldCheck[nrrdField_old_max](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

/*
** opens seperate datafile and stores FILE* in nio->dataFile,
** which otherwise will stay NULL
**
** Note lack of thread-safety: strerror(errno)
*/
int
_nrrdReadNrrdParse_data_file (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_data_file", err[AIR_STRLEN_MED],
    dataName[AIR_STRLEN_HUGE];
  char *info;

  info = nio->line + nio->pos;
  if (!strncmp(info, _nrrdRelativePathFlag, strlen(_nrrdRelativePathFlag))) {
    /* data file directory is relative to header directory */
    if (!nio->path) {
      sprintf(err, "%s: nrrd file refers to header-relative data file "
              "\"%s\", but don't know path of header", me, info);
      biffMaybeAdd(NRRD, err, useBiff); return 1;
    }
    info += strlen(_nrrdRelativePathFlag);
    sprintf(dataName, "%s/%s", nio->path, info);
  } else {
    /* data file's name is absolute (not header-relative) */
    strcpy(dataName, info);
  }
  if (!(nio->dataFile = fopen(dataName, "rb"))) {
    sprintf(err, "%s: fopen(\"%s\",\"rb\") failed: %s",
            me, dataName, strerror(errno));
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  /* the seperate data file will be closed elsewhere */
  return 0;
}

int
_nrrdReadNrrdParse_endian (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_endian", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  if (!(nio->endian = airEnumVal(airEndian, info))) {
    sprintf(err, "%s: couldn't parse endian \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_encoding (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_encoding", err[AIR_STRLEN_MED];
  char *info;
  int etype;

  info = nio->line + nio->pos;
  if (!(etype = airEnumVal(nrrdEncodingType, info))) {
    sprintf(err, "%s: couldn't parse encoding \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }

  nio->encoding = nrrdEncodingArray[etype];
  return 0;
}

int
_nrrdReadNrrdParse_line_skip (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_line_skip", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nio->lineSkip, "%d", "int");
  if (!(0 <= nio->lineSkip)) {
    sprintf(err, "%s: lineSkip value %d invalid", me, nio->lineSkip);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_byte_skip (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_byte_skip", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  _PARSE_ONE_VAL(nio->byteSkip, "%d", "int");
  if (!(-1 <= nio->byteSkip)) {
    sprintf(err, "%s: byteSkip value %d invalid", me, nio->byteSkip);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_keyvalue (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_keyvalue", err[AIR_STRLEN_MED];
  char *keysep, *line, *key, *value;

  /* we know this will find something */
  line = airStrdup(nio->line);
  if (!line) {
    sprintf(err, "%s: can't allocate parse line", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  keysep = strstr(line, ":=");
  if (!keysep) {
    sprintf(err, "%s: didn't see \":=\" key/value delimiter in \"%s\"",
            me, line);
    free(line); biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  keysep[0] = 0;
  keysep[1] = 0;
  key = line;
  value = keysep+2;
  
  /* convert escape sequences */
  airUnescape(key);
  airUnescape(value);

  nrrdKeyValueAdd(nrrd, key, value);

  free(line);
  return 0;
}

int
_nrrdReadNrrdParse_sample_units (Nrrd *nrrd, NrrdIoState *nio,
                                 int useBiff) {
  char me[]="_nrrdReadNrrdParse_sample_units", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  
  if (strlen(info) && !(nrrd->sampleUnits = airStrdup(info))) {
    sprintf(err, "%s: couldn't strdup() sampleUnits", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_sample_units](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_space (Nrrd *nrrd, NrrdIoState *nio,
                          int useBiff) {
  char me[]="_nrrdReadNrrdParse_space", err[AIR_STRLEN_MED], *info;
  int space;

  info = nio->line + nio->pos;
  if (nio->seen[nrrdField_space_dimension]) {
    sprintf(err, "%s: can't specify space after specifying "
            "space dimension (%d)", me, nrrd->spaceDim);
    biffAdd(NRRD, err); return 1;
  }
  if (!(space = airEnumVal(nrrdSpace, info))) {
    sprintf(err, "%s: couldn't parse space \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (nrrdSpaceSet(nrrd, space)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_space](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_space_dimension (Nrrd *nrrd, NrrdIoState *nio,
                                    int useBiff) {
  char me[]="_nrrdReadNrrdParse_space_dimension", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;
  if (nio->seen[nrrdField_space]) {
    sprintf(err, "%s: can't specify space dimension after specifying "
            "space (%s)", me, airEnumStr(nrrdSpace, nrrd->space));
    biffAdd(NRRD, err); return 1;
  }
  _PARSE_ONE_VAL(nrrd->spaceDim, "%d", "int");
  if (_nrrdFieldCheck[nrrdField_space_dimension](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_space_units (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParse_space_units", err[AIR_STRLEN_MED];
  char *h;  /* this is the "here" pointer which gradually progresses
               through all the units (for all axes) */
  int i;
  char *info;

  /* because we have to correctly interpret quote marks, we
     can't simply rely on airParseStrS */
  info = nio->line + nio->pos;
  /* printf("!%s: info |%s|\n", me, info); */
  _CHECK_HAVE_SPACE_DIM;
  h = info;
  for (i=0; i<nrrd->spaceDim; i++) {
    if (!( nrrd->spaceUnits[i] = _nrrdGetQuotedString(&h, useBiff) )) {
      sprintf(err, "%s: couldn't get get space unit %d of %d",
              me, i+1, nrrd->spaceDim);
      biffMaybeAdd(NRRD, err, useBiff); return 1;
    }
  }
  if (_nrrdGetQuotedString(&h, AIR_FALSE)) {
    sprintf(err, "%s: seemed to have more than expected %d space units",
            me, nrrd->spaceDim);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_space_units](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

int
_nrrdReadNrrdParse_space_origin (Nrrd *nrrd, NrrdIoState *nio,
                                 int useBiff) {
  char me[]="_nrrdReadNrrdParse_space_origin", err[AIR_STRLEN_MED];
  char *info;

  info = nio->line + nio->pos;

  _CHECK_HAVE_SPACE_DIM;

  if (_nrrdGetSpaceVector(nrrd->spaceOrigin, &info, nrrd->spaceDim, useBiff)) {
    sprintf(err, "%s: couldn't parse origin \"%s\"", me, info);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  if (_nrrdFieldCheck[nrrdField_space_origin](nrrd, useBiff)) {
    sprintf(err, "%s: trouble", me);
    biffMaybeAdd(NRRD, err, useBiff); return 1;
  }
  return 0;
}

/*
** _nrrdReadNrrdParseInfo[NRRD_FIELD_MAX+1]()
**
** These are all for parsing the stuff AFTER the colon
*/
int
(*_nrrdReadNrrdParseInfo[NRRD_FIELD_MAX+1])(Nrrd *, NrrdIoState *, int) = {
  _nrrdReadNrrdParse_nonfield,
  _nrrdReadNrrdParse_comment,
  _nrrdReadNrrdParse_content,
  _nrrdReadNrrdParse_number,
  _nrrdReadNrrdParse_type,
  _nrrdReadNrrdParse_block_size,
  _nrrdReadNrrdParse_dimension,
  _nrrdReadNrrdParse_space,
  _nrrdReadNrrdParse_space_dimension,
  _nrrdReadNrrdParse_sizes,
  _nrrdReadNrrdParse_spacings,
  _nrrdReadNrrdParse_thicknesses,
  _nrrdReadNrrdParse_axis_mins,
  _nrrdReadNrrdParse_axis_maxs,
  _nrrdReadNrrdParse_space_directions,
  _nrrdReadNrrdParse_centers,
  _nrrdReadNrrdParse_kinds,
  _nrrdReadNrrdParse_labels,
  _nrrdReadNrrdParse_units,
  _nrrdReadNrrdParse_min,
  _nrrdReadNrrdParse_max,
  _nrrdReadNrrdParse_old_min,
  _nrrdReadNrrdParse_old_max,
  _nrrdReadNrrdParse_data_file,
  _nrrdReadNrrdParse_endian,
  _nrrdReadNrrdParse_encoding,
  _nrrdReadNrrdParse_line_skip,
  _nrrdReadNrrdParse_byte_skip,
  _nrrdReadNrrdParse_keyvalue,
  _nrrdReadNrrdParse_sample_units,
  _nrrdReadNrrdParse_space_units,
  _nrrdReadNrrdParse_space_origin
};

/*
** _nrrdReadNrrdParseField()
**
** This is for parsing the stuff BEFORE the colon
*/
int
_nrrdReadNrrdParseField (Nrrd *nrrd, NrrdIoState *nio, int useBiff) {
  char me[]="_nrrdReadNrrdParseField", err[AIR_STRLEN_MED], *next,
    *buff, *colon, *keysep;
  int ret, fld=nrrdField_unknown, noField, badField=AIR_FALSE;
  
  next = nio->line + nio->pos;

  /* determining if the line is a comment is simple */
  if (NRRD_COMMENT_CHAR == next[0]) {
    return nrrdField_comment;
  }

  if (!( buff = airStrdup(next) )) {
    sprintf(err, "%s: couldn't allocate buffer!", me);
    biffMaybeAdd(NRRD, err, useBiff); return nrrdField_unknown;
  }

  /* #1: "...if you see a colon, then look for an equal sign..." */

  /* Look for colon: if no colon, or failed to parse as a field, look for
   * equal sign, if that failed then error */

  /* Let the separator be := */ 
  /* Escape \n */

  colon = strstr(buff, ": ");
  noField = !colon;
  if (colon) {
    *colon = '\0';
    badField = ( nrrdField_unknown == (fld = airEnumVal(nrrdField, buff)) );
  }
  if (noField || badField) {
    keysep = strstr(buff, ":=");
    if (!keysep) {
      if (noField) {
        sprintf(err, "%s: didn't see \": \" or \":=\" in line", me);
      } else {
        sprintf(err, "%s: failed to parse \"%s\" as field identifier",
                me, buff);
      }
      free(buff); biffMaybeAdd(NRRD, err, useBiff); return nrrdField_unknown;
    }

    free(buff);
    ret = nrrdField_keyvalue;
  } else {

    /* *colon = '\0'; */
    /* else we successfully parsed a field identifier */
    next += strlen(buff) + 2;
    free(buff);
  
    /* skip whitespace prior to start of first field descriptor */
    next += strspn(next, _nrrdFieldSep);
    nio->pos = next - nio->line;

    ret = fld;
  }
  return ret;
}

/* kernel parsing is all in kernel.c */

