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
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_PATIENT + "," + OBJ_COMMUNICATION, LAY_PATIENT + "," + LAY_GLOBALEVENTS)
   return


def changed(field):
   _cls_info.setAndNotify(OBJ_PATIENT, LAY_PATIENT, field.name, field.value)
   return


def handleObjectCreatedEvent():
   _cls_info.setAndNotify(OBJ_PATIENT, LAY_GLOBAL, INF_OBJTYPE, "Patient")
   return


def handleObjectRemovedEvent():
   ctx.field("Name").value = ""
   ctx.field("DOB").value = ""
   ctx.field("PID").value = ""
   ctx.field("Weight").value = ""
   ctx.field("Sex").value = ""
   return
   

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return


def handleAttributeRemovedEvent():
   layer  = _cls_info.activeLayer()
   if layer == LAY_PATIENT:
      info   = _cls_info.activeInfo()
      ctx.field(info).value = ""
   return


def handleAttributeModifiedEvent():
   layer  = _cls_info.activeLayer()
   if layer == LAY_PATIENT:
      info   = _cls_info.activeInfo()
      value  = _cls_info.get()
      ctx.field(info).value = value
   else:
      ctx.field("Name").value   = _cls_info.get(OBJ_PATIENT, LAY_PATIENT, INF_NAME)
      ctx.field("DOB").value    = _cls_info.get(OBJ_PATIENT, LAY_PATIENT, INF_DOB)
      ctx.field("PID").value    = _cls_info.get(OBJ_PATIENT, LAY_PATIENT, INF_PID)
      ctx.field("Weight").value = _cls_info.get(OBJ_PATIENT, LAY_PATIENT, INF_WEIGHT)
      ctx.field("Sex").value    = _cls_info.get(OBJ_PATIENT, LAY_PATIENT, INF_SEX)
      
   return
