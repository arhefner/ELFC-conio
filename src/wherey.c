#define _ELFCLIB_
#include "../include/conio.h"

#pragma             extrn C_curpos

/*
 * Returns the row the cursor is in, the first row is 1.
 */
int wherey(void) {
  return (_curpos() >> 8) & 255;
}
