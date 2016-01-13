# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
from NPRShadingParameter import *

# Global class instances
_cls_info= None

# Global variables
_scriptHeader = ""
_scriptBody = ""


def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))   
   global _scriptHeader, _scriptBody
   _scriptHeader = ""
   _scriptBody = ""   
   return



def buildNewScript(args):
   buildScript(False)
   return



def appendToScript(args):
   buildScript(True)
   return



def buildScript(appendValues):    
   global _scriptHeader, _scriptBody
   ctx.log("buildScript")
   if (ctx.field("length").value==""):
      ctx.log("Animation Length NULL")
      return

   oldTU=0
   oldSec=0
   if (appendValues and _scriptHeader!=""):    
      #get old LengthTimeUnit and LengthSeconds     
      lines = _scriptHeader.splitlines()      
      for singleline in lines:
         if (singleline.find("LengthTimeUnit")!=-1):
            strValues = singleline.split("=")
            if (len(strValues)>=2):
               oldTU = float(strValues[1])
         elif (singleline.find("LengthSeconds")!=-1):
            strValues = singleline.split("=")
            if (len(strValues)>=2):
               oldSec = float(strValues[1])

   newTU = oldTU+ctx.field("length").value
   newSec = oldSec+ctx.field("length").value

   strOldTU = str(oldTU)   
   strNewTU = str(newTU)      
   if (oldTU==0): strOldTU = "0"
   if (newTU==0): strNewTU = "0"
      
   _scriptHeader = "[Ini]\nLengthTimeUnit="+strNewTU+"\nLengthSeconds="+str(newSec)+"\n"
   if (ctx.field("realtime").value == True): 
      _scriptHeader = _scriptHeader+"RealTime=yes\n"
   else:
      _scriptHeader = _scriptHeader+"RealTime=no\n"
   
   
   ctx.log(_scriptHeader)
   
   if (oldTU!=newTU):
      baseTimeRange = "["+strOldTU+","+strNewTU+"] "
   else:      
      baseTimeRange = "["+strNewTU+"] "
   tempBody = ""
   if (appendValues):
      tempBody = _scriptBody
   objectID = _cls_info.firstObject()
   while _cls_info.success():
      if (_cls_info.get( LAY_GLOBAL, INF_OBJTYPE)=="Result"):
         objectName = _cls_info.get( LAY_DESCRIPTION, INF_NAME)
         if (objectName!=""):
         
            if (ctx.field("useVisibility").value == True):               
               if (ctx.field("realtime").value == True):
                  visible = _cls_info.get( LAY_APPEARANCE, INF_VISIBLE)
                  if (visible!=""):                                    
                     if (ctx.field("setVisibilityLast").value == True ):
                        timeRange = "["+strNewTU+"] "
                     else: timeRange = "["+strOldTU+"] "
                     tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setVisible "+str(visible)
               else: #Videoerzeugung                  
                  visible = _cls_info.getBool( LAY_APPEARANCE, INF_VISIBLE)                  
                  transp = _cls_info.getDouble( LAY_APPEARANCE, INF_TRANSPARENCY)
                  timeRange = baseTimeRange
                  if (visible==False):
                     tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setTransparency 1"
                     tempBody = tempBody +"\n["+strNewTU+"] '"+objectName+"' setVisible "+str(visible)
                  else:
                     tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setTransparency "+str(transp)                
                     if (ctx.field("setVisibilityLast").value == True ):
                        timeRange = "["+strNewTU+"] "
                     tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setVisible "+str(visible)
                     
               

         
            if (ctx.field("useColor").value == True):
               color = _cls_info.get( LAY_APPEARANCE, INF_COLOR)
               if (color!=""):                                   
                  #color Umrechnung da im Script mit 0...255 gearbeitet wird
                  colors = color.split(" ")
                  colorR = str(float(colors[0])*255)
                  colorG = str(float(colors[1])*255)
                  colorB = str(float(colors[2])*255)
                  if (ctx.field("setColorLast").value == True):
                     timeRange = "["+strNewTU+"] "
                  else: timeRange = baseTimeRange
                  tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setColor "+colorR+","+colorG+","+colorB

            if (ctx.field("useTransparency").value == True):               
               transp = _cls_info.get( LAY_APPEARANCE, INF_TRANSPARENCY)
               if (transp!=""):                  
                  transp = transp.replace(" ",",")
                  if (ctx.field("setTransparencyLast").value == True ):
                     timeRange = "["+strNewTU+"] "
                  else: timeRange = baseTimeRange
                  tempBody = tempBody +"\n"+timeRange+"'"+objectName+"' setTransparency "+transp
                  
            #if (ctx.field("useNPR").value == True ):               
               #timeRange = baseTimeRange
               #tempBody = tempBody +"\n"+timeRange+"'System' setBool '"+objectID+"' "+LAY_APPEARANCE+" "+INF_STIPPLING+" "+_cls_info.get( LAY_APPEARANCE, INF_STIPPLING)
               #tempBody = tempBody +"\n"+timeRange+"'System' setBool '"+objectID+"' "+LAY_APPEARANCE+" "+INF_TEXTURIZE+" "+_cls_info.get( LAY_APPEARANCE, INF_TEXTURIZE)
            if (_cls_info.existInfo(LAY_APPEARANCE,INF_SURFACEDIST)):
               timeRange = "["+strNewTU+"] "
               tempBody = tempBody +"\n"+timeRange+"'System' setBool '"+objectID+"' "+ LAY_APPEARANCE + " "+INF_SURFACEDIST+" "+_cls_info.get(LAY_APPEARANCE,INF_SURFACEDIST)
                                  
      objectID = _cls_info.nextObject()
      

   #Creating NPR Commands      
   if (ctx.field("useNPR").value == True ):
      ctx.log("USE NPR ....")
      timeRange = baseTimeRange
      #tempBody = tempBody +"\n"+timeRange+"'System' setString "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_BUFFERTYPE+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_BUFFERTYPE)
      #tempBody = tempBody +"\n"+timeRange+"'System' setVec3 "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_GLOBALLIGHT+" "+"'"+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_GLOBALLIGHT)+"'"
      #tempBody = tempBody +"\n"+timeRange+"'System' setInt "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_COSSMOOTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_COSSMOOTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setInt "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_EXAGSTRENGTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_EXAGSTRENGTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_EXAGLOD+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_EXAGLOD)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTCOSINE+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTCOSINE)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTRAKING+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTRAKING)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTPLATEAU+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTPLATEAU)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTMAXCURV+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTMAXCURV)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTVALLRIDG+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTVALLRIDG)
      #tempBody = tempBody +"\n"+timeRange+"'System' setString "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_NORMALIZATION+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_NORMALIZATION)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_MAXDIST+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_MAXDIST)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTDIST+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTDIST)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTFOCUS+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTFOCUS)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTEMPH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTEMPH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTDEPTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTDEPTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTRIMLIGHT+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTRIMLIGHT)
      #tempBody = tempBody +"\n"+timeRange+"'System' setInt "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_RIMLIGHTFILTER+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_RIMLIGHTFILTER)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTDEPSHADOW+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTDEPSHADOW)
      #tempBody = tempBody +"\n"+timeRange+"'System' setInt "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_SHADOWFILTER+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_SHADOWFILTER)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTDEPTHFADING+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTDEPTHFADING)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_DEPTHVALUE+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_DEPTHVALUE)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_MINDEPTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_MINDEPTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_MAXDEPTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_MAXDEPTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_DEPHTEXP+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_DEPHTEXP)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_WEIGHTLINES+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_WEIGHTLINES)
      #tempBody = tempBody +"\n"+timeRange+"'System' setInt "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_LINEWIDTH+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_LINEWIDTH)
      #tempBody = tempBody +"\n"+timeRange+"'System' setDouble "+OBJ_NPRSHADING+" "+LAY_NPRSHADING+" "+INF_GAMMA+" "+_cls_info.get(OBJ_NPRSHADING,LAY_NPRSHADING,INF_GAMMA)
      
      
      
      
  
   
      
   #Build camera commands
   viewerName = ctx.field("viewerName").value
   cameraType = _cls_info.get(viewerName, LAY_GLOBAL, "ObjectType")
   if (cameraType=="Viewer3D" and ctx.field("useCamera").value == True):
      focalDistance = _cls_info.get(viewerName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_FOCALDISTANCE)
      if (focalDistance!=""):
         tempBody = tempBody +"\n"+baseTimeRange+"'Cam' setCameraFocalDistance "+focalDistance

      height = _cls_info.get(viewerName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT)
      if (height!=""):
         tempBody = tempBody +"\n"+baseTimeRange+"'Cam' setCameraHeight "+height
         
      orientation = _cls_info.get(viewerName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION)
      if (orientation!=""):
         orientation = orientation.replace(" ",",")
         tempBody = tempBody +"\n"+baseTimeRange+"'Cam' setCameraOrientation "+orientation

      position = _cls_info.get(viewerName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION)
      if (position!=""):
         position = position.replace(" ",",")
         tempBody = tempBody +"\n"+baseTimeRange+"'Cam' setCameraPosition "+position
   #ctx.log("tempBody: " +tempBody)

   _scriptBody = tempBody
   
   ctx.log("*"+_scriptBody)
   ctx.field("script").value = _scriptHeader+"\n[Script]\n"+_scriptBody

   return
   
   
def clear(args):
   global _scriptHeader, _scriptBody
   _scriptHeader = ""
   _scriptBody = ""
   ctx.field("script").value = ""
   pass