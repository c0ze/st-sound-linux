## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po

stsoundlinuxdocdir = ${prefix}/doc/stsoundlinux
stsoundlinuxdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


INTLTOOL_FILES = intltool-extract.in \
	intltool-merge.in \
	intltool-update.in

EXTRA_DIST = $(stsoundlinuxdoc_DATA) \
	$(INTLTOOL_FILES)

AM_LDFLAGS = -lasound
ACLOCAL_AMFLAGS = -I m4

DISTCLEANFILES = intltool-extract \
	intltool-merge \
	intltool-update \
	po/.intltool-merge-cache


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(stsoundlinuxdocdir)
