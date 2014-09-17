# -----------------------------------------------------------------------------
# MLPDF project profile
#
# \file    MLPDF.pro
# \author  Axel Newe
# \date    2014-02-06
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = MLPDF

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM zlib MLBaseListExtensions

MLAB_PACKAGES += MeVisLab_Standard MeVis_ThirdParty Community_General

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLPDF_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
  MLPDFInit.h \
  MLPDFSystem.h \
  shared/MLPDF_DataTypes.h \
  shared/MLPDF_Tools.h \
  SaveU3D/SaveU3D.h \
  SaveU3D/U3DFileFormat/U3D_BitStream.h \
  SaveU3D/U3DFileFormat/U3D_Constants.h \
  SaveU3D/U3DFileFormat/U3D_ContextManager.h \
  SaveU3D/U3DFileFormat/U3D_DataBlock.h \
  SaveU3D/U3DFileFormat/U3D_DataBlockWriter.h \
  SaveU3D/U3DFileFormat/U3D_DataTypes.h \
  SaveU3D/U3DFileFormat/U3D_FileWriter.h \
  SaveU3D/U3DFileFormat/U3D_Tools.h \
		
SOURCES += \
  MLPDFInit.cpp \
  shared/MLPDF_Tools.cpp \
  SaveU3D/SaveU3D.cpp \
  SaveU3D/SaveU3D_Tools.cpp \
  SaveU3D/SaveU3D_ProcessPointSets.cpp \
  SaveU3D/SaveU3D_ProcessLineSets.cpp \
  SaveU3D/SaveU3D_ProcessMeshes.cpp \
  SaveU3D/U3DFileFormat/U3D_BitStream.cpp \
  SaveU3D/U3DFileFormat/U3D_ContextManager.cpp \
  SaveU3D/U3DFileFormat/U3D_DataBlock.cpp \
  SaveU3D/U3DFileFormat/U3D_DataBlockWriter.cpp \
  SaveU3D/U3DFileFormat/U3D_FileWriter.cpp \
  SaveU3D/U3DFileFormat/U3D_Tools.cpp \

	