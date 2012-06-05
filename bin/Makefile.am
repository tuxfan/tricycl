#------------------------------------------------------------------------------#
#------------------------------------------------------------------------------#

bin_PROGRAMS = poisson

INCLUDES = -I${top_srcdir}/src/include \
	-I${top_builddir}/local \
	@EXTRA_CPPFLAGS@

poisson_SOURCES = @top_builddir@/bin/poisson.c
poisson_LDFLAGS = @EXTRA_LDFLAGS@
poisson_LDADD = @EXTRA_LIBS@ ${top_builddir}/lib/libtricycl.la
