# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
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
   
   #ctx.log("ObjCont Event   " + object+ "  " + layer + "  " + info + "  " + value)
               
   if layer == LAY_FILES:   
      if info == INF_IVFILE:
         loadData()
   return
   
def initialize(fieldP = 0):   
   ctx.field("UMDInventorToPointer.objectID").setStringValue(ctx.field("objectID").value)
   ctx.field("UMDInventorToPointer.layerID").setStringValue(LAY_FILES)
   ctx.field("UMDInventorToPointer.infoID").setStringValue(INF_IVPOINTER)
   
   ctx.field("label.label").setStringValue(ctx.field("objectID").value)
   
   directory = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   
   ctx.field("filename").setStringValue(directory + _cls_info.get(ctx.field("objectID").value, LAY_FILES, INF_IVFILE))
   return


def writeBoundingBox(fieldP = 0):   
   if (_cls_info.get(ctx.field("info.searchObjPattern").value, LAY_FILES, INF_IVFILE)!=""):
      _cls_info.set( LAY_MEASURES, "BoundingBox_OldString", ctx.field("UMDBoundingBoxCalculator2.boundingBoxString").value) # obj = current object
      _cls_info.set( LAY_MEASURES, "WorldBoundingBox", ctx.field("UMDBoundingBoxCalculator2.boundingBoxString").value) # obj = current object
      _cls_info.typedSet( LAY_MEASURES, "BoundingBox_min", str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMin").value[0]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMin").value[1]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMin").value[2]),'omAttribute_vec3') # obj = current object
      _cls_info.typedSet( LAY_MEASURES, "BoundingBox_max", str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMax").value[0]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMax").value[1]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boundingBoxMax").value[2]),'omAttribute_vec3') # obj = current object
      _cls_info.typedSet( LAY_MEASURES, "objectCenter", str(ctx.field("UMDBoundingBoxCalculator2.objectCenter").value[0]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.objectCenter").value[1]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.objectCenter").value[2]),'omAttribute_vec3') # obj = current object
      _cls_info.typedSet( LAY_MEASURES, "boxCenter", str(ctx.field("UMDBoundingBoxCalculator2.boxCenter").value[0]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boxCenter").value[1]) + ' ' + str(ctx.field("UMDBoundingBoxCalculator2.boxCenter").value[2]),'omAttribute_vec3') # obj = current object
      _cls_info.notify()        
      ctx.field("measuresValid").setValue(True)
   return
   
   
def loadData(): 
   objectID = ctx.field("objectID").value
   ctx.log("load data "+objectID)
   directory = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   file = _cls_info.get(objectID, LAY_FILES, INF_IVFILE)
   ctx.field("filename").value = directory + file
      
   return
   
   
def callParent(fieldP=0):   
   arg = [ctx.field("objectID").value]
   ctx.parent().call("measuresValid",arg)
   return