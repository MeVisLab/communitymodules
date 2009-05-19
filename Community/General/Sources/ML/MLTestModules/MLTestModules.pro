# -----------------------------------------------------------------------------
# MLTestModules project profile
#
# \file    MLTestModules.pro
# \author  Wolf Spindler
# \date    2009-05-19
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLTestModules

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

DEFINES += MLTESTMODULES_EXPORTS

HEADERS += \
    MLTestModulesInit.h \
    MLTestModulesSystem.h \
    mlNOP.h

SOURCES += \
    MLTestModulesInit.cpp \
    mlNOP.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLTestModules/MLTestModules.def

