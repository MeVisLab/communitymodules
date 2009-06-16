from ObjMgr import *
from VisDefinitions import *
from linear import *

_cls_info= None
_waitForAnimation = False


def init():
   global _cls_info   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))   
   
   global _waitForAnimation
   _waitForAnimation = False
   
   return


def resetState(field):
   global _waitForAnimation
   _waitForAnimation = False
   return
   
   
def loadInitScript(field):
   global _waitForAnimation
   _waitForAnimation = True
   _cls_info.typedSet(OBJ_ANIMATION, LAY_ANIMATION_GLOBAL, INF_ANIMATION_RESET, "1", INFOTYPE_MESSAGE)
   _cls_info.notify()
   _cls_info.typedSet(OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_SCRIPTFILE, ctx.field("scriptFile").value,INFOTYPE_MESSAGE)
   
   # Open InfoWin
   #_cls_info.set(OBJ_GUI, LAY_INFOWIN, INF_INFOHEADER,"Loading Case")
   #_cls_info.set(OBJ_GUI, LAY_INFOWIN, INF_INFOTEXT,"Load initial configuration")   
   #_cls_info.typedSet(OBJ_GUI, LAY_INFOWIN, INF_ISON,"1",INFOTYPE_BOOL)   
   
   ctx.callLater(0.001,"_cls_info.notify")
   return
   
   
   
   
#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():   
   handleAttributeModifiedEvent()
   pass

def handleObjectRemovedEvent():
   return

def handleAttributeCreatedEvent():   
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   pass

def handleAttributeModifiedEvent():
   global _waitForAnimation
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   if (layer == LAY_GLOBALEVENTS and info == INF_CASELOADED and value == MSG_LOADED):
         loadInitScript(0)
   if (object == OBJ_ANIMATION and layer == LAY_ANIMATION_SCRIPT and info == INF_SCRIPT_STATUS and value == "ready"):
      if (_waitForAnimation):         
         _waitForAnimation=False
         _cls_info.typedSet(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED, MSG_TOTALLOADED, INFOTYPE_MESSAGE)
         ctx.callLater(0.0001,"_cls_info.notify")
         ctx.field("scriptExecuted").touch()
   pass