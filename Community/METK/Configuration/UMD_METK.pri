isEmpty(UMD_METK_PRI_INCLUDED) {
  message ( loading UMD_METK.pri )
}
# **InsertLicense** code
# -----------------------------------------------------------------------------
# UMD_METK prifile
#
# \file    UMD_METK.pri
# \author  Konrad
# \date    2009-04-24
#
# The Medical Explortation Toolkit (www.metk.net)
#
# -----------------------------------------------------------------------------

# include guard against multiple inclusion
isEmpty(UMD_METK_PRI_INCLUDED) {

UMD_METK_PRI_INCLUDED = 1

# -- System -------------------------------------------------------------

include( $(MLAB_MeVis_Foundation)/Configuration/SystemInit.pri )

# -- Define local PACKAGE variables -------------------------------------

PACKAGE_ROOT    = $$(MLAB_UMD_METK)
PACKAGE_SOURCES = "$$(MLAB_UMD_METK)"/Sources

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

MLkAviWriter {
  CONFIG_FOUND += MLkAviWriter
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/kAviWriter
  win32:LIBS += MLkAviWriter$${d}.lib
  unix:LIBS += -lMLkAviWriter$${d}
}


UMDAnimation2 {
  CONFIG_FOUND += UMDAnimation2
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/Animation
  win32:LIBS += Animation$${d}.lib
  unix:LIBS += -lAnimation$${d}
}

METKMessaging {
  CONFIG_FOUND += METKMessaging
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/METKMessaging
  win32:LIBS += METKMessaging$${d}.lib
  unix:LIBS += -lMETKMessaging$${d}
}

METK2DLabeling {
  CONFIG_FOUND += METK2DLabeling
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML/METK2DLabeling
  win32:LIBS += METK2DLabeling$${d}.lib
  unix:LIBS += -lMETK2DLabeling$${d}
}



# -- Inventor Projects -------------------------------------------------------

UMDQHull3D {
  CONFIG_FOUND += UMDQHull3D
  INCLUDEPATH += $${PACKAGE_SOURCES}/Inventor/SoQHull3D
  win32:LIBS += UMDQHull3D$${d}.lib
  unix:LIBS += -lUMDQHull3D$${d}
}

UMDSoMeasurement {
  CONFIG_FOUND += UMDSoMeasurement
  INCLUDEPATH += $${PACKAGE_SOURCES}/Inventor/UMDSoMeasurement
  win32:LIBS += UMDSoMeasurement$${d}.lib
  unix:LIBS += -lUMDSoMeasurement$${d}
}

UMDmnOffscreenRenderer {
  CONFIG_FOUND += UMDmnOffscreenRenderer
  INCLUDEPATH += $${PACKAGE_SOURCES}/Inventor/UMDmnOffscreenRenderer
  win32:LIBS += UMDmnOffscreenRenderer$${d}.lib
  unix:LIBS += -lUMDmnOffscreenRenderer$${d}
}

UMDStraightLine {
  CONFIG_FOUND += UMDStraightLine
  INCLUDEPATH += $${PACKAGE_SOURCES}/Inventor/UMDStraightLine
  win32:LIBS += UMDStraightLine$${d}.lib
  unix:LIBS += -lUMDStraightLine$${d}
}



# -- Shared Projects ---------------------------------------------------------

common {
  CONFIG_FOUND += common
  INCLUDEPATH += $${PACKAGE_SOURCES}/common
  win32:LIBS += common$${d}.lib
  unix:LIBS += -lcommon$${d}
}


METK {
  CONFIG_FOUND += METK
  INCLUDEPATH += $${PACKAGE_SOURCES}/ML
}

# End of projects ------------------------------------------------------------

}

