# -----------------------------------------------------------------------------
# MLCMCurveList project profile
#
# \file    MLCMCurveList.pro
# \author  Reinhard Hameeteman
# \date    2010-03-05
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = MLCMCurveList

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here

CONFIG += dll ML MLBase boost 

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLCMCURVELIST_EXPORTS

HEADERS += \
    MLCMCurveListInit.h \
    MLCMCurveListSystem.h \
    CMCurveFilter/mlCMCurveFilter.h \
    StringToCurve/mlStringToCurve.h \

SOURCES += \
    MLCMCurveListInit.cpp \
    CMCurveFilter/mlCMCurveFilter.cpp \
    StringToCurve/mlStringToCurve.cpp \

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLCMCurveList/MLCMCurveList.def






