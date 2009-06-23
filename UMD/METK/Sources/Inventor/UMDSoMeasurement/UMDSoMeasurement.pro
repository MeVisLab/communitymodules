# --------- UMDtemp profile -------------------

TEMPLATE = lib

TARGET  = UMDSoMeasurement

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLOpenGL MLBase SoView2D inventor UMDQHull3D

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSOMEASUREMENT_EXPORTS

# Add your project headers here
HEADERS += UMDMainAxis.h \
  UMDMeasurementInit.h \
  UMDMinimalDistance.h \
  UMDSoAngleLines.h \
  UMDSoAngleToObjects.h \
  UMDSoArrow.h \
  UMDSoAutoMeasureTool.h \
  UMDSoCake.h \
  UMDSoDistanceLine.h \
  UMDSoMainAxis.h \
  UMDSoMaximumExtension.h \
  UMDSoMeasureText.h \
  UMDSoMeasureTool.h \
  UMDSoMinimalDistance.h \
  SoVerticalRuler.h \
  UMDSoRuler.h \
  UMDSoShapeToPointSet.h \
  UMDSoThresholdToPointSet.h \
  UMDSoVolEstimation.h

# Add your project cpp files here
SOURCES += initDll.cpp \
  UMDMainAxis.cpp \
  UMDMinimalDistance.cpp \
  UMDSoAngleLines.cpp \
  UMDSoAngleToObjects.cpp \
  UMDSoArrow.cpp \
  UMDSoAutoMeasureTool.cpp \
  UMDSoCake.cpp \
  UMDSoDistanceLine.cpp \
  UMDSoMainAxis.cpp \
  UMDSoMaximumExtension.cpp \
  UMDSoMeasureText.cpp \
  UMDSoMeasureTool.cpp \
  UMDSoMinimalDistance.cpp \
  SoVerticalRuler.cpp \
  UMDSoRuler.cpp \
  UMDSoShapeToPointSet.cpp \
  UMDSoThresholdToPointSet.cpp \
  UMDSoVolEstimation.cpp