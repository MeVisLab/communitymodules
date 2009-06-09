# -----------------------------------------------------------------------------
# MLMatlabScriptWrapper project profile
#
# \file    MLMatlabScriptWrapper.pro
# \author  Alexander Gryanik, Markus Harz, Ola Friman
# \date    2009-02-23
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLMatlabScriptWrapper

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here
CONFIG += dll ML MLBase

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)

win64 {
  QMAKE_LIBDIR += "$$(MLAB_MeVisResearch_General)"/Sources/ML/MLMatlabScriptWrapper/MatlabCommon/win64

  LIBS += libeng.lib
  LIBS += libmat.lib
  LIBS += libmex.lib
  LIBS += libmwblas.lib
  LIBS += libmwlapack.lib
  LIBS += libmwmathutil.lib
  LIBS += libmx.lib
}
else:win32 {
  QMAKE_LIBDIR += "$$(MLAB_MeVisResearch_General)"/Sources/ML/MLMatlabScriptWrapper/MatlabCommon/win32

  LIBS += libdflapack.lib
  LIBS += libeng.lib
  LIBS += libmat.lib
  LIBS += libmex.lib
  LIBS += libmwlapack.lib
  LIBS += libmx.lib
}
else:macx {
  MATLAB_LIBDIR = "$$(MLAB_MeVisResearch_General)"/Sources/ML/MLMatlabScriptWrapper/MatlabCommon/macos

  LIBS += $${MATLAB_LIBDIR}/libeng.dylib
  LIBS += $${MATLAB_LIBDIR}/libmx.dylib
  LIBS += $${MATLAB_LIBDIR}/libut.dylib
  LIBS += $${MATLAB_LIBDIR}/libmat.dylib
  LIBS += $${MATLAB_LIBDIR}/libhdf5.0.dylib
  LIBS += $${MATLAB_LIBDIR}/libicudata.dylib.36
  LIBS += $${MATLAB_LIBDIR}/libicui18n.dylib.36
  LIBS += $${MATLAB_LIBDIR}/libicuio.dylib.36
  LIBS += $${MATLAB_LIBDIR}/libicuuc.dylib.36
  LIBS += $${MATLAB_LIBDIR}/libxerces-c.27.dylib
  LIBS += $${MATLAB_LIBDIR}/libz.1.dylib
}
else:linux {
  QMAKE_LIBDIR += "$$(MLAB_MeVisResearch_General)"/Sources/ML/MLMatlabScriptWrapper/MatlabCommon/linux

  LIBS += -lut
  LIBS += -leng
  LIBS += -lmat
  LIBS += -lmex
  LIBS += -lmwlapack
  LIBS += -lmx
}

DEFINES += MLMATLABSCRIPTWRAPPER_EXPORTS

HEADERS += \
    MLMatlabScriptWrapperInit.h \
    MLMatlabScriptWrapperSystem.h \
    mlMatlabScriptWrapper.h

SOURCES += \
    MLMatlabScriptWrapperInit.cpp \
    mlMatlabScriptWrapper.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLMatlabScriptWrapper/MLMatlabScriptWrapper.def
   

