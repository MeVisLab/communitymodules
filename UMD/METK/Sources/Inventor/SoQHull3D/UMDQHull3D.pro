# --------- UMDQHull3D profile -------------------

TEMPLATE = lib

TARGET  = UMDQHull3D

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add used packages here (see stdPackages.pro for packages):
CONFIG += dll inventor opengl

MLAB_PACKAGES += UMD_METK \
                 MeVisLab_Standard


# DONT REMOVE THE FOLLOWING LINE:
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

DEFINES += UMDQHULL3D_EXPORTS

MSVC_USE_SUBDIRS = 1

HEADERS += \
    UMDQHull3DInit.h \
    UMDQHull3DSystem.h \
    SoQHull3D.h \
	qhull/geom.h \
	qhull/io.h \
	qhull/mem.h \
	qhull/merge.h \
	qhull/poly.h \
	qhull/qhull.h \
	qhull/qhull_a.h \
	qhull/qset.h \
	qhull/stat.h \
	qhull/user.h \

SOURCES += \
    UMDQHull3DInit.cpp \
    SoQHull3D.cpp \
	qhull/geom.c \
	qhull/geom2.c \
	qhull/global.c \
	qhull/io.c \
	qhull/mem.c \
	qhull/merge.c \
	qhull/poly.c \
	qhull/poly2.c \
	qhull/qhalf.c \
	qhull/qhull.c \
	qhull/qset.c \
	qhull/stat.c \
	qhull/user.c \
