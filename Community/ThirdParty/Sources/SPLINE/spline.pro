# -----------------------------------------------------------------------------
# SPLINE Library of John Burkhardt, modified to be linkable as shared library
#
# \file    spline.pro
# \author  Ruben Stein
# \date    2010-07-01
#
# Original SPLINE Sources:
# http://people.sc.fsu.edu/~jburkardt/cpp_src/spline/spline.html
#
# License: LGPLv3 (see COPYING and COPYING.LESSER)
# 
# -----------------------------------------------------------------------------

MEVIS_MAINTAINER = ruben.stein

TEMPLATE = lib

TARGET  = SPLINE

DESTDIR    = ../../lib
DLLDESTDIR = ../../lib

CONFIG += dll

#needed to set system/configuration dependent library suffixes etc.
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += SPLINE_EXPORTS

HEADERS += spline.h

SOURCES += spline.cpp
