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
#if MICROPY_PY_K64F
#include "mreg.h"
#include "MK64F12.h"

/*-----------------------------------------------------------------------
 *      DAC
 *-----------------------------------------------------------------------*/
#define MREG_TYPE DAC_Type
STATIC const mreg_field_t k64f_DAC_DAT_fields[] = {
    MREG_ITEM_U8(DAT, DATL),
    MREG_ITEM_U8(DAT, DATH),
    {0}
};

STATIC const mreg_field_t k64f_DAC_fields[] = {
    MREG_ARRAY_STRUCT(DAT, k64f_DAC_DAT_fields),
    MREG_U8(SR),
    MREG_U8(C0),
    MREG_U8(C1),
    MREG_U8(C2),
    {0},
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_DAC0_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)DAC0,
    .fields = k64f_DAC_fields,
};

const mreg_struct_obj_t k64f_DAC1_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)DAC1,
    .fields = k64f_DAC_fields,
};

/*-----------------------------------------------------------------------
 *      DMA
 *-----------------------------------------------------------------------*/
#define MREG_TYPE DMA_Type
STATIC const mreg_field_t k64f_DMA_TCD_fields[] = {
    MREG_ITEM_U32(TCD, SADDR),
    MREG_ITEM_U16(TCD, SOFF),
    MREG_ITEM_U16(TCD, ATTR),
    MREG_ITEM_U32(TCD, NBYTES_MLNO),
    MREG_ITEM_U32(TCD, SLAST),
    MREG_ITEM_U32(TCD, DADDR),
    MREG_ITEM_U16(TCD, DOFF),
    MREG_ITEM_U16(TCD, CITER_ELINKNO),
    MREG_ITEM_U32(TCD, DLAST_SGA),
    MREG_ITEM_U16(TCD, CSR),
    MREG_ITEM_U16(TCD, BITER_ELINKNO),
    {0},
};

STATIC const mreg_field_t k64f_DMA_fields[] = {
    MREG_U32(CR),
    MREG_U32(ES),
    MREG_U32(ERQ),
    MREG_U32(EEI),
    // __O U8(CEEI),
    // __O U8(SEEI),
    // __O U8(CERQ),
    // __O U8(SERQ),
    // __O U8(CDNE),
    // __O U8(SSRT),
    // __O U8(CEEE),
    // __O U8(CINT),
    MREG_U32(INT),
    MREG_U32(ERR),
    MREG_U32(HRS),
    MREG_U8(DCHPRI3),
    MREG_U8(DCHPRI2),
    MREG_U8(DCHPRI1),
    MREG_U8(DCHPRI0),
    MREG_U8(DCHPRI7),
    MREG_U8(DCHPRI6),
    MREG_U8(DCHPRI5),
    MREG_U8(DCHPRI4),
    MREG_U8(DCHPRI11),
    MREG_U8(DCHPRI10),
    MREG_U8(DCHPRI9),
    MREG_U8(DCHPRI8),
    MREG_U8(DCHPRI15),
    MREG_U8(DCHPRI14),
    MREG_U8(DCHPRI13),
    MREG_U8(DCHPRI12),
    MREG_ARRAY_STRUCT(TCD, k64f_DMA_TCD_fields),
    {0},
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_DMA_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)DMA0,
    .fields = k64f_DMA_fields,
};

/*-----------------------------------------------------------------------
 *      DMAMUX
 *-----------------------------------------------------------------------*/
#define MREG_TYPE DMAMUX_Type
STATIC const mreg_field_t k64f_DMAMUX_fields[] = {
    MREG_ARRAY_U8(CHCFG),
    {0},
};

const mreg_struct_obj_t k64f_DMAMUX_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)DMAMUX,
    .fields = k64f_DMAMUX_fields,
};
#undef MREG_TYPE

/*-----------------------------------------------------------------------
 *      FTM
 *-----------------------------------------------------------------------*/
#define MREG_TYPE FTM_Type
STATIC const mreg_field_t k64f_FTM_CONTROLS_fields[] = {
    MREG_ITEM_U32(CONTROLS, CnSC),
    MREG_ITEM_U32(CONTROLS, CnV),
    {0},
};

STATIC const mreg_field_t k64f_FTM_fields[] = {
    MREG_U32(SC),
    MREG_U32(CNT),
    MREG_U32(MOD),
    MREG_ARRAY_STRUCT(CONTROLS, k64f_FTM_CONTROLS_fields),
    MREG_U32(CNTIN),
    MREG_U32(STATUS),
    MREG_U32(MODE),
    MREG_U32(SYNC),
    MREG_U32(OUTINIT),
    MREG_U32(OUTMASK),
    MREG_U32(COMBINE),
    MREG_U32(DEADTIME),
    MREG_U32(EXTTRIG),
    MREG_U32(POL),
    MREG_U32(FMS),
    MREG_U32(FILTER),
    MREG_U32(FLTCTRL),
    MREG_U32(QDCTRL),
    MREG_U32(CONF),
    MREG_U32(FLTPOL),
    MREG_U32(SYNCONF),
    MREG_U32(INVCTRL),
    MREG_U32(SWOCTRL),
    MREG_U32(PWMLOAD),
    {0},
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_FTM0_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)FTM0,
    .fields = k64f_FTM_fields,
};

