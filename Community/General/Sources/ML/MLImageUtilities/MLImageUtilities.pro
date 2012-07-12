
TEMPLATE = lib

TARGET = MLImageUtilities

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# Set high warn level (warn 4 on MSVC)
WARN = HIGH

# Add used projects here (see included pri files below for available projects)
CONFIG += dll ML MLTools

MLAB_PACKAGES += MeVisLab_Standard 

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


DEFINES += MLIMAGEUTILITIES_EXPORTS

# Enable ML deprecated API warnings. To completely disable the deprecated API, change WARN to DISABLE.
DEFINES += ML_WARN_DEPRECATED

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
     
# additional files that are NOT compiled   
RELATEDFILES += \
  ../../../Modules/ML/MLImageUtilities/MLImageUtilities.def 
