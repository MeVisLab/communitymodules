from ObjMgr import *
from VisDefinitions import *

_cls_info = None
_cls_info = None

_screenshotRun = False
_sgbWindow = 0
_thresWindow = 0
_aniSWindow = 0

def init():
   global _cls_iter
   global _cls_info
   
   #button für 3d aktivieren
   ctx.field("METKViewer3D.viewAll").touch()
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   ############ctx.field("METKCalcCamPos.autoApply").value = True
   ctx.field("METKViewer3D.viewer.applyCameraOrientation").value = "CAMERA_CORONAL"
   return

   
def openCase(field = 0):
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      ctx.window().setTitle("VisibilityDemonstrator | " + caseFile)
   return


def saveAs(field = 0):
   ctx.field("METKManager.saveAs").touch()
   pass


def save(field = 0):
   ctx.field("METKManager.save").touch()
   pass
      
      
def exit(field = 0):
   global _sgbWindow
   global _thresWindow
   global _aniSWindow
   
   ctx.field("METKManager.cleanup").touch()
   
   if _sgbWindow:
      _sgbWindow.close()
   if _thresWindow:
      _thresWindow.close()
   if _aniSWindow:
      _aniSWindow.close()
      
   if field:
      ctx.window().close()
   return      
   
def reset3D(field = 0):
   ctx.field("METKViewer3D.viewer.applyCameraOrientation").value = "CAMERA_CORONAL"
   ctx.field("METKViewer3D.RefreshObjMgr").touch()
   pass

def resetData(field = 0):
   preSets(ctx.field("preSets"))
   pass


def saveViewSeries(field = 0):
   global _screenshotRun
   print "saveViewSeries"
   _cls_iter.searchFirst()
   _screenshotRun = True
   saveSeriesBegin()
   pass


def saveSeriesBegin(field = 0):
   print "saveSeriesBegin"
   if _cls_iter.success():
      if not _cls_info.get(_cls_iter.object(), LAY_APPEARANCE, INF_PICKSTYLE) == "UNPICKABLE":
         _cls_info.typedSet(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, _cls_iter.object(), INFOTYPE_STRING)
         _cls_info.notify()
         message = _cls_iter.object() + " " + ctx.field("METKCalcCamPos.wVis").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wReg").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wCam").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wEnt").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wNum").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wImp").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wVisSta").stringValue()
         message +=                 " 0.7 " + ctx.field("METKCalcCamPos.wImpSta").stringValue() + " 0.7 "
         message +=                     " " + ctx.field("METKCalcCamPos.wDis").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wSilhouette").stringValue()
         message +=                     " " + ctx.field("METKCalcCamPos.wCenter").stringValue()
         ctx.field("METKMsgSender.data").value = message
         ctx.field("METKMsgSender.send").touch()
      else:
         print _cls_iter.object() + " is UNPICKABLE"
         _cls_iter.searchNext()
         saveSeriesBegin()
   pass


def saveSeriesEnd(field = 0):
   print "saveSeriesEnd"
   if field.value == True and _screenshotRun != 0:
      saveSingleView()
      _cls_iter.searchNext()
      saveSeriesBegin()
   else:
      ctx.field("METKViewer3D.viewer.applyCameraOrientation").value = "CAMERA_CORONAL"
   pass

   
