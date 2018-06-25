#!/usr/bin/env python

import sys
import os.path
import re
from six import print_

if len(sys.argv) < 3:
    print_("Usage: check-lasso-sections.py "
           "lasso docs/referenrece/lasso/lasso-sections.txt",
           file=sys.stderr)
    sys.exit(1)
source=sys.argv[1]
lasso_sections_txt=sys.argv[2]

methods=[]

for dirpath, dirnames, filenames in os.walk(source):
    for filename in filenames:
        _, ext = os.path.splitext(filename)
        with open(os.path.join(dirpath, filename)) as f:
            lines = f.readlines()
        while lines:
            line, lines = lines[0], lines[1:]
            line=line.strip()
            if line.startswith('LASSO_EXPORT'):
                while not ';' in line:
                    line=line.strip()
                    line, lines = line + lines[0], lines[1:]
                line=line.strip()
                m=re.match(r'LASSO_EXPORT[^(]*[ \t*](\w+)\s*\(', line)
                if m:
                    methods.append(m.group(1))

lasso_sections_txt=file(lasso_sections_txt).read()

print_(' = Methods missing from lasso-sections.txt =\n')
for method in methods:
    if not method in lasso_sections_txt:
        print_(method)

print_(' = Methods in lasso-sections.txt which does not exist anymore = \n')
for line in lasso_sections_txt.splitlines():
    if line.startswith('lasso_'):
        if line not in methods:
            print_(line)
