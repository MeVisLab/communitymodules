# --------- UMDInventorPointer profile -------------------
#
# Platform independent project and build file for
# project UMDInventorPointer.
#

TEMPLATE = lib

TARGET  = UMDInventorPointer

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor common

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += UMDINVENTORPOINTER_EXPORTS

HEADERS += \
    UMDInventorPointerInit.h \
    UMDInventorPointerSystem.h \
    mlUMDPointerToInventor.h \
    mlUMDInventorToPointer.h

SOURCES += \
    UMDInventorPointerInit.cpp \
    mlUMDPointerToInventor.cpp \
    mlUMDInventorToPointer.cpp