#define _ELFCLIB_
#include "bios.h"

/*
 * Read a character from the console and echo it.
 * Returns the character read.
 */
int getche(void) {
  asm("         ghi  re           ; save the baud rate and echo flag");
  asm("         stxd");
  asm("         ori  1            ; turn on the echo");
  asm("         phi  re");
  asm("         call f_read       ; read a character from input");
  asm("         plo  ra           ; save in return register");
  asm("         ldi  0            ; pad register with zero");
  asm("         phi  ra");
  asm("         irx               ; restore the echo flag");
  asm("         ldx");
  asm("         phi  re");
}
