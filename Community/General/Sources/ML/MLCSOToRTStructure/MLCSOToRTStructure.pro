# -----------------------------------------------------------------------------
# MLCSOToRTStructure project profile
#
# \file    MLCSOToRTStructure.pro
# \author  linal
# \date    2010-01-20
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLCSOToRTStructure

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLCSO MLDicomTree_OFFIS MLDicomTreeImagePropertyExtension

MLAB_PACKAGES += STUMevisApp_General \
                 MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLCSOTORTSTRUCTURE_EXPORTS

HEADERS += \
    MLCSOToRTStructureInit.h \
    MLCSOToRTStructureSystem.h \
    mlCSOToRTStructure.h \
    RTStructure.h
    
SOURCES += \
    MLCSOToRTStructureInit.cpp \
    mlCSOToRTStructure.cpp \
    RTStructure.cpp
    
# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLCSOToRTStructure/MLCSOToRTStructure.def

