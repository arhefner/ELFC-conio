#define _ELFCLIB_
#include "../include/conio.h"

#pragma             extrn C_curpos

/*
 * Returns the column the cursor is in, the first column is 1.
 */
int wherex(void) {
  return _curpos() & 255;
}