const mreg_struct_obj_t k64f_FTM1_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)FTM1,
    .fields = k64f_FTM_fields,
};

const mreg_struct_obj_t k64f_FTM2_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)FTM2,
    .fields = k64f_FTM_fields,
};

const mreg_struct_obj_t k64f_FTM3_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)FTM3,
    .fields = k64f_FTM_fields,
};

/*-----------------------------------------------------------------------
 *      GPIO
 *-----------------------------------------------------------------------*/
#define MREG_TYPE GPIO_Type
const mreg_field_t k64f_GPIO_fields[] = {
    MREG_U32(PDOR),
    MREG_U32(PSOR),                     /* __O */
    MREG_U32(PCOR),                     /* __O */
    MREG_U32(PTOR),                     /* __O */
    MREG_U32(PDIR),                     /* __I */
    MREG_U32(PDDR),
    {0},
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_PTA_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PTA,
    .fields = k64f_GPIO_fields,
};

const mreg_struct_obj_t k64f_PTB_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PTB,
    .fields = k64f_GPIO_fields,
};

const mreg_struct_obj_t k64f_PTC_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PTC,
    .fields = k64f_GPIO_fields,
};

const mreg_struct_obj_t k64f_PTD_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PTD,
    .fields = k64f_GPIO_fields,
};

const mreg_struct_obj_t k64f_PTE_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PTE,
    .fields = k64f_GPIO_fields,
};

/*-----------------------------------------------------------------------
 *      PIT
 *-----------------------------------------------------------------------*/
#define MREG_TYPE PIT_Type
const mreg_field_t k64f_PIT_CHANNEL_fields[] = {
    MREG_ITEM_U32(CHANNEL, LDVAL),
    MREG_ITEM_U32(CHANNEL, CVAL),
    MREG_ITEM_U32(CHANNEL, TCTRL),
    MREG_ITEM_U32(CHANNEL, TFLG),
    {0},
};

const mreg_field_t k64f_PIT_fields[] = {
    MREG_U32(MCR),
    MREG_ARRAY_STRUCT(CHANNEL, k64f_PIT_CHANNEL_fields),
    {0},
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_PIT_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PIT,
    .fields = k64f_PIT_fields,
};

/*-----------------------------------------------------------------------
 *      PORT
 *-----------------------------------------------------------------------*/
#define MREG_TYPE PORT_Type
const mreg_field_t k64f_PORT_fields[] = {
    MREG_ARRAY_U32(PCR),
    // __O GPCLR
    // __O GPCHR
    MREG_U32(ISFR),
    MREG_U32(DFER),
    MREG_U32(DFCR),
    MREG_U32(DFWR),
    {0}
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_PORTA_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PORTA,
    .fields = k64f_PORT_fields,
};

const mreg_struct_obj_t k64f_PORTB_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PORTB,
    .fields = k64f_PORT_fields,
};

const mreg_struct_obj_t k64f_PORTC_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PORTC,
    .fields = k64f_PORT_fields,
};

const mreg_struct_obj_t k64f_PORTD_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PORTD,
    .fields = k64f_PORT_fields,
};

const mreg_struct_obj_t k64f_PORTE_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)PORTE,
    .fields = k64f_PORT_fields,
};

/*-----------------------------------------------------------------------
 *      SIM
 *-----------------------------------------------------------------------*/
#define MREG_TYPE SIM_Type
const mreg_field_t k64f_SIM_fields[] = {
    MREG_U32(SOPT1),
    MREG_U32(SOPT1CFG),
    MREG_U32(SOPT2),
    MREG_U32(SOPT4),
    MREG_U32(SOPT5),
    MREG_U32(SOPT7),
    MREG_U32(SDID),
    MREG_U32(SCGC1),
    MREG_U32(SCGC2),
    MREG_U32(SCGC3),
    MREG_U32(SCGC4),
    MREG_U32(SCGC5),
    MREG_U32(SCGC6),
    MREG_U32(SCGC7),
    MREG_U32(CLKDIV1),
    MREG_U32(CLKDIV2),
    MREG_U32(FCFG1),
    MREG_U32(FCFG2),
    MREG_U32(UIDH),
    MREG_U32(UIDMH),
    MREG_U32(UIDML),
    MREG_U32(UIDL),
    {0}
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_SIM_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)SIM,
    .fields = k64f_SIM_fields,
};

/*-----------------------------------------------------------------------
 *      UART
 *-----------------------------------------------------------------------*/
