# --------- UMDSoSceneWriterMD profile -------------------
#
# Platform independent project and build file for
# project UMDSoSceneWriterMD.
#

TEMPLATE = lib

TARGET  = UMDSoSceneWriterMD

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML inventor MLOpenGL

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSOSCENEWRITERMD_EXPORTS

HEADERS += \
    UMDSoSceneWriterMDInit.h \
    UMDSoSceneWriterMDSystem.h \
    mlSoSceneWriterMD.h

SOURCES += \
    UMDSoSceneWriterMDInit.cpp \
    mlSoSceneWriterMD.cpp
    