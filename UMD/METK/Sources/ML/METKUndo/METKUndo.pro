# -----------------------------------------------------------------------------
# METKUndo project profile
#
# \file    METKUndo.pro
# \author  kmuehler
# \date    2008-06-19
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = METKUndo

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr common inventor

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += METKUNDO_EXPORTS

HEADERS += \
    METKUndoInit.h \
    METKUndoSystem.h \
    mlMETKUndo.h

SOURCES += \
    METKUndoInit.cpp \
    mlMETKUndo.cpp

