#! /usr/bin/env python
#
# Checks a documentation file for functions that do not exist

import os
import re
import sys
from six import print_

functions = {}
for filename in os.listdir('.'):
    if filename[-4:] not in ('.txt', '.rst'):
        continue
    with open(filename) as f:
        for line in f:
            if not 'lasso_' in line:
                continue
            if not '(' in line:
                continue
            for f in re.findall(r'(lasso_[a-zA-Z_]+?)\(', line):
                functions[f] = 1

with open('../reference/build/lasso-decl-list.txt') as f:
    known_symbols = [x.strip() for x in f]

failure = 0
for f in functions:
    if not f in known_symbols:
        print_(f)
        failure = 1

sys.exit(failure)

