/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2015 Damien P. George
 * Copyright (c) 2016 Paul Sokolovsky
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
#if MICROPY_PY_UOS

#include <string.h>
#include <errno.h>
#include "mbed_retarget.h"

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
        { mp_raise_OSError(error_val); } }

// mkdir
STATIC mp_obj_t uos_mkdir(size_t n_args, const mp_obj_t *args) {
    const char *path = mp_obj_str_get_str(args[0]);
    mode_t mode = n_args >= 2 ? mp_obj_get_int(args[1]) : 0777;
    int r = mkdir(path, mode);
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(uos_mkdir_obj, 1, 2,
                                           uos_mkdir);

// Support for iterative directory listing
typedef struct _mp_obj_listdir_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    DIR *dir;
} mp_obj_listdir_t;

STATIC mp_obj_t listdir_next(mp_obj_t self_in) {
    mp_obj_listdir_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->dir == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }
    struct dirent *de = readdir(self->dir);
    if (! de) {
        closedir(self->dir);
        self->dir = NULL;
        return MP_OBJ_STOP_ITERATION;
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    t->items[0] = mp_obj_new_str(de->d_name, strlen(de->d_name), false);
    t->items[1] = MP_OBJ_NEW_SMALL_INT(de->d_type);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); /* No inode */
    return MP_OBJ_FROM_PTR(t);
}

STATIC mp_obj_t uos_ilistdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    mp_obj_listdir_t *o = m_new_obj(mp_obj_listdir_t);
    o->base.type = &mp_type_polymorph_iter;
    o->dir = opendir(path);             /* FIXME error? */
    o->iternext = listdir_next;
    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(uos_ilistdir_obj,
                                 uos_ilistdir);

// Module globals
STATIC const mp_rom_map_elem_t uos_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),        MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir),        MP_ROM_PTR(&uos_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir),           MP_ROM_PTR(&uos_mkdir_obj) },
};

STATIC MP_DEFINE_CONST_DICT(uos_module_globals, uos_module_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&uos_module_globals,
};

#endif // MICROPY_PY_UOS
