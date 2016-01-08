# -----------------------------------------------------------------------------
# MLVectorcardiogram project profile
#
# \file    MLVectorcardiogram.pro
# \author  Axel Newe
# \date    2015-09-25
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = MLVectorcardiogram

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLFiberUtils MLBase MLBaseListExtensions MLWEM

MLAB_PACKAGES += MeVisLab_Standard FMEStable_Release Community_General

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLVECTORCARDIOGRAM_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    MLVectorcardiogramInit.h \
    MLVectorcardiogramSystem.h \
    shared/MLVectorcardiogram_Datatypes.h \
    shared/MLVectorcardiogram_Tools.h \
    VCGLoad/VCGLoad.h \
    VCGCoordinateSystem/VCGCoordinateSystem.h \
    VCGLineSetDefinition/VCGLineSetDefinition.h \


SOURCES += \
    MLVectorcardiogramInit.cpp \
    shared/MLVectorcardiogram_Tools.cpp \
    VCGLoad/VCGLoad.cpp \
    VCGCoordinateSystem/VCGCoordinateSystem.cpp \
    VCGLineSetDefinition/VCGLineSetDefinition.cpp \
