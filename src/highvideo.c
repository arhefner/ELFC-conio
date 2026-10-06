#define _ELFCLIB_
#include "bios.h"

/*
 * Show the text that follows in high intensity.
 */
void highvideo(void) {
  asm("         ldi  27           ; send bold");
  asm("         call f_type");
  asm("         ldi  '['");
  asm("         call f_type");
  asm("         ldi  '1'");
  asm("         call f_type");
  asm("         ldi  'm'");
  asm("         call f_type");
}
