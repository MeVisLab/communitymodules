# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *


# Global class instances
_toggleOverlay = True


# called from METKImageDisplay.py
def closeImage(image = ""):
   ctx.field("SoLUTEditor.colorPoints").setStringValue("0 0 0 0")
   ctx.field("SoLUTEditor.alphaPoints").setStringValue("0 0")
   pass


# called from METKImageDisplay.py
def getImage(parent):
   if ctx.field("image").isValid():
      #_cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_IMAGE + "," + LAY_GLOBALEVENTS + "," + LAY_APPEARANCE)
      updateLUT(parent)
   return


def imageSet(field = 0):
   if ctx.field("image").isValid():
      updateLUT(ctx.field("roiSelect").value)
   pass   
   
   
def appearanceChanged(field = 0):
   _cls_app = ObjInfo(ctx.module("appearance"))

   parent = ""
   oldParent = ""
   _cls_app.fld_eventFirst.touch();
   while _cls_app.fld_successful.value:
      info   = _cls_app.activeInfo()
      if info == INF_VISIBLE or info == INF_COLOR or info == INF_TRANSPARENCY:
         object = _cls_app.activeObject()
         # aus irgendwelchen unerfindlichen Gründen wird hier ab und zu als object "CSO" übergeben, das kann ja nun nicht sein...
         if object != "CSO":
            parent = _cls_info.get(object, LAY_GLOBAL, INF_PARENT)
            if (parent != oldParent and parent==ctx.field("roiSelect").value):
               updateLUT(parent)
               oldParent = parent
      _cls_app.fld_eventNext.touch();
   _cls_app.fld_clearQueue.touch();
   pass


def updateLUT(parent):
   ctx.log("updateLUT")
   colorPoints = "[ 0 0 0 0, "
   alphaPoints = "[ 0 0, "
   
   # Kinder der ROI
   childIDs = _cls_info.get(parent, LAY_GLOBAL, INF_CHILDS)
   
   if (not ctx.field("roiSelect").value):
      ctx.log("No ROI selected for Overlay")
      return
   _cls_info.activateLayer(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value)
   _cls_info.firstInfo()
      
   while _cls_info.success():
      if _cls_info.activeLayer() != LAY_GLOBAL:
         infoID = _cls_info.activeInfo()            
         value = _cls_info.get()
                  
         totalColor = [0.0, 0.0, 0.0]
         totalOpacity = 0.0
         
         objects = value.split(",") 
         maxOpacity = 0;
         for objID in objects:
            if (objID!=""):
               if ((str(_cls_info.get(objID, LAY_APPEARANCE, INF_VISIBLE))).upper()=="TRUE"):
                  #wenn die einträge für color und transperency hier nicht existieren kackt er ab
                  color = str(_cls_info.get(objID, LAY_APPEARANCE, INF_COLOR))
                  transp = _cls_info.getDouble(objID, LAY_APPEARANCE, INF_TRANSPARENCY)
                  if (1-transp>maxOpacity):
                     maxOpacity = 1-transp
                  colorParts = color.split(" ")
                  if (len(colorParts)==3):
                     color2 = [(float)(colorParts[0]), (float)(colorParts[1]), (float)(colorParts[2])]
                     totalColor[0] += color2[0]*(1-transp)
                     totalColor[1] += color2[1]*(1-transp)
                     totalColor[2] += color2[2]*(1-transp)
                     totalOpacity += (1-transp)
         
         #Die Farben der Objekte gehen gewichtet mit ihrer Transparenz in die Summenbildung ein
         if (totalOpacity!=0):
            colorToSet = [0.0, 0.0, 0.0]
            colorToSet[0] = totalColor[0] / totalOpacity
            colorToSet[1] = totalColor[1] / totalOpacity
            colorToSet[2] = totalColor[2] / totalOpacity
            transpToSet = maxOpacity / 2.0 #maximal 0,5 Transparenz später in der Anzeige
            
            #Rückumwandlung der mit den führenden Nullen versehenen InfoIDs durch str(int(infoID))
            colorPoints += str(int(infoID)) + " " + str(colorToSet[0])+" "+str(colorToSet[1])+" "+str(colorToSet[2]) + ", "
            alphaPoints += str(int(infoID)) + " " + str(transpToSet) + ", "
         else:
            colorPoints += str(int(infoID)) + " 0 0 0, "
	    alphaPoints += str(int(infoID)) + " 0, "

      
      _cls_info.activateInfo(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value, infoID)
      _cls_info.nextInfo()
   
   
   colorPoints += " ]"
   alphaPoints += " ]"
   
   ctx.field("SoLUTEditor.colorPoints").setStringValue(colorPoints)
   ctx.field("SoLUTEditor.alphaPoints").setStringValue(alphaPoints)

   # touch something from the overlay, otherwise viewer is not being updated
   ctx.field("SoView2DOverlay.alphaFactor").touch()
   pass
   


def selectOverlay(field = 0):
   if (ctx.field("setCSO").value==False):
      return
   
   objValue = ctx.field("GetVoxelValue.storedValue").value     
   
   if (not ctx.field("roiSelect").value):
      ctx.log("No ROI selected for Overlay")
      return

   objValueStr = str(int(objValue)).zfill(6)
   objects = _cls_info.get(OBJ_CODEDSEGMENTATION, ctx.field("roiSelect").value, objValueStr).split(",")      
   
   selectedObj = ""
   minTransp = 1.0
   #select object in list of possible objects as CSO with LOWEST TRANSPARENCY!!!!
   for objID in objects:
      if str(_cls_info.get(objID, LAY_APPEARANCE, INF_VISIBLE)).upper()=="TRUE" and (float)(_cls_info.get(objID, LAY_APPEARANCE, INF_TRANSPARENCY))<minTransp:
         if _cls_info.getBool(OBJ_CSO, LAY_GLOBAL, INF_IGNOREPICK) or _cls_info.get(objID, LAY_APPEARANCE, INF_PICKSTYLE) != "UNPICKABLE":
            minTransp = ((float)(_cls_info.get(objID, LAY_APPEARANCE, INF_TRANSPARENCY)))
            selectedObj = objID         
   
   if (selectedObj!=""):
      _cls_info.typedSet(OBJ_CSO, LAY_SELECTEDOBJ, INF_SELECTION, ctx.field("SoView2DPosition.worldPosition").stringValue(), INFOTYPE_VEC3)
   
   _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, selectedObj)
   _cls_info.notify()
   return
   

def toggleOverlay(field = 0):
   global _toggleOverlay
   if _toggleOverlay:
      ctx.field("showOverlay").value = False
      ctx.field("SoView2DMenuItem.labelOff").value = "Show Overlay"
   else:
      ctx.field("showOverlay").value = True
      ctx.field("SoView2DMenuItem.labelOff").value = "Hide Overlay"
   _toggleOverlay = not _toggleOverlay
   return