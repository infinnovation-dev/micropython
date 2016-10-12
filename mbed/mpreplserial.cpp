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

// mbed Serial client for Micropython REPL

#include "mprepl.h"

#ifndef LOGF
#define LOGF(...)
#endif

static void
_mprepl_Serial_output(void *handle, const char *str, size_t len) {
    Serial *serial = (Serial *)handle;
    if (len == 1) {
        LOGF("_mprepl_Serial_output: Serial=%p char %02x\r\n", handle, str[0]);
    } else {
        LOGF("_mprepl_Serial_output: %d bytes\r\n", (int)len);
    }
    while (len--) {
        while (! serial->writeable()) {
            Thread::yield();
        }
        serial->putc(*str++);
    }
    LOGF("_mprepl_Serial_output done\r\n");
}

static void
_mprepl_Serial_input(Serial *serial) {
    LOGF("_mprepl_Serial_input\r\n");
    while (true) {
        while (! serial->readable()) {
            Thread::yield();
        }
        int c = serial->getc();
        LOGF("mprepl: char 0x%02x\r\n", c);
        mprepl_input_char(c);
    }
}

void
mprepl_add_Serial(Serial &serial) {
    mprepl_add_client(_mprepl_Serial_output, &serial);
    Thread *serial_thread = new Thread();
    serial_thread->start(Callback<void()>(&serial, _mprepl_Serial_input));
}
