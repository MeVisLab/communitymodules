# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
import array

_cls_info2 = None
_codSegImage = ""
_instanceName = ""

#Is called from init() in ImageDisplay
#Achtung!: Fehler in der init2 werden nicht angezeigt, weil ein except in er init() vom ImageDisplay.py alles schluckt
def init2():
   global _cls_info2   
   #print "init2"
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info2 = METKObjInfo(ctx.module("info2"))
   
   ctx.field("SoLUTEditor2D.lutListItem").value="Default"
   instanceNameChanged()
   return



# called from METKImageDisplay.py
def closeImage(image = ""):
   pass


# called from METKImageDisplay.py
def getImage(parent):
   if ctx.field("input0").isValid():
      #_cls_info2.setEventReceiveMode(ERM_SELECTED, "*", LAY_IMAGE + "," + LAY_GLOBALEVENTS + "," + LAY_APPEARANCE)
      buildLUT()
   return


def buildLUT(fieldP=0):   
   LutData = ""
   visibleIDs = ","

   if (not ctx.field("roiSelect").value):
      ctx.log("No ROI selected for VolViz")
      return
      
   #win = ctx.field("window").value
   #center = ctx.field("center").value
   #defaultLUT = "<Default=0,1,[0 0 0 0 0, "+str(center-win/2)+" 1 1 1 0, "+str(center+win/2)+" 1 1 1 1, 4095 1 1 1 1]>"
   #print "default:"+defaultLUT
   #LutData = defaultLUT
   
   colorStr = []
   for i in range(256):
      colorStr.append("")
   
   #CodedSegmentation Iteration
   _cls_info2.activateLayer(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value)
   _cls_info2.firstInfo()
      
   while _cls_info2.success():
      if _cls_info2.activeLayer() != LAY_GLOBAL:
         infoID = _cls_info2.activeInfo()            
         value = _cls_info2.get()
                  
         totalColor = [0.0, 0.0, 0.0]
         totalOpacity = 0.0
         
         objects = value.split(",") 
         maxOpacity = 0;
         for objID in objects:
            if (objID!=""):
               if ((str(_cls_info2.get(objID, LAY_APPEARANCE, INF_VISIBLE))).upper()=="TRUE"):
                  #wenn die einträge für color und transparency hier nicht existieren kackt er ab
                  color = str(_cls_info2.get(objID, LAY_APPEARANCE, INF_COLOR))
                  transp = _cls_info2.getDouble(objID, LAY_APPEARANCE, INF_TRANSPARENCY)
                  if (1-transp>maxOpacity):
                     maxOpacity = 1-transp
                  colorParts = color.split(" ")
                  if (len(colorParts)==3):
                     color2 = [(float)(colorParts[0]), (float)(colorParts[1]), (float)(colorParts[2])]
                     totalColor[0] += color2[0]*(1-transp)
                     totalColor[1] += color2[1]*(1-transp)
                     totalColor[2] += color2[2]*(1-transp)
                     totalOpacity += (1-transp)
                  visibleIDs = visibleIDs + str(int(infoID)) + ","
         
         #Die Farben der Objekte gehen gewichtet mit ihrer Transparenz in die Summenbildung ein
         if (totalOpacity!=0):
            colorToSet = [0.0, 0.0, 0.0]
            colorToSet[0] = totalColor[0] / totalOpacity
            colorToSet[1] = totalColor[1] / totalOpacity
            colorToSet[2] = totalColor[2] / totalOpacity
            #transpToSetStr = str(maxOpacity/10.0) #max 10% opacity
                                                
            colorStr[int(infoID)] = str(colorToSet[0])+" "+str(colorToSet[1])+" "+str(colorToSet[2])
            
            #Hinweis: Rückumwandlung der mit den führenden Nullen versehenen InfoIDs durch str(int(infoID))
            #if LutData!="": LutData=LutData+","
            #LutData = LutData + "<METK"+str(int(infoID)) + "=0,1,[0 "+colorStr+" "+ transpToSetStr + ", 4095 " + colorStr + " " + transpToSetStr + "]>"
            

      
      _cls_info2.activateInfo(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value, infoID)
      _cls_info2.nextInfo()
      
   
   #print "LutData="+LutData
   #print "visibleIDs="+visibleIDs
   
   tagData = ""
   for i in range(256):
      if visibleIDs.find(","+str(i)+",")!=-1:
         tagData = tagData + "<Default,1,1,("+colorStr[i]+")> "
      else:         
         tagData = tagData + "<Default,1,1,(1 1 1)>"
         
   #print "tagData="+tagData
      
   #ctx.field("SoLUTEditor2D.lutData").value = LutData
   ctx.field("SoLUTEditor2D.tagData").value = tagData
   
   return   



