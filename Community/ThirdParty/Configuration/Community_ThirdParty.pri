isEmpty(Community_ThirdParty_PRI_INCLUDED) {
  message ( loading Community_ThirdParty.pri )
}
# **InsertLicense** code
# -----------------------------------------------------------------------------
# Community_ThirdParty prifile
#
# \file    Community_ThirdParty.pri
# \author  MeVisLab Community
# \date    2009-05-19
#
# 
#
# -----------------------------------------------------------------------------

# include guard against multiple inclusion
isEmpty(Community_ThirdParty_PRI_INCLUDED) {

Community_ThirdParty_PRI_INCLUDED = 1

# -- System -------------------------------------------------------------

include( $(MLAB_MeVis_Foundation)/Configuration/SystemInit.pri )

# -- Define local PACKAGE variables -------------------------------------

PACKAGE_ROOT    = $$(MLAB_Community_ThirdParty)
PACKAGE_SOURCES = "$$(MLAB_Community_ThirdParty)"/Sources

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

# -- ThirdParty Projects ---------------------------------------------------------

libhpdf {
  CONFIG_FOUND += libhpdf
  CONFIG += zlib png
  INCLUDEPATH += $${PACKAGE_SOURCES}/libhpdf
  INCLUDEPATH += $${PACKAGE_SOURCES}/libhpdf/src
  INCLUDEPATH += $${PACKAGE_SOURCES}/libhpdf/include
  win32:LIBS += libhpdf$${d}.lib
  unix:LIBS += -l$${LIB_NAME_PREFIX}libhpdf$${d}
}

# End of projects ------------------------------------------------------------

}

