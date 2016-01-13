# Python module import
from ObjMgr import *
from VisDefinitions import *

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
      if info == INF_COLOR:
         if (value != ""):
            ctx.field("material.diffuseColor").value = value          
      elif info == INF_TRANSPARENCY:
         ctx.field("material.transparency").value = value
      elif info == INF_VISIBLE:
         ctx.field("Visible.on").value = _cls_info.getBool()
      elif info == INF_ISOSURFACE:
         ctx.field("IsoVisible.on").value = _cls_info.getBool()
      elif info == INF_FLATISO:
         if (_cls_info.getBool()):
            ctx.field("material.diffuseColor").value = "0 0 0"
            ctx.field("material.emissiveColor").value = _cls_info.get(object,LAY_APPEARANCE, INF_COLOR)
         else:
            ctx.field("material.diffuseColor").value = _cls_info.get(object,LAY_APPEARANCE, INF_COLOR)
            ctx.field("material.emissiveColor").value = "0 0 0"
         

   elif layer == LAY_DESCRIPTION:
      if info == INF_STRUCTURE:
         ctx.field("structure").value = value
   return


def initialize(field = 0):
   objectID = ctx.field("objectID").value

   ctx.field("UMDPointerToInventor.objectID").setStringValue(objectID)
   ctx.field("UMDPointerToInventor.layerID").setStringValue(LAY_FILES)
   ctx.field("UMDPointerToInventor.infoID").setStringValue(INF_IVPOINTER)
   
   ctx.field("FullObjectPointer.objectID").setStringValue(objectID)
   ctx.field("FullObjectPointer.layerID").setStringValue(LAY_FILES)
   ctx.field("FullObjectPointer.infoID").setStringValue(INF_IVFULLPOINTER)
   
   ctx.field("IsoSurfacePointer.objectID").setStringValue(objectID)
   ctx.field("IsoSurfacePointer.layerID").setStringValue(LAY_FILES)
   ctx.field("IsoSurfacePointer.infoID").setStringValue(INF_ISOPOINTER)
      
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR) != "":
      ctx.field("material.diffuseColor").value = _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR)

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_VISIBLE) == "TRUE":
      ctx.field("Visible.on").value = True
   else:
      ctx.field("Visible.on").value = False

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_ISOSURFACE) == "FALSE":
      ctx.field("IsoVisible.on").value = False
   else:
      ctx.field("IsoVisible.on").value = True

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_TRANSPARENCY) != "":
      ctx.field("material.transparency").value = _cls_info.get(objectID, LAY_APPEARANCE, INF_TRANSPARENCY)

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_FLATISO) == "TRUE":
      ctx.field("material.diffuseColor").value = "0 0 0"
      ctx.field("material.emissiveColor").value = _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR)
   else:
      ctx.field("material.diffuseColor").value = _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR)
      ctx.field("material.emissiveColor").value = "0 0 0"
   pass