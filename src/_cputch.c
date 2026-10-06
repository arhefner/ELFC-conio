#define _ELFCLIB_
#include "bios.h"

/*
 * Send a character to the console.  This is the putch function,
 * conio.h defines that name as a macro for this one.
 * Returns the character sent.
 */
int _cputch(int ch) {
  asm("         gosub s_lget16    ; get character to send");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; ra holds character to send");
  asm("         call f_type       ; send character to the terminal");
  asm("         gosub s_lget16    ; get the character again");
  asm("           dw  0           ; for the result");
}
