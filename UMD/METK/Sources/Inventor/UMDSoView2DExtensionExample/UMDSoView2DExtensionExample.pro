# -----------------------------------------------------------------------------
# UMDSoView2DExtensionExample project profile
#
# \file    UMDSoView2DExtensionExample.pro
# \author  Konrad Mühler
# \date    2008-01-07
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDSoView2DExtensionExample

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor MLOpenGL SoView2D

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSOVIEW2DEXTENSIONEXAMPLE_EXPORTS

HEADERS += \
    UMDSoView2DExtensionExampleInit.h \
    UMDSoView2DExtensionExampleSystem.h \
    SoView2DExtensionExample.h

SOURCES += \
    UMDSoView2DExtensionExampleInit.cpp \
    SoView2DExtensionExample.cpp