def refreshObjMgr(fieldP=0):
   global _instanceName
   global _updatedByMyself
   
   #print "refreshObjMgr"
  
   instanceNameChanged(0)
   
   _cls_info2.typedSet(_instanceName, LAY_TRANSFERFUNCTION, INF_COLORPOINTS, ctx.field("colorPoints").value, INFOTYPE_MESSAGE)
   _cls_info2.typedSet(_instanceName, LAY_TRANSFERFUNCTION, INF_ALPHAPOINTS, ctx.field("alphaPoints").value, INFOTYPE_MESSAGE)
   _cls_info2.typedSet(_instanceName, LAY_GLOBAL, INF_GLOBAL_REFRESH, False, INFOTYPE_BOOL)
   ctx.callLater(0, "_cls_info2.notify")
   _updatedByMyself = False

   return



def instanceNameChanged(field=0):
   global _instanceName
   oldName = _instanceName
   _instanceName = ctx.field("instanceName").value
   #print "xxx:"+_instanceName
   
   if oldName == "":
      _cls_info2.set(_instanceName, LAY_GLOBAL, INF_OBJTYPE, "VolViz")
   else:
      _cls_info2.renameObject(oldName, _instanceName)
      
   #_cls_info2.setEventReceiveMode(ERM_SELECTED, _instanceName, "*")
   _cls_info2.notify()
   return




#--------------------------------------------
# Event handling
# Eigenes Event Handling, da die originäre
# Funktionen von der ImageDisplay.py
# "geklaut" wurden.
#--------------------------------------------

def myHandleEvents(fieldP=0):
   global _updatedByMyself   
   _cls_info2.fld_eventFirst.touch();
   while _cls_info2.fld_successful.value:

      object = _cls_info2.activeObject()
      layer  = _cls_info2.activeLayer()
      info   = _cls_info2.activeInfo()
      value  = _cls_info2.get()
            
      #print object +" "+layer+" "+info+" "+value
      
      #  wenn Sichtbarkeit/Farbe/Transparenz einer Struktur geändert wurde, LUT anpassen
      if info == INF_VISIBLE or info == INF_COLOR or info == INF_TRANSPARENCY:
         buildLUT()
      elif layer == LAY_GLOBAL and info == INF_GLOBAL_REFRESH and (value == "True" or value == "1"):
         refreshObjMgr()
      elif layer == LAY_TRANSFERFUNCTION and _updatedByMyself == False and object == _instanceName:         
         if info == INF_COLORPOINTS or INF_ALPHAPOINTS:
            ctx.field("colorPoints").value = _cls_info2.get(_instanceName, LAY_TRANSFERFUNCTION, INF_COLORPOINTS)
            ctx.field("alphaPoints").value = _cls_info2.get(_instanceName, LAY_TRANSFERFUNCTION, INF_ALPHAPOINTS)         
   
      _cls_info2.fld_eventNext.touch();
      
   _cls_info2.fld_clearQueue.touch();
   return
   

def handleLoadedEvent2():
   instanceNameChanged(0)   
   return