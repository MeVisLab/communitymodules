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
CONFIG += dll ML MLBase MLCSO MLCSOModules

MLAB_PACKAGES += Community_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLOSIRIXIMPORTER_EXPORTS

HEADERS += \
    MeVisOsiriXProxyProtocol.h \
    MeVisLab2OsiriXTBridge.h \
    SharedImagesManager.h \
    MLOsiriXImporterInit.h \
    MLOsiriXImporterSystem.h \
    mlOsiriXImporter.h \
    mlOsiriXExporter.h

OBJECTIVE_SOURCES += \
    MeVisLab2OsiriXTBridge.mm \
    SharedImagesManager.mm \
    MLOsiriXImporterInit.mm \
    mlOsiriXImporter.mm \
    mlOsiriXExporter.mm

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLOsiriXImporter/MLOsiriXImporter.def

LIBS += \
  -framework Foundation \
  -framework AppKit
