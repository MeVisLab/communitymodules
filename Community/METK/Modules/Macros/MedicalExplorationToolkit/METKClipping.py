# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

_cls_info= None

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))   
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass

def handleObjectRemovedEvent():
   #ctx.log("handleObjectRemovedEvent")
   #ctx.module("input").removeChildren()
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
        
   if layer == LAY_CLIPPING_LEFT:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.left").value = True
         else:
            ctx.field("SoClipBox.left").value = False 
      elif info == INF_CLIPPING_TRANSLATION:         
         ctx.field("SoClipBox.translation0").setStringValue(value)         
      elif info == INF_CLIPPING_ROTATION:         
         ctx.field("SoClipBox.rotation0").setStringValue(value)
   
   elif layer == LAY_CLIPPING_RIGHT:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.right").value = True
         else:
            ctx.field("SoClipBox.right").value = False 
      elif info == INF_CLIPPING_TRANSLATION:         
         ctx.field("SoClipBox.translation1").setStringValue(value)         
      elif info == INF_CLIPPING_ROTATION:         
         ctx.field("SoClipBox.rotation1").setStringValue(value)
   
   elif layer == LAY_CLIPPING_TOP:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.top").value = True
         else:
            ctx.field("SoClipBox.top").value = False 
      elif info == INF_CLIPPING_TRANSLATION:         
         ctx.field("SoClipBox.translation4").setStringValue(value)         
      elif info == INF_CLIPPING_ROTATION:         
         ctx.field("SoClipBox.rotation4").setStringValue(value)
   
   elif layer == LAY_CLIPPING_BOTTOM:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.bottom").value = True
         else:
            ctx.field("SoClipBox.bottom").value = False 
      elif info == INF_CLIPPING_TRANSLATION:         
         ctx.field("SoClipBox.translation5").setStringValue(value)         
      elif info == INF_CLIPPING_ROTATION:         
         ctx.field("SoClipBox.rotation5").setStringValue(value)
   
   elif layer == LAY_CLIPPING_REAR:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.rear").value = True
         else:
            ctx.field("SoClipBox.rear").value = False 
      elif info == INF_CLIPPING_TRANSLATION:         
         ctx.field("SoClipBox.translation2").setStringValue(value)         
      elif info == INF_CLIPPING_ROTATION:         
         ctx.field("SoClipBox.rotation2").setStringValue(value)
         
   elif layer == LAY_CLIPPING_FRONT:
      if info == INF_CLIPPING_ON:
         if (value=="ON"):
            ctx.field("SoClipBox.front").value = True
         else:
            ctx.field("SoClipBox.front").value = False
      elif info == INF_CLIPPING_TRANSLATION:
         ctx.field("SoClipBox.translation3").setStringValue(value)
      elif info == INF_CLIPPING_ROTATION:
         ctx.field("SoClipBox.rotation3").setStringValue(value)
         
         
         
         
         
   elif layer == LAY_GLOBAL:
      if info == INF_CLIPPING_SCALE:
         ctx.log("SCALE: " + value)
         ctx.field("SoScale1.scaleFactor").setStringValue(value)
         helpScale = ctx.field("SoScale1.scaleFactor").value;
         if (helpScale[0]!=0) and (helpScale[1]!=0) and (helpScale[2]!=0):
            ctx.field("SoScale2.scaleFactor").setStringValue(str(1/helpScale[0])+" "+str(1/helpScale[1])+" "+str(1/helpScale[2]))
      elif info == INF_CLIPPING_TRANSLATION:
         ctx.log("TRANS: "+ value)
         ctx.field("SoTranslation1.translation").setStringValue(value)
         helpTrans = ctx.field("SoTranslation1.translation").value;
         ctx.field("SoTranslation2.translation").setStringValue(str(-1*helpTrans[0])+" "+str(-1*helpTrans[1])+" "+str(-1*helpTrans[2]))
                 
   pass