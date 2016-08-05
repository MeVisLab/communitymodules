# -----------------------------------------------------------------------------
# MLPDF project profile
#
# \file    MLPDF.pro
# \author  Axel Newe (axel.newe@fau.de)
# \date    2014-02-06
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLPDF

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM MLFiberUtils zlib png MLBaseListExtensions libhpdf inventor xerces

MLAB_PACKAGES += MeVisLab_Standard MeVis_ThirdParty Community_General Community_ThirdParty FMEstable_Release

INCLUDEPATH += ./shared/

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += MLPDF_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
  MLPDFInit.h \
  MLPDFSystem.h \
  shared/MLPDF_Tools.h \
  shared/MLPDF_Defines.h \
  shared/MLPDF_DataTypes.h \
  shared/MLPDF_MarkerListTools.h \
  shared/MLPDF_PDFDocumentTools.h \
  shared/MLPDF_SpecificationGenerator.h \
  abstract/abstract.h \
  abstract/PDFGenerator/PDFGenerator.h \
  LoadPointLineGeometry/LoadPointLineGeometry.h \
  PDFUtils/PDFCalcCamera.h \
  PDFUtils/PDFCreateFibersFromLists.h \
  PDFUtils/PDFCreateViewSpecifications.h \
  U3D/U3D.h \
  U3D/U3DFileFormat/U3D_BitStream.h \
  U3D/U3DFileFormat/U3D_Constants.h \
  U3D/U3DFileFormat/U3D_ContextManager.h \
  U3D/U3DFileFormat/U3D_DataBlock.h \
  U3D/U3DFileFormat/U3D_DataBlockWriter.h \
  U3D/U3DFileFormat/U3D_DataTypes.h \
  U3D/U3DFileFormat/U3D_FileWriter.h \
  U3D/U3DFileFormat/U3D_Tools.h \
  U3D/SaveU3D/SaveU3D.h \
  U3D/LoadU3D/LoadU3D.h \
#  PRC/PRC.h \
#  PRC/PRCFileFormat/PRC_Macros.h \
#  PRC/PRCFileFormat/PRC_Defines.h \
#  PRC/PRCFileFormat/PRC_DataTypes.h \
#  PRC/PRCFileFormat/PRC_BitStream.h \
#  PRC/PRCFileFormat/PRC_Write.h \
#  PRC/PRCFileFormat/PRC_File.h \
#  PRC/PRCFileFormat/PRC_Tools.h \
#  PRC/SavePRC/SavePRC.h \


SOURCES += \
  MLPDFInit.cpp \
  shared/MLPDF_Tools.cpp \
  shared/MLPDF_MarkerListTools.cpp \
  shared/MLPDF_PDFDocumentTools.cpp \
  shared/MLPDF_SpecificationGenerator.cpp \
  abstract/abstract.cpp \
  abstract/PDFGenerator/PDFGenerator.cpp \
  abstract/PDFGenerator/PDFGenerator_XML.cpp \
  abstract/PDFGenerator/PDFGenerator_Page.cpp \
  abstract/PDFGenerator/PDFGenerator_Text.cpp \
  abstract/PDFGenerator/PDFGenerator_Images.cpp \
  abstract/PDFGenerator/PDFGenerator_Document.cpp \
  abstract/PDFGenerator/PDFGenerator_Graphics.cpp \
  abstract/PDFGenerator/PDFGenerator_3DModels.cpp \
  LoadPointLineGeometry/LoadPointLineGeometry.cpp \
  PDFUtils/PDFCalcCamera.cpp \
  PDFUtils/PDFCreateFibersFromLists.cpp \
  PDFUtils/PDFCreateViewSpecifications.cpp \
  U3D/U3D.cpp \
  U3D/U3DFileFormat/U3D_BitStream.cpp \
  U3D/U3DFileFormat/U3D_ContextManager.cpp \
  U3D/U3DFileFormat/U3D_DataBlock.cpp \
  U3D/U3DFileFormat/U3D_DataBlockWriter.cpp \
  U3D/U3DFileFormat/U3D_FileWriter.cpp \
  U3D/U3DFileFormat/U3D_Tools.cpp \
  U3D/SaveU3D/SaveU3D.cpp \
  U3D/SaveU3D/SaveU3D_Tools.cpp \
  U3D/SaveU3D/SaveU3D_ProcessPointSets.cpp \
  U3D/SaveU3D/SaveU3D_ProcessLineSets.cpp \
  U3D/SaveU3D/SaveU3D_ProcessMeshes.cpp \
  U3D/LoadU3D/LoadU3D.cpp \
#  PRC/PRC.cpp \
#  PRC/PRCFileFormat/PRC_DataTypes.cpp \
#  PRC/PRCFileFormat/PRC_BitStream.cpp \
#  PRC/PRCFileFormat/PRC_Write.cpp \
#  PRC/PRCFileFormat/PRC_File.cpp \
#  PRC/PRCFileFormat/PRC_Tools.cpp \
#  PRC/SavePRC/SavePRC.cpp \
#  PRC/SavePRC/SavePRC_Tools.cpp \
#  PRC/SavePRC/SavePRC_ProcessPointSets.cpp \
#  PRC/SavePRC/SavePRC_ProcessLineSets.cpp \
#  PRC/SavePRC/SavePRC_ProcessMeshes.cpp \

# Additional files that are NOT compiled
RELATEDFILES += \
  ../../../Modules/ML/MLPDF/MLPDF.def \
  ../../../Modules/ML/MLPDF/SaveU3D.script \
  ../../../Modules/ML/MLPDF/LoadU3D.script \
  ../../../Modules/ML/MLPDF/PDFCalcCamera.script \
  ../../../Modules/ML/MLPDF/PDFCreateFibersFromLists.script \
  ../../../Modules/ML/MLPDF/PDFCreateViewSpecifications.script \
  ../../../Modules/ML/MLPDF/LoadPointLineGeometry.script \
  ../../../Modules/ML/MLPDF/Wizard/PDFModuleWizard.script \
  ../../../Modules/ML/MLPDF/Wizard/PDFModuleWizard.mlab \
  ../../../Modules/ML/MLPDF/Wizard/PDFModuleWizard.lst \
  ../../../Modules/ML/MLPDF/Wizard/PDFModuleWizard.py \


