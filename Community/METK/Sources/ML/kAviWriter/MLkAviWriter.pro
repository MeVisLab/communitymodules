# --------- MLkAviWriter profile -------------------
#
# Platform independent project and build file for
# project MLkAviWriter.
#
# Last changed by $Author: barann $
# $Id: MLkAviWriter.pro 46495 2006-11-23 12:27:35Z barann $
# $Source$
# --------------------------------------------------

TARGET  = MLkAviWriter

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

CONFIG += dll ML

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


win32:LIBS  += vfw32.lib Gdi32.lib User32.lib


DEFINES += MLKAVIWRITER_EXPORTS UNICODE

HEADERS += \
    MLkAviWriterInit.h \
    MLkAviWriterSystem.h \
    netsrc/aviUtils.h \
    mlkAviWriter.h

SOURCES += \
    MLkAviWriterInit.cpp \
    netsrc/aviUtils.cpp \
    mlkAviWriter.cpp