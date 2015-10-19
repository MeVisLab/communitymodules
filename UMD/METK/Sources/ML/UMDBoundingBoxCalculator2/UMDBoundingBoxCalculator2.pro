# --------- UMDBoundingBoxCalculator2 profile -------------------
#
# Platform independent project and build file for
# project UMDBoundingBoxCalculator2.
#

TEMPLATE = lib

TARGET  = UMDBoundingBoxCalculator2

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML inventor common

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += UMDBOUNDINGBOXCALCULATOR2_EXPORTS

HEADERS += \
    UMDBoundingBoxCalculator2Init.h \
    UMDBoundingBoxCalculator2System.h \
    mlUMDBoundingBoxCalculator2.h

SOURCES += \
    UMDBoundingBoxCalculator2Init.cpp \
    mlUMDBoundingBoxCalculator2.cpp
