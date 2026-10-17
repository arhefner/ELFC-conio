# ELFC-conio

A `conio` console I/O library for the [ElfC](https://github.com/arhefner/ELFC)
C compiler for the CDP1802.

The library calls the BIOS console routines `f_type`, `f_read` and `f_msg`
directly and makes no operating system calls, so the same `conio.lib` works
in programs for both Elf/OS and ELF-DOS. The screen is controlled with
standard ANSI escape sequences, so an ANSI or VT100 compatible terminal is
required.

The functions that do console I/O are written in inline assembly. Only
`cgets`, `cgetline`, `cprintf`, `wherex` and `wherey` and the decoding part of
`getkey` are written in C, on top of those.

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
| `int getkey(void)` | Read a key without echo. Returns the character, or a `KEY_` value for a key that sends an escape sequence, see below. |
| `int cgetline(char *buf, int size)` | Read and edit a line of up to `size - 1` characters, at most `CGETLINE_MAX` (255), see below. Returns its length, or `CGETLINE_CANCEL` for Ctrl-C. |
| `void cgethistory(char *hist, int size)` | Give `cgetline` `size` bytes at `hist` for a history of lines. |
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

## Reading keys

`getkey` returns these values for the keys that send ANSI or VT100 escape
sequences, and the character for every other key:

| Value | Key | Sequences |
|-------|-----|-----------|
| `KEY_UP`, `KEY_DOWN`, `KEY_RIGHT`, `KEY_LEFT` | Arrows | `ESC [ A` to `D`, `ESC O A` to `D` |
| `KEY_HOME` | Home | `ESC [ H`, `ESC O H`, `ESC [ 1 ~`, `ESC [ 7 ~` |
| `KEY_END` | End | `ESC [ F`, `ESC O F`, `ESC [ 4 ~`, `ESC [ 8 ~` |
| `KEY_INSERT`, `KEY_DELETE` | Insert, Delete | `ESC [ 2 ~`, `ESC [ 3 ~` |
| `KEY_PGUP`, `KEY_PGDN` | Page Up, Page Down | `ESC [ 5 ~`, `ESC [ 6 ~` |
| `KEY_F1` to `KEY_F4` | F1 to F4 | `ESC O P` to `S`, `ESC [ 11 ~` to `14 ~` |
| `KEY_F5` to `KEY_F12` | F5 to F12 | `ESC [ 15 ~` to `24 ~` |

The bytes of a key like an arrow arrive one right after another, and on a
real 1802 the UART or the bit-banged serial port drops the next byte unless
the BIOS is called again almost at once. Reading them with `getch` from C is
too slow, so `getkey` reads the whole sequence in assembly before it returns.
Use it, not `getch`, in a program that reads the arrow keys.

* A key with Shift, Ctrl or Alt, such as `ESC [ 1 ; 5 C` for Ctrl-Right, is
  returned as the key without them.
* A sequence for a key that has no `KEY_` value is skipped.
* ESC followed by a key that does not start a sequence is returned as two
  keys, ESC and then that key. Because the BIOS cannot test for a key without
  waiting, `getkey` does not return the ESC key until the next key is typed.
* Only the first 6 bytes after `ESC [` are read as part of the sequence, which
  is enough for every key above.

## Editing a line

`cgetline` reads a line and lets it be edited as it is typed, with a history
of the lines typed before. It is a C version of `lineedit.asm` of the ELF-DOS
SDK, with the history added.

```c
#include <conio.h>

static char line[80];
static char history[512];

  cgethistory(history, sizeof(history));    /* optional */
  ...
  if (cgetline(line, sizeof(line)) == CGETLINE_CANCEL)
    ...
```

| Key | Action |
|-----|--------|
| Left, Ctrl-B | Cursor left. |
| Right, Ctrl-F | Cursor right. |
| Home, Ctrl-A | Start of the line. |
| End, Ctrl-E | End of the line. |
| Backspace, DEL | Delete the character before the cursor. |
| Delete, Ctrl-D | Delete the character at the cursor. |
| Ctrl-K | Delete from the cursor to the end of the line. |
| Ctrl-U | Delete the whole line. |
| Up, Ctrl-P | The line before in the history. |
| Down, Ctrl-N | The line after in the history. |
| Enter | Done. |
| Ctrl-C | Give up the line. |

* The Enter key is not stored in `buf`. The cursor is moved to the start of
  the next line, with `"\r\n"`, whether the line was ended with Enter or
  Ctrl-C, and after Ctrl-C the line is empty.
* Each line that is not empty and not the same as the one before is added to
  the history, and the oldest lines are dropped to make room. A line takes its
  length and one more byte, so 512 bytes keep about 20 lines of 25
  characters. Without a call to `cgethistory` there is no history.
* The terminal has to move its cursor left for a backspace without erasing.

## Notes

* Console I/O through this library goes straight to the BIOS, so it is not
  redirected to or from a file by the operating system.
* `stdio.h` defines `getch` and `putch` as macros for the `_getch` and
  `_putch` functions of the operating system. `conio.h` defines them as
  macros for the functions of this library instead, `_cgetch` and `_cputch`,
  whichever of the two files is included first.
* `getch`, `getche` and `getkey` set the echo themselves, with the echo flag
  in bit 0 of `RE.1`, and put it back afterwards, so they behave the same
  whether the operating system leaves the echo on or off.
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
