# --------- CVLAscendingAortaDetection profile -------------------
#
# Platform independent project and build file for
# project CVLAscendingAortaDetection.
#

TEMPLATE = lib

TARGET  = CVLAscendingAortaDetection

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here

CONFIG += dll ML MLTools MLBase

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += CVLASCENDINGAORTADETECTION_EXPORTS

HEADERS += \
    CVLAscendingAortaDetectionInit.h \
    CVLAscendingAortaDetectionSystem.h \
    mlAscendingAortaDetection.h 

SOURCES += \
    CVLAscendingAortaDetectionInit.cpp \
    mlAscendingAortaDetection.cpp 

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/CVLAscendingAortaDetection/CVLAscendingAortaDetection.def


