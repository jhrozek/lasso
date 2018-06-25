#! /usr/bin/env python

import glob
import re
import sys
import os
from six import print_, StringIO

srcdir = sys.argv[1]

messages = dict()
description = ''

with open('%s/errors.h' % srcdir,'r') as f:
    for line in f:
        m = re.match(r'^ \* LASSO.*ERROR', line)
        if m:
            description = ''
            continue
        m = re.match(r'^ \* (.*[^:])$', line)
        if m:
            description += m.group(1)
        m = re.match(r'#define (LASSO_\w*ERROR\w+)', line)
        if m and description:
            description = re.sub(r'[ \n]+', ' ', description).strip()
            messages[m.group(1)] = description
            description = ''
        else:
            m = re.match(r'#define (LASSO_\w*ERROR\w+)',line)
            if m:
                messages[m.group(1)] = m.group(1)

with open('%s/errors.c.in' % srcdir,'r') as f:
    for line in f:
        if '@ERROR_CASES@' in line:
            keys = sorted(messages.keys())
            for k in keys:
                print_('		case %s:\n'
                       '			return "%s";' %
                       (k,messages[k].rstrip('\n')))
        else:
            print_(line, end="")
