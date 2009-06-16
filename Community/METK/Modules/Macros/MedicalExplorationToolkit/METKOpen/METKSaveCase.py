# Python module import
from ObjMgr import *
from VisDefinitions import *

_cls_info = None
_caseLoaded = False
_caseDir = ""

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CASE, LAY_CASE)
   pass


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass


def handleObjectRemovedEvent():
   pass


def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   pass


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   global _caseDir
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   ctx.field("fileName").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE)
   pass


def handleLoadedEvent():
   global _caseDir
   
   handleAttributeModifiedEvent()
   _caseLoaded = True
   ctx.field("caseLoaded").value = True
   pass


def handleSaveEvent():
   pass
   
   
def handleCleanupEvent():
   ctx.field("fileName").value = ""
   ctx.field("caseLoaded").value = False
   pass


def save(field):
   _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_SAVE,INFOTYPE_MESSAGE)      	  
   _cls_info.notify()
   ctx.field("ObjDump.fileName").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE)
   ctx.field("ObjDump.save").touch()
   pass
   
   
def saveAs(field):
   global _caseDir
   result = ctx.showModalDialog("SaveAs", "Save Case File")
   if result:
      caseFile = ctx.field("fileName").value
      if caseFile != "":
         if caseFile[-4:] != ".xml":
            caseFile = caseFile + ".xml"
         _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_SAVE,INFOTYPE_MESSAGE)      	  
         _cls_info.set(OBJ_CASE, LAY_CASE, INF_XMLFILE, caseFile)
         _cls_info.notify()
         ctx.field("ObjDump.fileName").value = _caseDir + caseFile
         ctx.field("ObjDump.save").touch()
   pass


def modalDialogSave(field = 0):
   if MLABFileManager.exists(_caseDir + ctx.field("fileName").value):
      result = MLAB.showInformation("File already exists", "Warning", ["Overwrite","Cancel"],1)
      if not result:
         ctx.window().done(1)
   else:
      ctx.window().done(1)
   pass

def modalDialogCancel(field = 0):
   ctx.window().done(0)
   pass
