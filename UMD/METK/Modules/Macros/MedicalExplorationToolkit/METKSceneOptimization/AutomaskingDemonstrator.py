# Python module import
from ObjMgr import *
from VisDefinitions import *

_cls_info = None
_cls_iter = None

_screenshotRun = False
_sgbWindow     = 0
_thresWindow   = 0
_aniSWindow    = 0

def init():
   global _cls_iter
   global _cls_info
   
   #button für 3d aktivieren
   ctx.field("METKViewer3D.viewAll").touch()

   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   ctx.field("METKViewer3D.viewer.applyCameraOrientation").value = "CAMERA_CORONAL"
   return

   
def openCase(field = 0):
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      ctx.window().setTitle("AutomaskingDemonstrator | " + caseFile)
   return


def restart(field = 0):
  if ctx.field("METKManager.status").value == "Please close the current case first or select an existent file.":
    ctx.field("METKManager.cleanup").touch()
  if ctx.field("METKManager.fileName").value != "":
    ctx.field("METKManager.load").touch()
  else:
    openCase()
  return

def setPresetting0():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 5
  ctx.field("METKAutoMasking.dist_far").value             = 10
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 1  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 1
  ctx.field("METKAutoMasking.weight_correl").value        = 0
  
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def setPresetting1():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 5
  ctx.field("METKAutoMasking.dist_far").value             = 10
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 0  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 1
  ctx.field("METKAutoMasking.weight_correl").value        = 0
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def setPresetting2():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 5
  ctx.field("METKAutoMasking.dist_far").value             = 80
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 0  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 1
  ctx.field("METKAutoMasking.weight_correl").value        = 0
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def setPresetting3():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 5
  ctx.field("METKAutoMasking.dist_far").value             = 30
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 0.2  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 1
  ctx.field("METKAutoMasking.weight_correl").value        = 0
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def setPresetting4():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 0
  ctx.field("METKAutoMasking.dist_far").value             = 100
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 0.2  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 0.95
  ctx.field("METKAutoMasking.weight_correl").value        = 0.5
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def setPresettingDefault():
  ctx.field("METKAutoMasking.maxPriority").value          = 0.999
  ctx.field("METKAutoMasking.dist_close").value           = 2
  ctx.field("METKAutoMasking.dist_far").value             = 2
  ctx.field("METKAutoMasking.prio_dist_close").value      = 0.999
  ctx.field("METKAutoMasking.prio_dist_far").value        = 0
  ctx.field("METKAutoMasking.prio_charact_same").value    = 0.15  
  ctx.field("METKAutoMasking.prio_charact_inverse").value = 1
  ctx.field("METKAutoMasking.prio_charact_other").value   = 1
  ctx.field("METKAutoMasking.weight_correl").value        = 1
  ctx.field("METKAutoMasking.recalcPrios").touch()
  pass

def saveSettingAS():
  fileName = ""
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.dist_close").value = ' + str(ctx.field("METKAutoMasking.dist_close").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.dist_far").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  MLABFileManager.writeStringToFile(fileName , 'ctx.field("METKAutoMasking.maxPriority").value = ' + str(ctx.field("METKAutoMasking.maxPriority").value))
  pass

def saveAs(field = 0):
   ctx.field("METKManager.saveAs").touch()
   pass


def save(field = 0):
   ctx.field("METKManager.save").touch()
   pass
     
     
def exit(field = 0):
   global _sgbWindow
   global _thresWindow
   global _aniSWindow
   
   ctx.field("METKManager.cleanup").touch()
   
   if _sgbWindow:
      _sgbWindow.close()
   if _thresWindow:
      _thresWindow.close()
   if _aniSWindow:
      _aniSWindow.close()
      
   if field:
      ctx.window().close()
   return