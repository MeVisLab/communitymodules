# --------- UMDStippling profile -------------------

TEMPLATE = lib

TARGET  = UMDStippling

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
# RP: neues Paket SoShader
CONFIG += dll inventor MLOpenGL SoShader

WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


win32::LIBS += glaux.lib 
#win32::LIBS += advapi32.lib


DEFINES += UMDSTIPPLING_EXPORTS

HEADERS += \
    UMDStipplingInit.h \
    UMDStipplingSystem.h \
    SoStippling.h



SOURCES += \
    UMDStipplingInit.cpp \
    SoStippling.cpp


# additional files that are NOT compiled
RELATEDFILES += \
    fragment.glsl \
    vertex.glsl

