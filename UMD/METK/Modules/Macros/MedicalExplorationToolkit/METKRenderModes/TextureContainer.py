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
      if info == INF_TEXTURIZE:
         ctx.field("textureToggle.on").value = _cls_info.getBool()
      elif info == INF_TEXTURETYPE:
         if value == "HATCHING_OPACITY":
            ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256_alpha.rgb"
            ctx.field("SoTexture2.model").value = "MODULATE"
      elif info == INF_TEXTURESCALE:
         ctx.field("SoTexture2Transform.scaleFactor").value = _cls_info.getDouble(), _cls_info.getDouble()
   
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
   
   ctx.field("TexturePointer.objectID").setStringValue(objectID)
   ctx.field("TexturePointer.layerID").setStringValue(LAY_FILES)
   ctx.field("TexturePointer.infoID").setStringValue(INF_TEXPOINTER)
   
   
   ctx.field("textureToggle.on").value = _cls_info.getBool(objectID, LAY_APPEARANCE, INF_TEXTURIZE)
   
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_TEXTURETYPE) != "":
      if _cls_info.get(objectID, LAY_APPEARANCE, INF_TEXTURETYPE) == "HATCHING_TAM":
         ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256.rgb"
         ctx.field("SoTexture2.model").value = "MODULATE"
      elif _cls_info.get(objectID, LAY_APPEARANCE, INF_TEXTURETYPE) == "HATCHING_OPACITY":
         ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256_alpha.rgb"
         ctx.field("SoTexture2.model").value = "MODULATE"
   else:
      ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256.rgb"
      ctx.field("SoTexture2.model").value = "MODULATE"
   
   if _cls_info.get(objectID, LAY_APPEARANCE, INF_TEXTURESCALE) != "":
      ctx.field("SoTexture2Transform.scaleFactor").value = _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_TEXTURESCALE),  _cls_info.getDouble(objectID, LAY_APPEARANCE, INF_TEXTURESCALE)
   pass


def visibleOnChanged(field = 0):
   object = _cls_info.activeObject()
   if _cls_info.getBool(object, LAY_APPEARANCE, INF_TEXTURIZE) == True and (_cls_info.getBool(object, LAY_APPEARANCE, INF_VISIBLE) == True or ctx.field("visibleOn").stringValue() == "TextureVisible"):
      ctx.field("textureToggle.on").value = True
   else:
      ctx.field("textureToggle.on").value = False
   pass


def modeChanged(field = 0):
   if field.value == "HATCHING_TAM":
      ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256.rgb"
      ctx.field("SoTexture2.model").value = "MODULATE"
   elif field.value == "HATCHING_OPACITY":
      ctx.field("SoTexture2.filename").value = ctx.localPath() + "/textures/TAM256x256_alpha.rgb"
      ctx.field("SoTexture2.model").value = "MODULATE"
   pass