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
// High-level mbed objects

#include "py/runtime.h"

// Only if selected in mpconfigport.h
#if MICROPY_PY_MBED

#include "modmbed_i.h"

#if MICROPY_MBED_DIGITALOUT
//-----------------------------------------------------------------------
//	DigitalOut class
//-----------------------------------------------------------------------
STATIC const mp_obj_type_t mbed_DigitalOut_type;

typedef struct _mbed_DigitalOut_obj_t {
  mp_obj_base_t base;
  void *dout;
} mbed_DigitalOut_obj_t;

// constructor DigitalOut(pin)
STATIC mp_obj_t mbed_DigitalOut_make_new(const mp_obj_type_t *type,
					 mp_uint_t n_args, mp_uint_t n_kw,
					 const mp_obj_t *args)
{
  (void)type;
  mp_arg_check_num(n_args, n_kw, 1, 1, false);
  int pin = mp_obj_get_int(args[0]);

  mbed_DigitalOut_obj_t *o = m_new_obj_with_finaliser(mbed_DigitalOut_obj_t);
  o->base.type = (mp_obj_t)&mbed_DigitalOut_type;
  o->dout = mbed_DigitalOut__create(pin);

  return o;
}

STATIC mp_obj_t mbed_DigitalOut_write(mp_obj_t self_in, mp_obj_t value_in)
{
  mbed_DigitalOut_obj_t *self = self_in;
  int value = mp_obj_get_int(value_in);

  mbed_DigitalOut__write(self->dout, value);
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_DigitalOut_write_obj,
				 mbed_DigitalOut_write);

STATIC const mp_map_elem_t mbed_DigitalOut_locals_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mbed_DigitalOut_write_obj },
};
  
STATIC MP_DEFINE_CONST_DICT(mbed_DigitalOut_locals_dict,
			    mbed_DigitalOut_locals_dict_table);

STATIC const mp_obj_type_t mbed_DigitalOut_type = {
  { &mp_type_type },
  .name = MP_QSTR_DigitalOut,
  // .print
  .make_new = mbed_DigitalOut_make_new,
  .locals_dict = (mp_obj_t)&mbed_DigitalOut_locals_dict,
};

#endif // MICROPY_MBED_DIGITALOUT

#if MICROPY_MBED_SERIAL
/*-----------------------------------------------------------------------
 *	Serial class
 *-----------------------------------------------------------------------*/
STATIC const mp_obj_type_t mbed_Serial_type;

typedef struct _mbed_Serial_obj_t {
  mp_obj_base_t base;
  void *serial;
} mbed_Serial_obj_t;

// constructor Serial(pin)
STATIC mp_obj_t mbed_Serial_make_new(const mp_obj_type_t *type,
				     mp_uint_t n_args, mp_uint_t n_kw,
				     const mp_obj_t *args)
{
  (void)type;
  mp_arg_check_num(n_args, n_kw, 2, 2, false);
  int tx = mp_obj_get_int(args[0]);
  int rx = mp_obj_get_int(args[1]);

  mbed_Serial_obj_t *o = m_new_obj_with_finaliser(mbed_Serial_obj_t);
  o->base.type = (mp_obj_t)&mbed_Serial_type;
  o->serial = mbed_Serial__create(tx, rx);

  return o;
}

STATIC mp_obj_t mbed_Serial_putc(mp_obj_t self_in, mp_obj_t chr_in)
{
  mbed_Serial_obj_t *self = self_in;
  int chr = mp_obj_get_int(chr_in);

  mbed_Serial__putc(self->serial, chr);
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_Serial_putc_obj,
				 mbed_Serial_putc);

STATIC mp_obj_t mbed_Serial_puts(mp_obj_t self_in, mp_obj_t str_in)
{
  mbed_Serial_obj_t *self = self_in;
  const char *str = mp_obj_str_get_str(str_in);

  mbed_Serial__puts(self->serial, str);
  return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_Serial_puts_obj,
				 mbed_Serial_puts);

STATIC mp_obj_t mbed_Serial_getc(mp_obj_t self_in)
{
  mbed_Serial_obj_t *self = self_in;
  int chr = mbed_Serial__getc(self->serial);
  return MP_OBJ_NEW_SMALL_INT(chr);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mbed_Serial_getc_obj,
				 mbed_Serial_getc);

STATIC const mp_map_elem_t mbed_Serial_locals_dict_table[] = {
  { MP_OBJ_NEW_QSTR(MP_QSTR_putc), (mp_obj_t)&mbed_Serial_putc_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_puts), (mp_obj_t)&mbed_Serial_puts_obj },
  { MP_OBJ_NEW_QSTR(MP_QSTR_getc), (mp_obj_t)&mbed_Serial_getc_obj },
};
  
STATIC MP_DEFINE_CONST_DICT(mbed_Serial_locals_dict,
			    mbed_Serial_locals_dict_table);

STATIC const mp_obj_type_t mbed_Serial_type = {
  { &mp_type_type },
  .name = MP_QSTR_Serial,
  // .print
  .make_new = mbed_Serial_make_new,
  .locals_dict = (mp_obj_t)&mbed_Serial_locals_dict,
};

#endif // MICROPY_MBED_SERIAL

// Module
STATIC const mp_rom_map_elem_t mp_mbed_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mbed) },
#if MICROPY_MBED_DIGITALOUT
  { MP_ROM_QSTR(MP_QSTR_DigitalOut), MP_ROM_PTR(&mbed_DigitalOut_type) },
#endif
#if MICROPY_MBED_SERIAL
  { MP_ROM_QSTR(MP_QSTR_Serial), MP_ROM_PTR(&mbed_Serial_type) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_mbed_module_globals,
			    mp_mbed_module_globals_table);

const mp_obj_module_t mp_module_mbed = {
  .base = { &mp_type_module },
  .name = MP_QSTR_mbed,
  .globals = (mp_obj_dict_t *)&mp_mbed_module_globals,
};

#endif
