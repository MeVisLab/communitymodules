# -----------------------------------------------------------------------------
# PDF3DWork project profile
#
# \file    PDF3DWork.pro
# \author  Axel Newe (axel.newe@fau.de)
# \date    2015-10-20
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = PDF3DWork

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLPDF

MLAB_PACKAGES += MeVisLab_Standard Community_General

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += PDF3DWORK_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    PDF3DWorkInit.h \
    PDF3DWorkSystem.h \
    PDF3DWork.h \


SOURCES += \
    PDF3DWorkInit.cpp \
    PDF3DWork.cpp \


# Additional files that are NOT compiled
RELATEDFILES += \
    ../Modules/PDF3DWork.def \
    ../Modules/PDF3DWork.script
