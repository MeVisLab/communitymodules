# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Frequently used controls
# ...

# Frequently used fields
# ...

# Global class instances
_cls_info = None
_cls_iter = None
_containerList = []


def init():
   global _cls_info
   global _cls_iter
   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")

   ctx.field("SoSwitch.whichChild").value = 0
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
   
   if layer == LAY_APPEARANCE:      
      if info == INF_CLIPPING:
         containerName = _cls_info.adjustObjectID(object)
         if (value=="ON"):
            ctx.module("clipNo").removeChild(ctx.field(containerName+".outInventor"))
            #remove first, so that not 2 of the same hangs on it
            ctx.module("clipYes").removeChild(ctx.field(containerName+".outInventor"))
            ctx.module("clipYes").addChild(ctx.field(containerName+".outInventor"))            
         else:
            ctx.module("clipYes").removeChild(ctx.field(containerName+".outInventor"))         
            #remove first, so that not 2 of the same hangs on it
	    ctx.module("clipNo").removeChild(ctx.field(containerName+".outInventor"))
            ctx.module("clipNo").addChild(ctx.field(containerName+".outInventor"))
      if info == INF_VISIBLE and _cls_info.getBool() == True and ctx.field("creationTime").stringValue() == "OnVisible":
         addContainer(object)
   pass
   
   
def handleCleanupEvent():
   _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Cleaning up METKIsoSurface...", INFOTYPE_MESSAGE)
   _cls_info.notify()
   ctx.log("Clean METKIsoSurface")
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
   ctx.log("METKIsoSurface is clean")

   _containerList = []
   pass


def handleLoadedEvent(field = 0):
   #global _caseDir   
   #_caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   _cls_iter.searchFirst()
   while _cls_iter.success():
      if ctx.field("creationTime").stringValue() == "OnLoad" or _cls_info.getBool(_cls_iter.object(), LAY_APPEARANCE, INF_VISIBLE) == True:
         addContainer(_cls_iter.object())
      _cls_iter.searchNext()
   useDepthPeeling()
   pass

   
def handleSaveEvent():
   pass


def addContainer(object):
   global _containerList

   if _containerList.count(_cls_info.adjustObjectID(object)) == 0:
      # Container erstellen und benennen
      container = ctx.addModule("IsoSurfaceContainer")
      container.field("instanceName").setStringValue(_cls_info.adjustObjectID(object))
      container.field("objectID").setStringValue(object)
      _containerList.append(_cls_info.adjustObjectID(object))
      
      # verbinden mit Viewer und ObjMgr
      ctx.module("clipNo").addChild(container.field("outInventor"))
      infoout = ctx.field("info.outObjectContainer")
      ocin = container.field("inObjectContainer")
      ocin.connectFrom(infoout)
      container.field("initialize").touch()
      
      count = len(_containerList)
      container.setFrameTopLeft(200*(count%3),75*round((count/3.0)-0.5,0)+600)
   pass
   
   
def useDepthPeeling(field = 0):
   ctx.field("SoSwitch.whichChild").value = ctx.field("useDepthPeeling").value
   pass
