# MeVisLabHub

macx:release:!debug {

  # build the plugin using xcodebuild
  system(xcodebuild -configuration Development clean build)

}
