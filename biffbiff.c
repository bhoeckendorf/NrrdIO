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
** This is mostly garbage.
** It needs to be re-written.
** I apologize.
*/

/*
** _biffEntry struct
**
** hold information and messages associated with one key
*/
typedef struct {
  char key[BIFF_MAXKEYLEN+1]; /* the key */
  char **err;                 /* array of error strings; the err array itself
				 is NOT null-terminated */
  int num;                    /* length of "err" == # strings stored */
  airArray *AA;               /* air array for err and num */
} _biffEntry;

_biffEntry **_biffErr=NULL;   /* master array of _biffEntry pointers */
int _biffNum=0,               /* length of _biffErr == # keys maintained */
  _biffIdx=-1;                /* hack: index of latest key found */
airArray *_biffAA=NULL;       /* air array of _biffErr and _biffNum */

#define _BIFF_INCR 2

/*
** _biffInit()
**
** allocates data structers needed by biff.  Panics and exit(1)s if 
** anything goes wrong.  Can be harmlessly called multiple times.
*/
void
_biffInit() {
  char me[]="_biffInit";

  if (!_biffAA) {
    _biffAA = airArrayNew((void**)&_biffErr, &_biffNum, 
			  sizeof(_biffEntry*), _BIFF_INCR);
    if (!_biffAA) {
      fprintf(stderr, "%s: PANIC: couldn't allocate internal data\n", me);
      exit(1);
    }
  }
}

/*
** _biffCheckKey()
**
** makes sure given key is kosher.  Panics and exit(1)s if given a NULL key
** or if key is too long
*/
void
_biffCheckKey(const char *key) {
  char me[] = "_biffCheckKey";

  if (!key) {
    fprintf(stderr, "%s: PANIC: given NULL key\n", me);
    exit(1);
  }
  if (strlen(key) > BIFF_MAXKEYLEN) {
    fprintf(stderr, "%s: PANIC: key \"%s\" exceeds %d chars\n",
	    me, key, BIFF_MAXKEYLEN);
    exit(1);
  }
}

/*
** _biffFindKey()
**
** returns a pointer to the entry which contains the given key, or
** NULL if it was not found
*/
_biffEntry *
_biffFindKey(const char *key) {
  int i = -1;
  _biffEntry *e;

  if (_biffNum) {
    for (i=0; i<=_biffNum-1; i++) {
      /* printf("HEY: comparing key[%d]=\"%s\" to \"%s\"\n", 
	 i, _biffErr[i]->key, key); */
      if (!strcmp(_biffErr[i]->key, key))
	break;
    }
    if (i == _biffNum) {
      i = -1;
    }
  }
  /* printf("HEY: index(\"%s\") = %d\n", key, i); */
  if (-1 == i) {
    e = NULL;
    _biffIdx = -1;
  }
  else {
    e = _biffErr[i];
    _biffIdx = i;
  }
  return(e);
}

/*
** _biffNewEntry()
**
** creates and initializes one new _biffEntry, returning a pointer to it
** panics and exit(1)s if there is a problem.
*/
_biffEntry *
_biffNewEntry(const char *key) {
  char me[]="_biffInitEntry";
  _biffEntry *e;

  e = calloc(1, sizeof(_biffEntry));
  if (!e) {
    fprintf(stderr, "%s: couldn't make entry for new key \"%s\"\n", me, key);
    exit(1);
  }
  strcpy(e->key, key);
  e->AA = airArrayNew((void**)&(e->err), &(e->num), sizeof(char*), _BIFF_INCR);
  if (!e->AA) {
    fprintf(stderr, "%s: couldn't make array for new key \"%s\"\n", me, key);
    exit(1);
  }
  airArrayPointerCB(e->AA, NULL, airFree);
  return(e);
}

/*
** _biffNukeEntry()
**
** deletes given entry, and all info contained therein
*/
void
_biffNukeEntry(_biffEntry *e) {

  if (e) {
    airArraySetLen(e->AA, 0);
    airArrayNuke(e->AA);
    free(e);
  }
}

