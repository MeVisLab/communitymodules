# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Global class instances
_cls_input= None
_cls_output= None


def init():
   global _cls_input, _cls_output
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_input  = ObjInfo(ctx.module("input"))
   _cls_output = ObjInfo(ctx.module("output"))

   return
   
def handleEvents(againstTypeError = 0):   
   ctx.field("ready").value = False
   ctx.field("readableFormat").value = False
   application = _cls_input.get(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONNAME)
   version     = _cls_input.get(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONVERSION)
   if version == "1.0" and application == "MedicalExplorationToolkit" or application == "NeckSegmenter":
      ctx.log("Loading MedicalExplorationToolkit XML data")
      ctx.field("readableFormat").value = True
      _cls_input.handleEvents()
      _cls_output.notify()
      ctx.field("ready").value = True
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass


def handleObjectRemovedEvent():
   pass


def handleAttributeCreatedEvent():
   object = _cls_input.activeObject()
   layer  = _cls_input.activeLayer()
   info   = _cls_input.activeInfo()
   value  = _cls_input.get()
   type   = _cls_input.getType(object,layer,info)

   # Jana speichert illegalerweise den Communication object layer mit ab...
   if object != OBJ_COMMUNICATION:
      _cls_output.typedSet(object, layer, info, value, type)
   else:
      MLAB.logWarning("Removed illegal Communcation object entry.")
   return


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   pass
