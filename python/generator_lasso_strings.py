#! /usr/bin/env python
#
# $Id$ 
# 
# PyLasso - Python bindings for Lasso library
# Automatic generation of lasso_strings.py with lasso/xml/strings.c
#
# Copyright (C) 2004 Entr'ouvert
# http://lasso.entrouvert.org
#
# Author: Valery Febvre <vfebvre@easter-eggs.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import re, sys, string

MATCH_BLANK   = re.compile(r'^\s*\n$')
MATCH_COMMENT = re.compile(r'^[ /]+(?P<text>.*)\n$')
MATCH_CONST   = re.compile(r'^const\s+xmlChar\s+(?P<name>\w+)\[\]\s*=\s+(?P<value>.*)\n$')

header = """# $%s$
# 
# PyLasso - Python bindings for Lasso library
#
# Copyright (C) 2004 Entr'ouvert
# http://lasso.entrouvert.org
#
# Author: Valery Febvre <vfebvre@easter-eggs.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
""" % "Id"

#file_in = sys.argv[1]
file_in  = "../lasso/xml/strings.c"
file_out = "./lasso_strings.py"

fd_in  = open(file_in,  "r")
fd_out = open(file_out, "w+")
fd_out.write(header)

line  = fd_in.readline()
while line:
    match = MATCH_CONST.search(line)
    if match:
        name = match.group('name')[5:]
        name = string.lower(name[0]) + name[1:]
        value = match.group('value')[:-1]
        fd_out.write("%s = %s\n" % (name, value))
    else:
        match = MATCH_COMMENT.search(line)
        if match:
            fd_out.write("# %s\n" % match.group('text'))
        else:
            match = MATCH_BLANK.search(line)
            if match:
                fd_out.write("\n")
            else:
                print "Unknown format, this line is ignored: %s" % line
    line  = fd_in.readline()

fd_in.close()
fd_out.close()
