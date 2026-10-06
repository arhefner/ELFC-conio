#define _ELFCLIB_
#include <stdio.h>
#include "../include/conio.h"

#pragma             extrn Cvsprintf
#pragma             extrn Ccputs

static char _cbuf[CPRINTF_MAX];

/*
 * Send formatted output to the console.  The output is formatted into
 * a buffer of CPRINTF_MAX characters first, so it must be shorter than
 * that.  A line feed is not expanded to a carriage return and line feed.
 * Returns the number of characters sent.
 */
int cprintf(const char *fmt, ...) {
  int n;

  n = vsprintf(_cbuf, fmt, (void **) &fmt + 1);
  cputs(_cbuf);
  return n;
}
