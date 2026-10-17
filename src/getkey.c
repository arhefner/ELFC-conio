#define _ELFCLIB_
#include "bios.h"
#include "../include/conio.h"

/* room for the bytes after an ESC */
#define GK_SEQ  8

/* the bytes after an ESC, ended by a zero */
static char _gk_seq[GK_SEQ];

/* a key read after an ESC that was not part of a sequence */
static int _gk_have;
static int _gk_key;

/*
 * Read a key without echo and return it.  When the key is ESC, the
 * bytes after it are read too, without a pause, and put in seq, ended
 * by a zero: the [ or O and, after [, the bytes of an ANSI sequence up
 * to its final byte, 40h to 7Eh, or GK_SEQ - 2 of them, and after O
 * the one byte that follows it.
 *
 * The bytes of a key like an arrow arrive one right after the other,
 * and on a real 1802 the UART or the bit-banged serial port drops the
 * next byte unless the BIOS is called again almost at once.  Going
 * back to C between the bytes is too slow, so they are read here.
 *
 * As in the rest of the library nothing is kept in a register across a
 * BIOS call: the pointer into seq is kept on the stack, above the
 * saved echo flag.
 */
static int _gk_read(char *seq) {
  asm("         ghi  re           ; save the baud rate and echo flag");
  asm("         stxd");
  asm("         ani  0feh         ; turn off the echo");
  asm("         phi  re");
  asm("         gosub s_lget16    ; get seq");
  asm("           dw  0           ; from arg 1");
  asm("         sex  r2");
  asm("         glo  ra           ; and keep it on the stack");
  asm("         stxd");
  asm("         ghi  ra");
  asm("         stxd");
  asm("         call f_read       ; read a key");
  asm("         plo  ra           ; it is the value returned");
  asm("         ldi  0");
  asm("         phi  ra");
  asm("         glo  ra");
  asm("         xri  27           ; done unless it is ESC");
  asm("         lbnz gk_done");
  asm("         call f_read       ; the byte after the ESC");
  asm("         plo  rc");
  asm("         irx               ; put it in seq");
  asm("         ldxa");
  asm("         phi  rd");
  asm("         ldx");
  asm("         plo  rd");
  asm("         glo  rc");
  asm("         str  rd");
  asm("         inc  rd");
  asm("         glo  rd");
  asm("         stxd");
  asm("         ghi  rd");
  asm("         stxd");
  asm("         glo  rc");
  asm("         xri  5bh          ; [ starts an ANSI sequence");
  asm("         lbz  gk_csi");
  asm("         glo  rc");
  asm("         xri  4fh          ; O has one byte after it");
  asm("         lbnz gk_end");
  asm("         call f_read");
  asm("         plo  rc");
  asm("         irx");
  asm("         ldxa");
  asm("         phi  rd");
  asm("         ldx");
  asm("         plo  rd");
  asm("         glo  rc");
  asm("         str  rd");
  asm("         inc  rd");
  asm("         glo  rd");
  asm("         stxd");
  asm("         ghi  rd");
  asm("         stxd");
  asm("         lbr  gk_end");
  asm("gk_csi:  ldi  6            ; GK_SEQ - 2 bytes at most");
  asm("         plo  rc");
  asm("gk_loop: glo  rc           ; keep the count on the stack too");
  asm("         stxd");
  asm("         call f_read       ; read up to the final byte");
  asm("         plo  re");
  asm("         irx");
  asm("         ldxa");
  asm("         plo  rc");
  asm("         ldxa");
  asm("         phi  rd");
  asm("         ldx");
  asm("         plo  rd");
  asm("         glo  re");
  asm("         str  rd");
  asm("         inc  rd");
  asm("         glo  rd");
  asm("         stxd");
  asm("         ghi  rd");
  asm("         stxd");
  asm("         glo  re");
  asm("         smi  40h          ; 40h and up is the final byte");
  asm("         lbdf gk_end");
  asm("         dec  rc");
  asm("         glo  rc");
  asm("         lbnz gk_loop");
  asm("gk_end:  irx               ; end the bytes with a zero");
  asm("         ldxa");
  asm("         phi  rd");
  asm("         ldx");
  asm("         plo  rd");
  asm("         ldi  0");
  asm("         str  rd");
  asm("         lbr  gk_echo");
  asm("gk_done: irx               ; drop the pointer");
  asm("         irx");
  asm("gk_echo: irx               ; restore the echo flag");
  asm("         ldx");
  asm("         phi  re");
}

/* the key for a sequence that ends in a letter, or 0 */
static int _gk_letter(int c) {
  switch (c) {
  case 'A':
    return KEY_UP;
  case 'B':
    return KEY_DOWN;
  case 'C':
    return KEY_RIGHT;
  case 'D':
    return KEY_LEFT;
  case 'H':
    return KEY_HOME;
  case 'F':
    return KEY_END;
  case 'P':
    return KEY_F1;
  case 'Q':
    return KEY_F2;
  case 'R':
    return KEY_F3;
  case 'S':
    return KEY_F4;
  }
  return 0;
}

/* the key for a sequence ESC [ n ~, or 0 */
static int _gk_tilde(int n) {
  if (n == 1 || n == 7)
    return KEY_HOME;
  if (n == 4 || n == 8)
    return KEY_END;
  if (n == 2)
    return KEY_INSERT;
  if (n == 3)
    return KEY_DELETE;
  if (n == 5)
    return KEY_PGUP;
  if (n == 6)
    return KEY_PGDN;
  if (n >= 11 && n <= 15)
    return KEY_F1 + n - 11;
  if (n >= 17 && n <= 21)
    return KEY_F6 + n - 17;
  if (n == 23 || n == 24)
    return KEY_F11 + n - 23;
  return 0;
}

/*
 * Read a key without echo.  Returns the character, or one of the KEY_
 * values for a key that sends an ANSI or VT100 escape sequence.  A
 * sequence for a key that has no KEY_ value is skipped.
 */
int getkey(void) {
  char *s;
  int c, n, key;

  for (;;) {
    if (_gk_have) {
      _gk_have = 0;
      return _gk_key;
    }

    c = _gk_read(_gk_seq);
    if (c != 27)
      return c;

    /* ESC and a key that does not start a sequence are two keys */
    s = _gk_seq;
    c = *s++;
    if (c != '[' && c != 'O') {
      _gk_have = 1;
      _gk_key = c;
      return 27;
    }

    if (c == 'O') {
      key = _gk_letter(*s);
    } else {
      /* the first number, then any more after a ; are skipped */
      n = 0;
      c = *s++;
      while (c >= '0' && c <= '9') {
        n = n * 10 + c - '0';
        c = *s++;
      }
      while (c == ';' || (c >= '0' && c <= '9'))
        c = *s++;
      key = c == '~' ? _gk_tilde(n) : _gk_letter(c);
    }

    if (key)
      return key;
  }
}
