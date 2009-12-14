# -----------------------------------------------------------------------------
# This file is part of the FuzzyConnectedness module for MeVisLab.
#
# Copyright (c) 2001-2009, Fraunhofer MEVIS, Bremen, Germany
# ALL RIGHTS RESERVED

# The FuzzyConnectedness module for MeVisLab is free software:
# you can redistribute it and/or modify it under the terms of the 
# GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# The FuzzyConnectedness module for MeVisLab is distributed in 
# the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
# for more details.

# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

# -----------------------------------------------------------------------------
# MLFuzzyConnectedness project profile
#
# \file    MLFuzzyConnectedness.pro
# \author  Markus Hüllebrand
# \date    2009-06-12
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLFuzzyConnectedness

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = MAXIMUM64

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML

#MLAB_PACKAGES += 

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLFUZZYCONNECTEDNESS_EXPORTS

HEADERS += \
    MLFuzzyConnectednessSystem.h \
    mlFuzzyConnectedness.h \
    CMIVFuzzyConnectedness.h

SOURCES += \
    MLFuzzyConnectednessInit.cpp \
    mlFuzzyConnectedness.cpp \
    CMIVFuzzyConnectedness.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLFuzzyConnectedness/MLFuzzyConnectedness.def

