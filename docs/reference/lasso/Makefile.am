# -*- mode: makefile -*-
#
# Makefile.am - template makefile for gtk-doc module
# Copyright (C) 2007-2017  Stefan Sauer
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# As a special exception, the above copyright owner gives unlimited
# permission to copy, distribute and modify this Makefile.am template.
# You need not follow the terms of the GNU General Public License when
# using or distributing such Makefile.am files, even though portions of
# the text of the Makefile.am appear in them. The GNU General Public
# License (GPL) does govern all other use of the material that constitutes
# the Makefile.am template.

# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=lasso

# Uncomment for versioned docs and specify the version of the module, e.g. '2'.
#DOC_MODULE_VERSION=2

# The top-level XML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.xml


# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting the functions and macros.
# e.g. DOC_SOURCE_DIR=../../../gtk
DOC_SOURCE_DIR=../../../lasso

# Extra options to pass to gtkdoc-scangobj. Not normally needed.
SCANGOBJ_OPTIONS=

# Extra options to supply to gtkdoc-scan.
# e.g. SCAN_OPTIONS=--deprecated-guards="GTK_DISABLE_DEPRECATED"
SCAN_OPTIONS=--ignore-decorators=LASSO_EXPORT


# Extra options to supply to gtkdoc-mkdb
# Extra options to supply to gtkdoc-mktmpl
# e.g. MKTMPL_OPTIONS=--only-section-tmpl
MKTMPL_OPTIONS=
# e.g. MKDB_OPTIONS=--xml-mode --output-format=xml
MKDB_OPTIONS=--xml-mode --output-format=xml

# Extra options to supply to gtkdoc-mkhtml
MKHTML_OPTIONS=--path=$(shell pwd)

# Extra options to supply to gtkdoc-fixref. Not normally needed.
# e.g. FIXXREF_OPTIONS=--extra-dir=../gdk-pixbuf/html --extra-dir=../gdk/html
FIXXREF_OPTIONS=

# Used for dependencies. The docs will be rebuilt if any of these change.
# e.g. HFILE_GLOB=$(top_srcdir)/gtk/*.h
# e.g. CFILE_GLOB=$(top_srcdir)/gtk/*.c
HFILE_GLOB=
CFILE_GLOB=

# Extra header to include when scanning, which are not under DOC_SOURCE_DIR
# e.g. EXTRA_HFILES=$(top_srcdir}/contrib/extra.h
EXTRA_HFILES=

# Header files to ignore when scanning. Use base file name, no paths
# e.g. IGNORE_HFILES=gtkdebug.h gtkintl.h
IGNORE_HFILES= sessionprivate.h logoutprivate.h loginprivate.h profileprivate.h \
identityprivate.h serverprivate.h providerprivate.h id_ff_extensions_private.h \
wsf_profile_private.h private.h saml_2_0_loginprivate.h \
saml_2_0_profileprivate.h saml_2_0_logoutprivate.h ecpprivate.h \
saml_2_0_serverprivate.h saml_2_0_providerprivate.h federationprivate.h \
registry-private.h id_wsf_2_0_sessionprivate.h id_wsf_2_0_serverprivate.h \
saml2_login_private.h id_wsf_2_0_private.h \
backward_comp.h id_wsf_2.h id_wsf.h lasso_config.h utils.h debug.h
# Images to copy into HTML directory.
# e.g. HTML_IMAGES=$(top_srcdir)/gtk/stock-icons/stock_about_24.png
HTML_IMAGES=

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
# e.g. content_files=running.sgml building.sgml changes-2.0.sgml
content_files=

# SGML files where gtk-doc abbrevations (#GtkWidget) are expanded
# These files must be listed here *and* in content_files
# e.g. expand_content_files=running.sgml
expand_content_files=

# CFLAGS and LDFLAGS for compiling gtkdoc-scangobj with your library.
# Only needed if you are using gtkdoc-scangobj to dynamically query widget
# signals and properties.
# e.g. GTKDOC_CFLAGS=-I$(top_srcdir) -I$(top_builddir) $(GTK_DEBUG_FLAGS)
# e.g. GTKDOC_LIBS=$(top_builddir)/gtk/$(gtktargetlib)
GTKDOC_CFLAGS=$(LASSO_CFLAGS) -I$(top_srcdir) -I$(top_builddir)
GTKDOC_LIBS= \
	$(top_builddir)/lasso/liblasso.la \
	$(LASSO_LIBS)

# This includes the standard gtk-doc make rules, copied by gtkdocize.
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
# e.g. EXTRA_DIST += version.xml.in
EXTRA_DIST += lasso-sections.txt lasso-docs.xml version.xml.in lasso.types.in style.css

# Files not to distribute
# for --rebuild-types in $(SCAN_OPTIONS), e.g. $(DOC_MODULE).types
# for --rebuild-sections in $(SCAN_OPTIONS) e.g. $(DOC_MODULE)-sections.txt
# DISTCLEANFILES =

# Generate lasso.types depending on the WSF_ENABLED flag
if WSF_ENABLED
lasso.types: lasso.types.in
	sed -e 's/#WSF# //g' < $(srcdir)/lasso.types.in > lasso.types
else
lasso.types: lasso.types.in
	grep -v '#WSF#' $(srcdir)/lasso.types.in > lasso.types
endif


# Comment this out if you want your docs-status tested during 'make check'
if ENABLE_GTK_DOC
TESTS_ENVIRONMENT = cd $(srcdir)
#TESTS = $(GTKDOC_CHECK)
endif

-include $(top_srcdir)/git.mk
