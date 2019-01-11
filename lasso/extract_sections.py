#! /usr/bin/env python

import io
import re
import sys
import os
import os.path
from six import print_

enable_wsf = False

if '-wsf' in sys.argv:
    enable_wsf = True

if len(sys.argv) == 2+enable_wsf:
    srcdir = sys.argv[1]
else:
    srcdir = '.'

for root, dirs, files in os.walk(srcdir):
    prefixes = list()
    for file in files:
        if file.endswith('.c'):
            prefixes.append(os.path.splitext(file)[0])
    for prefix in prefixes:
        try:
            header = io.open(os.path.join(root, prefix + '.h'), encoding='utf-8').read()
            implementation = io.open(os.path.join(root, prefix + '.c'), encoding='utf-8').read()
            exported_functions = re.findall('LASSO_EXPORT.*(lasso_\w*)', header)
            normal_functions = sorted ([ x for x in exported_functions if not x.endswith('get_type') ])
            get_type = [ x for x in exported_functions if x.endswith('get_type') ][0]
            file_name = re.findall('lasso_(.*)_get_type', get_type)[0]
            try:
                macro_type = re.findall('LASSO_(\w*)_CLASS\(', header)[0]
            except:
                macro_type = None
            try:
                type = re.findall(r'^struct _(Lasso\w*)', header, re.MULTILINE)[0]
            except:
                type = None
            types = re.findall('^} (Lasso\w*);', header)
            def convert(x):
                if '%s' in x:
                    return x % macro_type
                else:
                    return x
            if type and macro_type:
                standard_decl = [ convert(x) for x in [ 'LASSO_%s', 'LASSO_IS_%s', 'LASSO_TYPE_%s', get_type, 'LASSO_%s_CLASS', 'LASSO_IS_%s_CLASS', 'LASSO_%s_GET_CLASS' ] ]
                print_('')
                print_('<SECTION>')
                print_('<FILE>%s</FILE>' % file_name)
                print_('<TITLE>%s</TITLE>' % type)
                print_(type)
                for x in types + normal_functions:
                    print_(x)
                print_('<SUBSECTION Standard>')
                for x in standard_decl:
                    print_(x)
                print_('</SECTION>')
        except:
            continue

