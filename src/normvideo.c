#define _ELFCLIB_
#include "bios.h"

/*
 * Set the colors and all other text attributes back to the defaults
 * of the terminal.
 */
void normvideo(void) {
  asm("         ldi  27           ; send attributes off");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  '0'");
  asm("         call f_type");
  asm("         ldi  'm'");
  asm("         call f_type");
}
