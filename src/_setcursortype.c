#define _ELFCLIB_
#include "bios.h"

/*
 * Hide the cursor when type is _NOCURSOR, show it for any other type.
 * A terminal has one shape of cursor, so _SOLIDCURSOR and _NORMALCURSOR
 * do the same thing.
 */
void _setcursortype(int type) {
  asm("         load rf, sc_seq   ; send the start of cursor mode");
  asm("         call f_msg");
  asm("         gosub s_lget16    ; get the cursor type");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra");
  asm("         lbnz sc_show");
  asm("         ghi  ra");
  asm("         lbnz sc_show");
  asm("         ldi  'l'          ; reset mode to hide the cursor");
  asm("         lbr  sc_send");
  asm("sc_show: ldi  'h'          ; set mode to show the cursor");
  asm("sc_send: call f_type");
  asm("         lbr  sc_end");
  asm("sc_seq:  db   27,'[?25',0");
  asm("sc_end:");
}
