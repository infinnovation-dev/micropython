/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <stdint.h>

// mbed's printf outputs to USB serial but does not do LF->CRLF conversion.
// Ensure lib/utils/printf.c implementation is used instead.
// But not for C++ where cstdio would get upset.
#ifndef __cplusplus
extern int mp__printf(const char *, ...);
#define printf mp__printf
#endif

// options to control how Micro Python is built

// The mbed online compiler uses different assembler syntax, so avoid
#define MICROPY_NLR_SETJMP          (1)

#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (0)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (0)
#define MICROPY_ENABLE_DOC_STRING   (0)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_BUILTINS_BYTEARRAY (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_FROZENSET (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY___FILE__         (0)
#define MICROPY_PY_GC               (0)
#define MICROPY_PY_ARRAY            (0)
#define MICROPY_PY_ATTRTUPLE        (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_MATH             (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_SYS              (1)
// Build "machine" module (port-specific)
#define MICROPY_PY_MACHINE          (1)

// Build "mbed" module
#define MICROPY_PY_MBED             (1)
//   Enable features in mbed module, when requested (mbed_lib.json) and
//   available (mbed-os/hal/targets.json)
#if DEVICE_ANALOGIN
    #define MICROPY_MBED_ANALOGIN   MBED_CONF_MICROPYTHON_WITH_ANALOGIN
#else
    #define MICROPY_MBED_ANALOGIN   0
#endif
#if DEVICE_ANALOGOUT
    #define MICROPY_MBED_ANALOGOUT  MBED_CONF_MICROPYTHON_WITH_ANALOGOUT
#else
    #define MICROPY_MBED_ANALOGOUT  0
#endif
#if DEVICE_DIGITALIN
    #define MICROPY_MBED_DIGITALIN  MBED_CONF_MICROPYTHON_WITH_DIGITALIN
#else
    #define MICROPY_MBED_DIGITALIN  0
#endif
#if DEVICE_DIGITALOUT
    #define MICROPY_MBED_DIGITALOUT MBED_CONF_MICROPYTHON_WITH_DIGITALOUT
#else
    #define MICROPY_MBED_DIGITALOUT 0
#endif
#if DEVICE_I2C
    #define MICROPY_MBED_I2C        MBED_CONF_MICROPYTHON_WITH_I2C
#else
    #define MICROPY_MBED_I2C        0
#endif
#if DEVICE_PWMOUT
    #define MICROPY_MBED_PWMOUT     MBED_CONF_MICROPYTHON_WITH_PWMOUT
#else
    #define MICROPY_MBED_PWMOUT     0
#endif
#if DEVICE_SERIAL
    #define MICROPY_MBED_SERIAL     MBED_CONF_MICROPYTHON_WITH_SERIAL
#else
    #define MICROPY_MBED_SERIAL     0
#endif
#if DEVICE_SPI
    #define MICROPY_MBED_SPI        MBED_CONF_MICROPYTHON_WITH_SPI
#else
    #define MICROPY_MBED_SPI        0
#endif

// Build "net" module
#define MICROPY_PY_NET              MBED_CONF_MICROPYTHON_WITH_NET

// Build "k64f" module
#if defined(TARGET_K64F)
#define MICROPY_PY_K64F             (1)
#endif

#define MICROPY_MODULE_FROZEN       (0)
#define MICROPY_CPYTHON_COMPAT      (0)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_LONGLONG)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)
#define MP_ENDIANNESS_LITTLE 1

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size

typedef void *machine_ptr_t; // must be of pointer size
typedef const void *machine_const_ptr_t; // must be of pointer size
typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// extra built in names to add to the global namespace
#if MBED_CONF_MICROPYTHON_WITH_HELP
extern const struct _mp_obj_fun_builtin_t mp_builtin_help_obj;
#define MICROPY_MAYBE_BUILTIN_HELP \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj },
#else
#define MICROPY_MAYBE_BUILTIN_HELP
#endif

#define MICROPY_PORT_BUILTINS                   \
    MICROPY_MAYBE_BUILTIN_HELP


// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MICROPY_PY_SYS_PLATFORM "mbed"
#define MICROPY_HW_BOARD_NAME "mbed"
#define MICROPY_HW_MCU_NAME "arm"

#define MP_STATE_PORT MP_STATE_VM

// Extra modules to build in
#if MICROPY_PY_MBED
extern const struct _mp_obj_module_t mp_module_mbed;
#define MICROPY_MAYBE_MODULE_MBED \
    { MP_ROM_QSTR(MP_QSTR_mbed), MP_ROM_PTR(&mp_module_mbed) },
#else
#define MICROPY_MAYBE_MODULE_MBED
#endif

#if MICROPY_PY_NET
extern const struct _mp_obj_module_t mp_module_net;
#define MICROPY_MAYBE_MODULE_NET \
    { MP_ROM_QSTR(MP_QSTR_net), MP_ROM_PTR(&mp_module_net) },
#else
#define MICROPY_MAYBE_MODULE_NET
#endif

#if MICROPY_PY_MACHINE
extern const struct _mp_obj_module_t mp_module_machine;
#define MICROPY_MAYBE_MODULE_MACHINE \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) },
#else
#define MICROPY_MAYBE_MODULE_MACHINE
#endif

#if MBED_CONF_MICROPYTHON_WITH_PINS
extern const struct _mp_obj_module_t mp_module_pins;
#define MICROPY_MAYBE_MODULE_PINS \
    { MP_ROM_QSTR(MP_QSTR_pins), MP_ROM_PTR(&mp_module_pins) },
#else
#define MICROPY_MAYBE_MODULE_PINS
#endif

#if defined(TARGET_K64F) && MICROPY_PY_K64F
extern const struct _mp_obj_module_t mp_module_k64f;
#define MICROPY_MAYBE_MODULE_K64F \
    { MP_ROM_QSTR(MP_QSTR_k64f), MP_ROM_PTR(&mp_module_k64f) },
#else
#define MICROPY_MAYBE_MODULE_K64F
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_MAYBE_MODULE_MBED              \
    MICROPY_MAYBE_MODULE_NET               \
    MICROPY_MAYBE_MODULE_MACHINE           \
    MICROPY_MAYBE_MODULE_PINS              \
    MICROPY_MAYBE_MODULE_K64F              \

// Use by readline.c
#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[MBED_CONF_MICROPYTHON_REPL_HISTORY_SIZE]; \
    vstr_t *repl_line; \

