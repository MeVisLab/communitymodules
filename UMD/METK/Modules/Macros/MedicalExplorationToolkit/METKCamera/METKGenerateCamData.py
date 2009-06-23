# Python module import
from ObjMgr import *
from VisDefinitions import *

_cls_info= None

def init():
   global _cls_info   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))         
   return





#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():   
   pass

def handleObjectRemovedEvent():
   return

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
   
   #if layer == LAY_VIEWER_CAMERA:    
   #   if info == INF_VIEWER_CAMERA_POSITION:
   #      ctx.field("camera.position").setStringValue(value)
   #   elif info == INF_VIEWER_CAMERA_ORIENTATION:
   #      ctx.field("camera.orientation").setStringValue(value)
   #elif layer == LAY_VIEWER_PROPERTIES:
   #    if info == INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS:
   #       ctx.field("connected").setStringValue(value)

   pass