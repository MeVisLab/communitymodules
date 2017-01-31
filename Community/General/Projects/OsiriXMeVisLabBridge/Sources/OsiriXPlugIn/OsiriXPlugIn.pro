#
#  OsiriXPlugIn.pro
#  OsiriXMeVisLabBridgePlugin
#
#  Created by Felix Ritter on 18.01.08.
#  This code is in the public domain.
#

MEVIS_MAINTAINER = fritter

TEMPLATE = lib

# Create Info.plist file from template file given in MLAB_INFO_PLIST_TEMPLATE
ML_INFO_PLIST_TEMPLATE = $$PWD/Info-template.plist
include ( $$(MLAB_MeVis_Foundation)/Configuration/CreateInfoPListFromTemplate.pri )
system(mv -f \"$$QMAKE_INFO_PLIST\" \"$$PWD/Info.plist\")

# detect compiler
include ($(MLAB_MeVis_BuildSystem)/Configuration/System.pri)
  
macx:release:!debug {
  
  # build the plugin using xcodebuild
  clang {
    !system(xcodebuild -configuration Release GCC_VERSION=com.apple.compilers.llvm.clang.1_0 MACOSX_DEPLOYMENT_TARGET=$$QMAKE_MACOSX_DEPLOYMENT_TARGET clean build):error(Build error.)
  } else {
    !system(xcodebuild -configuration Release GCC_VERSION=com.apple.compilers.llvmgcc42 clean build):error(Build error.)
  }

}
