# **InsertLicense** code
# -----------------------------------------------------------------------------
# MLCSOCommunityModules profile
#
# \file    MLCSOCommunityModules.pro
# \author  Bart De Dobbelaer
# \date    05/2009
# -----------------------------------------------------------------------------

TARGET = MLCSOCommunityModules

DESTDIR = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add dependencies of this project here

CONFIG += dll ML MLBase MLTools MathUtils newmat MLCSO boost

MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += MLCSOCOMMUNITYMODULES_EXPORTS

MSVC_USE_SUBDIRS = 1

HEADERS += \
    MLCSOCommunityModulesInit.h \
    MLCSOCommunityModulesSystem.h \
    CSODistance\CSODistance.h

SOURCES += \
    MLCSOCommunityModulesInit.cpp \
    CSODistance\CSODistance.cpp

RELATEDFILES += \
    ../../../Modules/ML/MLCSOCommunityModules/MLCSOCommunityModules.def 

