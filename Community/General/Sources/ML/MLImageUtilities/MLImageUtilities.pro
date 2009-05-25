
TEMPLATE = lib

TARGET = MLImageUtilities

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

CONFIG += dll ML MLTools

MLAB_PACKAGES += MeVisLab_Standard 

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += MLIMAGEUTILITIES_EXPORTS

MSVC_USE_SUBDIRS = 1

HEADERS += \    
  MLImageUtilitiesInit.h \
  MLImageUtilitiesSystem.h \
  MLImageUtilitiesIncludes.h \
	PlaneClip/PlaneClip.h \
	AnalyzeHeader/AnalyzeHeader.h \
	MIP/MIP.h
    
SOURCES += \    
  MLImageUtilitiesInit.cpp \
	PlaneClip/PlaneClip.cpp \
	AnalyzeHeader/AnalyzeHeader.cpp \
	MIP/MIP.cpp
        
RELATEDFILES += \
  ../../MLabModules/MLImageUtilities/MLImageUtilities.def 
