#define _ELFCLIB_
#include "bios.h"

/*
 * Send a string to the console.  A newline is not added, and a line
 * feed in the string is not expanded to a carriage return and line feed.
 * Returns 0.
 */
int cputs(const char *s) {
  asm("         gosub s_lget16    ; get string pointer");
  asm("           dw  0           ; from arg 1");
  asm("         glo  ra           ; send nothing for a null pointer");
  asm("         lbnz cps_msg");
  asm("         ghi  ra");
  asm("         lbz  cps_end");
  asm("cps_msg: copy ra, rf       ; set string pointer");
  asm("         call f_msg        ; send string to the terminal");
  asm("         ldi  0            ; set the result");
  asm("         plo  ra");
  asm("         phi  ra");
  asm("cps_end:");
}
