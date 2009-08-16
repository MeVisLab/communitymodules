# -----------------------------------------------------------------------------
# MLOsiriXImporter project profile
#
# \file    MLOsiriXImporter.pro
# \author  Chunliang Wang
# \date    2009-07-07
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLOsiriXImporter

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML

MLAB_PACKAGES += Community_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLOSIRIXIMPORTER_EXPORTS

HEADERS += \
    MLOsiriXImporterInit.h \
    MLOsiriXImporterSystem.h \
    mlOsiriXImporter.h \
    mlOsiriXExporter.h

SOURCES += \
    MLOsiriXImporterInit.mm \
    mlOsiriXImporter.mm \
    mlOsiriXExporter.mm

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLOsiriXImporter/MLOsiriXImporter.def

