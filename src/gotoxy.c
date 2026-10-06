#define _ELFCLIB_
#include "bios.h"

/*
 * Move the cursor to column x of row y.  The top left corner of the
 * screen is column 1 of row 1, and the largest value for either is 255.
 */
void gotoxy(int x, int y) {
  asm("         ldi  27           ; send the start of cursor position");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the row");
  asm("           dw  2           ; from arg 2");
  asm("         glo  ra");
  asm("         call gxy_num      ; send it as a decimal number");
  asm("         ldi  ';'");
  asm("         call f_type");
  asm("         gosub s_lget16    ; get the column");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra");
  asm("         call gxy_num      ; send it as a decimal number");
  asm("         ldi  'H'");
  asm("         call f_type");
  asm("         lbr  gxy_end");

  /*
   * Send the byte in D as a decimal number with no leading zeros.  The
   * digits still to send are kept on the stack, not in a register.
   */
  asm("gxy_num: plo  rc           ; rc.0 is the value left to convert");
  asm("         ldi  0            ; rf.0 counts hundreds, rf.1 counts tens");
  asm("         plo  rf");
  asm("         phi  rf");
  asm("gxy_hun: glo  rc           ; subtract hundreds");
  asm("         smi  100");
  asm("         lbnf gxy_ten      ; until the value is less than 100");
  asm("         plo  rc");
  asm("         inc  rf");
  asm("         lbr  gxy_hun");
  asm("gxy_ten: glo  rc           ; subtract tens");
  asm("         smi  10");
  asm("         lbnf gxy_one      ; until the value is less than 10");
  asm("         plo  rc");
  asm("         ghi  rf");
  asm("         adi  1");
  asm("         phi  rf");
  asm("         lbr  gxy_ten");
  asm("gxy_one: glo  rc           ; push the ones digit");
  asm("         ori  '0'");
  asm("         stxd");
  asm("         glo  rf           ; check for a hundreds digit");
  asm("         lbz  gxy_two");
  asm("         ghi  rf           ; push the tens digit");
  asm("         ori  '0'");
  asm("         stxd");
  asm("         glo  rf           ; send the hundreds digit");
  asm("         ori  '0'");
  asm("         call f_type");
  asm("         irx               ; pop the tens digit");
  asm("         ldx");
  asm("         lbr  gxy_snd");
  asm("gxy_two: ghi  rf           ; check for a tens digit");
  asm("         lbz  gxy_lst");
  asm("         ori  '0'");
  asm("gxy_snd: call f_type       ; send the tens digit");
  asm("gxy_lst: irx               ; pop the ones digit");
  asm("         ldx");
  asm("         call f_type       ; and send it");
  asm("         rtn");
  asm("gxy_end:");
}
