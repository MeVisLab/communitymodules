isEmpty(Community_General_PRI_INCLUDED) {
  message ( loading Community_General.pri )
}
# **InsertLicense** code
# -----------------------------------------------------------------------------
# Community_General prifile
#
# \file    Community_General.pri
# \author  MeVisLab Community
# \date    2009-05-19
# 
#
# -----------------------------------------------------------------------------

# Projects:

# MLCSOCommunityModules - CSO community modules
# MLPDF                 - PDF and 3D-PDF creation (Axel Newe)
# MLBaseListExtensions  - IndexPair[List[Container]], ColoredMarker[List[Container]] (Axel Newe)
# MLOsiriXServices      - OsiriX interface


# include guard against multiple inclusion
isEmpty(Community_General_PRI_INCLUDED) {

Community_General_PRI_INCLUDED = 1

# -- System -------------------------------------------------------------

include( $(MLAB_MeVis_Foundation)/Configuration/SystemInit.pri )

# -- Define local PACKAGE variables -------------------------------------

PACKAGE_ROOT     = $$(MLAB_Community_General)
PACKAGE_SOURCES  = "$$(MLAB_Community_General)"/Sources
PACKAGE_PROJECTS = "$$(MLAB_Community_General)"/Projects

# Add package library path
LIBS          += -L"$${PACKAGE_ROOT}"/lib

# -- Projects -------------------------------------------------------------

# NOTE: Add projects below to make them available to other projects via the CONFIG mechanism

# You can use this example template for typical projects:
#MLMyProject {
#  CONFIG_FOUND += MLMyProject
#  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLMyProject
#  win32:LIBS += MLMyProject$${d}.lib
#  unix:LIBS += -lMLMyProject$${d}
#}

# -- ML Projects -------------------------------------------------------------

MLCSOCommunityModules {
  CONFIG_FOUND += MLCSOCommunityModules
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLCSOCommunityModules
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLCSOCommunityModules/CSODistance
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLCSOCommunityModules/MinimalDistancePointClouds
  win32:LIBS += MLCSOCommunityModules$${d}.lib
  unix:LIBS += -lMLCSOCommunityModules$${d}
}

MLPDF {
  CONFIG_FOUND += MLPDF
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLPDF/Sources
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLPDF/Sources/abstract
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLPDF/Sources/abstract/PDFGenerator
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLPDF/Sources/PDFUtils
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLPDF/Sources/shared
  win32:LIBS += MLPDF$${d}.lib
  unix:LIBS  += -lMLPDF$${d}
  CONFIG += MLBase
  CONFIG += MLWEM
  CONFIG += MLFiberUtils
  CONFIG += zlib
  CONFIG += png
  CONFIG += MLBaseListExtensions
  CONFIG += libhpdf
  CONFIG += inventor
  CONFIG += xerces
}

MLU3D {
  CONFIG_FOUND += MLU3D
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/abstract
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/abstract/BaseModules
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/modules
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/shared
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/shared/U3DFileFormat
  INCLUDEPATH += $${PACKAGE_PROJECTS}/MLU3D/Sources/shared/U3DInternalFormat
  win32:LIBS += MLU3D$${d}.lib
  unix:LIBS  += -lMLU3D$${d}
  CONFIG += MLBase
  CONFIG += MLWEM
  CONFIG += MLFiberUtils
  CONFIG += zlib
  CONFIG += png
  CONFIG += MLBaseListExtensions
  CONFIG += libhpdf
  CONFIG += inventor
  CONFIG += xerces
}

MLBaseListExtensions {
  CONFIG_FOUND += MLBaseListExtensions
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions/ColoredMarkerList
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions/IndexPairList
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions/ListTools
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions/MarkerListImport
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/MLBaseListExtensions/shared  
  win32:LIBS += MLBaseListExtensions$${d}.lib
  unix:LIBS  += -lMLBaseListExtensions$${d}
}

MLOsiriXServices {
  CONFIG_FOUND += MLOsiriXServices
  macx {
    INCLUDEPATH += $${PACKAGE_PROJECTS}/OsiriXMeVisLabBridge/Sources/MLOsiriXServices
    LIBS        += -lMLOsiriXServices$${d}
  }
}

# -- Inventor Projects -------------------------------------------------------

# -- Shared Projects ---------------------------------------------------------

# -- ThirdParty packages --------------------------------------------------

include( ../../ThirdParty/Configuration/Community_ThirdParty.pri )

# End of projects ------------------------------------------------------------

}