#define _ELFCLIB_
#include "../include/conio.h"

#pragma             extrn C_cgetch
#pragma             extrn C_cputch

/*
 * Read a line from the console, with echo, until Enter is typed.
 * Backspace and Delete remove the last character typed.
 *
 * buf[0] must hold the size of the string buffer that starts at buf[2],
 * which has to include room for the zero at the end of the string.  On
 * return buf[1] holds the number of characters read.  Neither the Enter
 * key nor a newline is echoed or stored.
 * Returns a pointer to the string, which is buf + 2.
 */
char *cgets(char *buf) {
  char *s;
  int max, len, ch;

  s = buf + 2;
  max = (buf[0] & 255) - 1;
  len = 0;

  while ((ch = getch()) != '\r' && ch != '\n') {
    if (ch == '\b' || ch == 127) {
      if (len > 0) {
        len--;
        putch('\b');
        putch(' ');
        putch('\b');
      }
    } else if (ch >= ' ' && len < max) {
      s[len++] = ch;
      putch(ch);
    }
  }

  s[len] = 0;
  buf[1] = len;
  return s;
}
