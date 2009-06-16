# --------- UMDMETKMessaging profile -------------------
#
# Platform independent project and build file for
# project UMDMETKMessaging.
#

TEMPLATE = lib

TARGET  = METKMessaging

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor

# set high warn level (warn 4 on MSCV)
#WARN = HIGH

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += METKMESSAGING_EXPORTS

HEADERS += \
    ../METK.h \
    METKMessagingInit.h \
    METKMessagingSystem.h \
    METKMsgManager.h \
    mlMETKMsgReceiver.h \
    mlMETKMsgSender.h \
    mlMETKGlobalMessages.h

SOURCES += \
    METKMessagingInit.cpp \
    METKMsgManager.cpp \
    mlMETKMsgReceiver.cpp \
    mlMETKMsgSender.cpp \
    mlMETKGlobalMessages.cpp
