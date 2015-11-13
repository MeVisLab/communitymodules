# -----------------------------------------------------------------------------
# PDF3DFigurePage project profile
#
# \file
# \author  Axel Newe (axel.newe@fau.de)
# \date    2015-10-01
# -----------------------------------------------------------------------------


TEMPLATE   = lib

TARGET     = PDF3DFigurePage

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLBase MLWEM MLPDF MLFiberUtils

MLAB_PACKAGES += MeVisLab_Standard MeVis_ThirdParty FMEstable_Release Community_General Community_ThirdParty 

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += PDF3DFIGUREPAGE_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

HEADERS += \
    PDF3DFigurePageInit.h \
    PDF3DFigurePageSystem.h \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils.h \
    PDF3DFigurePage_SavePDF/PDF3DFigurePage_SavePDF.h \
    PDF3DFigurePage_LoadPointLineGeometry/PDF3DFigurePage_LoadPointLineGeometry.h \
    
SOURCES += \
    PDF3DFigurePageInit.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils_Views.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils_Lists.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils_Meshes.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils_Camera.cpp \
    PDF3DFigurePage_Utils/PDF3DFigurePage_Utils_Inventor.cpp \
    PDF3DFigurePage_SavePDF/PDF3DFigurePage_SavePDF.cpp \
    PDF3DFigurePage_LoadPointLineGeometry/PDF3DFigurePage_LoadPointLineGeometry.cpp \