#define MREG_TYPE UART_Type
const mreg_field_t k64f_UART_fields[] = {
    MREG_U8(BDH),
    MREG_U8(BDL),
    MREG_U8(C1),
    MREG_U8(C2),
    MREG_U8(S1),
    MREG_U8(S2),
    MREG_U8(C3),
    MREG_U8(D),
    MREG_U8(MA1),
    MREG_U8(MA2),
    MREG_U8(C4),
    MREG_U8(C5),
    MREG_U8(ED),
    MREG_U8(MODEM),
    MREG_U8(IR),
    MREG_U8(PFIFO),
    MREG_U8(CFIFO),
    MREG_U8(SFIFO),
    MREG_U8(TWFIFO),
    MREG_U8(TCFIFO),
    MREG_U8(RWFIFO),
    MREG_U8(RCFIFO),
    MREG_U8(C7816),
    MREG_U8(IE7816),
    MREG_U8(IS7816),
    MREG_U8(WP7816T0),
    MREG_U8(WP7816T1),
    MREG_U8(WN7816),
    MREG_U8(WF7816),
    MREG_U8(ET7816),
    MREG_U8(TL7816),
    {0}
};
#undef MREG_TYPE

const mreg_struct_obj_t k64f_UART0_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART0,
    .fields = k64f_UART_fields,
};

const mreg_struct_obj_t k64f_UART1_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART1,
    .fields = k64f_UART_fields,
};

const mreg_struct_obj_t k64f_UART2_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART2,
    .fields = k64f_UART_fields,
};

const mreg_struct_obj_t k64f_UART3_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART3,
    .fields = k64f_UART_fields,
};

const mreg_struct_obj_t k64f_UART4_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART4,
    .fields = k64f_UART_fields,
};

const mreg_struct_obj_t k64f_UART5_obj = {
    .base = { &mreg_struct_type },
    .ptr = (char *)UART5,
    .fields = k64f_UART_fields,
};

/*-----------------------------------------------------------------------
 *      Module
 *-----------------------------------------------------------------------*/
STATIC const mp_rom_map_elem_t k64f_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_k64f) },
    { MP_ROM_QSTR(MP_QSTR_DAC0), MP_ROM_PTR(&k64f_DAC0_obj) },
    { MP_ROM_QSTR(MP_QSTR_DAC1), MP_ROM_PTR(&k64f_DAC1_obj) },
    { MP_ROM_QSTR(MP_QSTR_DMA), MP_ROM_PTR(&k64f_DMA_obj) },
    { MP_ROM_QSTR(MP_QSTR_DMAMUX), MP_ROM_PTR(&k64f_DMAMUX_obj) },
    { MP_ROM_QSTR(MP_QSTR_FTM0), MP_ROM_PTR(&k64f_FTM0_obj) },
    { MP_ROM_QSTR(MP_QSTR_FTM1), MP_ROM_PTR(&k64f_FTM1_obj) },
    { MP_ROM_QSTR(MP_QSTR_FTM2), MP_ROM_PTR(&k64f_FTM2_obj) },
    { MP_ROM_QSTR(MP_QSTR_FTM3), MP_ROM_PTR(&k64f_FTM3_obj) },
    { MP_ROM_QSTR(MP_QSTR_PIT), MP_ROM_PTR(&k64f_PIT_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORTA), MP_ROM_PTR(&k64f_PORTA_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORTB), MP_ROM_PTR(&k64f_PORTB_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORTC), MP_ROM_PTR(&k64f_PORTC_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORTD), MP_ROM_PTR(&k64f_PORTD_obj) },
    { MP_ROM_QSTR(MP_QSTR_PORTE), MP_ROM_PTR(&k64f_PORTE_obj) },
    { MP_ROM_QSTR(MP_QSTR_PTA), MP_ROM_PTR(&k64f_PTA_obj) },
    { MP_ROM_QSTR(MP_QSTR_PTB), MP_ROM_PTR(&k64f_PTB_obj) },
    { MP_ROM_QSTR(MP_QSTR_PTC), MP_ROM_PTR(&k64f_PTC_obj) },
    { MP_ROM_QSTR(MP_QSTR_PTD), MP_ROM_PTR(&k64f_PTD_obj) },
    { MP_ROM_QSTR(MP_QSTR_PTE), MP_ROM_PTR(&k64f_PTE_obj) },
    { MP_ROM_QSTR(MP_QSTR_SIM), MP_ROM_PTR(&k64f_SIM_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART0), MP_ROM_PTR(&k64f_UART0_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART1), MP_ROM_PTR(&k64f_UART1_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART2), MP_ROM_PTR(&k64f_UART2_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART3), MP_ROM_PTR(&k64f_UART3_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART4), MP_ROM_PTR(&k64f_UART4_obj) },
    { MP_ROM_QSTR(MP_QSTR_UART5), MP_ROM_PTR(&k64f_UART5_obj) },
};

STATIC MP_DEFINE_CONST_DICT(k64f_module_globals,
                            k64f_module_globals_table);

const mp_obj_module_t mp_module_k64f = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&k64f_module_globals,
};

#endif
