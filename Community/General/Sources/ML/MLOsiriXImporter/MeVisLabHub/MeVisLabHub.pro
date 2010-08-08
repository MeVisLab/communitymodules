# MeVisLabHub

TEMPLATE = lib
CONFIG -= qt
LIBS += -framework CoreFoundation

macx:release:!debug {

  # build the plugin using xcodebuild
  system(xcodebuild -configuration Development clean build)

}
