# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

_cls_info= None

def init():
   global _cls_info
   # Create an METKObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))      
   _cls_info.setEventReceiveMode(ERM_NONE) 
   return
   
   
def loadMask(fieldP=0):
   objID = ctx.field("objID").value
   if (objID==""):
      ctx.field("ImgLoad.filename").value=""
      return
   
   filename = _cls_info.get(objID, LAY_IMAGE, INF_FILENAME)
   if (filename==""):
      ctx.field("ImgLoad.filename").value=""
      return   
   
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   ctx.field("ImgLoad.filename").value = _caseDir + "/" + filename
   objValue = _cls_info.get(objID,LAY_IMAGE, INF_OBJVALUE)
   if (objValue!=""):
      ctx.field("IntervalThresh.threshMin").value = int(objValue)
      ctx.field("IntervalThresh.threshMax").value = int(objValue)
   else:
      ctx.field("IntervalThresh.threshMin").value = int(ctx.field("IntervalThresh.imageMin").value)
      ctx.field("IntervalThresh.threshMax").value = int(ctx.field("IntervalThresh.imageMax").value)
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   #_cls_info.setAndNotify(OBJ_CSO, LAY_GLOBAL, INF_OBJTYPE, "CSO")
   return

def handleObjectRemovedEvent():
   pass

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   pass

def handleAttributeModifiedEvent():
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()         
   return


def handleLoadedEvent():
   #loadMask()
   pass


def handleCleanupEvent():
   pass   


def handleSaveEvent():
   pass