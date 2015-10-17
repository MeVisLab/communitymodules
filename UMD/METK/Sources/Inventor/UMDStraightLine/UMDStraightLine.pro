# -----------------------------------------------------------------------------
# UMDStraightLine project profile
#
# \file    UMDStraightLine.pro
# \author  kmuehler
# \date    2008-08-06
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDStraightLine

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor MLOpenGL

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += UMDSTRAIGHTLINE_EXPORTS

HEADERS += \
    UMDStraightLineInit.h \
    UMDStraightLineSystem.h \
    SoStraightLine.h

SOURCES += \
    UMDStraightLineInit.cpp \
    SoStraightLine.cpp
