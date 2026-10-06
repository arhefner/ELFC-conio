#define _ELFCLIB_
#include "bios.h"

/*
 * Read a character from the console without echoing it.  This is the
 * getch function, conio.h defines that name as a macro for this one.
 * Returns the character read.
 */
int _cgetch(void) {
  asm("         ghi  re           ; save the baud rate and echo flag");
  asm("         stxd");
  asm("         ani  0feh         ; turn off the echo");
  asm("         phi  re");
  asm("         call f_read       ; read a character from input");
  asm("         plo  ra           ; save in return register");
  asm("         ldi  0            ; pad register with zero");
  asm("         phi  ra");
  asm("         irx               ; restore the echo flag");
  asm("         ldx");
  asm("         phi  re");
}
