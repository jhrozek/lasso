#!/bin/bash -e
set -e
for python_version in `ls /usr/bin/python*.* | grep 'python[0-9]\+\.[0-9]\+$'`; do
	echo Building for $python_version
	./autogen.sh --enable-gtk-doc --enable-gtk-doc-html --enable-gtk-doc-pdf --with-python=$python_version
	make clean
	make all V=1
	make check V=1
	make distcheck
done
