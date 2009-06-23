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
      if info == INF_SILHOUETTECOLOR:
         if (value != ""):
            ctx.field("SoSilhouette.outlineColor").setStringValue(_cls_info.getString())
      elif info == INF_SILHOUETTE or info == INF_VISIBLE:
         visibleOnChanged()
         
      elif info == INF_SILHOUETTEWIDTH:
         width = _cls_info.getDouble()
         if width <= 0:
            width = 1
         ctx.field("SoSilhouette.outlineWidth").value = width
            	
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
   
   ctx.field("SilhouettePointer.objectID").setStringValue(objectID)
   ctx.field("SilhouettePointer.layerID").setStringValue(LAY_FILES)
   ctx.field("SilhouettePointer.infoID").setStringValue(INF_SILHOUETTEPOINTER)
      
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTECOLOR) != "":
      ctx.field("SoSilhouette.outlineColor").setStringValue(_cls_info.getString(objectID, LAY_APPEARANCE, INF_SILHOUETTECOLOR))
      
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH) != "":
      width = _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH)
      if width <= 0:
         width = 1
      ctx.field("SoSilhouette.outlineWidth").value = width
   else:
      ctx.field("SoSilhouette.outlineWidth").value = 1
      
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTE) == "TRUE":
      ctx.field("SoSilhouette.on").value = True
   else:
      ctx.field("SoSilhouette.on").value = False
   pass
   

def visibleOnChanged(field = 0):
   object = _cls_info.activeObject()
   if ctx.field("visibleOn").stringValue() == "GeneralVisible":
      ctx.field("SoSilhouette.on").value = _cls_info.getBool(object, LAY_APPEARANCE, INF_VISIBLE) and _cls_info.getBool(object, LAY_APPEARANCE, INF_SILHOUETTE)
   else:
      ctx.field("SoSilhouette.on").value = _cls_info.getBool(object, LAY_APPEARANCE, INF_SILHOUETTE)
   pass
   

def modeChanged(field = 0):
   if field.value == "Silhouette":
      ctx.field("SoSwitch.whichChild").value = 0
   else:
      ctx.field("SoSwitch.whichChild").value = 1
   pass