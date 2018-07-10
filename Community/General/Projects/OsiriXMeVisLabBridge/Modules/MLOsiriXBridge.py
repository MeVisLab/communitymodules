# **InsertLicense** code
# -*- coding: iso-8859-1 -*-


from mevis import MLAB,MLABFileManager
import os.path

def installOsiriXPlugin():
  # Make sure OsiriX is running beforehand. Due to the modified environment of
  # MeVisLab OsiriX may behave differently than usual when started from this
  # environment...
  if MLAB.macIsApplicationRunning('com.rossetantoine.osirix'):
    MLAB.macOpenFileInApplication(os.path.join(ctx.package().binPath(),'PlugIns','OsiriXMeVisLabBridge.osirixplugin'), 'com.rossetantoine.osirix')
  else:
    MLAB.showInformation('OsiriX must be running to install the plugin. Please start OsiriX.', 'The plugin will be handed to OsiriX for installation. Click on the Install button again after launching OsiriX.', 'OsiriXBridge', ctx.window())

def installHorosPlugin():
  # Make sure Horos is running beforehand. Due to the modified environment of
  # MeVisLab Horos may behave differently than usual when started from this
  # environment...
  if MLAB.macIsApplicationRunning('org.horosproject.horos'):
    MLAB.macOpenFileInApplication(os.path.join(ctx.package().binPath(),'PlugIns','OsiriXMeVisLabBridge.osirixplugin'), 'org.horosproject.horos')
  else:
    MLAB.showInformation('Horos must be running to install the plugin. Please start Horos.', 'The plugin will be handed to Horos for installation. Click on the Install button again after launching Horos.', 'OsiriXBridge', ctx.window())

def showPlugin():
  MLAB.openFile(os.path.join(ctx.package().binPath(),'PlugIns'))

def showGuide():
  MLAB.openFile(os.path.join(ctx.package().documentationPath(),'Publish','OsiriXMeVisLabBridge.pdf'))

def ShowEatDicomOptions():
  MLAB.runCommandInConsole(MLABFileManager.getExecutable("eatDicom"), ["-help"], "", "", "Dicom Import Options Help", ctx.window())

def SetDefaultEatDicomOptions():
  ctx.field("eatDicomOptions").setStringValue(MLAB.variable("EatDicomOptions"))
