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
 * Hardware Abstraction Layer.
 *
 * Implementations or stubs for functions provided by the specific port.
 * Note that two functions are delegated to the user of the library:
 * - mp_hal_stdout_tx_chr
 * - mp_hal_stdin_rx_chr
 */
#include <string.h>
#include "py/mpstate.h"
#include "py/lexer.h"

// Since we have no filesystem, no file exists
mp_import_stat_t mp_import_stat(const char *path) {
    (void)path;
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    (void)filename;
    return NULL;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    (void)n_args;
    (void)args;
    (void)kwargs;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void mp_hal_stdout_tx_chr(char c);

// Text output
void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    while (len--) {
        mp_hal_stdout_tx_chr(*str++);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    while (len--) {
        char c = *str++;
        if (c == '\n') {
            mp_hal_stdout_tx_chr('\r');
        }
        mp_hal_stdout_tx_chr(c);
    }
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_set_interrupt_char(int c) {
    (void)c;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return 0;
}

void mp_hal_delay_ms(mp_uint_t ms) {
    (void)ms;
}

void gc_collect(void) {}
#if ! MICROPY_ENABLE_GC
void gc_dump_info(void) {}
#endif
