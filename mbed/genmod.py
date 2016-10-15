#=======================================================================
#	Generate boilerplate for wrapping mbed classes
#
#	module mbed:
#	- section network:
#         - class EthernetInterface
#	    - method connect
#	      - return int
#	  - class UDPSocket:
#	    - method open
#	      - arg EthernetInterface
#	    - method connect
#=======================================================================
import yaml
from contextlib import contextmanager
from warnings import warn

class Gen(object):
    def __init__(self, file=None):
	if file is None:
	    import sys
	    file = sys.stdout
	self.file = file

    #-----------------------------------------------------------------------
    #	Header file
    #-----------------------------------------------------------------------

    def h_module(self, mod):
	name = mod['name']
	self.prefix = 'mpy_%s' % name
	guard = '__MICROPY_INCLUDED_MOD%s_I_H' % name.upper()
	self.out('#ifndef %s' % guard)
	self.out('#define %s' % guard)
	for sect in mod['sections']:
	    with self.condition(sect):
		self.h_section(sect)
	self.out('')
	self.out('#endif // %s' % guard)

    def h_section(self, sect):
	for name, cls in sect.get('classes', {}).items():
	    self.h_class(cls, name)

    def h_class(self, cls, name):
	csym = '%s_%s' % (self.prefix, name)
	self.out('')
	self.out('// class %s' % name)
	self.out('extern const mp_obj_type_t %s_type;' % csym)
	self.out('extern mp_obj_t %s_make_new(const mp_obj_type_t *, mp_uint_t, mp_uint_t, const mp_obj_t *);' % csym)
	for mname, meth in cls.get('methods', {}).items():
	    self.h_method(meth, name, mname)

    def h_method(self, meth, cname, mname):
	msym = '%s_%s_%s' % (self.prefix, cname, mname)
	args = meth.get('args', [])
	nargs = len(args)
	if nargs <= 2:
	    cargs = ', '.join(['mp_obj_t self_in'] + ['mp_obj_t']*nargs)
	else:
	    cargs = 'size_t n_args, const mp_obj_t *args'
	self.out('extern mp_obj_t %s(%s);' % (msym, cargs))

    #-----------------------------------------------------------------------
    #	C module
    #-----------------------------------------------------------------------

    def c_module(self, mod):
	name = mod['name']
	self.prefix = 'mpy_%s' % name
	self.out('#include "py/runtime.h"')
	self.out('#if MICROPY_PY_%s' % name.upper())
	self.out('')
	self.out('#include "mod%s_i.h"' % name)
	for sect in mod['sections']:
	    with self.condition(sect):
		self.c_section(sect)
	self.out('')
	self.out('// Module')
	self.out('STATIC const mp_rom_map_elem_t %s_module_globals_table[] = {' %
		 self.prefix)
	self.out('    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_%s) },' % name)
	for sect in mod['sections']:
	    with self.condition(sect):
		for cname, cls in sect.get('classes', {}).items():
		    self.out('    { MP_ROM_QSTR(MP_QSTR_%s), MP_ROM_PTR(&%s_%s_type) },' %
			     (cname, self.prefix, cname))
	self.out('};')
	self.out('')
	self.out('STATIC MP_DEFINE_CONST_DICT(%s_module_globals,' %
		 self.prefix)
	self.out('                            %s_module_globals_table);' %
		 self.prefix)
	self.out('')
	self.out('const mp_obj_module_t mp_module_%s = {' % name)
	self.out('    .base = { &mp_type_module },')
	self.out('    .name = MP_QSTR_%s,' % name)
	self.out('    .globals = (mp_obj_dict_t *)&%s_module_globals,' % self.prefix)
	self.out('};')
	self.out('')
	self.out('#endif // MICROPY_PY_%s' % name.upper())

    def c_section(self, sect):
	for name, cls in sect.get('classes', {}).items():
	    self.c_class(cls, name)

    def c_class(self, cls, name):
	csym = '%s_%s' % (self.prefix, name)
	self.out('')
	self.out('// class %s' % name)
	for mname, meth in cls.get('methods', {}).items():
	    self.c_define_method(meth, name, mname)
	self.out('')
	self.out('STATIC const mp_map_elem_t %s_locals_dict_table[] = {' % csym)
	for mname, meth in cls.get('methods', {}).items():
	    self.out('  { MP_ROM_QSTR(MP_QSTR_%s), (mp_obj_t)&%s_%s_obj },' %
		     (mname, csym, mname))
	self.out('};')
	self.out('')
	self.out('STATIC MP_DEFINE_CONST_DICT(%s_locals_dict,' % csym)
	self.out('                            %s_locals_dict_table);' % csym)
	self.out('')
	self.out('const mp_obj_type_t %s_type = {' % csym)
	self.out('    { &mp_type_type },')
	self.out('    .name = MP_QSTR_%s,' % name)
	self.out('    .make_new = %s_make_new,' % csym)
	self.out('    .locals_dict = (mp_obj_t)&%s_locals_dict,' % csym)
	self.out('};')

    def c_define_method(self, meth, cname, mname):
	msym = '%s_%s_%s' % (self.prefix, cname, mname)
	args = meth.get('args', [])
	nargs = len(args)
	if nargs <= 2:
	    self.out('STATIC MP_DEFINE_CONST_FUN_OBJ_%d(%s_obj,' %
		     (1+nargs, msym))
	    self.out('                                 %s);' % msym)
	else:
	    self.out('STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(%s_obj, %d, %d,' %
		     (msym, nargs, nargs))
	    self.out('                                           %s);' % msym)

    #-----------------------------------------------------------------------
    #	C++ glue
    #-----------------------------------------------------------------------
    
    def cpp_module(self, mod):
	name = mod['name']
	self.prefix = 'mpy_%s' % name
	self.out('extern "C" {')
	self.out('    #include "py/mpconfig.h"')
	self.out('}')
	self.out('#if MICROPY_PY_%s' % name.upper())
	for inc in mod.get('include', []):
	    self.out('#include %s' % inc)
	self.out('extern "C" {')
	self.out('    #include "py/runtime.h"')
	self.out('    #include "mod%s_i.h"' % name)
	self.out('}')
	for sect in mod['sections']:
	    with self.condition(sect):
	        for cname, cls in sect.get('classes', {}).items():
                    self.cpp_class_struct(cls, cname)
	for sect in mod['sections']:
	    with self.condition(sect):
		self.cpp_section(sect)
	self.out('#endif // MICROPY_PY_%s' % name.upper())

    def cpp_class_struct(self, cls, name):
	csym = '%s_%s' % (self.prefix, name)
	self.out('struct %s_obj_t {' % csym)
	self.out('    mp_obj_base_t base;')
	self.out('    %s *cpp;' % name)
	self.out('};')

    def cpp_section(self, sect):
	for name, cls in sect.get('classes', {}).items():
	    self.cpp_class(cls, name)

    def cpp_class(self, cls, name):
	csym = '%s_%s' % (self.prefix, name)
	#args = [a.split()  for a in cls.get('args', [])]
        args = [Arg(a) for a in cls.get('args', [])]
	nargs = len(args)
	self.out('')
	self.out('// ----------------------------------------')
	self.out('// class %s' % name);
	self.out('// ----------------------------------------')
	self.out('mp_obj_t %s_make_new(const mp_obj_type_t *type,' % csym)
	self.out('        mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {')
	self.out('    (void)type;')
	self.out('    mp_arg_check_num(n_args, n_kw, %d, %d, false);' %
		 (nargs, nargs))
        #	for i, (aname, atype) in enumerate(args):
        #	    self.cpp_arg_in(aname, atype, 'args[%d]' % i)
        for i, arg in enumerate(args):
            self.cpp_arg_in(arg.name, arg.type, 'args[%d]' % i)
	self.out('    %s_obj_t *o =' % csym)
	self.out('        m_new_obj_with_finaliser(%s_obj_t);' % csym)
	self.out('    o->base.type = &%s_type;' % csym)
	cppargs = ', '.join([arg.cpparg  for arg in args])
	self.out('    o->cpp = new %s(%s);' % (name, cppargs))
	self.out('    return o;')
	self.out('}')
	for mname, meth in cls.get('methods', {}).items():
	    self.cpp_method(meth, name, mname)

    def cpp_method(self, meth, cname, mname):
	csym = '%s_%s' % (self.prefix, cname)
	msym = '%s_%s_%s' % (self.prefix, cname, mname)
	#args = [a.split()  for a in meth.get('args', [])]
        args = [Arg(a)  for a in meth.get('args', [])]
	nargs = len(args)
	ret = meth.get('ret')
	self.out('')
	cargs = ['mp_obj_t self_in']
	if nargs <= 2:
	    for arg in args:
		cargs.append('mp_obj_t %s_in' % arg.name)
	else:
	    raise NotImplementedError('> 3 args')
	self.out('mp_obj_t %s(%s) {' % (msym, ', '.join(cargs)))
	self.out('    %s_obj_t *self = (%s_obj_t *)self_in;' % (csym, csym))
	if nargs <= 2:
	    for arg in args:
		self.cpp_arg_in(arg.name, arg.type, arg.name+'_in')
	else:
	    for i, (aname, atype) in enumerate(args):
		self.cpp_arg_in(aname, atype, 'args[%d]' % i)
	cppargs = ', '.join(['%s.buf, %s.len' % (arg.name, arg.name)  if arg.type=='buffer'  else arg.cpparg
                             for arg in args])
        if 'code' in meth:
            for line in meth['code']:
                self.out('    %s' % line)
        else:
            if ret:
                if ret == 'int':
                    decl = 'int ret'
                    ret = 'MP_OBJ_NEW_SMALL_INT(ret)'
                elif ret == 'string':
                    decl = 'const char *ret'
                    ret = 'mp_obj_new_str(ret, strlen(ret), false)'
                else:
                    warn('return type %s for %s.%s' % (ret, cname, mname))
                    decl = 'FIXME'
                    ret = 'FIXME(ret)'
                lhs = '%s = ' % decl
            else:
                lhs = ''
                ret = 'mp_const_none'
            self.out('    %sself->cpp->%s(%s);' % (lhs, mname, cppargs))
            self.out('    return %s;' % ret)
	self.out('}')

    def cpp_arg_in(self, aname, atype, var):
	if atype == 'int':
	    self.out('    int %s = mp_obj_get_int(%s);' % (aname, var))
	elif atype == 'bool':
	    self.out('    int %s = mp_obj_is_true(%s);' % (aname, var))
	elif atype == 'string':
	    self.out('    const char *%s = mp_obj_str_get_str(%s);' %
		     (aname, var))
        elif atype == 'buffer':
            self.out('    mp_buffer_info_t %s;' % aname)
            self.out('    mp_get_buffer_raise(%s, &%s, MP_BUFFER_READ);' %
                     (var, aname))
        elif atype[0].isupper()  and  atype != 'SPECIAL':
            csym = '%s_%s' % (self.prefix, atype)
            self.out('    if (! MP_OBJ_IS_TYPE(%s, &%s_type)) {' %
                     (var, csym))
            self.out('        nlr_raise(mp_obj_new_exception_msg(&mp_type_TypeError, "%s required"));' % atype)
            self.out('    }')
            self.out('    %s *%s = ((%s_obj_t *)%s)->cpp;' %
                     (atype, aname, csym, var))
	else:
            warn('arg type %s for %s' % (atype, var))
	    self.out('    FIXME %s <- %s;' % (aname, var))

    #-----------------------------------------------------------------------
    #	Internals
    #-----------------------------------------------------------------------
    def out(self, line):
	print >>self.file, line

    @contextmanager
    def condition(self, obj):
	cond = obj.get('condition')
	if cond:
	    self.out('#if %s' % cond)
	    yield
	    self.out('#endif // %s' % cond)
	else:
	    yield

class Arg(object):
    def __init__(self, obj):
        if isinstance(obj, dict):
            self.d = obj.copy()
        else:
            n, t = obj.split()
            self.d = dict(name=n, type=t)

    @property
    def name(self):
        return self.d['name']

    @property
    def type(self):
        return self.d['type']

    @property
    def cpparg(self):
        return self.d.get('cpparg', self.name)

if __name__=='__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('-a','--all', action='store_true')
    ap.add_argument('-H','--header', action='store_true')
    ap.add_argument('-m','--module', action='store_true')
    ap.add_argument('-i','--implementation', action='store_true')
    ap.add_argument('yamlfile')
    args = ap.parse_args()
    with open(args.yamlfile) as f:
	obj = yaml.load(f)
    if all:
	stub = 'mod%s' % obj['name']
	with open(stub+'_i.h','w') as f:
	    Gen(f).h_module(obj)
	with open(stub+'_i.cpp','w') as f:
	    Gen(f).cpp_module(obj)
	with open(stub+'.c','w') as f:
	    Gen(f).c_module(obj)
    else:
	gen = Gen()
	if args.header:
	    gen.h_module(obj)
	elif args.module:
	    gen.c_module(obj)
	elif args.implementation:
	    gen.cpp_module(obj)
