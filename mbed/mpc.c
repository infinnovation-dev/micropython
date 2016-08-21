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
// Access to C functions and data from MicroPython

#include "mpc.h"
#include <stdlib.h>
#include <string.h>

typedef enum {
    DEFTYPE_FUNC,
    DEFTYPE_INT,
    DEFTYPE_STRING,
    DEFTYPE_ADDR,
    DEFTYPE_STRUCT
} mpc_deftype_t;

typedef struct {
    mpc_deftype_t type;
    const char *name;
    union {
        mp_fun_var_t func;
        int _int;
        const char *string;
        void *addr;
        struct {
            void *addr;
            const mreg_field_t *fields;
        } _struct;
    } u;
} mpc_def_t;

// Record functions to be defined, before interpreter started
static size_t defs_alloc = 0;
static size_t defs_used = 0;
static mpc_def_t *defs;

static mpc_def_t *new_def(void) {
    if (defs_used == defs_alloc) {
        if (defs_alloc == 0) {
            defs_alloc = 10;
            defs = (mpc_def_t *) malloc(sizeof(mpc_def_t) *
                                        defs_alloc);
        } else {
            defs_alloc *= 2;
            defs = (mpc_def_t *) realloc(defs,
                                         sizeof(mpc_def_t) *
                                         defs_alloc);
        }
    }
    return &defs[defs_used ++];
}

void
mpc_add_func(const char *name, mp_fun_var_t func) {
    mpc_def_t *def = new_def();
    def->name = name;
    def->type = DEFTYPE_FUNC;
    def->u.func = func;
}

void
mpc_add_int(const char *name, int value) {
    mpc_def_t *def = new_def();
    def->name = name;
    def->type = DEFTYPE_INT;
    def->u._int = value;
}

void
mpc_add_string(const char *name, const char *string)
{
    mpc_def_t *def = new_def();
    def->name = name;
    def->type = DEFTYPE_STRING;
    def->u.string = string;
}

void
mpc_add_addr(const char *name, const void *addr) {
    mpc_def_t *def = new_def();
    def->name = name;
    def->type = DEFTYPE_ADDR;
    def->u.addr = *(void **)&addr;
}

void
mpc_add_struct(const char *name, const void *addr, const mreg_field_t *fields) {
    mpc_def_t *def = new_def();
    def->name = name;
    def->type = DEFTYPE_STRUCT;
    def->u._struct.addr = *(void **)&addr;
    def->u._struct.fields = fields;
}

void
mpc_populate(mp_obj_t ns) {
    unsigned i;
    for (i=0; i < defs_used; i++) {
        mpc_def_t *def = &defs[i];
        mp_obj_t obj = mp_const_none;
        switch (def->type) {
        case DEFTYPE_FUNC: {
            mp_obj_fun_builtin_t *fobj = m_new_obj(mp_obj_fun_builtin_t);
            fobj->base.type = &mp_type_fun_builtin;
            fobj->is_kw = false;
            fobj->n_args_min = 0;
            fobj->n_args_max = MP_OBJ_FUN_ARGS_MAX;
            fobj->fun.var = def->u.func;
            obj = MP_OBJ_FROM_PTR(fobj);
        } break;
        case DEFTYPE_INT:
            obj = mp_obj_new_int(def->u._int);
            break;
        case DEFTYPE_STRING:
            obj = mp_obj_new_str(def->u.string, strlen(def->u.string), 0);
            break;
        case DEFTYPE_ADDR:
            obj = mp_obj_new_int_from_uint((uint32_t)def->u.addr);
            break;
        case DEFTYPE_STRUCT: {
            mreg_struct_obj_t *sobj = m_new_obj(mreg_struct_obj_t);
            sobj->base.type = &mreg_struct_type;
            sobj->ptr = def->u._struct.addr;
            sobj->fields = def->u._struct.fields;
            obj = MP_OBJ_FROM_PTR(sobj);
        } break;
        }
        mp_obj_dict_store(ns,
                          mp_obj_new_str(def->name, strlen(def->name), 0),
                          obj);
    }
}

