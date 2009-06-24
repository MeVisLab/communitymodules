# --------- UMDmnOffscreenRenderer profile -------------------

TEMPLATE = lib

TARGET  = UMDmnOffscreenRenderer

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

CONFIG += dll inventor MLOpenGL ML MLBase MLkAviWriter common boost

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDMNOFFSCREENRENDERER_EXPORTS

HEADERS += \
	UMDmnOffscreenRendererInit.h \
	UMDmnOffscreenRendererSystem.h \
	SomnOffscreenRenderer.h \
	mnOGLOffscreen.h \
	image_basic.h
	
win32:HEADERS += wglext.h

SOURCES += \
	UMDmnOffscreenRendererInit.cpp \
	SomnOffscreenRenderer.cpp \
	mnOGLOffscreen.cpp \
	image_basic.cpp


