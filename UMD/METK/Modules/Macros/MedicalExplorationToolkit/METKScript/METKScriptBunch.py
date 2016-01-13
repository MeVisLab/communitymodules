from ObjMgr import *
from VisDefinitions import *

_cls_METKInfo = None

def init():
   global _cls_METKInfo
   _cls_METKInfo = METKObjInfo(ctx.module("info"))   
   
   return



def entScriptSelected(item, column):   
   #ctx.field("UMDAnimation2.pathTypeScriptPath").value="Use module path"
   _cls_METKInfo.typedSet(OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_SCRIPTFILE, ctx.field("baseScriptPath").value + "/ENT/"+item.text(1), INFOTYPE_MESSAGE)
   _cls_METKInfo.notify()   
   return

def liverScriptSelected(item, column):   
   #ctx.field("UMDAnimation2.pathTypeScriptPath").value="Use module path"
   _cls_METKInfo.typedSet(OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_SCRIPTFILE, ctx.field("baseScriptPath").value + "/Liver/"+item.text(1), INFOTYPE_MESSAGE)
   _cls_METKInfo.notify()   
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
   object = _cls_METKInfo.activeObject()
   layer  = _cls_METKInfo.activeLayer()
   info   = _cls_METKInfo.activeInfo()
   value  = _cls_METKInfo.get()
   pass


def handleLoadedEvent():   
   pass

def handleSaveEvent():
   pass

def handleCleanupEvent():   
   pass