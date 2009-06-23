# -----------------------------------------------------------------------------
# UMDRegionDistanceTransformation project profile
#
# \file    UMDRegionDistanceTransformation.pro
# \author  Konrad Muehler
# \date    2009-03-17
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDRegionDistanceTransformation

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLTools MLSmallImageInterface common METK2DLabeling MLOpenGL inventor

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDREGIONDISTANCETRANSFORMATION_EXPORTS

HEADERS += \
    UMDRegionDistanceTransformationInit.h \
    UMDRegionDistanceTransformationSystem.h \
    mlRegionDistanceTransformation.h

SOURCES += \
    UMDRegionDistanceTransformationInit.cpp \
    mlRegionDistanceTransformation.cpp

