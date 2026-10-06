/*
 * demo.c - demonstration of the conio library
 *
 * Build for Elf/OS with:   make
 * Build for ELF-DOS with:  make TARGET_OS=elfdos
 */
#include <stdio.h>
#include "../include/conio.h"

int main(void) {
  char line[20];
  char *name;
  int color, x, y, ch;

  clrscr();
  textattr((BLUE << 4) | YELLOW);
  gotoxy(30, 2);
  cputs(" ElfC conio demo ");
  normvideo();

  /* the sixteen text colors */
  for (color = BLACK; color <= WHITE; color++) {
    gotoxy(5 + (color & 7) * 9, 5 + (color >> 3));
    textcolor(color);
    cprintf("color %d", color);
  }
  normvideo();

  /* the eight background colors */
  gotoxy(5, 8);
  for (color = BLACK; color <= LIGHTGRAY; color++) {
    textbackground(color);
    cputs("    ");
  }
  normvideo();

  /* text attributes */
  gotoxy(45, 8);
  textstyle(A_UNDERLINE);
  cputs("underline");
  textstyle(A_NOUNDERLINE);
  putch(' ');
  textstyle(A_REVERSE);
  cputs("reverse");
  textstyle(A_NORMAL);

  /* line input */
  gotoxy(5, 10);
  cputs("What is your name? ");
  line[0] = 18;
  name = cgets(line);
  gotoxy(5, 11);
  highvideo();
  cprintf("Hello, %s!", name);
  lowvideo();
  cprintf(" (%d characters)", line[1]);

  /* cursor position */
  x = wherex();
  y = wherey();
  gotoxy(5, 12);
  cprintf("The cursor was at column %d of row %d.", x, y);

  /* character input */
  gotoxy(5, 14);
  cputs("Press a key: ");
  ch = getche();
  gotoxy(5, 15);
  cprintf("That was character %d.", ch);
  clreol();

  gotoxy(5, 17);
  _setcursortype(_NOCURSOR);
  cputs("The cursor is hidden, press a key to finish.");
  getch();
  _setcursortype(_NORMALCURSOR);

  gotoxy(1, 19);
  return 0;
}
