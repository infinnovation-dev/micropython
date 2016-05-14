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
#ifndef __MICROPY_INCLUDED_MREG_H
#define __MICROPY_INCLUDED_MREG_H

#include <stdint.h>

typedef enum {
    MREG_TYPE_STRUCT,
    MREG_TYPE_U32
} mreg_type_t;

typedef struct {
    mreg_type_t type;
    uint32_t dim;
    uint32_t stride;
    const struct _mreg_field_t *itemtype;
} mreg_desc_t;

typedef struct _mreg_field_t {
    const char *name;
    uint32_t offset;
    mreg_desc_t desc;
} mreg_field_t;

typedef struct {
    mp_obj_base_t base;
    char *ptr;
    const mreg_field_t *fields;
} mreg_struct_obj_t;

typedef struct {
    mp_obj_base_t base;
    char *ptr;
    const mreg_desc_t *desc;
} mreg_array_obj_t;

#define MREG_DIM(a) (sizeof(a) / sizeof((a)[0]))
#define MREG_TDIM(f) MREG_DIM(((MREG_TYPE *)0)->f)
#define MREG_TSTRIDE(f) sizeof(((MREG_TYPE *)0)->f[0])
#define MREG_ITEM_U32(a,f) {#f, MREG_OFF(a[0].f) - MREG_OFF(a[0]), {MREG_TYPE_U32, 0, 0, 0}}
#define MREG_ITEM_STRUCT(s,f,d) {#f, MREG_OFF(s.f) - MREG_OFF(a.s), {MREG_TYPE_STRUCT, MREG_TDIM(f), MREG_TSTRIDE(f), d}}
#define MREG_OFF(f) offsetof(MREG_TYPE, f)
#define MREG_U32(f) {#f, MREG_OFF(f), {MREG_TYPE_U32, 0, 0, 0}}
#define MREG_STRUCT(f,d) {#f, MREG_OFF(f), {MREG_TYPE_STRUCT, 0, 0, d}}
#define MREG_ARRAY_U32_N(f,n) {#f, MREG_OFF(f), {MREG_TYPE_U32, n, 0, 0}}
#define MREG_ARRAY_U32(f) MREG_ARRAY_U32_N(f,MREG_TDIM(f))
#define MREG_ARRAY_STRUCT_N(f,d,n) {#f, MREG_OFF(f), {MREG_TYPE_STRUCT, n, MREG_OFF(f[1]) - MREG_OFF(f[0]), d}}
#define MREG_ARRAY_STRUCT(f,d) MREG_ARRAY_STRUCT_N(f,d,MREG_TDIM(f))

extern const mp_obj_type_t mreg_struct_type;
extern const mp_obj_type_t mreg_array_type;

#endif // __MICROPY_INCLUDED_MREG_H
