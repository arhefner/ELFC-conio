#define _ELFCLIB_
#include "bios.h"

/*
 * Set the background color of the text that follows, which is one of
 * the colors from BLACK to LIGHTGRAY.
 */
void textbackground(int color) {
  asm("         ldi  27           ; send the start of set attributes");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  '4'          ; 40 to 47 are the background colors");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the color");
  asm("           dw  0           ; from arg 1");
  asm("         load rf, tb_ansi  ; look up the digit for the color");
  asm("         glo  ra");
  asm("         ani  7");
  asm("         str  r2");
  asm("         glo  rf");
  asm("         add");
  asm("         plo  rf");
  asm("         ghi  rf");
  asm("         adci 0");
  asm("         phi  rf");
  asm("         ldn  rf");
  asm("         call f_type");
  asm("         ldi  'm'");
  asm("         call f_type");
  asm("         lbr  tb_end");
  asm("tb_ansi: db   '04261537'   ; ANSI colors swap red and blue");
  asm("tb_end:");
}
