# -----------------------------------------------------------------------------
# MLU3D project profile
#
# \file    MLU3D.pro
# \author  Axel Newe (axel.newe@fau.de)
# \date    2016-10-01
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLU3D

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM MLFiberUtils MLBaseListExtensions MLTools inventor png

MLAB_PACKAGES += MeVisLab_Standard MeVis_ThirdParty Community_General FMEstable_Release

INCLUDEPATH += ./abstract
INCLUDEPATH += ./abstract/BaseModules
INCLUDEPATH += ./modules
INCLUDEPATH += ./modules/auxiliary
INCLUDEPATH += ./shared
INCLUDEPATH += ./shared/U3DFileFormat
INCLUDEPATH += ./shared/U3DInternalFormat

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLU3D_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
  MLU3DInit.h \
  MLU3DSystem.h \
  abstract/abstract.h \
  abstract/BaseModules/U3DModule.h \
  abstract/BaseModules/U3DGenerator.h \
  abstract/BaseModules/U3DProcessor.h \
  abstract/BaseModules/U3DInspector.h \
  shared/MLU3D_Constants.h \
  shared/MLU3D_Tools.h \
  shared/MLU3D_DataTypes.h \
  shared/MLU3D_MarkerListTools.h \
  shared/MLU3D_SpecificationGenerator.h \
  shared/U3DFileFormat/U3D_BitStream.h \
  shared/U3DFileFormat/U3D_DataBlock.h \
  shared/U3DFileFormat/U3D_FileWriter.h \
  shared/U3DFileFormat/U3D_ContextManager.h \
  shared/U3DFileFormat/U3D_DataBlockWriter.h \
  shared/U3DInternalFormat/U3D_Object.h \
  shared/U3DInternalFormat/U3D_Object_DataTypes.h \
  shared/U3DInternalFormat/U3D_Object_Nodes.h \
  shared/U3DInternalFormat/U3D_Object_Modifiers.h \
  shared/U3DInternalFormat/U3D_Object_Resources.h \
  shared/U3DInternalFormat/U3D_Object_GeometryGenerators.h \
  modules/modules.h \
  modules/U3DInit/U3DInit.h \
  modules/U3DInfo/U3DInfo.h \
  modules/U3DLoad/U3DLoad.h \
  modules/U3DSave/U3DSave.h \
  modules/U3DManager/U3DManager.h \
  modules/U3DAssemble/U3DAssemble.h \
  modules/U3DAddModel/U3DAddModelFromWEM.h \
  modules/U3DAddModel/U3DAddModelFromFiberSet.h \
  modules/U3DAddModel/U3DAddModelFromMarkerList.h \
  modules/U3DAddTexture/U3DAddTextureFromFile.h \
  modules/U3DAddTexture/U3DAddTextureFromImage.h \
  modules/U3DAddTexture/U3DAddVolumeFromView.h \
  modules/U3DGeometryTo/U3DGeometryToXMarkerList.h \
  modules/U3DGeometryTo/U3DGeometryToFiberSet.h \
  modules/U3DGeometryTo/U3DGeometryToWEM.h \
  modules/U3DSpecificationGenerator/U3DSpecificationGenerator.h \
  modules/auxiliary/ListsToFibers.h \
  modules/auxiliary/LoadPointLineGeometry.h \

SOURCES += \
  MLU3DInit.cpp \ 
  abstract/abstract.cpp \
  abstract/BaseModules/U3DModule.cpp \
  abstract/BaseModules/U3DGenerator.cpp \
  abstract/BaseModules/U3DProcessor.cpp \
  abstract/BaseModules/U3DInspector.cpp \
  shared/MLU3D_Tools.cpp \
  shared/MLU3D_MarkerListTools.cpp \
  shared/MLU3D_SpecificationGenerator.cpp \
  shared/U3DFileFormat/U3D_BitStream.cpp \
  shared/U3DFileFormat/U3D_DataBlock.cpp \
  shared/U3DFileFormat/U3D_FileWriter.cpp \
  shared/U3DFileFormat/U3D_ContextManager.cpp \
  shared/U3DFileFormat/U3D_DataBlockWriter.cpp \
  shared/U3DInternalFormat/U3D_Object.cpp \
  shared/U3DInternalFormat/U3D_Object_Nodes.cpp \
  shared/U3DInternalFormat/U3D_Object_Modifiers.cpp \
  shared/U3DInternalFormat/U3D_Object_Resources.cpp \
  shared/U3DInternalFormat/U3D_Object_GeometryGenerators.cpp \
  modules/modules.cpp \
  modules/U3DInit/U3DInit.cpp \
  modules/U3DInfo/U3DInfo.cpp \
  modules/U3DLoad/U3DLoad.cpp \
  modules/U3DSave/U3DSave.cpp \
  modules/U3DSave/U3DSave_Nodes.cpp \
  modules/U3DSave/U3DSave_Resources.cpp \
  modules/U3DSave/U3DSave_GeometryGenerators.cpp \
  modules/U3DManager/U3DManager.cpp \
  modules/U3DAssemble/U3DAssemble.cpp \
  modules/U3DAssemble/U3DAssemble_Tools.cpp \
  modules/U3DAddModel/U3DAddModelFromWEM.cpp \
  modules/U3DAddModel/U3DAddModelFromFiberSet.cpp \
  modules/U3DAddModel/U3DAddModelFromMarkerList.cpp \
  modules/U3DAddTexture/U3DAddTextureFromFile.cpp \
  modules/U3DAddTexture/U3DAddTextureFromImage.cpp \
  modules/U3DAddTexture/U3DAddVolumeFromView.cpp \
  modules/U3DGeometryTo/U3DGeometryToXMarkerList.cpp \
  modules/U3DGeometryTo/U3DGeometryToFiberSet.cpp \
  modules/U3DGeometryTo/U3DGeometryToWEM.cpp \
  modules/U3DSpecificationGenerator/U3DSpecificationGenerator.cpp \
  modules/auxiliary/ListsToFibers.cpp \
  modules/auxiliary/LoadPointLineGeometry.cpp \

# Additional files that are NOT compiled
RELATEDFILES += \
  ../Modules/MLU3D.def \
  ../Modules/U3DInit.script \
  ../Modules/U3DInfo.script \
  ../Modules/U3DLoad.script \
  ../Modules/U3DSave.script \
  ../Modules/U3DAssemble.script \
  ../Modules/U3DInspector.script \
  ../Modules/U3DAddModelFromFiberSet.script \
  ../Modules/U3DAddModelFromMarkerList.script \
  ../Modules/U3DAddModelFromWEM.script \
  ../Modules/U3DGeometryToXMarkerList.script \
  ../Modules/U3DGeometryToFiberSet.script \
  ../Modules/U3DGeometryToWEM.script \
  ../Modules/ListsToFibers.script \
  ../Modules/LoadPointLineGeometry.script \
  ../Modules/U3DSpecificationGenerator.script \


