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
CONFIG += dll ML MLBase libhpdf inventor xerces

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
  shared/MLPDF_PDFDocumentTools.h \
  abstract/abstract.h \
  abstract/PDFGenerator/PDFGenerator.h \
  PDFUtils/PDFCalcCamera.h \
  PDFUtils/PDFCreateViewSpecifications.h \

SOURCES += \
  MLPDFInit.cpp \
  shared/MLPDF_Tools.cpp \
  shared/MLPDF_PDFDocumentTools.cpp \
  abstract/abstract.cpp \
  abstract/PDFGenerator/PDFGenerator.cpp \
  abstract/PDFGenerator/PDFGenerator_XML.cpp \
  abstract/PDFGenerator/PDFGenerator_Page.cpp \
  abstract/PDFGenerator/PDFGenerator_Text.cpp \
  abstract/PDFGenerator/PDFGenerator_Images.cpp \
  abstract/PDFGenerator/PDFGenerator_Document.cpp \
  abstract/PDFGenerator/PDFGenerator_Graphics.cpp \
  abstract/PDFGenerator/PDFGenerator_3DModels.cpp \
  PDFUtils/PDFCalcCamera.cpp \
  PDFUtils/PDFCreateViewSpecifications.cpp \

# Additional files that are NOT compiled
RELATEDFILES += \
  ../Modules/MLPDF.def \
  ../Modules/PDFCalcCamera.script \
  ../Modules/PDFCreateViewSpecifications.script \
  ../Modules/Wizard/PDFModuleWizard.script \
  ../Modules/Wizard/PDFModuleWizard.lst \
  ../Modules/Wizard/PDFModuleWizard.py \


