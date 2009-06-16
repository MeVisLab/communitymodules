# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Global class instances
_cls_info = None
_cls_iter = None
_containerList = []

def init():
   global _cls_info
   global _cls_iter

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_APPEARANCE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   return


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
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()     

   if info == INF_STIPPLING or info == INF_VISIBLE:
      if _cls_info.existInfo(object, layer, INF_STIPPLING) and _cls_info.getBool(object, layer, INF_STIPPLING) == True:
         addContainer(object)
   pass


def handleLoadedEvent():
   global _caseDir
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   _cls_iter.searchFirst()
   while _cls_iter.success():
      if ctx.field("creationTime").stringValue() == "OnLoad" or (_cls_info.existInfo(_cls_iter.object(), LAY_APPEARANCE, INF_STIPPLING) and _cls_info.getBool(_cls_iter.object(), LAY_APPEARANCE, INF_STIPPLING) == True):
         addContainer(_cls_iter.object())
      _cls_iter.searchNext()
   pass

   
def handleSaveEvent():
   pass   
   

def handleCleanupEvent():
   _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Cleaning up METKStippling...", INFOTYPE_MESSAGE)
   _cls_info.notify()
   ctx.log("Clean METKStippling")
   ctx.callLater(0, "deleteModules")
   pass


def deleteModules():
   global _containerList
   moduleCounter = 0.0
   for module in _containerList:
      moduleCounter = moduleCounter + 1.0
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS, (moduleCounter / float(len(_containerList))), INFOTYPE_DOUBLE)
      _cls_info.notify()
      ctx.module(module).remove()
   ctx.log("METKStippling is clean")

   _containerList = []
   pass


def addContainer(objectID):
   global _containerList
   
   if _containerList.count(_cls_info.adjustObjectID(objectID)) == 0:
      # Container erstellen und benennen
      ctx.log("Add Stippling Container: " + objectID)
      container = ctx.addModule("StipplingContainer")
      container.field("instanceName").setStringValue(_cls_info.adjustObjectID(objectID))
      container.field("objectID").setStringValue(objectID)
      container.field("visibleOn").value = ctx.field("visibleOn").value
      _containerList.append(_cls_info.adjustObjectID(objectID))
      
      # verbinden mit Ausgang und ObjMgr
      infoout = ctx.field("info.outObjectContainer")
      ocin = container.field("inObjectContainer")
      ocin.connectFrom(infoout)
      container.field("initialize").touch()  

      ctx.module("output").addChild(container.field("outInventor"))
      count = len(_containerList)
      container.setFrameTopLeft(200*(count%3),75*round((count/3.0)-0.5,0)+600)         
   pass


def visibleOnChanged(field = 0):
   global _containerList
   for module in _containerList:
      ctx.module(module).field("visibleOn").setStringValue(ctx.field("visibleOn").value)
   pass
