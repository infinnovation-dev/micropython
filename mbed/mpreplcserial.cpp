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

// mbed C serial_t client for Micropython REPL

#include "mprepl.h"

#ifndef LOGF
#define LOGF(...)
#endif

static void
_mprepl_serial_output(void *handle, const char *str, size_t len) {
    serial_t *serial = (serial_t *)handle;
    while (len-- > 0) {
        char c = *str++;
        while (! serial_writable(serial)) {
            Thread::yield();
        }
        serial_putc(serial, (int)c);
    }
}

static void
_mprepl_serial_input(serial_t *serial) {
    while (true) {
        while (! serial_readable(serial)) {
            Thread::yield();
        }
        int c = serial_getc(serial);
        mprepl_input_char(c);
    }
}

void
mprepl_add_serial(serial_t *serial) {
    mprepl_add_client(_mprepl_serial_output, serial);
    Thread *serial_thread = new Thread();
    serial_thread->start(Callback<void()>(serial, _mprepl_serial_input));
}
