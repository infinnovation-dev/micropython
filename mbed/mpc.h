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
#ifndef __MICROPY_INCLUDED_MPC_h
#define __MICROPY_INCLUDED_MPC_h

#include <stdint.h>
#include <string.h>
#include "py/runtime.h"
#include "mreg.h"

typedef enum {
    MPC_TYPE_INT8,
    MPC_TYPE_INT16,
    MPC_TYPE_INT32,
    MPC_TYPE_UINT8,
    MPC_TYPE_UINT16,
    MPC_TYPE_UINT32,
    MPC_TYPE_FLOAT,
    MPC_TYPE_DOUBLE,
    MPC_TYPE_STRING,
    MPC_TYPE_CHARBUF,                   // Filled by function
    MPC_TYPE_PTR
} mpc_type_t;
#define MPC_TYPE_INT MPC_TYPE_INT32
#define MPC_TYPE_UINT MPC_TYPE_UINT32

#define MPC_OUT 0x80

typedef struct {
    mpc_type_t type;
    union {
        size_t size;
        mp_obj_type_t objtype;
    } u;
} mpc_argdef_t;

typedef union {
    int8_t i8;
    int16_t i16;
    int32_t i32;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    float f;
    double d;
    char *s;
    char *buf;
    void *p;
} mpc_val_t;

/* User-callable */
#define MPC_FUNC_0(r,n) MPC__FUNC(n,MPC_L1(MPC_ARG_O_##r), *MPC_VAL_O_##r(_a[0]) = n())
#define MPC_FUNC_1(r,n,t1) MPC__FUNC(n,MPC_L2(MPC_ARG_O_##r,MPC_ARG_##t1), *MPC_VAL_O_##r(_a[0]) = n(t1MPC_VAL_##(_a[1])))
#define MPC_FUNC_2(r,n,t1,t2) MPC__FUNC(n,MPC_L3(MPC_ARG_O_##r,MPC_ARG_##t1,MPC_ARG_##t2), *MPC_VAL_O_##r(_a[0]) = n(MPC_VAL_##t1(_a[1]),MPC_VAL_##t2(_a[2])))
#define MPC_FUNC_3(r,n,t1,t2,t3) MPC__FUNC(n,MPC_L4(MPC_ARG_O_##r,MPC_ARG_##t1,MPC_ARG_##t2,MPC_ARG_##t3), *MPC_VAL_O_##r(_a[0]) = n(MPC_VAL_##t1(_a[1]),MPC_VAL_##t2(_a[2]),MPC_VAL_##t3(_a[3])))

#define MPC_VOIDFUNC_0(n) MPC__FUNC0(n)
#define MPC_VOIDFUNC_1(n,t1) MPC__FUNC(n,MPC_L1(MPC_ARG_##t1), n(MPC_VAL_##t1(_a[0])))
#define MPC_VOIDFUNC_2(n,t1,t2) MPC__FUNC(n,MPC_L2(MPC_ARG_##t1,MPC_ARG_##t2), n(MPC_VAL_##t1(_a[0]),MPC_VAL_##t2(_a[1])))
#define MPC_VOIDFUNC_3(n,t1,t2,t3) MPC__FUNC(n,MPC_L3(MPC_ARG_##t1,MPC_ARG_##t2,MPC_ARG_##t3), n(MPC_VAL_##t1(_a[0]),MPC_VAL_##t2(_a[1]),MPC_VAL_##t3(_a[2])))

extern void mpc_add_func(const char */*name*/, mp_fun_var_t /*wrapper*/);
extern void mpc_add_int(const char */*name*/, int);
extern void mpc_add_string(const char */*name*/, const char *);
extern void mpc_add_addr(const char */*name*/, const void */*addr*/);
extern void mpc_add_struct(const char */*name*/, const void */*addr*/,
                           const mreg_field_t *);

extern void mpc_populate(mp_obj_t /*dict*/);
extern void mpc_populate_globals(void);

/*--- Internals */
#define MPC_L1(s1) {s1}
#define MPC_L2(s1,s2) {s1,s2}
#define MPC_L3(s1,s2,s3) {s1,s2,s3}
#define MPC_L4(s1,s2,s3,s4) {s1,s2,s3,s4}
#define MPC_L5(s1,s2,s3,s4,s5) {s1,s2,s3,s4,s5}

