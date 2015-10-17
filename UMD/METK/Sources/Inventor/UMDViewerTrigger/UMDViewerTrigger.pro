# --------- UMDViewerTrigger profile -------------------

TEMPLATE = lib

TARGET  = UMDViewerTrigger

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor opengl

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDVIEWERTRIGGER_EXPORTS

HEADERS += \
    UMDViewerTriggerInit.h \
    UMDViewerTriggerSystem.h \
    SoUMDViewerTrigger.h

SOURCES += \
    UMDViewerTriggerInit.cpp \
    SoUMDViewerTrigger.cpp

