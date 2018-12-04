#!/bin/sh

${PHP7:?PHP7 variable is not defined} -n -d extension_dir=../.libs -d extension=lasso.so ${SRCDIR}binding_tests.php
