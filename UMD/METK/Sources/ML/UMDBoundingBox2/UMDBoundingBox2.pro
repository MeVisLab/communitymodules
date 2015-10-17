# --------- UMDBoundingBox2 profile -------------------
#
# Platform independent project and build file for
# project UMDBoundingBox2.
#

TEMPLATE = lib

TARGET  = UMDBoundingBox2

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor common

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# set high warn level (warn 4 on MSCV)
#WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDBOUNDINGBOX2_EXPORTS

HEADERS += \
    UMDBoundingBox2Init.h \
    UMDBoundingBox2System.h \
    mlUMDBoundingBox2.h

SOURCES += \
    UMDBoundingBox2Init.cpp \
    mlUMDBoundingBox2.cpp