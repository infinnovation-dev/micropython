#=======================================================================
#       Generate qstrdefs, preserving #if structure
#=======================================================================
from __future__ import print_function
import re

if_rx = re.compile(r'^\s*#\s*(if|ifdef|ifndef)\s*(.*)$')
elif_rx = re.compile(r'^\s*#\s*elif\s+(.*)')
endif_rx = re.compile(r'^\s*#\s*endif')
mp_qstr_rx = re.compile(r'\b(MP_QSTR_\w*)')

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
#	Partial data structure in sym_uses
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
#	#elif F
#	Q(e)
#	#endif
#-------------------------
#	cond:	'A || B'
#	group:	['A || B', 'C']
#	nest:	[['A || B', 'C'], ['D', 'E']]
#	uses:	list of nest, or True
class QstrParser(object):
    def __init__(self, verbose=False):
        self.verbose = verbose
        self.nest = []        # stack of group
        self.sym_uses = {}    # symbol -> [nest]

    def parse_file(self, filename):
        groups = []
        with open(filename) as f:
            lineno = 0
            for line in f:
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
                else:
                    syms = mp_qstr_rx.findall(line)
                    if syms: # 'QSTR_' in line:
                        #print(line, end='')
                        self.debug('@@ %r' % self.nest)
                        for symbol in syms:
                            self.record_qstr_use(symbol[8:])

    def record_qstr_use(self, sym):
        self.debug('Q(%s)' % sym)
        if not self.nest:
            # QSTR outside any #if
            self.sym_uses[sym] = True
        else:
            uses = self.sym_uses.get(sym, None)
            if uses is True:
                pass
            else:
                if uses is None:
                    uses = self.sym_uses[sym] = []
                # Copy groups since may be changed later
                nestcopy = [group[:] for group in self.nest]
                uses.append(nestcopy)

    def generate(self):
        self.debug('// ---------------------')
        for sym in sorted(self.sym_uses.keys()):
            uses = self.sym_uses[sym]
            if uses is True:
                print('QQQ(%s)' % sym)
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
                    print('Q(%s)' % sym)
                    first = False
                print('#endif')

    def debug(self, text):
        if self.verbose:
            print(text)
                        
if __name__=='__main__':
    import sys
    parser = QstrParser()
    for filename in sys.argv[1:]:
        parser.parse_file(filename)
    parser.generate()
