#define _ELFCLIB_
#include "../include/conio.h"

#pragma             extrn C_cputch
#pragma             extrn Ccputs
#pragma             extrn Cgetkey

/*
 * A line editor with a history, a C version of lib/lineedit.asm of the
 * ELF-DOS SDK.  The editing is the same: the line is kept in the
 * caller's buffer, characters are inserted at the cursor, and after an
 * insert or a delete the rest of the line is printed again and the
 * terminal cursor is moved back with backspaces.  Added to it are a
 * history of lines, Ctrl-K, Ctrl-U, Ctrl-C, DEL as a backspace, and the
 * other sequences terminals send for Home and End, which getkey knows.
 *
 * The keys are read with getkey, which reads the whole escape sequence
 * of an arrow key before it returns, so its bytes are not dropped.
 */

#define CTRL_A  1
#define CTRL_B  2
#define CTRL_C  3
#define CTRL_D  4
#define CTRL_E  5
#define CTRL_F  6
#define CH_BS   8
#define CH_LF   10
#define CTRL_K  11
#define CH_CR   13
#define CTRL_N  14
#define CTRL_P  16
#define CTRL_U  21
#define CH_DEL  127

/* the line being edited: the caller's buffer, its length and cursor */
static char *_cgl_buf;
static int _cgl_max;
static int _cgl_len;
static int _cgl_cur;

/*
 * The history is the lines one after the other, each ended by a zero,
 * the oldest first, in the room the program gave cgethistory.
 * _cgl_hused is the number of bytes they take and _cgl_hcount the number
 * of lines.  _cgl_hpos is the line being shown while the user goes up
 * and down, _cgl_hcount for the line that is being typed, which is kept
 * in _cgl_save meanwhile.
 */
static char *_cgl_hist;
static int _cgl_hsize;
static int _cgl_hused;
static int _cgl_hcount;
static int _cgl_hpos;
static char _cgl_save[CGETLINE_MAX + 1];

/* send n backspaces, which move the cursor left */
static void cgl_back(int n) {
  while (n-- > 0)
    putch(CH_BS);
}

/*
 * Print the line from position from to the end and then blanks spaces,
 * to erase what was left after the end, and move the terminal cursor
 * back to _cgl_cur.  The terminal cursor has to be at from.
 */
static void cgl_show(int from, int blanks) {
  int i;

  for (i = from; i < _cgl_len; i++)
    putch(_cgl_buf[i]);
  for (i = 0; i < blanks; i++)
    putch(' ');
  cgl_back(_cgl_len + blanks - _cgl_cur);
}

/* insert a character at the cursor, if there is room */
static void cgl_insert(int c) {
  int i;

  if (_cgl_len >= _cgl_max)
    return;
  for (i = _cgl_len; i > _cgl_cur; i--)
    _cgl_buf[i] = _cgl_buf[i - 1];
  _cgl_buf[_cgl_cur] = c;
  _cgl_len++;
  _cgl_cur++;
  cgl_show(_cgl_cur - 1, 0);
}

/* delete n characters at the cursor, n is no more than are there */
static void cgl_delete(int n) {
  int i;

  if (n <= 0)
    return;
  for (i = _cgl_cur; i + n < _cgl_len; i++)
    _cgl_buf[i] = _cgl_buf[i + n];
  _cgl_len -= n;
  cgl_show(_cgl_cur, n);
}

static void cgl_left(void) {
  if (_cgl_cur > 0) {
    _cgl_cur--;
    putch(CH_BS);
  }
}

static void cgl_right(void) {
  if (_cgl_cur < _cgl_len)
    putch(_cgl_buf[_cgl_cur++]);
}

static void cgl_home(void) {
  cgl_back(_cgl_cur);
  _cgl_cur = 0;
}

static void cgl_end(void) {
  while (_cgl_cur < _cgl_len)
    putch(_cgl_buf[_cgl_cur++]);
}

/* put a line in place of the one on the screen, with the cursor at the end */
static void cgl_replace(char *s) {
  int old;

  old = _cgl_len;
  cgl_home();
  _cgl_len = 0;
  while (*s && _cgl_len < _cgl_max)
    _cgl_buf[_cgl_len++] = *s++;
  _cgl_cur = _cgl_len;
  cgl_show(0, old > _cgl_len ? old - _cgl_len : 0);
}

