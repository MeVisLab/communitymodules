# -----------------------------------------------------------------------------
# UMDSoExtractColoredFaces project profile
#
# \file    UMDSoExtractColoredFaces.pro
# \author  Konrad Mühler
# \date    2008-02-01
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDSoExtractColoredFaces

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

CONFIG += dll inventor MLOpenGL

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# set high warn level (warn 4 on MSCV)
WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSOEXTRACTCOLOREDFACES_EXPORTS

HEADERS += \
    UMDSoExtractColoredFacesInit.h \
    UMDSoExtractColoredFacesSystem.h \
    SoExtractColoredFaces.h

SOURCES += \
    UMDSoExtractColoredFacesInit.cpp \
    SoExtractColoredFaces.cpp