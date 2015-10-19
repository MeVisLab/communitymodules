# -----------------------------------------------------------------------------
# UMDPickInfo project profile
#
# \file    UMDPickInfo.pro
# \author  Stefan Hiller
# \date    2007-09-11
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDPickInfo

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

CONFIG += dll inventor MLOpenGL

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDPICKINFO_EXPORTS

HEADERS += \
    UMDPickInfoInit.h \
    UMDPickInfoSystem.h \
    SoPickInfo.h

SOURCES += \
    UMDPickInfoInit.cpp \
    SoPickInfo.cpp


