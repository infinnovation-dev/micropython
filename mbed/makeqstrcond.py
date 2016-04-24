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
Generate qstrdefs, preserving #if structure
"""
from __future__ import print_function
import sys
sys.path.append('../py')
from makeqstrdata import qstr_escape, make_bytes
import re

BYTES_IN_LEN = 1
BYTES_IN_HASH = 2

if_rx = re.compile(r'^\s*#\s*(if|ifdef|ifndef)\s+(.*)$')
elif_rx = re.compile(r'^\s*#\s*elif\s+(.*)')
endif_rx = re.compile(r'^\s*#\s*endif')
mp_qstr_rx = re.compile(r'\b(MP_QSTR_\w*)')
Q_rx = re.compile(r'^Q\((.*)\)$')

def matches(line, rx, matches):
    m = rx.match(line)
    if not m:
        return False
    matches[:] = m.groups()
    return True

#	Example source code
#	-------------------
#	#if A || B
#	  MP_QSTR_ab
#	#elif C
#	  MP_QSTR_c
#	  #if D
#	    MP_QSTR_d
#	  #elif E
#	    MP_QSTR_e
#	  #endif
#	  #if F
#	    #if G
#	      MP_QSTR_e
#	    #endif
#	  #endif
#	#endif
#	#if F
#	  MP_QSTR_e
#	#endif
#
#	Partial data structure in qstr_uses
#	----------------------------------
#	e -> [
#	  [
#	    ['A || B', 'C'],
#	    ['D', 'E'],
#	  ],
#	  [
#	    ['A || B', 'C'],
#	    ['F'],
#	    ['G'],
#	  ],
#	  [
#	    ['F'],
#	  ],
#	]
#
#	Output code
#	-----------
#	#if !(A || B) && (C) && !(D) && E
#	Q(e)
#	#if !(A || B) && (C) && (F) && (G)
#	Q(e)
#	#elif F
#	Q(e)
#	#endif
#-------------------------
#	cond:	'A || B'
#	group:	['A || B', 'C']
#	nest:	[['A || B', 'C'], ['D', 'E']]
#	uses:	list of nest, or True

class QstrParser(object):
    def __init__(self, name='QstrParser', verbose=False):
        self.name = name
        self.verbose = verbose
        self.nest = []        # stack of group
        self.qstr_uses = {}    # symbol -> [nest]

    def parse_file(self, filename):
        groups = []
        with open(filename) as f:
            lineno = 0
            for line in iter_full_lines(f):
                line = line.rstrip()
                lineno += 1
                if matches(line, if_rx, groups):
                    cmd, cond = groups
                    cond = cond.rstrip()
                    if cmd == 'ifdef':
                        cond = 'defined(%s)' % cond
                    elif cmd == 'ifndef':
                        cond = '!defined(%s)' % cond
                    group = [cond]
                    self.nest.append(group)
                    self.debug(line.rstrip())
                elif matches(line, elif_rx, groups):
                    cond, = groups
                    cond = cond.rstrip()
                    group = self.nest[-1]
                    group.append(cond)
                    self.debug(line.rstrip())
                elif matches(line, endif_rx, groups):
                    if not self.nest:
                        raise Exception('%s:%d: Nesting error' % (filename, lineno))
                    self.nest.pop()
                    self.debug(line.rstrip())
                elif matches(line, Q_rx, groups):
                    qstr, = groups
                    self.record_qstr_use(qstr)
                else:
                    syms = mp_qstr_rx.findall(line)
                    if syms:    # 'MP_QSTR_' in line:
                        #print(line, end='')
                        self.debug('@@ %r' % self.nest)
                        for symbol in syms:
                            qstr = symbol[8:]
                            self.record_qstr_use(qstr)

    def record_qstr_use(self, qstr):
        self.debug('Q(%s)' % qstr)
        if not self.nest:
            # QSTR outside any #if
            self.qstr_uses[qstr] = True
        else:
            uses = self.qstr_uses.get(qstr, None)
            if uses is True:
                pass
            else:
                if uses is None:
                    uses = self.qstr_uses[qstr] = []
                # Copy groups since may be changed later
                nestcopy = [group[:] for group in self.nest]
                uses.append(nestcopy)

    def generate(self):
        self.debug('// ---------------------')
        print('// Generated by %s' % self.name)
        # Always generate NULL qstr with no hash or data as first item
        self.gen_qdef_NULL()
        id_q = dict([(qstr_escape(qstr), qstr)
                     for qstr in self.qstr_uses
                     if qstr != 'NULL'])
        for ident in sorted(id_q.keys()):
            qstr = id_q[ident]
            uses = self.qstr_uses[qstr]
            if uses is True:
                self.gen_qdef(ident, qstr)
            else:
                self.debug('// uses: %r' % uses)
                first = True
                for nest in uses:
                    self.debug('//     nest: %r' % nest)
                    conds = []
                    for group in nest:
                        self.debug('//       group: %r' % group)
                        for cond in group[:-1]:
                            conds.append('!(%s)' % cond)
                        conds.append('(%s)' % group[-1])
                    ucond = ' && '.join(conds)
                    print('#%s %s' % ('if' if first else 'elif', ucond))
                    self.gen_qdef(ident, qstr)
                    first = False
                print('#endif')

    def gen_qdef_NULL(self):
        qbytes = '(const byte *)"%s%s" ""' % \
                 ('\\x00' * BYTES_IN_HASH,
                  '\\x00' * BYTES_IN_LEN)
        print('QDEF(MP_QSTR_%s, %s)' % ('NULL', qbytes))
                      
    def gen_qdef(self, ident, qstr):
        qbytes = make_bytes(BYTES_IN_LEN,
                            BYTES_IN_HASH,
                            qstr)
        print('QDEF(MP_QSTR_%s, %s)' % (ident, qbytes))

    def debug(self, text):
        if self.verbose:
            print(text)

def iter_full_lines(f):
    """Unsplit lines broken with backslash-newline."""
    parts = []
    for line in f:
        line = line.rstrip('\n').rstrip('\r')
        if line.endswith('\\'):
            parts.append(line[:-1])
        else:
            parts.append(line)
            yield ''.join(parts)
            parts = []
    # Mop up any trailing incomplete line
    if parts:
        yield ''.join(parts)

if __name__=='__main__':
    import sys
    parser = QstrParser()
    for filename in sys.argv[1:]:
        parser.parse_file(filename)
    parser.generate()