void
mpc_in(size_t n_defs, const mpc_argdef_t *argdefs, mpc_val_t *vals,
       size_t n_args, const mp_obj_t *args) {
    unsigned i;
    // Count expected inputs
    int nexp = 0;
    for (i = 1; i < n_defs; i++) {
        if (argdefs[i].type & MPC_OUT) {
            ;
        } else if (argdefs[i].type == MPC_TYPE_CHARBUF) {
            ;
        } else {
            ++ nexp;
        }
    }
    mp_arg_check_num(n_args, 0, nexp, nexp, 0);
    int iarg = 0;
    for (i = 1; i < n_defs; i++) {
        if (argdefs[i].type & MPC_OUT) {
            // Output
        } else if (argdefs[i].type == MPC_TYPE_CHARBUF) {
            vals[i].buf = (char *) m_malloc(argdefs[i].u.size);
        } else {
            switch (argdefs[i].type) {
            case MPC_TYPE_INT8:
                vals[i].i8 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_INT16:
                vals[i].i16 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_INT32:
                vals[i].i32 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_UINT8:
                vals[i].u8 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_UINT16:
                vals[i].u16 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_UINT32:
                vals[i].u32 = mp_obj_get_int(args[iarg]);
                break;
            case MPC_TYPE_FLOAT:
                vals[i].f = mp_obj_get_float(args[iarg]);
                break;
            case MPC_TYPE_DOUBLE:
                vals[i].d = mp_obj_get_float(args[iarg]);
                break;
            case MPC_TYPE_STRING:
                *(const char **)&vals[i].s = mp_obj_str_get_str(args[iarg]);
                break;
            case MPC_TYPE_PTR:
                vals[i].p = (void *) mp_obj_get_int(args[iarg]);
                break;
            }
            ++ iarg;
        }
    }
}

mp_obj_t
mpc_out(size_t n_defs, const mpc_argdef_t *argdefs, mpc_val_t *vals) {
    mp_obj_t ret;
    mp_obj_t *objs;
    unsigned i;
    int nvals = 0;
    // Count outputs
    for (i=0; i < n_defs; i++) {
        if (argdefs[i].type & MPC_OUT) {
            ++ nvals;
        } else if (argdefs[i].type == MPC_TYPE_CHARBUF) {
            ++ nvals;
        }
    }
    if (nvals <= 1) {
        objs = &ret;
    } else if (nvals > 1) {
        objs = (mp_obj_t *) alloca(sizeof(mp_obj_t) * nvals);
    }
    int iobj = 0;
    for (i=0; i < n_defs; i++) {
        if (argdefs[i].type & MPC_OUT) {
            switch (argdefs[i].type & ~ MPC_OUT) {
            case MPC_TYPE_INT8:
                objs[iobj++] = mp_obj_new_int(vals[i].i8);
                break;
            case MPC_TYPE_INT16:
                objs[iobj++] = mp_obj_new_int(vals[i].i16);
                break;
            case MPC_TYPE_INT32:
                objs[iobj++] = mp_obj_new_int(vals[i].i32);
                break;
            case MPC_TYPE_UINT8:
                objs[iobj++] = mp_obj_new_int_from_uint(vals[i].u8);
                break;
            case MPC_TYPE_UINT16:
                objs[iobj++] = mp_obj_new_int_from_uint(vals[i].u16);
                break;
            case MPC_TYPE_UINT32:
                objs[iobj++] = mp_obj_new_int_from_uint(vals[i].u32);
                break;
            case MPC_TYPE_FLOAT:
                objs[iobj++] = mp_obj_new_float(vals[i].f);
                break;
            case MPC_TYPE_DOUBLE:
                objs[iobj++] = mp_obj_new_float(vals[i].d);
                break;
            case MPC_TYPE_STRING:
                objs[iobj++] = mp_obj_new_str(vals[i].s, strlen(vals[i].s), 0);
                break;
            }
        } else if (i != 0 && defs[i].type == MPC_TYPE_CHARBUF) {
            objs[iobj++] = mp_obj_new_str(vals[i].buf, strlen(vals[i].buf), 0);
            m_free(vals[i].buf);
        }
    }
    if (nvals == 0) {
        ret = mp_const_none;
    } else if (nvals > 1) {
        ret = mp_obj_new_tuple(nvals, objs);
    }
    return ret;
}
