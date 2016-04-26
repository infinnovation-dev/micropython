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
#ifndef __MICROPY_INCLUDED_MODMBED_I_H
#define __MICROPY_INCLUDED_MODMBED_I_H

extern const mp_obj_type_t mbed_DigitalOut_type;
extern mp_obj_t mbed_DigitalOut_make_new(const mp_obj_type_t *, mp_uint_t, mp_uint_t, const mp_obj_t *);
extern mp_obj_t mbed_DigitalOut_write(mp_obj_t self_in, mp_obj_t value_in);

extern const mp_obj_type_t mbed_Serial_type;
extern mp_obj_t mbed_Serial_make_new(const mp_obj_type_t *type,
                                     mp_uint_t n_args, mp_uint_t n_kw,
                                     const mp_obj_t *args);
extern mp_obj_t mbed_Serial_baud(mp_obj_t self_in, mp_obj_t baud_in);
extern mp_obj_t mbed_Serial_putc(mp_obj_t self_in, mp_obj_t chr_in);
extern mp_obj_t mbed_Serial_puts(mp_obj_t self_in, mp_obj_t str_in);
extern mp_obj_t mbed_Serial_getc(mp_obj_t self_in);

#endif // __MICROPY_INCLUDED_MODMBED_I_H
