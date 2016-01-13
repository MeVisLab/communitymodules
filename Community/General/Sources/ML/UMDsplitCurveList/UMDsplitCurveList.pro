# -----------------------------------------------------------------------------
# UMDsplitCurveList project profile
#
# \file    UMDsplitCurveList.pro
# \author  Tobias Mönch
# \date    2009-08-05
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = UMDsplitCurveList

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase

MLAB_PACKAGES += Community_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSPLITCURVELIST_EXPORTS

HEADERS += \
    UMDsplitCurveListInit.h \
    UMDsplitCurveListSystem.h \
    mlUMDsplitCurveList.h

SOURCES += \
    UMDsplitCurveListInit.cpp \
    mlUMDsplitCurveList.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/UMDsplitCurveList/UMDsplitCurveList.def

