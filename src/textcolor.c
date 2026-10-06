#define _ELFCLIB_
#include "bios.h"

/*
 * Set the color of the text that follows.  The colors from DARKGRAY to
 * WHITE are shown as the colors from BLACK to LIGHTGRAY in high
 * intensity.  Adding BLINK to the color makes the text blink, which
 * stays on until normvideo or textattr is called.
 */
void textcolor(int color) {
  asm("         ldi  27           ; send the start of set attributes");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the color");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; light colors are high intensity");
  asm("         ani  8");
  asm("         lbz  tc_low");
  asm("         ldi  '1'          ; 1 is bold");
  asm("         lbr  tc_int");
  asm("tc_low:  ldi  '2'          ; 22 is normal intensity");
  asm("         call f_type");
  asm("         ldi  '2'");
  asm("tc_int:  call f_type");
  asm("         gosub s_lget16    ; get the color again");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; check for blinking text");
  asm("         ani  128");
  asm("         lbz  tc_col");
  asm("         ldi  ';'          ; 5 is blink");
  asm("         call f_type");
  asm("         ldi  '5'");
  asm("         call f_type");
  asm("tc_col:  ldi  ';'          ; 30 to 37 are the text colors");
  asm("         call f_type");
  asm("         ldi  '3'");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the color again");
  asm("           dw  0           ; from arg 1");
  asm("         load rf, tc_ansi  ; look up the digit for the color");
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
  asm("         lbr  tc_end");
  asm("tc_ansi: db   '04261537'   ; ANSI colors swap red and blue");
  asm("tc_end:");
}