#define MPC_ARG_VOID {MPC_TYPE_VOID}
#define MPC_ARG_INT8 {MPC_TYPE_INT8}
#define MPC_ARG_INT16 {MPC_TYPE_INT16}
#define MPC_ARG_INT32 {MPC_TYPE_INT32}
#define MPC_ARG_UINT8 {MPC_TYPE_UINT8}
#define MPC_ARG_UINT16 {MPC_TYPE_UINT16}
#define MPC_ARG_UINT32 {MPC_TYPE_UINT32}
#define MPC_ARG_FLOAT {MPC_TYPE_FLOAT}
#define MPC_ARG_DOUBLE {MPC_TYPE_DOUBLE}
#define MPC_ARG_STRING {MPC_TYPE_STRING}
#define MPC_ARG_CHARBUF(n) MPC_L2((mpc_type_t)(MPC_OUT | MPC_TYPE_CHARBUF), {.size = (n)})
#define MPC_ARG_PTR {MPC_TYPE_PTR}
#define MPC_ARG_O_INT8 {(mpc_type_t)(MPC_OUT | MPC_TYPE_INT8)}
#define MPC_ARG_O_INT16 {(mpc_type_t)(MPC_OUT | MPC_TYPE_INT16)}
#define MPC_ARG_O_INT32 {(mpc_type_t)(MPC_OUT | MPC_TYPE_INT32)}
#define MPC_ARG_O_UINT8 {(mpc_type_t)(MPC_OUT | MPC_TYPE_UINT8)}
#define MPC_ARG_O_UINT16 {(mpc_type_t)(MPC_OUT | MPC_TYPE_UINT16)}
#define MPC_ARG_O_UINT32 {(mpc_type_t)(MPC_OUT | MPC_TYPE_UINT32)}
#define MPC_ARG_O_FLOAT {(mpc_type_t)(MPC_OUT | MPC_TYPE_FLOAT)}
#define MPC_ARG_O_DOUBLE {(mpc_type_t)(MPC_OUT | MPC_TYPE_DOUBLE)}
#define MPC_ARG_O_STRING {(mpc_type_t)(MPC_OUT | MPC_TYPE_STRING)}
#define MPC_ARG_O_PTR {(mpc_type_t)(MPC_OUT | MPC_TYPE_PTR)}

#define MPC_ARG_INT MPC_ARG_INT32
#define MPC_ARG_UINT MPC_ARG_UINT32
#define MPC_ARG_O_INT MPC_ARG_O_INT32
#define MPC_ARG_O_UINT MPC_ARG_O_UINT32

#define MPC_VAL_INT8(a) ((a).i8)
#define MPC_VAL_INT16(a) ((a).i16)
#define MPC_VAL_INT32(a) ((a).i32)
#define MPC_VAL_UINT8(a) ((a).u8)
#define MPC_VAL_UINT16(a) ((a).u16)
#define MPC_VAL_UINT32(a) ((a).u32)
#define MPC_VAL_FLOAT(a) ((a).f)
#define MPC_VAL_DOUBLE(a) ((a).d)
#define MPC_VAL_STRING(a) ((a).s)
#define MPC_VAL_CHARBUF(n) MPC_VAL__CHARBUF
#define MPC_VAL__CHARBUF(a) ((a).buf)
#define MPC_VAL_PTR(a) ((a).p)
#define MPC_VAL_O_INT8(a) (&(a).i8)
#define MPC_VAL_O_INT16(a) (&(a).i16)
#define MPC_VAL_O_INT32(a) (&(a).i32)
#define MPC_VAL_O_UINT8(a) (&(a).u8)
#define MPC_VAL_O_UINT16(a) (&(a).u16)
#define MPC_VAL_O_UINT32(a) (&(a).u32)
#define MPC_VAL_O_FLOAT(a) (&(a).f)
#define MPC_VAL_O_DOUBLE(a) (&(a).d)
#define MPC_VAL_O_STRING(a) (&(a).s)
#define MPC_VAL_O_PTR(a) (&(a).p)

#define MPC_VAL_INT(a) MPC_VAL_INT32(a)
#define MPC_VAL_UINT(a) MPC_VAL_UINT32(a)
#define MPC_VAL_O_INT(a) MPC_VAL_O_INT32(a)
#define MPC_VAL_O_UINT(a) MPC_VAL_O_UINT32(a)

#define MPC_LEN(v) (sizeof(v)/sizeof((v)[0]))

// Define mp wrapper function 
#define MPC__FUNC(n,ad,cs)                                              \
static const mpc_argdef_t mpc_a_##n[] = ad;                             \
static mp_obj_t mpc_f_##n(size_t n_args, const mp_obj_t *args) {        \
    mpc_val_t _a[MPC_LEN(mpc_a_##n)];                                   \
    mpc_in(MPC_LEN(mpc_a_##n), mpc_a_##n, _a, n_args, args);            \
    cs;                                                                 \
    return mpc_out(MPC_LEN(mpc_a_##n), mpc_a_##n, _a);                  \
}
#define MPC__FUNC0(n)                                                   \
    static mp_obj_t mpc_f_##n(size_t n_args, const mp_obj_t *args) {    \
    mp_arg_check_num(n_args, 0, 0, 0, 0);                               \
    n();                                                               \
    return mp_const_none;                                               \
}

extern void mpc_in(size_t, const mpc_argdef_t *, mpc_val_t *,
                   size_t, const mp_obj_t *);
extern mp_obj_t mpc_out(size_t, const mpc_argdef_t *, mpc_val_t *);

#endif // __MICROPY_INCLUDED_MPC_h
