# Python module import
from ObjMgr import *
from VisDefinitions import *
from string import *

# Global class instances
_cls_info = None

def init():
   global _cls_info

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = ObjInfo(ctx.module("info"))
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
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
               
   if layer == LAY_APPEARANCE:
      if info == INF_STIPPLING or info == INF_VISIBLE:
         visibleOnChanged()
      elif info == INF_STIPPLINGGAMMA:
         ctx.field("SoStippling.gamma").value = _cls_info.getDouble(object, LAY_APPEARANCE, INF_STIPPLINGGAMMA)
      elif info == INF_TRANSPARENCY:
         ctx.field("SoStippling.transparency").value = _cls_info.getDouble(object, LAY_APPEARANCE, INF_TRANSPARENCY)
      elif info == INF_STIPPLINGCOLOR:
         ctx.field("SoStippling.pointColor").value = _cls_info.getDouble(object, LAY_APPEARANCE, INF_STIPPLINGCOLOR)
            	
   elif layer == LAY_DESCRIPTION:
      if info == INF_STRUCTURE:
         ctx.field("structure").value = value
         
   return


def initialize(field = 0):
   global _cls_info
   objectID = ctx.field("objectID").value

   ctx.field("UMDPointerToInventor.objectID").setStringValue(objectID)
   ctx.field("UMDPointerToInventor.layerID").setStringValue(LAY_FILES)
   ctx.field("UMDPointerToInventor.infoID").setStringValue(INF_IVPOINTER)
   
   ctx.field("StipplePointer.objectID").setStringValue(objectID)
   ctx.field("StipplePointer.layerID").setStringValue(LAY_FILES)
   ctx.field("StipplePointer.infoID").setStringValue(INF_STIPPLEPOINTER)

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_STIPPLINGGAMMA) != "":
      gamma = _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_STIPPLINGGAMMA)
      if gamma < 0:
         gamma = 0
      ctx.field("SoStippling.gamma").value = gamma
   else:
      ctx.field("SoStippling.gamma").value = 1.5

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_STIPPLING) == "TRUE":
      ctx.field("SoToggle.on").value = True
   else:
      ctx.field("SoToggle.on").value = False
   pass
   

def visibleOnChanged(field = 0):
   object = _cls_info.activeObject()
   if _cls_info.getBool(object, LAY_APPEARANCE, INF_STIPPLING) == True and (_cls_info.getBool(object, LAY_APPEARANCE, INF_VISIBLE) == True or ctx.field("visibleOn").stringValue() == "StipplingVisible"):
      ctx.field("SoToggle.on").value = True
   else:
      ctx.field("SoToggle.on").value = False
   pass
