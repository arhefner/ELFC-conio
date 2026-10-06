# ELFC-conio

A `conio` console I/O library for the [ElfC](https://github.com/arhefner/ELFC)
C compiler for the CDP1802.

The library calls the BIOS console routines `f_type`, `f_read` and `f_msg`
directly and makes no operating system calls, so the same `conio.lib` works
in programs for both Elf/OS and ELF-DOS. The screen is controlled with
standard ANSI escape sequences, so an ANSI or VT100 compatible terminal is
required.

The functions that do console I/O are written in inline assembly. Only
`cgets`, `cprintf`, `wherex` and `wherey` are written in C, on top of those.

## Building

ElfC has to be on the path.

```
make                 build lib/conio.lib
make examples        build the example program for Elf/OS
make install         copy conio.lib and conio.h into ElfC (default /opt/elfc)
make clean           remove the generated files
```

Use `make install ELFC_DIR=folder` if ElfC is installed somewhere else, and
`make -C examples TARGET_OS=elfdos` to build the example for ELF-DOS.

## Using the library

Include `conio.h` in the program. The header file tells the linker to search
`conio.lib`, so no linker option is needed:

```c
#include <conio.h>

int main(void) {
  clrscr();
  gotoxy(10, 5);
  textcolor(YELLOW);
  cputs("Hello from conio");
  normvideo();
  getch();
  return 0;
}
```

```
elfc hello.c         for Elf/OS
elfc -E hello.c      for ELF-DOS
```

The linker looks for `conio.lib` in the `lib` folder of ElfC and in the
current folder. Without `make install`, copy `lib/conio.lib` next to the
program and include the header file by its path.

## Functions

| Function | Description |
|----------|-------------|
| `int getch(void)` | Read a character without echo. |
| `int getche(void)` | Read a character and echo it. |
| `int putch(int ch)` | Send a character, returns the character. |
| `int cputs(const char *s)` | Send a string, returns 0. |
| `char *cgets(char *buf)` | Read a line with echo. `buf[0]` holds the size of the string buffer at `buf + 2`, `buf[1]` is set to the length read. Returns `buf + 2`. |
| `int cprintf(const char *fmt, ...)` | Send formatted output, at most `CPRINTF_MAX` (128) characters. |
| `void clrscr(void)` | Clear the screen and home the cursor. |
| `void clreol(void)` | Clear from the cursor to the end of the line. |
| `void gotoxy(int x, int y)` | Move the cursor to column `x` of row `y`, both start at 1. |
| `int wherex(void)` | Column of the cursor. |
| `int wherey(void)` | Row of the cursor. |
| `void insline(void)` | Insert a blank line at the cursor. |
| `void delline(void)` | Delete the line at the cursor. |
| `void _setcursortype(int type)` | Hide the cursor with `_NOCURSOR`, show it with `_NORMALCURSOR` or `_SOLIDCURSOR`. |
| `void textcolor(int color)` | Set the text color, `BLACK` to `WHITE`, optionally plus `BLINK`. |
| `void textbackground(int color)` | Set the background color, `BLACK` to `LIGHTGRAY`. |
| `void textattr(int attr)` | Set both colors and blinking: `(background << 4) \| color`. |
| `void textstyle(int attr)` | Turn one text attribute on or off: `A_BOLD`, `A_DIM`, `A_ITALIC`, `A_UNDERLINE`, `A_BLINK`, `A_REVERSE`, `A_HIDDEN`, `A_STRIKE`, the same names with `A_NO` in front to turn one off, or `A_NORMAL` to reset everything. |
| `void highvideo(void)` | High intensity text. |
| `void lowvideo(void)` | Normal intensity text. |
| `void normvideo(void)` | Reset the colors and attributes to the terminal defaults. |

## Notes

* Console I/O through this library goes straight to the BIOS, so it is not
  redirected to or from a file by the operating system.
* `stdio.h` defines `getch` and `putch` as macros for the `_getch` and
  `_putch` functions of the operating system. `conio.h` defines them as
  macros for the functions of this library instead, `_cgetch` and `_cputch`,
  whichever of the two files is included first.
* `getch` and `getche` set the echo themselves, with the echo flag in bit 0
  of `RE.1`, and put it back afterwards, so they behave the same whether
  the operating system leaves the echo on or off.
* A line feed is not expanded to a carriage return and line feed. To start
  a new line with `cputs` and `cprintf`, send `"\n"` if your terminal adds
  a carriage return automatically, otherwise send `"\r\n"`.
* The colors `DARKGRAY` to `WHITE` are sent as bold with the colors `BLACK`
  to `LIGHTGRAY`, which most terminals show as the bright color.
* `textcolor` turns blinking on with `BLINK` but does not turn it off, that
  is done by `textattr`, `normvideo` and `textstyle(A_NOBLINK)`.
* `textstyle` is not a Borland function. It leaves the colors and the other
  attributes alone, but `textattr` and `normvideo` turn off everything it
  set, and `textcolor` sets the intensity, so call it after those.
* `wherex` and `wherey` ask the terminal for the cursor position and wait
  for the answer. They do not return on a terminal that does not answer a
  cursor position request (`ESC [ 6 n`), and the answer can be lost on a
  bit-banged serial port.
* `cgets` does not echo the Enter key, so the cursor stays at the end of the
  line that was typed.
* There is no `kbhit`, because the three BIOS routines the library is
  limited to cannot test for a key without waiting for one.
* The BIOS routines are only ever reached with `CALL`, with the character in
  `D` for `f_type` and the string pointer in `RF` for `f_msg`. Nothing is
  kept in `RA`, `RC`, `RD` or `RF` across a BIOS call. The library does rely
  on the BIOS to preserve the registers ElfC reserves, `R7`, `R9` and `RB`,
  as the ElfC standard library for Elf/OS does.
