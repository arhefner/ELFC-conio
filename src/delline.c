#define _ELFCLIB_
#include "bios.h"

/*
 * Delete the line at the cursor, the lines below move up.
 */
void delline(void) {
  asm("         ldi  27           ; send delete line");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  'M'");
  asm("         call f_type");
}
