# -----------------------------------------------------------------------------
# METK2DLabeling project profile
#
# \file    METK2DLabeling.pro
# \author  Konrad Mühler
# \date    2008-04-11
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = METK2DLabeling

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr common inventor MLOpenGL SoView2D

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += METK2DLABELING_EXPORTS
DEFINES += UMDDRAW2DLABELS_EXPORTS

# postprocessing flags
MSVC_USE_SUBDIRS = 1

HEADERS += \
    METK2DLabelingInit.h \
    METK2DLabelingSystem.h \
    mlMETK2DLabeling.h \
    labField.h \    
    SoDraw2DLabels/UMDDraw2DLabelsInit.h \
    SoDraw2DLabels/UMDDraw2DLabelsSystem.h \
    SoDraw2DLabels/SoDraw2DLabels.h
    

SOURCES += \
    METK2DLabelingInit.cpp \
    mlMETK2DLabeling.cpp \
    labField.cpp \
    SoDraw2DLabels/UMDDraw2DLabelsInit.cpp \
    SoDraw2DLabels/SoDraw2DLabels.cpp

