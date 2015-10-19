# --------- Animation profile -------------------

TEMPLATE = lib

TARGET  = Animation

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll ML MLObjMgr inventor common METKMessaging xerces METK UMDmnOffscreenRenderer MLOpenGL opengl MLkAviWriter

# set high warn level (warn 4 on MSCV)
WARN = HIGH


MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard

# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)



DEFINES += ANIMATION_EXPORTS

# postprocessing flags
MSVC_USE_SUBDIRS = 1



HEADERS += \
    AnimationInit.h \
    AnimationSystem.h \
    UMDAnimation2/mlUMDAnimation2.h \
    UMDAnimation2/AnimationExecuter.h \
    UMDAnimation2/AnimationCache.h \
    UMDCampath/UMDCampath.h \
    UMDCampath/LinearCampath.h \
    UMDCampath/GoogleCampath.h \
    UMDCampath/RectangleCampath.h \
    UMDCampath/GoogleRectangleCampath.h \
    UMDCampath/HeightCampath.h \
    AnimationParser/AnimationParser.h \
    AnimationParser/kScriptCommandObject.h \
    AnimationParser/kScriptFrameEntry.h \
    AnimationParser/kScriptFrameList.h \
    AnimationParser/kScriptFrameObject.h \
    AnimationParser/kScriptIniObject.h \
    METKObjXMLWriter/mlMETKObjXMLWriter.h
    


SOURCES += \
    AnimationInit.cpp \
    UMDAnimation2/mlUMDAnimation2.cpp \
    UMDAnimation2/AnimationExecuter.cpp \    
    UMDAnimation2/AnimationCache.cpp \    
    UMDCampath/UMDCampath.cpp \
    UMDCampath/LinearCampath.cpp \
    UMDCampath/GoogleCampath.cpp \
    UMDCampath/RectangleCampath.cpp \
    UMDCampath/GooglerectangleCampath.cpp \
    UMDCampath/HeightCampath.cpp \
    AnimationParser/AnimationParser.cpp \
    AnimationParser/kScriptCommandObject.cpp \
    AnimationParser/kScriptFrameEntry.cpp \
    AnimationParser/kScriptFrameList.cpp \
    AnimationParser/kScriptFrameObject.cpp \
    AnimationParser/kScriptIniObject.cpp \
    METKObjXMLWriter/mlMETKObjXMLWriter.cpp \
    UMDAnimation2/ScriptCommands/cmdVolRend.cpp \
    UMDAnimation2/ScriptCommands/cmdCam.cpp \    
    UMDAnimation2/ScriptCommands/cmdClip.cpp \
    UMDAnimation2/ScriptCommands/cmdFading.cpp \
    UMDAnimation2/ScriptCommands/cmdStandard.cpp
     