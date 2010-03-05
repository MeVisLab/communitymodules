# -----------------------------------------------------------------------------
# MLXMarkerListCommunityModules project profile
#
# \file    MLXMarkerListCommunityModules.pro
# \author  Coert Metz
# \date    2010-03-05
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLXMarkerListCommunityModules

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLTools

MLAB_PACKAGES += Community_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLXMARKERLISTCOMMUNITYMODULES_EXPORTS

HEADERS += \
    MLXMarkerListCommunityModulesInit.h \
    MLXMarkerListCommunityModulesSystem.h \
    mlTransformXMarkerlist.h \
    mlLinearInterpolateXMarkerList.h \
    mlXMarkerListFromFile.h \
    mlXMarkerListToFile.h
    

SOURCES += \
    MLXMarkerListCommunityModulesInit.cpp \
    mlTransformXMarkerlist.cpp \
    mlLinearInterpolateXMarkerList.cpp \
    mlXMarkerListFromFile.cpp \
    mlXMarkerListToFile.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLXMarkerListCommunityModules/MLXMarkerListCommunityModules.def

