# -----------------------------------------------------------------------------
# SavePDFTemplate project profile
#
# \file    SavePDFTemplate.pro
# \author  Axel Newe (axel.newe@fau.de)
# \date    2015-10-20
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = SavePDFTemplate

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLPDF

MLAB_PACKAGES += MeVisLab_Standard Community_General

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += SAVEPDFTEMPLATE_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    SavePDFTemplateInit.h \
    SavePDFTemplateSystem.h \
    SavePDFTemplate.h \


SOURCES += \
    SavePDFTemplateInit.cpp \
    SavePDFTemplate.cpp \


# Additional files that are NOT compiled
RELATEDFILES += \
    ../Modules/SavePDFTemplate.def \
    ../Modules/SavePDFTemplate.script
