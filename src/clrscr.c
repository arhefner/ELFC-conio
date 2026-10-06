#define _ELFCLIB_
#include "bios.h"

/*
 * Clear the screen and move the cursor to the top left corner.
 */
void clrscr(void) {
  asm("         load rf, clr_seq  ; send erase display and cursor home");
  asm("         call f_msg");
  asm("         lbr  clr_end");
  asm("clr_seq: db   27,'[2J',27,'[H',0");
  asm("clr_end:");
}
