# -----------------------------------------------------------------------------
# MLWEMColorizeNodes project profile
#
# \file
# \author  Axel Newe
# \date    2015-12-03
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = MLWEMColorizeNodes

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM MLLUT

MLAB_PACKAGES += Community_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLWEMCOLORIZENODES_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    MLWEMColorizeNodesInit.h \
    MLWEMColorizeNodesSystem.h \
    WEMColorizeNodes.h \

SOURCES += \
    MLWEMColorizeNodesInit.cpp \
   WEMColorizeNodes.cpp \