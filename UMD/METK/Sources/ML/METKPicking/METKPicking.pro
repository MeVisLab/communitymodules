# --------- UMDPicking profile -------------------
#
# Platform independent project and build file for
# project METKPicking.
#

TEMPLATE = lib

TARGET  = METKPicking

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr common inventor

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)



DEFINES += METKPICKING_EXPORTS

HEADERS += \
    ../METK.h \
    METKPickingInit.h \
    METKPickingSystem.h \
    mlMETKPicking.h \
    SoPickObject.h

SOURCES += \
    METKPickingInit.cpp \
    mlMETKPicking.cpp \
    SoPickObject.cpp
