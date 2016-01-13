# Python module import
from ObjMgr import *
from VisDefinitions import *
from time import *

_cls_info= None
_lastUpdate = 0

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = ObjInfoExt(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_COMMUNICATION, LAY_STATUS)
   pass


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass

def handleObjectRemovedEvent():
   pass

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   pass

def handleAttributeRemovedEvent():
   handleAttributeModifiedEvent()
   pass

def handleAttributeModifiedEvent():
   global _lastUpdate
   ctx.field("statusMsg").value = _cls_info.get(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG)
   if (_cls_info.existInfo(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS)):
      ctx.field("progress").value = _cls_info.getDouble(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS)
   _lastUpdate = time()
   ctx.field("IdleTrigger.on").value = True
   pass
   
   
def setReady(field):
   global _lastUpdate
   if (time() - _lastUpdate) > 1.0:
      ctx.field("IdleTrigger.on").value = False
      ctx.field("statusMsg").value = "Ready."
      ctx.field("progress").value = 0
   pass