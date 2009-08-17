# MeVisLabHub

macx {

  # build the plugin using xcodebuild
  release:!debug:system(xcodebuild -configuration Development clean build)

  # the plugin will end up in 'build/Development' (MeVisLabHub.osirixplugin)

}
