#ifndef _CONIO_BIOS_
#define _CONIO_BIOS_

/*
 * BIOS interface for the conio library.
 *
 * The asm() statements in this library call the BIOS console routines
 * by name, using the BIOS include file in the folder above this one.
 * The path is relative to this folder, which is where the library is
 * built.
 *
 * Only f_type, f_read and f_msg are used, and they are always reached
 * with CALL, never with a branch:
 *
 *   f_type   D = character to send
 *   f_read   returns D = character read, echoed if bit 0 of RE.1 is set
 *   f_msg    RF = pointer to a zero terminated string to send
 *
 * Nothing is kept in RA, RC, RD or RF across a call.  A value that is
 * needed afterwards is kept on the stack or read from the argument again.
 */
#pragma #include ../bios.inc

#endif
