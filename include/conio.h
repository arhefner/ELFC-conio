#ifndef _CONIO_
#define _CONIO_

/*
 * conio.h - console I/O library for ElfC
 *
 * The functions in this library call the BIOS console routines f_type,
 * f_read and f_msg directly, and make no operating system calls, so a
 * program that uses them runs under both Elf/OS and ELF-DOS.  Because
 * the kernel is bypassed, console I/O through this library is never
 * redirected to or from a file.
 *
 * The screen is controlled with standard ANSI (ECMA-48) escape
 * sequences, so an ANSI or VT100 compatible terminal is required.
 *
 * Including this file in a program makes the linker search conio.lib,
 * which has to be in the lib folder of ElfC or in the current folder.
 */
#ifndef _ELFCLIB_
#pragma .link .library conio.lib
#endif

/* text colors for textcolor, textbackground and textattr */
#define BLACK         0
#define BLUE          1
#define GREEN         2
#define CYAN          3
#define RED           4
#define MAGENTA       5
#define BROWN         6
#define LIGHTGRAY     7
#define DARKGRAY      8
#define LIGHTBLUE     9
#define LIGHTGREEN    10
#define LIGHTCYAN     11
#define LIGHTRED      12
#define LIGHTMAGENTA  13
#define YELLOW        14
#define WHITE         15

/* add to a text color to make the text blink */
#define BLINK         128

/*
 * text attributes for textstyle, these are the ANSI select graphic
 * rendition numbers.  Not every terminal shows every attribute.
 * A_NORMAL turns all the attributes off and sets the default colors.
 */
#define A_NORMAL        0
#define A_BOLD          1
#define A_DIM           2
#define A_ITALIC        3
#define A_UNDERLINE     4
#define A_BLINK         5
#define A_REVERSE       7
#define A_HIDDEN        8
#define A_STRIKE        9

/*
 * text attributes for textstyle that turn an attribute off, bold and
 * dim are turned off together
 */
#define A_NOBOLD        22
#define A_NODIM         22
#define A_NOITALIC      23
#define A_NOUNDERLINE   24
#define A_NOBLINK       25
#define A_NOREVERSE     27
#define A_NOHIDDEN      28
#define A_NOSTRIKE      29

/* cursor types for _setcursortype */
#define _NOCURSOR      0
#define _SOLIDCURSOR   1
#define _NORMALCURSOR  2

/* size of the buffer cprintf formats its output in */
#ifndef CPRINTF_MAX
#define CPRINTF_MAX   128
#endif

/*
 * stdio.h defines getch and putch as macros for the _getch and _putch
 * functions of the operating system, unless they are defined already.
 * They are defined here as the functions of this library instead, so
 * it does not matter which of the two files is included first.
 */
#ifdef getch
#undef getch
#endif
#define getch _cgetch

#ifdef putch
#undef putch
#endif
#define putch _cputch

/* character input and output */
int getch(void);
int getche(void);
int putch(int ch);
int cputs(const char *s);
char *cgets(char *buf);
int cprintf(const char *fmt, ...);

/* screen and cursor control */
void clrscr(void);
void clreol(void);
void gotoxy(int x, int y);
int wherex(void);
int wherey(void);
void insline(void);
void delline(void);
void _setcursortype(int type);

/* text attributes */
void textcolor(int color);
void textbackground(int color);
void textattr(int attr);
void textstyle(int attr);
void highvideo(void);
void lowvideo(void);
void normvideo(void);

/* support function, returns the cursor row * 256 + the cursor column */
int _curpos(void);

#endif
