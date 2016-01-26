from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import *
from VisDefinitions import *

_cls_info = None
_button_play = None
_button_stop = None
_button_pause = None
_mainWin = None


def init():
   global _cls_info
   
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   return


def wakeup():
   ctx.field("caseType").touch()
   return


def initWindow(args=0):
   global _button_play, _button_stop, _button_pause
   _button_play = ctx.control("b_play")
   _button_stop = ctx.control("b_stop")
   _button_pause = ctx.control("b_pause")
   
   global _mainWin
   _mainWin = ctx.window()
   
   return



def openCase(field = 0):
   ctx.field("METKScriptInit.scriptFile").value = ctx.field("baseScriptPath").value + "/" + ctx.field("caseType").value + "/init-script.txt"
   
   ctx.field("METKManager.cleanup").touch()
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      _mainWin.setTitle("PathApp | " + caseFile)
   return


def openENT(field=0):
    ctx.field("caseType").setStringValue("ENT")
    openCase()
    return

def openLST(field=0):
    ctx.field("caseType").setStringValue("Liver")
    openCase()
    return



def exit(field = 0):
   global _sgbWindow
   global _thresWindow
   global _aniSWindow
   
   ctx.field("METKManager.cleanup").touch()
   
   if field:
      ctx.window().close()
   return

def initScript(args=0):
   ctx.field("METKScriptInit.manualInit").touch()
   return



def startAnimation(args=0):
   print "startAnimation"
   _button_pause.setTitle("Pause")
   ctx.field("UMDAnimation2.ScriptAreaExecute").touch()
   return

def stopAnimation(args=0):
   print "stopAnimation"
   _button_pause.setTitle("Pause")
   ctx.field("UMDAnimation2.stopAnimation").touch()
   return


def pauseAnimation(args=0):
   print "pauseAnimation"
   if (ctx.field("UMDAnimation2.playStatus").value=="Play"):
      ctx.field("UMDAnimation2.pauseAnimation").touch()
      _button_pause.setTitle("Continue")
   elif (ctx.field("UMDAnimation2.playStatus").value=="Pause"):
      ctx.field("UMDAnimation2.continueAnimation").touch()
      _button_pause.setTitle("Pause")
   return




def generateScript(args=0):
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   return

def handleObjectRemovedEvent():
   return

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   return

def handleAttributeModifiedEvent():
   return

def handleLoadedEvent():
   pass

def handleCleanupEvent():
   pass