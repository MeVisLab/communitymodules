# --------- UMDTools profile -------------------

TEMPLATE = lib

TARGET  = UMDTools

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor MLOpenGL UMDSoMeasurement

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

WARN = HIGH

DEFINES += UMDTOOLS_EXPORTS

HEADERS += \
   UMDToolsInit.h \
   UMDToolsSystem.h \
   SoClearGLBuffers.h \
   SoFileToSceneGraph.h \
   SoFramesPerSecond.h \
   SoMinimalEnclosingCircle.h \
   SoMousePosition.h \
   SoShapeInfo.h \
   SoShapePartition.h

SOURCES += \
   UMDToolsInit.cpp \
   SoClearGLBuffers.cpp \
   SoFileToSceneGraph.cpp \
   SoFramesPerSecond.cpp \
   SoMinimalEnclosingCircle.cpp \
   SoMousePosition.cpp \
   SoShapeInfo.cpp \
   SoShapePartition.cpp

