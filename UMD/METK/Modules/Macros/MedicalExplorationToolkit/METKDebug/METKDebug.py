# Python module import
from ObjMgr import *
from VisDefinitions import *
from time import *

_cls_info= None
_record = ctx.field("record")
_log = ""

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfoExt(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_ALL)
   pass


def handleEventsOnce():
   global _log
   if _record.value:
      _log = _log + strftime("%H:%M:%S") + ": <b>" + str(ctx.field("ObjInfo.numEvents").value) + "</b> Event(s) <br>"
      ctx.field("log").value = _log
   pass


def clear(field = 0):
   global _log
   _log = ""
   ctx.field("log").value = _log
   pass