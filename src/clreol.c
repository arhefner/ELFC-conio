#define _ELFCLIB_
#include "bios.h"

/*
 * Clear from the cursor to the end of the line.
 */
void clreol(void) {
  asm("         ldi  27           ; send erase in line");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  'K'");
  asm("         call f_type");
}
