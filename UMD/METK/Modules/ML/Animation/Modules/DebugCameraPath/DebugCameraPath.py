# Python module import
from ObjMgr import *
from linear import vec3
from mevis import *
from math import *

_cls_info = None
_moduleCounter = 0

def init():
   global _cls_info, _moduleCounter
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))
   _moduleCounter = 0
   return


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
   pass

def handleAttributeModifiedEvent():
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   #print "ObjCont Event   " + object+ "  " + layer + "  " + info + "  " + value
   
   if object == "Animation":
      if layer == "DebugCameraPath":
         if (info == "PositionCount" and value!="-1"):
            positionCount = value
            #clearSpheres()
            i = 0
            while _cls_info.success():
               position = _cls_info.get("Animation", "DebugCameraPath", "Position"+leadingZero(i,5))
               height = _cls_info.get("Animation", "DebugCameraPath", "Height"+leadingZero(i,5))
               orientation = _cls_info.get("Animation", "DebugCameraPath", "Orientation"+leadingZero(i,5))
               sphereCenter = _cls_info.get("Animation", "DebugCameraPath", "SphereCenter")
               sphereRadius = float(_cls_info.get("Animation", "DebugCameraPath", "SphereRadius"))
               if (position!="" and i<=int(positionCount)):
                  if (height!="" and sphereCenter!=""):
                     splitPos = position.split(" ")
                     splitCenter = sphereCenter.split(" ")
                     if (len(splitPos)==3 and len(splitCenter)==3):
                        
                        pX=float(splitPos[0])
                        pY=float(splitPos[1])
                        pZ=float(splitPos[2])
                        
                        cX=float(splitCenter[0])
                        cY=float(splitCenter[1])
                        cZ=float(splitCenter[2])
                        
                        zeroHeight = sphereRadius
                        heightFactor = ctx.field("heightFactor").value
                        
                        pointToRadiusX = pX-cX
                        pointToRadiusY = pY-cY
                        pointToRadiusZ = pZ-cZ
                                                                        
                        pointToRadiusLen = sqrt(pointToRadiusX*pointToRadiusX+pointToRadiusY*pointToRadiusY+pointToRadiusZ*pointToRadiusZ)
                        normX = pointToRadiusX/pointToRadiusLen
                        normY = pointToRadiusY/pointToRadiusLen
                        normZ = pointToRadiusZ/pointToRadiusLen
                        
                        factor = ( float(height)-zeroHeight ) * heightFactor
                        pX = pX + normX * factor
                        pY = pY + normY * factor
                        pZ = pZ + normZ * factor

                        addSphere(str(pX)+" "+str(pY)+" "+str(pZ),orientation,i)
                     else:
                        print "len(splitPos)!=3 and/or len(splitCenter)!=3"
                  else:
                     addSphere(position,orientation,i)
               i = i + 1
               position = _cls_info.get("Animation", "DebugCameraPath", "Position"+leadingZero(i,5)) #for check cls_info.success
            _cls_info.set("Animation","DebugCameraPath","PositionCount","-1")
   pass
   
   
def addSphere(pos,orientation,nr):
   global _moduleCounter, _oldSpherePos
   
   _moduleCounter = _moduleCounter + 1
   
   if (ctx.field("useRealCams").value==False):
      object = "Position_"+str(_moduleCounter)   
      sphere = ctx.addModule("Sphere2")
      sphere.field("instanceName").setStringValue(object)
      ctx.field(object+".Center").setStringValue(pos)
      ctx.field(object+".Radius").value = ctx.field("radius").value
      ctx.field(object+".Color").value = ctx.field("color").value
      ctx.field(object+".Transparency").value = ctx.field("transparency").value
      ctx.module("output3D").addChild(sphere.field("self"))
   
   setRealCam = False
   if (ctx.field("useRealCams").value==True):
      setRealCam = True
   elif (ctx.field("realCamStep").value>0):
      if (nr%ctx.field("realCamStep").value==0):
         setRealCam = True
   
      
   if (setRealCam):   
      object = "PositionRC_"+str(_moduleCounter)
      cam = ctx.addModule("UMDSoLittleCamera") 
      cam.field("instanceName").setStringValue(object)
      ctx.field(object+".position").setStringValue(pos)
      ctx.field(object+".scale").value = ctx.field("radius").value*3      
      ctx.field(object+".showUpVector").value = False
      if (orientation!=""):
         ctx.field(object+".rotation").setStringValue(orientation)
      ctx.module("output3D").addChild(cam.field("self"))         
   
   if (ctx.field("useArrows").value==True and _moduleCounter>1):
      object = "PositionArrow_"+str(_moduleCounter)
      arrow = ctx.addModule("Arrow2")
      arrow.field("instanceName").setStringValue(object)
      ctx.field(object+".startPoint").setStringValue(_oldSpherePos)
      ctx.field(object+".endPoint").setStringValue(pos)
      ctx.field(object+".Color").value = ctx.field("arrowColor").value
      ctx.field(object+".scale").value = ctx.field("arrowScale").value
      ctx.field(object+".lengthFactor").value = ctx.field("arrowLengthScale").value      
      ctx.module("output3D").addChild(arrow.field("self"))
      
   _oldSpherePos = pos
   
   pass 


def clearSpheres(args=0):
   global _moduleCounter
   
   list = ctx.modules()
   for module in list:
      if module[:8] == "Position":
         ctx.module(module).remove()   

   _moduleCounter = 0
   return
   
def leadingZero(number,totalLength):
   s = str(number)
   i=len(s)
   while i<=totalLength:
      s = "0" + s
      i = i+1
   return s