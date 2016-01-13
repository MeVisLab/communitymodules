# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   pass


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
   pass

   
def handleLoadedEvent():
   pass

   
def handleSaveEvent():
   pass

   
def handleCleanupEvent():
   pass


def objectChanged(field = 0):
   objectID = field.value
   # objectID exists and is valid
   if objectID != "" and _cls_info.existObject(objectID):
      # objectID is a structure
      if _cls_info.get(objectID, LAY_GLOBAL, INF_OBJTYPE) == "Result" and _cls_info.get(objectID, LAY_IMAGE, INF_IMAGETYPE) == "Segmentation":
         ctx.field("valid").value = True
         
         ctx.field("parent").value = _cls_info.get(objectID, LAY_GLOBAL, INF_PARENT)
         
         ctx.field("name").value = _cls_info.get(objectID, LAY_DESCRIPTION, INF_NAME)
         ctx.field("structure").value = _cls_info.get(objectID, LAY_DESCRIPTION, INF_STRUCTURE)
         ctx.field("strGroup").value = _cls_info.get(objectID, LAY_DESCRIPTION, INF_STRUCTUREGROUP)
         
         ctx.field("color").setStringValue(_cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR))
         ctx.field("flatShading").value = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_FLATISO)
         ctx.field("transparency").value = _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_TRANSPARENCY)
         ctx.field("visible").value = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_VISIBLE)
      else:
         ctx.field("valid").value = False
   else:
      ctx.field("valid").value = False
   pass
   
   
def getSingleValue(field = 0):
   ctx.field("exist").value = _cls_info.existInfo(ctx.field("objectID").value, ctx.field("layer").value, ctx.field("info").value)
   if ctx.field("exist").value:
      ctx.field("value").value = _cls_info.get(ctx.field("objectID").value, ctx.field("layer").value, ctx.field("info").value)
   pass
   

def applyStandard(field = 0):
   objectID = ctx.field("objectID").value
   
   _cls_info.set(objectID, LAY_GLOBAL,      INF_PARENT,           ctx.field("parent").value)
         
   _cls_info.set(objectID, LAY_DESCRIPTION, INF_NAME,             ctx.field("name").value)
   _cls_info.set(objectID, LAY_DESCRIPTION, INF_STRUCTURE,        ctx.field("structure").value)
   _cls_info.set(objectID, LAY_DESCRIPTION, INF_STRUCTUREGROUP,   ctx.field("strGroup").value)
         
   _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_COLOR,        ctx.field("color").stringValue(), INFOTYPE_VEC3)
   _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_FLATISO,      ctx.field("flatShading").value,   INFOTYPE_BOOL)
   _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TRANSPARENCY, ctx.field("transparency").value,  INFOTYPE_DOUBLE)
   _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,      ctx.field("visible").value,       INFOTYPE_BOOL)
   _cls_info.notify()
   pass
