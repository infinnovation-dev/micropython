#!/usr/bin/env python
#
# The MIT License (MIT)
#
# Copyright (c) 2016 Colin Hogben
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
"""
Manage differences between github and mbed (hg)

Allow viewing differences and synchronisong between git and hg
repositories, from the perspective of the git repository.
"""
from __future__ import print_function
import os
import difflib
import shutil
import subprocess

# Default locations
GITDIR = '/home/colin/micropython'
HGPARENT = '/home/colin/mbed'
branch = subprocess.check_output(['git','rev-parse','--abbrev-ref','HEAD'],
                                 cwd=GITDIR).strip()
if branch.startswith('mbed-'):
    # Each sub-branch has its own hg repo
    REPLDIR = HGPARENT + '/micropython-' + branch[5:]
    LIBDIR = REPLDIR + '/micropython'
else:
    LIBDIR = HGPARENT + '/micropython-dev'
    REPLDIR = HGPARENT + '/micropython-repl'

# Sets of files to be synced
extmod_files = ('machine_mem.h',
                'machine_mem.c')

lib_files = ('utils/pyexec.h',
             'utils/pyexec.c',
             'utils/pyhelp.h',
             'utils/pyhelp.c',
             'utils/printf.c',
             'mp-readline/readline.h',
             'mp-readline/readline.c')

mbed_lib_files = ('modmachine.c',
                  'modmbed.c',
                  'modmbed_i.cpp',
                  'modmbed_i.h',
                  'modpins.c',
                  'modk64f.c',
                  'mreg.h',
                  'mreg.c',
                  'mpconfigport.h',
                  'mphalport.c',
                  'mphalport.h',
                  'qstrdefsport.h',
                  'unistd.h')

mbed_repl_files = ('main.cpp',
                   'help.c')

mbed_gen_files = (('mbedpins.h','mbedpins.h'),
                  ('qstrdefscond.h','genhdr/qstrdefs.generated.h'),
                  ('build/genhdr/mpversion.h', 'genhdr/mpversion.h'))

other_lib_files = (('README-mbed.md', 'README.md'),)

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
        for file in extmod_files:
            self.copy(self.gitpath('extmod',*file.split('/')),
                      self.libpath('extmod',*file.split('/')))
        for file in lib_files:
            self.copy(self.gitpath('lib', *file.split('/')),
                      self.libpath('lib', *file.split('/')))
        # mbed-specifics
        for file in mbed_lib_files:
            self.copy(self.gitpath('mbed', *file.split('/')),
                      self.libpath(*file.split('/')))
        for afile, bfile in other_lib_files:
            self.copy(self.gitpath('mbed', *afile.split('/')),
                      self.libpath(*bfile.split('/')))
        # mbed repl
        for file in mbed_repl_files:
            self.copy(self.gitpath('mbed', *file.split('/')),
                      self.replpath(*file.split('/')))

        # other files generated in git dir
        for afile, bfile in mbed_gen_files:
            self.a_master(self.gitpath('mbed', *afile.split('/')),
                          self.libpath(*bfile.split('/')))

    def gitpath(self, *path):
        return joinpath(self.gitdir, *path)

    def libpath(self, *path):
        return joinpath(self.libdir, *path)

    def replpath(self, *path):
        return joinpath(self.repldir, *path)

class MbedDiff(MbedSync):
    def copy(self, apath, bpath):
        if files_equal(apath, bpath):
            # print('===',apath)
            pass
        else:
            # print('---',apath)
            # print('+++',bpath)
            show_diff(bpath, apath)

    a_master = copy

class MbedRdiff(MbedDiff):
    def copy(self, apath, bpath):
        MbedDiff.copy(self, bpath, apath)

    a_master = copy

class MbedPull(MbedSync):
    def copy(self, apath, bpath):
        if not files_equal(apath, bpath):
            # print('cp %s %s' % (bpath, apath))
            shutil.copyfile(bpath, apath)

    def a_master(self, apath, pbath):
        pass

class MbedPush(MbedSync):
    def copy(self, apath, bpath):
        if not files_equal(apath, bpath):
            # print('cp %s %s' % (apath, bpath))
            shutil.copyfile(apath, bpath)

    a_master = copy

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

def joinpath(*steps):
    return os.path.normpath(os.path.join(*steps))

def iterfiles(dir):
    for f in os.listdir(dir):
        if os.path.isfile(os.path.join(dir, f)):
            if f.endswith('~'): # Emacs backup
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
                bdata = b.read(4096)
                if adata != bdata:
                    return False
                if not adata:
                    # Equal if b at EOF too
                    return not bdata
                if not bdata:
                    return False
    return True

if __name__=='__main__':
    import argparse
    ap = argparse.ArgumentParser()
    ap.add_argument('-g','--git-dir', default=GITDIR,
                    help='Git repo')
    ap.add_argument('-l','--lib-dir', default=LIBDIR,
                    help='micropython library hg repo')
    ap.add_argument('-r','--repl-dir', default=REPLDIR,
                    help='micropython repl hg repo')
    act = ap.add_subparsers(dest='action', metavar='action', help='Action')
    diff = act.add_parser('diff', help='Show differences')
    rdiff = act.add_parser('rdiff', help='Show reverse differences')
    pull = act.add_parser('pull', help='Pull from hg repos into git')
    push = act.add_parser('push', help='Push from git to hg repos')
    args = ap.parse_args()
    if args.action == 'diff':
        cls = MbedDiff
    elif args.action == 'rdiff':
        cls = MbedRdiff
    elif args.action == 'pull':
        cls = MbedPull
    elif args.action == 'push':
        cls = MbedPush
    sync = cls(args.git_dir, args.lib_dir, args.repl_dir)
    sync.run()
