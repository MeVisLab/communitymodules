# --------- Viewpoint profile -------------------

TEMPLATE = lib

TARGET  = Viewpoint

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor opengl MLOpenGL ML MLObjMgr METK common UMDmnOffscreenRenderer METKMessaging MLkAviWriter UMDAnimation2


MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)



# set high warn level (warn 4 on MSCV)
WARN = HIGH


DEFINES += VIEWPOINT_EXPORTS
DEFINES += UMDVISDATAVIEWER_EXPORTS
DEFINES += UMDVISDATAVIEWER2D_EXPORTS
DEFINES += VPBASIC_EXPORTS
DEFINES += UMDVISIBILITYSOLVER_EXPORTS


# postprocessing flags
MSVC_USE_SUBDIRS = 1




HEADERS += \
    UMDVisDataViewer/UMDVisDataViewerInit.h \
    UMDVisDataViewer/UMDVisDataViewerSystem.h \
    UMDVisDataViewer/SoVisDataViewer.h \
    UMDVisDataViewer2D/UMDVisDataViewer2DInit.h \
    UMDVisDataViewer2D/UMDVisDataViewer2DSystem.h \
    UMDVisDataViewer2D/SoVisDataViewer2D.h \
    METKAutoFading/mlMETKAutoFading.h \
    METKCalcCamPos/METKCalcCamPos.h \
    METKKeystates/METKKeystates.h \
    METKKeystates/Constants.h \
    METKToSolverConnection/METKToSolverConnection.h \
    vpBasic/vpField.h \
    vpBasic/vpCalcVis.h \
    vpBasic/vpPoint.h \
    vpBasic/vpEdge.h \
    vpBasic/vpTriangle.h \
    vpBasic/vpDivSphere.h \
    vpBasic/vpAlgorithms.h \
    UMDVisibilitySolver/UMDVisibilitySolverInit.h \
    UMDVisibilitySolver/UMDVisibilitySolverSystem.h \
    UMDVisibilitySolver/SoVisibilitySolver.h \   
    METKCreateIntraOPDummy/mlMETKCreateIntraOPDummy.h \
    METKIntraOPViewpoint/mlMETKIntraOPViewpoint.h \
    METKShowClusteredObjects/METKShowClusteredObjects.h \
    METKCalcCamPath/METKCalcCamPath.h \
    ViewpointSystem.h \
    ViewpointInit.h
    




SOURCES += \
    UMDVisDataViewer/UMDVisDataViewerInit.cpp \
    UMDVisDataViewer/SoVisDataViewer.cpp \
    UMDVisDataViewer2D/UMDVisDataViewer2DInit.cpp \
    UMDVisDataViewer2D/SoVisDataViewer2D.cpp \
    METKAutoFading/mlMETKAutoFading.cpp \
    METKCalcCamPos/METKCalcCamPos.cpp \
    METKKeystates/METKKeystates.cpp \
    METKToSolverConnection/METKToSolverConnection.cpp \
    vpBasic/vpField.cpp \
    vpBasic/vpCalcVis.cpp \
    vpBasic/vpPoint.cpp \
    vpBasic/vpEdge.cpp \
    vpBasic/vpTriangle.cpp \
    vpBasic/vpDivSphere.cpp \
    vpBasic/vpAlgorithms.cpp \
    UMDVisibilitySolver/UMDVisibilitySolverInit.cpp \
    UMDVisibilitySolver/SoVisibilitySolver.cpp \
    METKCreateIntraOPDummy/mlMETKCreateIntraOPDummy.cpp \
    METKIntraOPViewpoint/mlMETKIntraOPViewpoint.cpp \
    METKShowClusteredObjects/METKShowClusteredObjects.cpp \
    METKCalcCamPath/METKCalcCamPath.cpp \
    ViewpointInit.cpp
