# -----------------------------------------------------------------------------
# MLMatlabScriptWrapper project profile
#
# \file    MLMatlabScriptWrapper.pro
# \author  Alexander Gryanik, Markus Harz, Ola Friman, Felix Ritter
# \date    2009-02-23
# -----------------------------------------------------------------------------

TEMPLATE   = lib

TARGET     = MLMatlabScriptWrapper

DESTDIR    = ../../../lib
DLLDESTDIR = ../../../lib

# add dependencies of this project here
CONFIG += dll ML MLBase MLWEM MLOSXSupport

# set high warn level (warn 4 on MSCV)
WARN = HIGH

MLAB_PACKAGES += MeVisLab_Standard

# make sure that this file is included after CONFIG and MLAB_PACKAGES
include ($(MLAB_MeVis_Foundation)/Configuration/IncludePackages.pri)


# --------- CHANGE PATHS TO MATLAB FILES HERE ---------

macx {

  # Locate MATLAB bundle via its bundle id (Tested for MATLAB 2010a that yields: '/Applications/MATLAB_R2010a.app')
  # If this does not work set MATLAB_INSTALL_DIR manually
  MATLAB_INSTALL_DIR = $$system(osascript -e \'tell application \"Finder\" to get POSIX path of \(application file id \"com.mathworks.StartMATLAB\" as string\)\' 2>/dev/null)
  
  isEmpty(MATLAB_INSTALL_DIR) {
    error( Automatic detection of MATLAB failed. Please add the location of the MATLAB bundle to the MATLAB_INSTALL_DIR variable in the .pro file )
  } else {
    message ( MATLAB_INSTALL_DIR is $$MATLAB_INSTALL_DIR )
  }

} else:linux {

  # add path to the Matlab engine.h file
  INCLUDEPATH += 
  
  # path to the Matlab *.so files 
  MATLAB_LIBDIR = 

} else {

  # add path to the Matlab engine.h file
  INCLUDEPATH += "C:/Program Files/MATLAB/R2007b/extern/include"

  # path to the Matlab *.lib files 
  MATLAB_LIBDIR = "C:/Program Files/MATLAB/R2007b/extern/lib/win32/microsoft"

} 

# -----------------------------------------------------

win64 {
  QMAKE_LIBDIR += $${MATLAB_LIBDIR}

  LIBS += libeng.lib
  LIBS += libmat.lib
  LIBS += libmex.lib
  LIBS += libmwblas.lib
  LIBS += libmwlapack.lib
  LIBS += libmwmathutil.lib
  LIBS += libmx.lib
}
else:win32 {
  QMAKE_LIBDIR += $${MATLAB_LIBDIR}

  LIBS += libeng.lib
  LIBS += libmat.lib
  LIBS += libmex.lib
  LIBS += libmwlapack.lib
  LIBS += libmx.lib
}
else:macx {
  MATLAB_INCDIR = $${MATLAB_INSTALL_DIR}/extern/include
  64bit {
    MATLAB_LIBDIR = $${MATLAB_INSTALL_DIR}/bin/maci64
  } else {
    MATLAB_LIBDIR = $${MATLAB_INSTALL_DIR}/bin/maci
  }
  
  message ( MATLAB headers location is $$MATLAB_INCDIR )
  message ( MATLAB libraries location is $$MATLAB_LIBDIR )

  INCLUDEPATH  += $$MATLAB_INCDIR
  QMAKE_LIBDIR += $$MATLAB_LIBDIR
  
  LIBS += -leng
  LIBS += -lmx
  
  contains(MAKEFILE_GENERATOR,XCODE):message ( Execute the following commands in the Terminal: \
            install_name_tool -change @loader_path/libeng.dylib $${MATLAB_LIBDIR}/libeng.dylib $${DESTDIR}/lib$${TARGET}$${d}.dylib ; \
            install_name_tool -change @loader_path/libmx.dylib  $${MATLAB_LIBDIR}/libmx.dylib  $${DESTDIR}/lib$${TARGET}$${d}.dylib )
  
  # Due to a limitiation in qmake, this does not work in Xcode currently 
  QMAKE_POST_LINK = \  
      install_name_tool -change @loader_path/libeng.dylib $${MATLAB_LIBDIR}/libeng.dylib $${DESTDIR}/lib$${TARGET}$${d}.dylib && \
      install_name_tool -change @loader_path/libmx.dylib  $${MATLAB_LIBDIR}/libmx.dylib  $${DESTDIR}/lib$${TARGET}$${d}.dylib
}
else:linux {
  QMAKE_LIBDIR += $${MATLAB_LIBDIR}

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
    mlMatlabScriptWrapper.h \
    mlMatlabScriptWrapperOutputData.h

SOURCES += \
    MLMatlabScriptWrapperInit.cpp \
    mlMatlabScriptWrapper.cpp

# additional files that are NOT compiled
RELATEDFILES += \
    ../../../Modules/ML/MLMatlabScriptWrapper/MLMatlabScriptWrapper.def

