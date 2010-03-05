# -----------------------------------------------------------------------------
# MLCMFields project profile
#
# \file    MLCMFields.pro
# \author  Reinhard Hameeteman
# \date    2010-03-05
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = MLCMFields

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here

CONFIG += dll ML

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLCMFIELDS_EXPORTS

HEADERS += \
    MLCMFieldsInit.h \
    MLCMFieldsSystem.h \
    SyncFields/mlSyncFields.h \

SOURCES += \
    MLCMFieldsInit.cpp \
    SyncFields/mlSyncFields.cpp \

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLCMFields/MLCMFields.def


