# --------- UMDMETKMessaging profile -------------------
#
# Platform independent project and build file for
# project METKSurfaceDistance.
#

TEMPLATE = lib

TARGET  = METKSurfaceDistance

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

DEFINES += METKSURFACEDISTANCE_EXPORTS

HEADERS += \
    ../METK.h \
    METKSurfaceDistanceInit.h \
    METKSurfaceDistanceSystem.h \
    mlMETKSurfaceDistance3D.h \
	SoColorShape.h

SOURCES += \
    METKSurfaceDistanceInit.cpp \
    mlMETKSurfaceDistance3D.cpp \
	SoColorShape.cpp