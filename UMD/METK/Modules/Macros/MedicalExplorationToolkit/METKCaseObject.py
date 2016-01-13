# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info= None

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("ObjInfo"))
   
   # Only receive events which occur in Object OBJ_PATIENT and Layer LAY_PATIENT
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CASE + "," + OBJ_COMMUNICATION, LAY_CASE + "," + LAY_GLOBALEVENTS)
   return


def changed(field):
   _cls_info.setAndNotify(OBJ_CASE, LAY_CASE, field.name, field.value)
   return


def handleObjectCreatedEvent():
   _cls_info.setAndNotify(OBJ_CASE, LAY_GLOBAL, INF_OBJTYPE, "Case")
   return


def handleObjectRemovedEvent():
   ctx.field("Directory").value = ""
   ctx.field("XMLFile").value = ""
   ctx.field("Finding").value = ""
   ctx.field("Order").value = ""
   return
   

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return


def handleAttributeRemovedEvent():
   layer  = _cls_info.activeLayer()
   if layer == LAY_CASE:
      info   = _cls_info.activeInfo()
      ctx.field(info).value = ""
   return


def handleAttributeModifiedEvent():
   layer  = _cls_info.activeLayer()
   if layer == LAY_CASE:
      info   = _cls_info.activeInfo()
      value  = _cls_info.get()
      ctx.field(info).value = value
   else:
      ctx.field("Directory").value   = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      ctx.field("XMLFile").value   = _cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE)
      ctx.field("Finding").value    = _cls_info.get(OBJ_CASE, LAY_CASE, INF_FINDING)
      ctx.field("Order").value    = _cls_info.get(OBJ_CASE, LAY_CASE, INF_ORDER)
      
   return