/*
** _biffAddKey()
**
** adds a key to _biffErr, and returns a pointer to the new entry
** assumes that given key does NOT appear in current list.
** panics and exit(1)s if there is a problem
*/
_biffEntry *
_biffAddKey(const char *key) {
  char me[]="_biffAddKey";
  int i, newIdx;
  _biffEntry *e;

  /* find index of new key */
  for (i=0; i<=_biffNum-1; i++) {
    if (strcmp(key, _biffErr[i]->key) < 0) {
      /* we've hit the one which comes after the new key */
      break;
    }
  }
  /* if the for loop was never broken, _biffNum is the correct new index */
  newIdx = i;
  /* printf("HEY: index(new key \"%s\") = %d\n", key, i); */
  
  if (-1 == airArrayIncrLen(_biffAA, 1)) {
    fprintf(stderr, "%s: PANIC: couldn't accomodate one more key\n", me);
    exit(1);
  }

  /* _biffNum is now one bigger */
  for (i=_biffNum-2; i>=newIdx; i--) {
    _biffErr[i+1] = _biffErr[i];
  }
  e = _biffErr[newIdx] = _biffNewEntry(key);

  return(e);
}

/*
** _biffAddErr()
**
** adds a given message to the given entry.  The message is processed to
** convert all whitespace into ' ', and to eliminate whitespace at the
** end of the message.
** panics and exit(1)s if there is a problem
*/
void
_biffAddErr(_biffEntry *e, const char *err) {
  char *buf, me[]="_biffAddErr";
  int i, len;

  /* printf("%s: HEY(before): err[%s]->num = %d\n", me, e->key, e->num); */
  if (-1 == airArrayIncrLen(e->AA, 1)) {
    fprintf(stderr, "%s: PANIC: couldn't add message for key %s\n",
	    me, e->key);
    exit(1);
  }
  /* printf("%s: HEY(after): err[%s]->num = %d\n", me, e->key, e->num); */
  buf = airStrdup(err);
  len = strlen(buf);
  for (i=0; i<=len-1; i++) {
    if (isspace(buf[i]))
      buf[i] = ' ';
  }
  i = len-1;
  while (isspace(buf[i])) {
    buf[i--] = 0;
  }
  /* printf("%s: HEY(after): err[%s]->num = %d\n", me, e->key, e->num); */
  /* printf("%s: HEY: err[%s][%d] now \"%s\"\n", me, e->key, e->num-1, buf); */
  e->err[e->num-1] = buf;
}

/***********************************************************************/
/***********************************************************************/

/*
******** biffSet()
**
** Sets given message "err" to be only message at "key".  "key" can be
** a new or existing key, but if it is an existing key, then existing
** messages at that key are lost
*/
void
biffSet(const char *key, const char *err) {
  _biffEntry *e;

  _biffInit();
  _biffCheckKey(key);

  e = _biffFindKey(key);
  if (!e) {
    /* not a key we remember seeing */
    e = _biffAddKey(key);
  }

  /* delete any existing messages at this index */
  airArraySetLen(e->AA, 0);

  /* add the new message */
  _biffAddErr(e, err);
}

/*
******** biffAdd()
**
** just like biffSet(), but doesn't delete existing messages
*/
void
biffAdd(const char *key, const char *err) {
  _biffEntry *e;

  _biffInit();
  _biffCheckKey(key);
  
  e = _biffFindKey(key);
  if (!e) {
    e = _biffAddKey(key);
  }

  /* add the new message */
  _biffAddErr(e, err);
}

/*
******** biffMaybeAdd()
**
** wrapper around biffAdd() but doesn't actually do anything if !useBiff
*/
void
biffMaybeAdd(const char *key, const char *err, int useBiff) {

  if (useBiff) {
    biffAdd(key, err);
  }
}

