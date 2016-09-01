#
#  MLABOsiriXBridgeControl.pro
#  MLABOsiriXBridgeControl
#
#  Created by Felix Ritter on 18.01.08.
#  This code is in the public domain.
#

TEMPLATE = lib

TARGET = MLABOsiriXBridgeControl

# add dependencies of this project here

CONFIG += dll MLABBase MLABGUICore MLABFields MLABNetwork MLUtilitiesHeader MLOSXSupport
CONFIG += MLOsiriXServices

# make sure to set CONFIG and non-default MLAB_PACKAGES before the following line
include ($(MLAB_Community_General)/Configuration/DefaultProjectSetup.pri)

CONFIG += qt
QT += widgets macextras
DEFINES += UNICODE _UNICODE

DEFINES += MEVISLAB_CORE ML_DISABLE_DEPRECATED

macx {

HEADERS += \
        mlabOsiriXBridgeCocoaWindowDelegate.h \
        mlabOsiriXBridgeWindowDelegate.h \
        OsiriXPasteboardMime.h

OBJECTIVE_SOURCES += \
        mlabOsiriXBridgeCocoaWindowDelegate.m \
        mlabOsiriXBridgeWindowDelegate.mm

SOURCES += \
        OsiriXPasteboardMime.cpp

LIBS += \
        -framework Cocoa

}

HEADERS += \
        mlabOsiriXBridgeControl.h

SOURCES += \
        mlabOsiriXBridgeControl.cpp
