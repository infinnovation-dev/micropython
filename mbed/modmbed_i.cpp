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

typedef struct _mbed_DigitalOut_obj_t {
  mp_obj_base_t base;
  DigitalOut *dout;
} mbed_DigitalOut_obj_t;

// constructor DigitalOut(pin)
mp_obj_t mbed_DigitalOut_make_new(const mp_obj_type_t *type,
				  mp_uint_t n_args, mp_uint_t n_kw,
				  const mp_obj_t *args)
{
  (void)type;
  mp_arg_check_num(n_args, n_kw, 1, 1, false);
  int pin = mp_obj_get_int(args[0]);

  mbed_DigitalOut_obj_t *o = m_new_obj_with_finaliser(mbed_DigitalOut_obj_t);
  o->base.type = &mbed_DigitalOut_type;
  o->dout = new DigitalOut((PinName)pin);

  return o;
}

mp_obj_t mbed_DigitalOut_write(mp_obj_t self_in, mp_obj_t value_in)
{
  mbed_DigitalOut_obj_t *self = (mbed_DigitalOut_obj_t *)self_in;
  int value = mp_obj_get_int(value_in);

  self->dout->write(value);
  return mp_const_none;
}

typedef struct _mbed_Serial_obj_t {
  mp_obj_base_t base;
  Serial *serial;
} mbed_Serial_obj_t;

// constructor Serial(pin)
mp_obj_t mbed_Serial_make_new(const mp_obj_type_t *type,
			      mp_uint_t n_args, mp_uint_t n_kw,
			      const mp_obj_t *args)
{
  (void)type;
  mp_arg_check_num(n_args, n_kw, 2, 2, false);
  int tx = mp_obj_get_int(args[0]);
  int rx = mp_obj_get_int(args[1]);

  mbed_Serial_obj_t *o = m_new_obj_with_finaliser(mbed_Serial_obj_t);
  o->base.type = &mbed_Serial_type;
  o->serial = new Serial((PinName)tx, (PinName)rx);

  return o;
}

mp_obj_t mbed_Serial_putc(mp_obj_t self_in, mp_obj_t chr_in)
{
  mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
  int chr = mp_obj_get_int(chr_in);

  self->serial->putc(chr);
  return mp_const_none;
}

mp_obj_t mbed_Serial_puts(mp_obj_t self_in, mp_obj_t str_in)
{
  mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
  const char *str = mp_obj_str_get_str(str_in);

  self->serial->puts(str);
  return mp_const_none;
}

mp_obj_t mbed_Serial_getc(mp_obj_t self_in)
{
  mbed_Serial_obj_t *self = (mbed_Serial_obj_t *)self_in;
  int chr = self->serial->getc();
  return MP_OBJ_NEW_SMALL_INT(chr);
}

#endif
