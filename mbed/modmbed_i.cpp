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
#include "py/runtime.h"
#include "modmbed_i.h"
}

//-----------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------
#if MICROPY_PY_BUILTINS_FLOAT
mp_obj_t mbed_wait(mp_obj_t s_in) {
    mp_float_t s = mp_obj_get_float(s_in);
    wait((float)s);
    return mp_const_none;
}
#endif

mp_obj_t mbed_wait_ms(mp_obj_t ms_in) {
    int ms = mp_obj_get_int(ms_in);
    wait_ms(ms);
    return mp_const_none;
}

mp_obj_t mbed_wait_us(mp_obj_t us_in) {
    int us = mp_obj_get_int(us_in);
    wait_us(us);
    return mp_const_none;
}

//-----------------------------------------------------------------------
// DigitalOut
//-----------------------------------------------------------------------
struct mbed_DigitalOut_obj_t {
    mp_obj_base_t base;
    DigitalOut *dout;
};

// constructor DigitalOut(pin)
mp_obj_t mbed_DigitalOut_make_new(const mp_obj_type_t *type,
                                  mp_uint_t n_args, mp_uint_t n_kw,
                                  const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    int pin = mp_obj_get_int(args[0]);
    mbed_DigitalOut_obj_t *o =
        m_new_obj_with_finaliser(mbed_DigitalOut_obj_t);
    o->base.type = &mbed_DigitalOut_type;
    o->dout = new DigitalOut((PinName)pin);
    return o;
}

mp_obj_t mbed_DigitalOut_write(mp_obj_t self_in, mp_obj_t value_in) {
    mbed_DigitalOut_obj_t *self = (mbed_DigitalOut_obj_t *)self_in;
    int value = mp_obj_get_int(value_in);
    self->dout->write(value);
    return mp_const_none;
}

mp_obj_t mbed_DigitalOut_read(mp_obj_t self_in) {
    mbed_DigitalOut_obj_t *self = (mbed_DigitalOut_obj_t *)self_in;
    int value = self->dout->read(); // 0 or 1
    return MP_OBJ_NEW_SMALL_INT(value);
}

mp_obj_t mbed_DigitalOut_is_connected(mp_obj_t self_in) {
    mbed_DigitalOut_obj_t *self = (mbed_DigitalOut_obj_t *)self_in;
    int conn = self->dout->is_connected();
    return conn ? mp_const_true : mp_const_false;
}

#if MICROPY_MBED_DIGITALIN
//-----------------------------------------------------------------------
// DigitalIn
//-----------------------------------------------------------------------
struct mbed_DigitalIn_obj_t {
  mp_obj_base_t base;
  DigitalIn *din;
};

mp_obj_t mbed_DigitalIn_make_new(const mp_obj_type_t *type,
                                 size_t n_args, size_t n_kw,
                                 const mp_obj_t *args)
{
    (void)type;
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    int pin = mp_obj_get_int(args[0]);
    int mode = (n_args == 2) ? mp_obj_get_int(args[1]) : 0;
    mbed_DigitalIn_obj_t *o = m_new_obj_with_finaliser(mbed_DigitalIn_obj_t);
    o->base.type = &mbed_DigitalIn_type;
    if (n_args == 2) {
        o->din = new DigitalIn((PinName)pin, (PinMode)mode);
    } else {
        o->din = new DigitalIn((PinName)pin);
    }
    return o;
}

mp_obj_t mbed_DigitalIn_read(mp_obj_t self_in) {
    mbed_DigitalIn_obj_t *self = (mbed_DigitalIn_obj_t *)self_in;
    int value = self->din->read(); // 0 or 1
    return MP_OBJ_NEW_SMALL_INT(value);
}

mp_obj_t mbed_DigitalIn_mode(mp_obj_t self_in, mp_obj_t mode_in) {
    mbed_DigitalIn_obj_t *self = (mbed_DigitalIn_obj_t *)self_in;
    int mode = mp_obj_get_int(mode_in);
    self->din->mode((PinMode)mode);
    return mp_const_none;
}

mp_obj_t mbed_DigitalIn_is_connected(mp_obj_t self_in) {
    mbed_DigitalIn_obj_t *self = (mbed_DigitalIn_obj_t *)self_in;
    int conn = self->din->is_connected();
    return conn ? mp_const_true : mp_const_false;
}

#endif // MICROPY_MBED_DIGITALIN

//-----------------------------------------------------------------------
// Serial
//-----------------------------------------------------------------------
struct mbed_Serial_obj_t {
    mp_obj_base_t base;
    Serial *serial;
};

// constructor Serial(pin)
mp_obj_t mbed_Serial_make_new(const mp_obj_type_t *type,
                              mp_uint_t n_args, mp_uint_t n_kw,
                              const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    int tx = mp_obj_get_int(args[0]);
    int rx = mp_obj_get_int(args[1]);
    mbed_Serial_obj_t *o = m_new_obj_with_finaliser(mbed_Serial_obj_t);
    o->base.type = &mbed_Serial_type;
    o->serial = new Serial((PinName)tx, (PinName)rx);
    return o;
}

mp_obj_t mbed_Serial_baud(mp_obj_t self_in, mp_obj_t baud_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    int baud = mp_obj_get_int(baud_in);
    self->serial->baud(baud);
    return mp_const_none;
}

mp_obj_t mbed_Serial_format(size_t n_args, const mp_obj_t *args) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)args[0];
    int bits = (n_args > 1) ? mp_obj_get_int(args[1]) : 8;
    int parity = (n_args > 2) ? mp_obj_get_int(args[2]) : SerialBase::None;
    int stop_bits = (n_args > 3) ? mp_obj_get_int(args[3]) : 1;
    self->serial->format(bits, (SerialBase::Parity)parity, stop_bits);
    return mp_const_none;
}

mp_obj_t mbed_Serial_readable(mp_obj_t self_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    int value = self->serial->readable();
    return value ? mp_const_true : mp_const_false;
}

mp_obj_t mbed_Serial_writeable(mp_obj_t self_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    int value = self->serial->writeable();
    return value ? mp_const_true : mp_const_false;
}

// TODO attach - interrupt handler

mp_obj_t mbed_Serial_send_break(mp_obj_t self_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    self->serial->send_break();
    return mp_const_none;
}

mp_obj_t mbed_Serial_putc(mp_obj_t self_in, mp_obj_t chr_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    int chr = mp_obj_get_int(chr_in);
    self->serial->putc(chr);
    return mp_const_none;
}

mp_obj_t mbed_Serial_puts(mp_obj_t self_in, mp_obj_t str_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    const char *str = mp_obj_str_get_str(str_in);
    self->serial->puts(str);
    return mp_const_none;
}

mp_obj_t mbed_Serial_getc(mp_obj_t self_in) {
    mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
    int chr = self->serial->getc();
    return MP_OBJ_NEW_SMALL_INT(chr);
}

#endif
