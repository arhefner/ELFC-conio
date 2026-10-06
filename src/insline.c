#define _ELFCLIB_
#include "bios.h"

/*
 * Insert a blank line at the cursor, the lines below move down.
 */
void insline(void) {
  asm("         ldi  27           ; send insert line");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  'L'");
  asm("         call f_type");
}
