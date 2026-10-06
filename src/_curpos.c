#define _ELFCLIB_
#include "bios.h"

/*
 * Ask the terminal where the cursor is.  The terminal answers a device
 * status report with ESC [ row ; column R, which is read without echo.
 * Returns the row in the high byte and the column in the low byte.
 *
 * This waits until the terminal answers, so it does not return on a
 * terminal that does not support the cursor position report.
 */
int _curpos(void) {
  asm("         ghi  re           ; save the baud rate and echo flag");
  asm("         stxd");
  asm("         ani  0feh         ; turn off the echo");
  asm("         phi  re");
  asm("         load rf, cp_seq   ; send the cursor position request");
  asm("         call f_msg");
  asm("cp_wait: call f_read       ; skip input until the escape");
  asm("         xri  27");
  asm("         lbnz cp_wait");
  asm("         call f_read       ; skip the bracket");
  asm("         call cp_num       ; read the row");
  asm("         stxd              ; and keep it on the stack");
  asm("         call cp_num       ; read the column");
  asm("         plo  ra");
  asm("         irx               ; get the row back");
  asm("         ldx");
  asm("         phi  ra");
  asm("         irx               ; restore the echo flag");
  asm("         ldx");
  asm("         phi  re");
  asm("         lbr  cp_end");
  asm("cp_seq:  db   27,'[6n',0");

  /*
   * Read a decimal number and the character that ends it, returned in D.
   * The number is kept on the stack between reads, not in a register.
   */
  asm("cp_num:  ldi  0            ; push the number, zero so far");
  asm("         stxd");
  asm("cp_dig:  call f_read       ; read a character");
  asm("         smi  '0'          ; anything but a digit ends the number");
  asm("         lbnf cp_ret");
  asm("         smi  10");
  asm("         lbdf cp_ret");
  asm("         adi  10           ; D is the value of the digit");
  asm("         plo  re");
  asm("         irx               ; multiply the number by ten");
  asm("         ldx");
  asm("         shl");
  asm("         shl");
  asm("         add");
  asm("         shl");
  asm("         str  r2");
  asm("         glo  re           ; add the digit");
  asm("         add");
  asm("         stxd              ; and push the number again");
  asm("         lbr  cp_dig");
  asm("cp_ret:  irx               ; pop the number");
  asm("         ldx");
  asm("         rtn");
  asm("cp_end:");
}
