# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

_cls_info = None
_cls_iter = None

def init():
   global _cls_info, _cls_iter
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))   
   _cls_iter = ObjIterator(ctx.module("iter"))
   _cls_iter.searchFirst()
   return
   
   
def createObjects(fieldP=0):
   ctx.log("create SafetyMarginObjects")
   objID = ctx.field("structure").value
   if objID=="":
      ctx.log("no structure given")
      return
 
   ctx.log("load segmask of "+ objID)
   ctx.field("METKLoadSegMask.load").touch()   
   MLAB.processEvents()
   
   caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)

   ctx.log("calculate distance transformation")
   #objValue = _cls_info.get(objID,LAY_IMAGE, INF_OBJVALUE)
   #ctx.field("EuclideanDTF.minValue").setStringValue(objValue)
   #ctx.field("EuclideanDTF.minValue").setStringValue(objValue)
   ctx.field("EuclideanDTF.update").touch()
   ctx.field("EuclideanDTF.apply").touch()
   MLAB.processEvents()
   
   #get safety margins
   margins = ctx.field("marginList").value.split(",")                  
   for singleMargin in margins:
      if (singleMargin!=""):         
         ctx.log("Margin="+singleMargin)         
         newObjID = objID + "_" + singleMargin + "mm"
         
         ctx.field("IntervalThresh.threshMax").setStringValue(singleMargin)         
         MLAB.processEvents()
         
         ctx.field("ImgSave.filename").value = caseDir+"/"+newObjID+".dcm"
         ctx.field("ImgSave.save").touch()
         MLAB.processEvents()
         
         #create structures in objmgr         
         newName = _cls_info.get(objID,LAY_DESCRIPTION, INF_NAME) + "_" + singleMargin + "mm"
         _cls_info.typedSet(newObjID, LAY_DESCRIPTION, INF_NAME, newName, INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_DESCRIPTION, INF_RESULTTYPE, "SafetyMarginCalculation", INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_DESCRIPTION, INF_STRUCTUREGROUP, "SafetyMargin", INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_DESCRIPTION, INF_STRUCTURE, "TumorSafetyMargin", INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_DESCRIPTION, "Margin", singleMargin, INFOTYPE_INT32)
         _cls_info.typedSet(newObjID, LAY_IMAGE, INF_FILENAME, newObjID+".dcm", INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_IMAGE, INF_OBJVALUE, "1", INFOTYPE_INT32)
         _cls_info.typedSet(newObjID, LAY_IMAGE, INF_IMAGETYPE, "Segmentation", INFOTYPE_STRING)
         _cls_info.typedSet(newObjID, LAY_GLOBAL, INF_OBJTYPE, "Result", INFOTYPE_STRING)

   setColors()   
   return


def setColors(args=0):
   ctx.log("set color and transparency for all safetyMargin structures")
   colors = ctx.field("colorMargins").value.split(",")
   if (len(colors)!=2):
      ctx.log("Error: type in exactly 2 color margins for red-yellow and yellow-green border")
      return
   
   #HIER MUSS UNBEDINGT GEKLÄRT WERDEN, WARUM DIE OBJMGR.py
   #MIT DEM ObjIterator NICHT MEHR KLAR KOMMT!!!
   
   #_cls_iter.searchFirst()
   ctx.field("iter.firstObject").touch()
   #while _cls_iter.success():
   while ctx.field("iter.operationSuccessful").value==True:      
      #objID = _cls_iter.object()
      objID = ctx.field("iter.objectID").value
      margin = int(_cls_info.get(objID, LAY_DESCRIPTION, "Margin"))
      if margin<=int(colors[0]):
         _cls_info.typedSet(objID, LAY_APPEARANCE, INF_COLOR, "1 0 0", INFOTYPE_VEC3)
      elif margin<=int(colors[1]):
         _cls_info.typedSet(objID, LAY_APPEARANCE, INF_COLOR, "1 1 0", INFOTYPE_VEC3)
      else:
         _cls_info.typedSet(objID, LAY_APPEARANCE, INF_COLOR, "0 1 0", INFOTYPE_VEC3)
      
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_TRANSPARENCY, str(ctx.field("transparency").value), INFOTYPE_DOUBLE)
      
      #_cls_iter.searchNext()
      ctx.field("iter.nextObject").touch()
   
   _cls_info.notify()
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
        
   #ctx.log(object + " " + layer + " " + info + " " + value)
        
   #if layer == LAY_CLIPPING_LEFT:
   #   if info == INF_CLIPPING_ON:         
   pass

   
def handleLoadedEvent():
   pass

   
def handleSaveEvent():
   pass

   
def handleCleanupEvent():
   pass
