#define _ELFCLIB_
#include "bios.h"

/*
 * Set the text color, the background color and blinking in one call.
 * Bits 0 to 3 of attr are the text color, bits 4 to 6 the background
 * color, and bit 7 makes the text blink:
 *
 *   textattr((BLUE << 4) | YELLOW);
 */
void textattr(int attr) {
  asm("         ldi  27           ; send the start of set attributes");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  '0'          ; 0 turns all the attributes off");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the attribute");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; light colors are high intensity");
  asm("         ani  8");
  asm("         lbz  ta_blk");
  asm("         ldi  ';'          ; 1 is bold");
  asm("         call f_type");
  asm("         ldi  '1'");
  asm("         call f_type");
  asm("ta_blk:  gosub s_lget16    ; get the attribute again");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; check for blinking text");
  asm("         ani  128");
  asm("         lbz  ta_fg");
  asm("         ldi  ';'          ; 5 is blink");
  asm("         call f_type");
  asm("         ldi  '5'");
  asm("         call f_type");
  asm("ta_fg:   ldi  ';'          ; 30 to 37 are the text colors");
  asm("         call f_type");
  asm("         ldi  '3'");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the attribute again");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra");
  asm("         call ta_col       ; send the digit for the color");
  asm("         ldi  ';'          ; 40 to 47 are the background colors");
  asm("         call f_type");
  asm("         ldi  '4'");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the attribute again");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; move the background color down");
  asm("         shr");
  asm("         shr");
  asm("         shr");
  asm("         shr");
  asm("         call ta_col       ; send the digit for the color");
  asm("         ldi  'm'");
  asm("         call f_type");
  asm("         lbr  ta_end");

  /* send the ANSI color digit for the color in bits 0 to 2 of D */
  asm("ta_col:  ani  7            ; look up the digit for the color");
  asm("         str  r2");
  asm("         load rf, ta_ansi");
  asm("         glo  rf");
  asm("         add");
  asm("         plo  rf");
  asm("         ghi  rf");
  asm("         adci 0");
  asm("         phi  rf");
  asm("         ldn  rf");
  asm("         call f_type       ; send it");
  asm("         rtn");
  asm("ta_ansi: db   '04261537'   ; ANSI colors swap red and blue");
  asm("ta_end:");
}
