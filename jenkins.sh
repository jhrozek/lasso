#!/bin/bash -e
./autogen.sh
./configure --enable-gtk-doc --enable-gtk-doc-html --enable-gtk-doc-pdf
make clean
make all V=1
make check V=1
make distcheck