def saveSingleView(field = 0):
   print "saveSingleView"
   global _screenshotRun
   comment =  "Non-occlusion: "           + ctx.field("METKCalcCamPos.wVis").stringValue() + "<br>"
   comment += "Non-occlusion Stability: " + ctx.field("METKCalcCamPos.wVisSta").stringValue() + "<br>"
   comment += "Preferred View Angle: "    + ctx.field("METKCalcCamPos.wReg").stringValue() + "<br>"
   comment += "Entropy: "                 + ctx.field("METKCalcCamPos.wEnt").stringValue() + "<br>"
   comment += "Overlapping Structures: "  + ctx.field("METKCalcCamPos.wNum").stringValue() + "<br>"
   comment += "Importance: "              + ctx.field("METKCalcCamPos.wImp").stringValue() + "<br>"
   comment += "Importance Stability: "    + ctx.field("METKCalcCamPos.wImpSta").stringValue() + "<br>"
   comment += "Viewpoint Distance: "      + ctx.field("METKCalcCamPos.wCam").stringValue() + "<br>"
   comment += "Vieplane Distance: "       + ctx.field("METKCalcCamPos.wDis").stringValue() + "<br>"
   comment += "Silhouette Length: "       + ctx.field("METKCalcCamPos.wSilhouette").stringValue() + "<br>"
   comment += "Image Space Center: "       + ctx.field("METKCalcCamPos.wImageSpaceCenter").stringValue()
   
   
   if not _screenshotRun:
      ctx.module("METKCollections").call("savePreConfiguredCollection", [ctx.field("METKCSO.cso").value, comment])
   else:
      ctx.module("METKCollections").call("savePreConfiguredCollection", [(ctx.field("postFix").value + " " + ctx.field("METKCSO.cso").value), comment])
   pass


def saveTIFF(field = 0):
   fileManager = MLABFileManager
   caseName = fileManager.splitPath(_cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE))["base"]
   structure = ctx.field("METKCSO.cso").value
   postfix = ctx.field("postFix").value
   print "save " + caseName + "_" + structure + "_" + postfix + ".tif"
   fileManager.mkdir(_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + "screenshots/")
   ctx.field("METKViewer3D.viewer.outputSnapshotFile").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + "screenshots/" + caseName + "_" + structure + "_" + postfix
   ctx.field("METKViewer3D.viewer.doSnapshot").touch()
   pass

def showDecoration(field = 0):
   ctx.field("METKViewer3D.decoration").value = not ctx.field("METKViewer3D.decoration").value
   pass
   

def showCollections(field = 0):
   ctx.field("collectionView").value = not ctx.field("collectionView").value
   pass


def structureGroupBrowser(field = 0):
   global _sgbWindow
   if not _sgbWindow:
      _sgbWindow = ctx.showWindow("Structures")
   pass
   
def closedSGB(field = 0):
   global _sgbWindow
   _sgbWindow = 0
   pass


def showThresholds(field = 0):
   global _thresWindow
   if not _thresWindow:
      _thresWindow = ctx.showWindow("Thresholds")
   pass
   
def closedThres(field = 0):
   global _thresWindow
   _thresWindow = 0
   pass


def showAnimationSettings(field = 0):
   global _aniSWindow
   if not _aniSWindow:
      _aniSWindow = ctx.showWindow("AnimationSettings")
   pass
   

def resetAnimationCache(field = 0):
   ctx.field("UMDAnimation2.Reset").touch()
   pass
   
   
def closedAniS(field = 0):
   global _aniSWindow
   _aniSWindow = 0
   pass
   
   
def preSets(field = 0):
   if field.value == "None":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 1 1 0 1 1 1 1 0.7 1 0.7 1 1 1"
   elif field.value == "Neck":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 30 50 50 50 5 50 30 0.7 50 0.7 50 50 50"
   elif field.value == "Liver":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 0 0 0 100 0 100 0 0.7 50 0.7 50 100 50"
   elif field.value == "Knee":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 0 0 0 100 0 0 0 0.7 0 0.7 50 20 50"
   elif field.value == "VPE":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 50 0 0 100 0 0 50 0.7 0 0.7 0 0 0"
   elif field.value == "VPEReal":
      ctx.field("METKCalcCamPos.data").value = ctx.field("METKCalcCamPos.currentStructure").value + " 50 0 0 0 0 0 0 0.7 0 0.7 0 0 0"
      
   pass
   
   
