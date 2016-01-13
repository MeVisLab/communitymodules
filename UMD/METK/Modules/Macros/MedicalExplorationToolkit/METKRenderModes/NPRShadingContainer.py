# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
from string import *

from NPRShadingParameter import *

# Global class instances
_cls_info = None

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = ObjInfoExt(ctx.module("ObjInfo"))
   pass


# Beim Laden eines Falles wird einmalig diese Methode aufgerufen
# Netzwerk und Felder müssen geupdatet werden
def initialize(field = 0):
   objectID = ctx.field("objectID").value
   ctx.field("ObjInfo.searchObjPattern").value = objectID

   ctx.field("UMDPointerToInventor.objectID").setStringValue(objectID)
   ctx.field("UMDPointerToInventor.layerID").setStringValue(LAY_FILES)
   ctx.field("UMDPointerToInventor.infoID").setStringValue(INF_IVPOINTER)
      
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR) != "":
      ctx.field("WEMInfoGenerator.color").setStringValue(_cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR))
   else:
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_COLOR, "0.8 0.8 0.8", INFOTYPE_VEC3)
      
   if _cls_info.get(objectID, LAY_DESCRIPTION, INF_IMPORTANCE) != "":
      value = _cls_info.getDouble(objectID, LAY_DESCRIPTION, INF_IMPORTANCE)
      if value > 1.0:
         value = 1.0
      ctx.field("WEMInfoGenerator.doi").value = value
   else:
      if _cls_info.get(objectID, LAY_APPEARANCE, INF_TRANSPARENCY) != "":
         ctx.field("WEMInfoGenerator.doi").value = 1.0 - _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_TRANSPARENCY)
      else:
         ctx.field("WEMInfoGenerator.doi").value = 1.0
         _cls_info.typedSet(objectID, LAY_DESCRIPTION, INF_IMPORTANCE, 1.0, INFOTYPE_DOUBLE)

   if _cls_info.get(objectID, LAY_APPEARANCE, INF_TRANSPARENCY) != "":
      ctx.field("WEMInfoGenerator.transparency").value = _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_TRANSPARENCY)
   else:
      ctx.field("WEMInfoGenerator.transparency").value = 0.0
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TRANSPARENCY, 0.0, INFOTYPE_DOUBLE)

   # Auswahl des richtigen displayType
   if _cls_info.existInfo(objectID, LAY_APPEARANCE, INF_VISIBLE):
      vis = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_VISIBLE)
   else:
      vis = False
   if _cls_info.existInfo(objectID, LAY_APPEARANCE, INF_STIPPLING):
      sti = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_STIPPLING)
   else:
      sti = False
   if _cls_info.existInfo(objectID, LAY_APPEARANCE, INF_TEXTURIZE):
      hat = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_TEXTURIZE)
   else:
      hat = False
   if vis and not sti and not hat:
      ctx.field("displayType").value = "ShadingMap"
   elif vis and sti:
      ctx.field("displayType").value = "Stippling"
   elif vis and hat:
      ctx.field("displayType").value = "Hatching"
   else:
      ctx.field("displayType").value = "Invisible"
   
   objectName = _cls_info.adjustObjectID(objectID)
   ctx.field("WEMInfoGenerator.name").value = objectName

   ctx.field("WEMInfoGenerator.autoApply").value = True
   
   _cls_info.notify()
   pass
   

# Wenn im Panel an den Reglern gespielt wurde, wird diese Methode aufgerufen
# Netzwerk und ObjMgr (METK) müssen geupdatet werden
def settingsChanged(field = 0):
   objectID = ctx.field("objectID").value
   if field == ctx.field("WEMInfoGenerator.color"):
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_COLOR, field.stringValue(), INFOTYPE_VEC3)
   
   elif field == ctx.field("displayType"):
      if field.value == "ShadingMap":
         ctx.field("showAttrib.on").value    = True
         ctx.field("showMap.on").value       = True
         ctx.field("showHatching.on").value  = False
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,   True,  INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_STIPPLING, False, INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TEXTURIZE, False, INFOTYPE_BOOL)
      elif field.value == "Stippling":
         ctx.field("showAttrib.on").value    = True
         ctx.field("showMap.on").value       = False
         ctx.field("showHatching.on").value  = False
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,   True,  INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_STIPPLING, True,  INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TEXTURIZE, False, INFOTYPE_BOOL)
      elif field.value == "Hatching":
         ctx.field("showAttrib.on").value    = True
         ctx.field("showMap.on").value       = False
         ctx.field("showHatching.on").value  = True
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,   True,  INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_STIPPLING, False, INFOTYPE_BOOL)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TEXTURIZE, True,  INFOTYPE_BOOL)
      elif field.value == "Invisible":
         ctx.field("showAttrib.on").value    = False
         ctx.field("showMap.on").value       = False
         ctx.field("showHatching.on").value  = False
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,   False,  INFOTYPE_BOOL)
   
   elif field == ctx.field("transparency"):
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TRANSPARENCY, field.value, INFOTYPE_DOUBLE)
   
   elif field == ctx.field("doi"):
      _cls_info.typedSet(objectID, LAY_DESCRIPTION, INF_IMPORTANCE, field.value, INFOTYPE_DOUBLE)

   _cls_info.notify()
   pass


#--------------------------------------------
# Event handling
#--------------------------------------------

# Wenn im ObjMgr (per METK) an den Reglern gespielt wurde, werden diese Methoden aufgerufen
# Netzwerk muss geupdatet werden
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
         ctx.field("WEMInfoGenerator.color").setStringValue(value)
      elif info == INF_TRANSPARENCY:
         ctx.field("WEMInfoGenerator.transparency").setStringValue(value)
      elif info == INF_VISIBLE or info == INF_STIPPLING or info == INF_TEXTURIZE:
         vis = _cls_info.getBool(object, LAY_APPEARANCE, INF_VISIBLE)
         sti = _cls_info.getBool(object, LAY_APPEARANCE, INF_STIPPLING)
         hat = _cls_info.getBool(object, LAY_APPEARANCE, INF_TEXTURIZE)
         if vis and not sti and not hat:
            ctx.field("displayType").value = "ShadingMap"
         elif vis and sti:
            ctx.field("displayType").value = "Stippling"
         elif vis and hat:
            ctx.field("displayType").value = "Hatching"
         else:
            ctx.field("displayType").value = "Invisible"
         
   elif layer == LAY_DESCRIPTION:
      if info == INF_IMPORTANCE:
         ctx.field("WEMInfoGenerator.doi").setStringValue(value)
   return
