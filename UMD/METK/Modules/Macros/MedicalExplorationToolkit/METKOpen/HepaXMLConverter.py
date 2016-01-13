# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
from string import *

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
   version = _cls_input.get("PatientObject", "Description", "Description") #eindeutig ein hepa-fall?
   if version == "MeVisLiverCase" or version == "HepaVision Import" or version == "Liver Analysis":
      ctx.field("readableFormat").value = True
      ctx.log("Es ist ein MeVisLiverCase")
      _cls_input.handleEvents()
      _cls_output.set(OBJ_APPLICATION, LAY_GLOBAL, INF_OBJTYPE, "Application")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONNAME, "MeVisLiver")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONVERSION, "Version unbekannt")
      _cls_output.set(OBJ_APPLICATION, LAY_APPLICATION, INF_MEVISLAB, "Version unbekannt")
      
      _cls_output.set(OBJ_CASE, LAY_GLOBAL, INF_OBJTYPE, "Case")
      _cls_output.set(OBJ_PATIENT, LAY_GLOBAL, INF_OBJTYPE, "Patient")
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
   
   if layer == "Description":
      if info == "ResultType":
         _cls_output.set(object,layer,info,"Segmentation")
      else:
         _cls_output.typedSet(object, layer, info, value, type)
   elif layer == "Appearance":      
      if info == "Color": #Type conversion da Milo die Farbe als String speicherte ... grrrrr
         _cls_output.typedSet(object,layer,info,"",INFOTYPE_VEC3)
         _cls_output.set(object,layer,info,value)
      elif info == "Transparency":
         _cls_output.typedSet(object,layer,info,"",INFOTYPE_DOUBLE)
         _cls_output.set(object,layer,info,value)
      else:
         _cls_output.typedSet(object, layer, info, value, type)
         
   elif layer == LAY_IMAGE:
      #herausfiltern der Parents, die eine Segmentation sein sollen aber Kinder haben
      childIDs = _cls_input.get(object,LAY_GLOBAL,"ChildIds")
      imageType = _cls_input.get(object,"Image","ImageType")      
      if (childIDs == "" or (imageType!="CodedSegmentation" and imageType!="Segmentation")):   
         if info == "Filename":            
            objValue = _cls_input.get(object,layer,INF_OBJVALUE)
            imageType = _cls_input.get(object,layer,INF_IMAGETYPE)
            value = value.replace("./","")                        
	    _cls_output.set(object,layer,info,value)
            if objValue!="-1" and objValue!="0" and objValue!="" and not _cls_input.existInfo(object,LAY_FILES,INF_IVFILE):
            #if objValue!="-1" and objValue!="0" and objValue!="":
               _cls_output.set(object, LAY_FILES, INF_IVFILE, object + "_" + objValue + ".iv")
               _cls_output.typedSet(object,layer,INF_OBJVALUE,objValue,INFOTYPE_INT32)
            
            #Ist kein ObjectValue vorhanden, wird der TraceColorCode als ObjectValue angenommen
            elif objValue=="" and not imageType == "Original":
               traceColor = _cls_input.get(object, "Appearance", "TraceColorCode")
               if traceColor != "":
                  #print traceColor
                  objValueNew = int(traceColor)*50+12025
                  #print "%s" %objValueNew
                  _cls_output.set(object, LAY_FILES, INF_IVFILE, object + "_" + str(objValueNew) + ".iv")
                  _cls_output.typedSet(object,layer,INF_OBJVALUE,objValueNew,INFOTYPE_INT32)
               else:
                  _cls_output.set(object, LAY_FILES, INF_IVFILE, object + ".iv")	 
               
         elif info == "ImageType":
            if value == "CodedSegmentation":
               _cls_output.set(object,layer,info,"Segmentation")
            else:
               _cls_output.typedSet(object, layer, info, value, type)
         elif info == INF_OBJVALUE:
            if not _cls_output.existInfo(object,LAY_IMAGE,INF_OBJVALUE):
               _cls_output.typedSet(object,LAY_IMAGE,INF_OBJVALUE, _cls_input.get(object,LAY_IMAGE,INF_OBJVALUE),INFOTYPE_INT32)
         
         else:
            _cls_output.typedSet(object, layer, info, value, type)
      else:
         _cls_input.set(object,"Image","ImageType","Group")
         _cls_output.set(object,"Image","ImageType","Group")
         
   else:
      _cls_output.typedSet(object, layer, info, value, type)
   return


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   pass
