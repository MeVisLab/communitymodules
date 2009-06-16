# --------- UMDObjDemo profile -------------------
#
# Platform independent project and build file for
# project UMDObjDemo.
#

TEMPLATE = lib

TARGET  = UMDObjDemo

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += UMDOBJDEMO_EXPORTS

HEADERS += \
    UMDObjDemoInit.h \
    UMDObjDemoSystem.h \
    mlUMDObjDemo.h

SOURCES += \
    UMDObjDemoInit.cpp \
    mlUMDObjDemo.cpp

