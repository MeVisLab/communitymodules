# --------- UMDCodedSegmentation profile -------------------
#
# Platform independent project and build file for
# project UMDCodedSegmentation.
#

TEMPLATE = lib

TARGET  = UMDCodedSegmentation

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML common inventor MLTools

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDCODEDSEGMENTATION_EXPORTS

HEADERS += \
    UMDCodedSegmentationInit.h \
    UMDCodedSegmentationSystem.h \
    mlCalcCodedSegmentation.h

SOURCES += \
    UMDCodedSegmentationInit.cpp \
    mlCalcCodedSegmentation.cpp