/* the start of a line of the history, 0 is the oldest */
static char *cgl_hline(int n) {
  char *s;

  s = _cgl_hist;
  while (n-- > 0) {
    while (*s)
      s++;
    s++;
  }
  return s;
}

/* copy the line being typed to s */
static void cgl_copy(char *s) {
  int i;

  for (i = 0; i < _cgl_len; i++)
    s[i] = _cgl_buf[i];
  s[i] = 0;
}

/* show the line before (step -1) or after (step 1) in the history */
static void cgl_recall(int step) {
  int pos;

  pos = _cgl_hpos + step;
  if (pos < 0 || pos > _cgl_hcount)
    return;
  if (_cgl_hpos == _cgl_hcount)
    cgl_copy(_cgl_save);
  _cgl_hpos = pos;
  if (pos == _cgl_hcount)
    cgl_replace(_cgl_save);
  else
    cgl_replace(cgl_hline(pos));
}

/* drop the oldest line of the history */
static void cgl_hdrop(void) {
  char *s;
  int n, i;

  s = cgl_hline(1);
  n = s - _cgl_hist;
  for (i = n; i < _cgl_hused; i++)
    _cgl_hist[i - n] = _cgl_hist[i];
  _cgl_hused -= n;
  _cgl_hcount--;
}

/* add the line that was typed to the history */
static void cgl_hadd(void) {
  char *s;
  int i;

  if (_cgl_len == 0 || _cgl_len + 1 > _cgl_hsize)
    return;

  /* not if it is the same as the last line */
  if (_cgl_hcount > 0) {
    s = cgl_hline(_cgl_hcount - 1);
    for (i = 0; i < _cgl_len && s[i] == _cgl_buf[i]; i++)
      ;
    if (i == _cgl_len && s[i] == 0)
      return;
  }

  while (_cgl_hused + _cgl_len + 1 > _cgl_hsize)
    cgl_hdrop();
  cgl_copy(_cgl_hist + _cgl_hused);
  _cgl_hused += _cgl_len + 1;
  _cgl_hcount++;
}

/*
 * Give cgetline room for a history, of size bytes at hist.  With a size
 * of 0 there is no history, which is how it starts.
 */
void cgethistory(char *hist, int size) {
  _cgl_hist = hist;
  _cgl_hsize = size > 0 ? size : 0;
  _cgl_hused = 0;
  _cgl_hcount = 0;
}

/*
 * Read and edit a line of up to size - 1 characters, at most
 * CGETLINE_MAX, into buf.  Returns its length, or CGETLINE_CANCEL for
 * Ctrl-C, and the line is empty then.
 */
int cgetline(char *buf, int size) {
  int c;

  _cgl_buf = buf;
  _cgl_max = size - 1;
  if (_cgl_max > CGETLINE_MAX)
    _cgl_max = CGETLINE_MAX;
  _cgl_len = 0;
  _cgl_cur = 0;
  _cgl_hpos = _cgl_hcount;

  for (;;) {
    c = getkey();

    if (c == CH_CR || c == CH_LF)
      break;

    switch (c) {
    case CTRL_C:
      cputs("^C\r\n");
      buf[0] = 0;
      return CGETLINE_CANCEL;
    case CH_BS:
    case CH_DEL:
      if (_cgl_cur > 0) {
        cgl_left();
        cgl_delete(1);
      }
      break;
    case KEY_DELETE:
    case CTRL_D:
      if (_cgl_cur < _cgl_len)
        cgl_delete(1);
      break;
    case CTRL_K:
      cgl_delete(_cgl_len - _cgl_cur);
      break;
    case CTRL_U:
      cgl_home();
      cgl_delete(_cgl_len);
      break;
    case KEY_HOME:
    case CTRL_A:
      cgl_home();
      break;
    case KEY_END:
    case CTRL_E:
      cgl_end();
      break;
    case KEY_LEFT:
    case CTRL_B:
      cgl_left();
      break;
    case KEY_RIGHT:
    case CTRL_F:
      cgl_right();
      break;
    case KEY_UP:
    case CTRL_P:
      cgl_recall(-1);
      break;
    case KEY_DOWN:
    case CTRL_N:
      cgl_recall(1);
      break;
    default:
      if (c >= ' ' && c < CH_DEL)
        cgl_insert(c);
      break;
    }
  }

  buf[_cgl_len] = 0;
  cputs("\r\n");
  if (_cgl_hsize > 0)
    cgl_hadd();
  return _cgl_len;
}
