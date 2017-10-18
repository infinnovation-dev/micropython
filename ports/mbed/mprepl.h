/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Colin Hogben
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Micropython REPL with multiple I/O channels

#ifndef __MICROPY_INCLUDED_MPREPL_H
#define __MICROPY_INCLUDED_MPREPL_H

#ifdef  __cplusplus
#include "mbed.h"
extern "C" {
#endif

#include <stdlib.h>

typedef void (mprepl_outfunc_t)(void *, const char *, size_t);

extern void mprepl_input_char(char);
extern void mprepl_reset(void);
extern void mprepl_add_client(mprepl_outfunc_t *, void *);
extern void mprepl_remove_client(mprepl_outfunc_t *, void *);
extern int mprepl_run(void);

// mbed-specific
extern void mprepl_add_serial(serial_t *);
#ifdef  __cplusplus
extern void mprepl_add_Serial(Serial &);
extern void mprepl_add_TCPSocket(TCPSocket &);
extern void mprepl_start_TCPServer(NetworkInterface *, int /*port*/);
#endif

#ifdef  __cplusplus
}
#endif

#endif // __MICROPY_INCLUDED_MPREPL_H