def setPosition(field = 0,obj = ""):
   #print "set Position obj: " + obj
   if (ctx.field("METKCSO.cso").stringValue()=="" and obj==""): return
   if (obj==""): obj = ctx.field("METKCSO.cso").stringValue()
   if (obj==""): return
   #print "*"+obj+"*"
   scriptText="[INI]\nLENGTHTIMEUNIT=1\nLENGTHSECONDS="+ctx.field("animationLength").stringValue()+"\nREALTIME=YES\n\n[SCRIPT]\n"
   scriptText += "[0] 'System' setBaseColors\n"
   scriptText += "[0,1] 'CAM' VIEWPOINTMOVE '"+obj+"' "+ctx.field("zoomFactor").stringValue()+" '"
   scriptText += ctx.field("wVis").stringValue()+" "
   scriptText += ctx.field("wReg").stringValue()+" "
   scriptText += ctx.field("wCam").stringValue()+" "
   scriptText += ctx.field("wEnt").stringValue()+" "
   scriptText += ctx.field("wNum").stringValue()+" "
   scriptText += ctx.field("wImp").stringValue()+" "
   scriptText += ctx.field("wVisSta").stringValue()+" 0.7 "
   scriptText += ctx.field("wImpSta").stringValue()+" 0.7 "
   scriptText += ctx.field("wDis").stringValue()+" "  
   scriptText += ctx.field("wSil").stringValue()+" "
   scriptText += ctx.field("wCenter").stringValue()
   scriptText += "'\n"
   scriptText += "[0,1] 'Cam' rotateCamZTo 0\n"
   global _cls_info
   #print "scriptText= " + scriptText
   _cls_info.typedSet(OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_FULLSCRIPT, scriptText, INFOTYPE_MESSAGE)
   _cls_info.notify()   
   return

def setCSO(field = 0):
   setPosition()
   return
   
   
def setSphereField(fieldP = 0):
   ctx.field("METKCalcCamPos.showField").value = int(ctx.field("sphereField").value)   
   ctx.field("VisDataView.viewAll").touch()
   return
          
          
def recalc(fieldP=0):
   #ctx.field("METKCalcCamPos.calc").touch()
   #ctx.field("METKCalcCamPos.writeCamToObjMgr").touch()
   setPosition()
   return
   
   

def allowWorstPositions(fieldP=0):
   if ctx.field("allowWorstPositions").value==True:
      ctx.field("wVis").setMinValue(-100)
      ctx.field("wVisSta").setMinValue(-100)
      ctx.field("wReg").setMinValue(-100)
      ctx.field("wEnt").setMinValue(-100)
      ctx.field("wNum").setMinValue(-100)
      ctx.field("wImp").setMinValue(-100)
      ctx.field("wImpSta").setMinValue(-100)
      ctx.field("wCam").setMinValue(-100)
      ctx.field("wDis").setMinValue(-100)
      ctx.field("wSil").setMinValue(-100)
      ctx.field("wCenter").setMinValue(-100)
   else:
      ctx.field("wVis").setMinValue(0)
      ctx.field("wVisSta").setMinValue(0)
      ctx.field("wReg").setMinValue(0)
      ctx.field("wEnt").setMinValue(0)
      ctx.field("wNum").setMinValue(0)
      ctx.field("wImp").setMinValue(0)
      ctx.field("wImpSta").setMinValue(0)
      ctx.field("wCam").setMinValue(0)
      ctx.field("wDis").setMinValue(0)
      ctx.field("wSil").setMinValue(0)
      ctx.field("wCenter").setMinValue(0)
   return


def showScripts(fieldP=0):
   ctx.showWindow("METKScriptBunch._default")
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
   return

def handleLoadedEvent():
   _cls_iter.setSearchCriteria(LAY_DESCRIPTION, INF_STRUCTURE)
   _cls_iter.searchFirst()      
   obj = _cls_iter.object()           
   ctx.field("METKMsgSender.data").value = _cls_iter.object() + " 1 1 0 1 1 1 1 0.9 1 0.9 1"
   pass

   
def handleSaveEvent():
   pass


def handleCleanupEvent():
   pass