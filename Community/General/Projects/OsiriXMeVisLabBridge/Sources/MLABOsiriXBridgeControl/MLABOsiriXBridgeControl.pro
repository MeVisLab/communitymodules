#
#  MLABOsiriXBridgeControl.pro
#  MLABOsiriXBridgeControl
#
#  Created by Felix Ritter on 01.02.10.
#  This code is in the public domain.
#

MEVIS_MAINTAINER = fritter

TEMPLATE = lib

TARGET = MLABOsiriXBridgeControl

# add dependencies of this project here

CONFIG += dll MLABBase MLABGUICore MLABFields MLABNetwork MLUtilitiesHeader MLOSXSupport
CONFIG += MLOsiriXServices

# make sure to set CONFIG and non-default MLAB_PACKAGES before the following line
include( $(MLAB_CURRENT_PACKAGE_DIR)/Configuration/DefaultProjectSetup.pri )

CONFIG += qt
QT += widgets
DEFINES += UNICODE _UNICODE

DEFINES += MEVISLAB_CORE ML_DISABLE_DEPRECATED

macos {

include( $(MLAB_MeVis_BuildSystem)/Configuration/features/OSXAddSilentQtHeaderSearchPaths.pri )

QT += macextras

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
        -framework AppKit \
        -framework Foundation

}

HEADERS += \
        mlabOsiriXBridgeControl.h

SOURCES += \
        mlabOsiriXBridgeControl.cpp