/*
******** biffGet()
**
** creates a string which records all the errors at given key and
** returns it.  Returns NULL in case of error.  This function should
** be considered a glorified strdup(): it is the callers responsibility
** to free this string later
*/
char *
biffGet(const char *key) {
  int i, max, len, sum;
  char me[] = "biffGet", *ret = NULL, *buf;
  _biffEntry *e;

  _biffInit();
  _biffCheckKey(key);

  /* find the index */
  e = _biffFindKey(key);
  if (!e) {
    /* error: not a key we remember seeing */
    fprintf(stderr, "%s: WARNING: no information for key \"%s\"\n", me, key);
    return(NULL);
  }
  if (!e->num) {
    /* there's a key, but no error messages.  Odd. */
    return(airStrdup(""));
  }

  max = sum = 0;
  for (i=0; i<=e->num-1; i++) {
    len = strlen(e->err[i]) + strlen(e->key) + 4;
    sum += len;
    max = AIR_MAX(max, len);
  }
  buf = (char*)calloc(max+1, sizeof(char));
  ret = (char*)calloc(sum+1, sizeof(char));
  if (!(buf && ret)) {
    fprintf(stderr, "%s: PANIC: unable to allocate buffers\n", me);
    exit(1);
  }
  for (i=e->num-1; i>=0; i--) {
    sprintf(buf, "[%s] %s\n", key, e->err[i]);
    strcat(ret, buf);
  }
  free(buf);

  return(ret);
}

/*
******** biffCheck()
**
** sees how many messages there are for a given key
** returns 0 if the key doesn't exist.
*/
int
biffCheck(const char *key) {
  _biffEntry *e;

  _biffInit();
  _biffCheckKey(key);
  
  e = _biffFindKey(key);
  if (!e) {
    return 0;
  }
  
  return e->num;
}

/*
******** biffDone()
**
** frees everything associated with given key, and shrinks list of keys
*/
void
biffDone(const char *key) {
  char me[]="biffDone";
  int i, idx;
  _biffEntry *e;

  _biffInit();
  _biffCheckKey(key);

  e = _biffFindKey(key);
  if (!e) {
    fprintf(stderr, "%s: WARNING: no information for key \"%s\"\n", me, key);
    return;
  }
  idx = _biffIdx;

  _biffNukeEntry(e);
  for (i=idx; i<=_biffNum-2; i++) {
    _biffErr[i] = _biffErr[i+1];
  }
  airArrayIncrLen(_biffAA, -1);
}

void
biffMove(const char *destKey, const char *err, const char *srcKey) {
  int i, len, max;
  char me[] = "biffMove", *buf;
  _biffEntry *dest, *src;

  _biffInit();
  _biffCheckKey(destKey);
  _biffCheckKey(srcKey);

  /* if srcKey and destKey are the same, this degenerates to biffAdd() */
  if (!strcmp(destKey, srcKey)) {
    biffAdd(srcKey, err);
    return;
  }

  dest = _biffFindKey(destKey);
  if (!dest) {
    dest = _biffAddKey(destKey);
  }
  src = _biffFindKey(srcKey);
  if (!src) {
    fprintf(stderr, "%s: WARNING: key \"%s\" unknown\n", me, srcKey);
    return;
  }

  max = 0;
  for (i=0; i<=src->num-1; i++) {
    len = strlen(src->err[i]) + strlen(src->key) + 4;
    max = AIR_MAX(max, len);
  }
  buf = (char*)calloc(max+1, sizeof(char));
  if (!buf) {
    fprintf(stderr, "%s: PANIC: can't allocate buffer\n", me);
    exit(1);
  }

  for (i=0; i<=src->num-1; i++) {
    sprintf(buf, "[%s] %s", srcKey, src->err[i]);
    /* printf("%s: HEY: moving \"%s\" to %s\n", me, buf, destKey); */
    _biffAddErr(dest, buf);
  }
  if (err) {
    _biffAddErr(dest, err);
  }
  biffDone(srcKey);
  
  free(buf);
}

char *
biffGetDone(const char *key) {
  char *ret;

  _biffInit();
  _biffCheckKey(key);

  ret = biffGet(key);
  biffDone(key);

  return(ret);
}

