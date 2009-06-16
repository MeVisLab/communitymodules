# --------- common profile -------------------
#
# Platform independent project and build file for
# project common.
#

TEMPLATE   = lib

TARGET  = common

DESTDIR    = ../../lib
DLLDESTDIR = ../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor xerces ML MLObjMgr
MLAB_PACKAGES += MeVisLab_Standard

# set high warn level (warn 4 on MSCV)
WARN = HIGH

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += common_EXPORTS

HEADERS += \    
    HashTable.h \
    kBasics.h \
    kCamera.h \
    #kObjMgrAttr.h \
    StdAfx.h \
    StrList.h \
    XMLParser.h \
    kSphere.h \
    kDebug.h \
    ObjMgrCommunicator.h \
    matrix.h \
    image_basic.h

SOURCES += \
    kBasics.cpp \
    kCamera.cpp \
    StdAfx.cpp \    
    XMLParser.cpp \
    kSphere.cpp \
    kDebug.cpp \
    ObjMgrCommunicator.cpp \
    image_basic.cpp