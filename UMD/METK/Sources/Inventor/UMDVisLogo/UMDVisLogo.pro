# -----------------------------------------------------------------------------
# UMDVisLogo project profile
#
# \file    UMDVisLogo.pro
# \author  Konrad Mühler
# \date    2007-06-12
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = UMDVisLogo

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib


# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor MLOpenGL png

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# set high warn level (warn 4 on MSCV)
WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += UMDVISLOGO_EXPORTS

HEADERS += \
    UMDVisLogoInit.h \
    UMDVisLogoSystem.h \
    SoVisLogo.h \

SOURCES += \
    UMDVisLogoInit.cpp \
    SoVisLogo.cpp \

