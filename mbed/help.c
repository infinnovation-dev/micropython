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
#include "py/runtime.h"
#include "lib/utils/pyhelp.h"

STATIC const char help_text[] =
  "Welcome to the (unofficial) mbed port of MicroPython!\n"
  "For online help please visit http://micropython.org/help/.\n"
  "For further help on a specific object, type help(obj)\n"
  "\n"
  "The following modules are available:\n"
#if MICROPY_PY_SYS
  "  sys\n"
#endif
#if MICROPY_PY_STRUCT
  "  ustruct -- cut-down version of struct\n"
#endif
  "  machine -- direct memory access and reset function\n"
  "  mbed    -- equivalents to mbed API classes:\n"
#if MICROPY_MBED_DIGITALOUT
  "    DigitalOut\n"
#endif
#if MICROPY_MBED_SERIAL
  "    Serial\n"
#endif
  "  pins    -- board-specific and generic named pins e.g. USBTX\n"
  ;

STATIC mp_obj_t mbed_help(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // print a general help message
        printf("%s", help_text);
    }
    else {
        // try to print something sensible about the given object
        pyhelp_print_obj(args[0]);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_help_obj, 0, 1, mbed_help);
