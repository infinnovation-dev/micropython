#include "py/mpstate.h"
#include "extmod/machine_mem.h"

#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_sim.h"

// machine.freq()
STATIC mp_obj_t machine_freq(size_t n_args, const mp_obj_t *args) {
    clock_name_t clock = kCLOCK_CoreSysClk;
    if (n_args > 0) {
        clock = (clock_name_t) mp_obj_get_int(args[0]);
    }
    return mp_obj_new_int_from_uint(CLOCK_GetFreq(clock));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_freq_obj, 0, 1, machine_freq);

// machine.reset()
STATIC NORETURN mp_obj_t machine_reset(void) {
    NVIC_SystemReset();
    for (;;);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

// machine.unique_id()
STATIC mp_obj_t machine_unique_id(void) {
    sim_uid_t uid;
    SIM_GetUniqueId(&uid);
    return mp_obj_new_bytes((byte *)&uid, sizeof(uid));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);

STATIC const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_machine) },

    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },

    { MP_ROM_QSTR(MP_QSTR_reset),               MP_ROM_PTR(&machine_reset_obj) },
#ifdef DEBUG
    { MP_ROM_QSTR(MP_QSTR_info),                MP_ROM_PTR(&machine_info_obj) },
#endif
    { MP_ROM_QSTR(MP_QSTR_freq),                MP_ROM_PTR(&machine_freq_obj) },
    { MP_ROM_QSTR(MP_QSTR_unique_id),           MP_ROM_PTR(&machine_unique_id_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_idle),                MP_ROM_PTR(&machine_idle_obj) },
    //{ MP_ROM_QSTR(MP_QSTR_sleep),               MP_ROM_PTR(&machine_sleep_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};
