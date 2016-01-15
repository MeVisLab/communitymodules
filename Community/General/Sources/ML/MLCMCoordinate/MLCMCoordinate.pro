# -----------------------------------------------------------------------------
# MLCMCoordinate project profile
#
# \file    MLCMCoordinate.pro
# \author  Reinhard Hameeteman
# \date    2010-03-05
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = MLCMCoordinate

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here

CONFIG += dll ML MLTools

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLCMCOORDINATE_EXPORTS

HEADERS += \
    MLCMCoordinateInit.h \
    MLCMCoordinateSystem.h \
    BoxArithmetic/mlBoxArithmetic.h \
    WorldInfo/mlWorldInfo.h \

SOURCES += \
    MLCMCoordinateInit.cpp \
    BoxArithmetic/mlBoxArithmetic.cpp \
    WorldInfo/mlWorldInfo.cpp \

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLCMCoordinate/MLCMCoordinate.def
