#define _ELFCLIB_
#include "bios.h"

/*
 * Show the text that follows in normal intensity.
 */
void lowvideo(void) {
  asm("         load rf, lv_seq   ; send normal intensity");
  asm("         call f_msg");
  asm("         lbr  lv_end");
  asm("lv_seq:  db   27,'[22m',0");
  asm("lv_end:");
}
