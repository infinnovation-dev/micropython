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
//      DigitalOut class
//-----------------------------------------------------------------------
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_DigitalOut_write_obj,
                                 mbed_DigitalOut_write);

STATIC const mp_map_elem_t mbed_DigitalOut_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mbed_DigitalOut_write_obj },
};

STATIC MP_DEFINE_CONST_DICT(mbed_DigitalOut_locals_dict,
                            mbed_DigitalOut_locals_dict_table);

const mp_obj_type_t mbed_DigitalOut_type = {
    { &mp_type_type },
    .name = MP_QSTR_DigitalOut,
    // .print
    .make_new = mbed_DigitalOut_make_new,
    .locals_dict = (mp_obj_t)&mbed_DigitalOut_locals_dict,
};

#endif // MICROPY_MBED_DIGITALOUT

#if MICROPY_MBED_SERIAL
/*-----------------------------------------------------------------------
 *      Serial class
 *-----------------------------------------------------------------------*/
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_Serial_baud_obj,
                                 mbed_Serial_baud);

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_Serial_putc_obj,
                                 mbed_Serial_putc);

STATIC MP_DEFINE_CONST_FUN_OBJ_2(mbed_Serial_puts_obj,
                                 mbed_Serial_puts);

STATIC MP_DEFINE_CONST_FUN_OBJ_1(mbed_Serial_getc_obj,
                                 mbed_Serial_getc);

STATIC const mp_map_elem_t mbed_Serial_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_baud), (mp_obj_t)&mbed_Serial_baud_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_putc), (mp_obj_t)&mbed_Serial_putc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_puts), (mp_obj_t)&mbed_Serial_puts_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getc), (mp_obj_t)&mbed_Serial_getc_obj },
};

STATIC MP_DEFINE_CONST_DICT(mbed_Serial_locals_dict,
                            mbed_Serial_locals_dict_table);

const mp_obj_type_t mbed_Serial_type = {
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
