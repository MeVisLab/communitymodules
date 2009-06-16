# -----------------------------------------------------------------------------
# Labeling project profile
#
# \file    Labeling.pro
# \author  Konrad Mühler
# \date    2007-08-02
# -----------------------------------------------------------------------------

TEMPLATE = lib

TARGET  = Labeling

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):

#old ... CONFIG += dll ML MLObjMgr inventor common MLOpenGL opengl SoView2D ftgl METK mlUtils
CONFIG += dll ML MLObjMgr inventor common MLOpenGL opengl SoView2D ftgl METK


# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += LABELING_EXPORTS


HEADERS += \
    LabelingInit.h \
    LabelingSystem.h \
    METKLabels/mlMETKLabels.h \   
    #UMDLabel3d/UMDLabel3dInit.h \
    #UMDLabel3d/UMDLabel3dSystem.h \
    #UMDLabel3d/SoLabel3d.h \
    #UMDLabel3d/Label/SoFrameBufferNode.h \
    #UMDLabel3d/Label/SoIDBufferNode.h \
    #UMDLabel3d/Label/LabelStore.h \
    #UMDLabel3d/Label/SoBackgroundColor.h \
    #UMDLabel3d/Label/SoFlatShadingNode.h \
    #UMDLabel3d/Label/SoPhongShadingNode.h \
    #UMDLabel3d/Label/font.h \
    #UMDLabel3d/Annotator/SoAnnotator2.h \
    #UMDLabel3d/Annotator/SoBuchstabe.h \
    #UMDLabel3d/Annotator/SoExternalLabel.h \    
    #UMDLabelModul/UMDLabelModulInit.h \
    #UMDLabelModul/UMDLabelModulSystem.h \
    UMDLabelModul/SoLabelModul.h \
    UMDLabelModul/Data/SoBackgroundColor.h \	
    UMDLabelModul/Data/SoPhongShadingNode.h \
    UMDLabelModul/Data/SoFlatShadingNode.h \
    UMDLabelModul/Data/SoIDBufferNode.h \
    UMDLabelModul/Data/SoLabelObject.h \
    UMDLabelModul/Data/SoAnnotator.h \    
    #UMDTextModul/UMDTextModulInit.h \
    #UMDTextModul/UMDTextModulSystem.h \
    UMDTextModul/SoTextModul.h \
    UMDTextModul/FontManager.h
    
    
      
        
SOURCES += \
    LabelingInit.cpp \    
    METKLabels/mlMETKLabels.cpp \    
    #UMDLabel3d/UMDLabel3dInit.cpp \
    #UMDLabel3d/SoLabel3d.cpp \
    #UMDLabel3d/Label/SoFrameBufferNode.cpp \
    #UMDLabel3d/Label/SoIDBufferNode.cpp \
    #UMDLabel3d/Label/LabelStore.cpp \
    #UMDLabel3d/Label/SoBackgroundColor.cpp \
    #UMDLabel3d/Label/SoFlatShadingNode.cpp \
    #UMDLabel3d/Label/SoPhongShadingNode.cpp \
    #UMDLabel3d/Label/font.cpp \
    #UMDLabel3d/Annotator/SoAnnotator2.cpp \
    #UMDLabel3d/Annotator/SoBuchstabe.cpp \
    #UMDLabel3d/Annotator/SoExternalLabel.cpp \
    #UMDLabelModul/UMDLabelModulInit.cpp \
    UMDLabelModul/SoLabelModul.cpp \
    UMDLabelModul/Data/SoBackgroundColor.cpp \
    UMDLabelModul/Data/SoPhongShadingNode.cpp \
    UMDLabelModul/Data/SoFlatShadingNode.cpp \
    UMDLabelModul/Data/SoIDBufferNode.cpp \
    UMDLabelModul/Data/SoLabelObject.cpp \
    UMDLabelModul/Data/SoAnnotator.cpp \    
    #UMDTextModul/UMDTextModulInit.cpp \
    UMDTextModul/SoTextModul.cpp \
    UMDTextModul/FontManager.cpp
    

