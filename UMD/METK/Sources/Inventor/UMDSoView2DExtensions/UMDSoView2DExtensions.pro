# --------- UMDPickMask profile -------------------

TEMPLATE = lib

TARGET  = UMDSoView2DExtensions

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor SoView2D ML MLLUT SoUtils

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDSOVIEW2DEXTENSIONS_EXPORTS

HEADERS += \
    UMDSoView2DExtensionsInit.h \
    UMDSoView2DExtensionsSystem.h \
    SoView2DHalo.h \
    SoTextureMaskedArea.h \

SOURCES += \
    UMDSoView2DExtensionsInit.cpp \
    SoView2DHalo.cpp \
    SoTextureMaskedArea.cpp \

