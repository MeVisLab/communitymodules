from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import *
from VisDefinitions import *

_buttonHeight = 15
_maxInt = 16777215

_cls_info = None
_button_play = None
_button_stop = None
_button_pause = None
_mainWin = None


_tw_listView = None
_tab_hierarchy = None
_tab_structuregroup = None

#_areaLog = None
#_areaScript = None
#_buttonCollapseScript = None

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
   
   global _tw_listView, _tab_hierarchy, _tab_structuregroup
   _tw_listView = ctx.control("ObjectListView")
   _tab_hierarchy = ctx.control("TabHierarchy")
   _tab_structuregroup = ctx.control("TabStructureGroup")
   
   #global _areaLog, _buttonCollapseLog#, _areaScript#, _buttonCollapseScript
   #_areaLog = ctx.control("areaLog")
   #_buttonCollapseLog = ctx.control("buttonCollapseLog")
   #_areaScript = ctx.control("areaScript")
   #_buttonCollapseScript = ctx.control("buttonCollapseScript")
   

   global _upImg, _downImg
   _upImg = ctx.field("local").value+"/up.png"
   _downImg = ctx.field("local").value+"/down.png"
   
   return



def openCase(field = 0):   
   ctx.field("METKScriptInit.scriptFile").value = ctx.field("baseScriptPath").value + "/" + ctx.field("caseType").value + "/init-script.txt"
   
   ctx.field("METKManager.cleanup").touch()
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      _mainWin.setTitle("Cinema | " + caseFile)
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



def loadScriptFile(field=0,scriptFile = ""):
   if (scriptFile == ""):
      if (ctx.field("UMDAnimation2.pathTypeScriptPath").value=="Use module path"):
         scriptFile = ctx.field("UMDAnimation2.modulePath").value+"/"+ctx.field("UMDAnimation2.scriptFile").value
      elif (ctx.field("UMDAnimation2.pathTypeScriptPath").value=="Use case path"):
         scriptFile = ctx.field("METKManager.Directory").value+"/"+ctx.field("UMDAnimation2.scriptFile").value
      else:
         scriptFile = ctx.field("UMDAnimation2.scriptFile").value
   
   ctx.log("load script file: "+scriptFile)
   
   #load file to script area
   ctx.field("UMDAnimation2.ScriptArea").setStringValue("")
   if (isfile(scriptFile)==1):
      f = file(scriptFile,'r')
      for line in f:
         ctx.field("UMDAnimation2.ScriptArea").setStringValue(ctx.field("UMDAnimation2.ScriptArea").value+line)
   
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




def entScriptSelected(item, column):
   ctx.field("UMDAnimation2.pathTypeScriptPath").value="Use module path"
   ctx.field("UMDAnimation2.scriptFile").value = ctx.field("baseScriptPath").value + "/ENT/"+item.text(1)
   loadScriptFile()
   return


def liverScriptSelected(item, column):
   ctx.field("UMDAnimation2.pathTypeScriptPath").value="Use module path"
   ctx.field("UMDAnimation2.scriptFile").value = ctx.field("baseScriptPath").value + "/Liver/"+item.text(1)
   loadScriptFile()
   return


def listViewTypeCahnged(field=0):
   if (ctx.field("listViewType").value == "Hierarchy"):
      _tw_listView.selectTab(_tab_hierarchy)
   else:
      _tw_listView.selectTab(_tab_structuregroup)
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