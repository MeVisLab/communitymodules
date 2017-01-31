#
#  MLOsiriXServices.pro
#  MLOsiriXServices
#
#  Created by Felix Ritter on 18.01.08.
#  This code is in the public domain.
#

MEVIS_MAINTAINER = fritter

TEMPLATE = lib

TARGET = MLOsiriXServices

# add dependencies of this project here

CONFIG += dll

# make sure to set CONFIG and non-default MLAB_PACKAGES before the following line
include( $(MLAB_CURRENT_PACKAGE_DIR)/Configuration/DefaultProjectSetup.pri )

INCLUDEPATH += ..

HEADERS += \
    mlOsiriXServices.h \
    mlOsiriXServicesProtocol.h

OBJECTIVE_SOURCES += \
    mlOsiriXServices.m

LIBS += \
    -framework Foundation
