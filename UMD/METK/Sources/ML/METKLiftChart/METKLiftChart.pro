# --------- UMDInventorPointer profile -------------------
#
# Platform independent project and build file for
# project METKLiftChart.
#

TEMPLATE = lib

TARGET  = METKLiftChart

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor MLOpenGL opengl SoView2D MLLUT SoUtils

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += METKLIFTCHART_EXPORTS

HEADERS += \
   METKLiftChartInit.h \
   METKLiftChartSystem.h \
   mlMETKLiftChart.h \
   SoView2DLiftChart.h \
   LiftChartData.h

SOURCES += \
   METKLiftChartInit.cpp \
   mlMETKLiftChart.cpp \
   SoView2DLiftChart.cpp \
   LiftChartData.cpp
