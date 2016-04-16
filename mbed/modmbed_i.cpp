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

/**
 * Shims to isolate the pure C world of micropython from the C++ world
 * of mbed high-level objects.  Maybe there's a way to write the module
 * directly in a compatible way but my C++-fu is not that strong.
 */
extern "C" {
  #include "py/mpconfig.h"
}

#if MICROPY_PY_MBED

#include "mbed.h"
extern "C" {
  #include "modmbed_i.h"
}

void *mbed_DigitalOut__create(int pin)
{
  DigitalOut *dout = new DigitalOut((PinName)pin);
  return (void *)dout;
}

void mbed_DigitalOut__write(void *self, int value)
{
  DigitalOut *dout = (DigitalOut *)self;
  *dout = value;
}

#endif
