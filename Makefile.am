bin_PROGRAMS = atc
atc_SOURCES = atc.c
man_MANS = atc.1

install-exec-local:
	ln -s $(bindir)/atc $(bindir)/second_align

uninstall-hook:
	rm $(bindir)/second_align

MAINTAINERCLEANFILES = aclocal.m4 compile depcomp install-sh missing configure config.h.in Makefile.in
