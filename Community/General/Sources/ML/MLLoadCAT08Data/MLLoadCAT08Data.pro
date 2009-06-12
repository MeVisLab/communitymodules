# Copyright (c) 2009, Fraunhofer MEVIS Institute for Medical Image Computing
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
# * Neither the name of the Fraunhofer MEVIS nor the
# names of its contributors may be used to endorse or promote products
# derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR THE COPYRIGHT HOLDER BE LIABLE 
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# -----------------------------------------------------------------------------
# MLLoadCAT08Data project profile
#
# \file    MLLoadCAT08Data.pro
# \author  Ola Friman
# \date    2009-05-20
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLLoadCAT08Data

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# set high warn level (warn 4 on MSCV)
WARN = MAXIMUM64

# add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase 

exists( $${PACKAGE_SOURCES}/ML/MLVesselGraph/mlGraph.h ) {
  CONFIG += MLVesselGraph
  DEFINES += ML_GRAPH_IS_AVAILABLE
}

MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLLOADCAT08DATA_EXPORTS

HEADERS += \
    MLLoadCAT08DataInit.h \
    MLLoadCAT08DataSystem.h \
    mlLoadCAT08Data.h

SOURCES += \
    MLLoadCAT08DataInit.cpp \
    mlLoadCAT08Data.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLLoadCAT08Data/MLLoadCAT08Data.def

