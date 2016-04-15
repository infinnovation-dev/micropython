#!/usr/bin/python
#=======================================================================
#       Manage differences between github and mbed (hg)
#=======================================================================
from __future__ import print_function
import os
import difflib

GITDIR = '/home/colin/micropython'
LIBDIR = '/home/colin/mbed/micropython-lib'
REPLDIR = '/home/colin/mbed/micropython-repl'

extmod_files = ('machine_mem.h',
                'machine_mem.c')

lib_files = ('utils/pyexec.h',
             'utils/pyexec.c',
             'mp-readline/readline.h',
             'mp-readline/readline.c')

mbed_lib_files = ('modmachine.c',
                  'modmbed.c',
                  'modmbed_i.cpp',
                  'modmbed_i.h',
                  'mpconfigport.h',
                  'mphalport.c',
                  'mphapport.h',
                  'qstrdefsport.h',
                  'unistd.h')

mbed_repl_files = ('main.cpp',)

def joinpath(*steps):
    return os.path.normpath(os.path.join(*steps))

class MbedSync(object):
    def __init__(self, gitdir, libdir, repldir):
        self.gitdir = gitdir
        self.libdir = libdir
        self.repldir = repldir

    def run(self):
        """Operate on all files mapped between the repos."""
        # Port-independent source files
        for file in iterfiles(self.gitpath('py')):
            # Use .c and .h but not .S (different assembler syntax)
            if file.endswith('.c') or file.endswith('.h'):
                self.copy(self.gitpath('py',file),
                          self.libpath('py',file))
                break
        for ext in extmod_files:
            self.copy(self.gitpath('extmod',*ext.split('/')),
                      self.libpath('extmod',*ext.split('/')))
        for lib in lib_files:
            self.copy(self.gitpath('lib', *lib.split('/')),
                      self.libpath('lib', *lib.split('/')))
        # mbed-specifics
        for lib in mbed_lib_files:
            self.copy(self.gitpath('mbed', *lib.split('/')),
                      self.libpath(*lib.split('/')))
        # mbed repl
        for file in mbed_repl_files:
            self.copy(self.gitpath('mbed', *lib.split('/')),
                      self.replpath(*lib.split('/')))

    def gitpath(self, *path):
        return joinpath(self.gitdir, *path)

    def libpath(self, *path):
        return joinpath(self.libdir, *path)

    def replpath(self, *path):
        return joinpath(self.repldir, *path)

class MbedDiff(MbedSync):
    def copy(self, apath, bpath):
        if files_equal(apath, bpath):
            print('===',apath)
        else:
            # show_diff(apath, bpath)
            print('---',apath)
            print('+++',bpath)

class MbedRdiff(MbedDiff):
    def copy(self, apath, bpath):
        MbedDiff.copy(self, bpath, apath)

def show_diff(apath, bpath):
    """Print unified diff of two files."""
    if os.path.exists(apath):
        with open(apath) as a:
            alines = a.readlines()
    else:
        apath = '/dev/null'
        alines = []
    if os.path.exists(bpath):
        with open(bpath) as b:
            blines = b.readlines()
    else:
        bpath = '/dev/null'
        blines = []
    delta = difflib.unified_diff(alines, blines,
                                 apath, bpath)
    for line in delta:
        print(line, end='')

def iterfiles(dir):
    for f in os.listdir(dir):
        if os.path.isfile(os.path.join(dir, f)):
            if f.endswith('~'):
                continue
            yield f

def files_equal(aname, bname):
    try:
        astat = os.stat(aname)
    except OSError: return False
    try:
        bstat = os.stat(bname)
    except OSError: return False
    if astat.st_size != bstat.st_size:
        return False
    with open(aname,'rb') as a:
        with open(bname,'rb') as b:
            while True:
                adata = a.read(4096)
                if not adata:
                    break
                bdata = b.read(4096)
                if adata != bdata:
                    return False
    return True

if __name__=='__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('-r','--reverse', action='store_true',
                    help='Reverse diff')
    args = ap.parse_args()
    if args.reverse:
        cls = MbedRdiff
    else:
        cls = MbedDiff
    sync = cls(GITDIR, LIBDIR, REPLDIR)
    sync.run()
