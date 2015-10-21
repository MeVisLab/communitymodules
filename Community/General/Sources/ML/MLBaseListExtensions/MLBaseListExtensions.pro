# -----------------------------------------------------------------------------
# MLBaseListExtensions project profile
#
# \file    MLBaseListExtensions.pro
# \author  Axel Newe
# \date    2014-09-01
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = MLBaseListExtensions

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here
CONFIG += dll ML MLTools MLBase MLLUT

MLAB_PACKAGES += MeVisLab_Standard

# Set maximum warn level with checks for 64 bit problems
WARN = MAXIMUM64

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

# Disable deprecated ML API
# DEFINES += ML_DISABLE_DEPRECATED

DEFINES +=  MLBASELISTEXTENSIONS_EXPORTS


HEADERS += \
    MLBaseListExtensionsInit.h \
    MLBaseListExtensionsSystem.h \
    shared/MLBaseListExtensions_DataTypes.h \
    shared/MLBaseListExtensions_Tools.h \
    IndexPairList/IndexPairList.h \
    ColoredMarkerList/ColoredMarkerList.h \
    ColoredMarkerList/XMarkerToColoredMarker.h \
    ColoredMarkerList/ColoredMarkerToXMarker.h \
    ListTools/ListInfo.h \
    ListTools/ListFilter.h \
    ListTools/ListModifyProperties.h \
    MarkerListImport/MarkerListImport.h \


SOURCES += \
    MLBaseListExtensionsInit.cpp \
    IndexPairList/IndexPairList.cpp \
    shared/MLBaseListExtensions_Tools.cpp \
    ColoredMarkerList/ColoredMarkerList.cpp \
    ColoredMarkerList/XMarkerToColoredMarker.cpp \
    ColoredMarkerList/ColoredMarkerToXMarker.cpp \
    ListTools/ListInfo.cpp \
    ListTools/ListFilter.cpp \
    ListTools/ListModifyProperties.cpp \
    MarkerListImport/MarkerListImport.cpp \
