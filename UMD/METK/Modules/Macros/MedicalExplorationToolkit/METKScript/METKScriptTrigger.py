from ObjMgr import *
from VisDefinitions import *
from linear import *

_cls_info= None


def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))   
   return



def executeScript(field):     
   _cls_info.typedSet( OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_SCRIPTFILE, ctx.field("scriptFile").value,INFOTYPE_MESSAGE)
   _cls_info.notify()
   return