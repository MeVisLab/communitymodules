# -----------------------------------------------------------------------------
# GraphToFibers project profile
#
# \file    GraphToFibers.pro
# \author  Axel Newe
# \date    2016-03-31
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = GraphToFibers

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLVesselGraph MLFiberUtils MLPDF MLU3D

MLAB_PACKAGES += Community_General MeVisLab_Standard FMEstable_Release

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += GRAPHTOFIBERS_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    GraphToFibersInit.h \
    GraphToFibersSystem.h \
    mlGraphToFibers.h \

SOURCES += \
    GraphToFibersInit.cpp \
    mlGraphToFibers.cpp \