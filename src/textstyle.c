#define _ELFCLIB_
#include "bios.h"

/*
 * Turn a text attribute on or off for the text that follows.  The
 * attribute is one of the A_ values in conio.h, or any other ANSI
 * select graphic rendition number from 0 to 99.
 */
void textstyle(int attr) {
  asm("         ldi  27           ; send the start of set attributes");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the attribute");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra");
  asm("         plo  rc           ; rc.0 is the value left to convert");
  asm("         ldi  0            ; rf.0 counts tens");
  asm("         plo  rf");
  asm("ts_ten:  glo  rc           ; subtract tens");
  asm("         smi  10");
  asm("         lbnf ts_one       ; until the value is less than 10");
  asm("         plo  rc");
  asm("         inc  rf");
  asm("         lbr  ts_ten");
  asm("ts_one:  glo  rc           ; push the ones digit");
  asm("         ori  '0'");
  asm("         stxd");
  asm("         glo  rf           ; check for a tens digit");
  asm("         lbz  ts_lst");
  asm("         ori  '0'");
  asm("         call f_type       ; send the tens digit");
  asm("ts_lst:  irx               ; pop the ones digit");
  asm("         ldx");
  asm("         call f_type       ; and send it");
  asm("         ldi  'm'");
  asm("         call f_type");
}
