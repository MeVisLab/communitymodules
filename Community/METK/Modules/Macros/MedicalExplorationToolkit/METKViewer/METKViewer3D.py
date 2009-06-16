from ObjMgr import *
from VisDefinitions import *
from linear import *
from mevis import *

_cls_info= None
_cls_iterSync = None

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))   

   global _cls_iterSync
   _cls_iterSync = ObjIterator(ctx.module("iterSyncViewer"))
   _cls_iterSync.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer3D")


   viewerNameChanged(0)
   switchShapeHints(0)
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

   if info == INF_CASELOADED and value == MSG_LOADED:
      ctx.field("viewer.viewAll").touch()
      ctx.field("viewer.applyCameraOrientation").value = "CAMERA_CORONAL"
      _cls_info.markObjectNonPersistent(ctx.field("instanceName").value)
      #_cls_info.markInfoNonPersistent(ctx.field("instanceName").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG)
      ctx.field("RefreshObjMgr").touch()
   elif info == INF_GETALLVIEWER and value == MSG_GET:
      if (ctx.field("UMDViewerCommunicator.ProtocolViewerStatus").value==False):
         ctx.log("RefreshObjMgr of " + ctx.field("instanceName").value)
         ctx.field("RefreshObjMgr").touch()
   elif object == ctx.field("syncWith").value:      
      if layer == LAY_VIEWER_CAMERA:    
         if info == INF_VIEWER_CAMERA_POSITION:
            ctx.field("camera.position").setStringValue(value)
         elif info == INF_VIEWER_CAMERA_ORIENTATION:
            ctx.field("camera.orientation").setStringValue(value)
         elif info == INF_VIEWER_CAMERA_HEIGHT:
            ctx.field("camera.height").setStringValue(value)
         elif info == INF_VIEWER_CAMERA_FOCALDISTANCE:
            ctx.field("camera.focalDistance").setStringValue(value)
            
   elif object == ctx.field("instanceName").value:
      if layer == LAY_VIEWER_SCREENSHOT and info == INF_SCREENSHOTFLAG and _cls_info.getBool():
         ctx.field("viewer.outputSnapshotFile").value = _cls_info.get(object, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFILE)
         if MLABFileManager.exists(ctx.field("viewer.outputSnapshotFile").value):
            MLABFileManager.remove(ctx.field("viewer.outputSnapshotFile").value)
         saveScreenshot()

   pass


def pickedObjectChanged(againstTError = 0):
   _cls_info.typedSet(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, ctx.field("METKPicking.objName").value, INFOTYPE_STRING)
   _cls_info.typedSet(OBJ_CSO, LAY_SELECTEDOBJ, INF_SELECTION, ctx.field("METKPicking.selection").stringValue(), INFOTYPE_VEC3)
   _cls_info.notify()
      
   return
   
def mouseOverObjectChanged(againstTError = 0):
   _cls_info.typedSet(OBJ_CSO, LAY_MOUSEOVEROBJ, INF_OBJID, ctx.field("METKPicking.mouseOverName").value, INFOTYPE_STRING)   
   _cls_info.notify()
      
   return
   
   
   
def viewerNameChanged(args):
   viewerName = ctx.field("instanceName").value
   if _cls_info.existObject(viewerName):
      MLAB.logWarning("At least two viewers share the same name (" + viewerName + ")!")
   ctx.field("UMDViewerCommunicator.ObjName").value = viewerName   
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_COMMUNICATION + "," + viewerName, LAY_GLOBALEVENTS + "," + LAY_VIEWER_SCREENSHOT + "," + LAY_VIEWER_CAMERA)
   return
   

def syncViewer(args):
   if (ctx.field("syncViewer").value==True):
      ctx.log("get all viewer")
      _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_GETALLVIEWER,MSG_GET,INFOTYPE_MESSAGE)           
      _cls_info.notify()
      syncWith(0)
   else:
      ctx.field("info.acceptedObjects").value  = "Communication"
   return
   
   
def syncWith(args):
   if (ctx.field("syncWith").value!="" and ctx.field("syncWith").value!=ctx.field("instanceName").value):
      ctx.log("correct viewer")
      ctx.field("info.acceptedObjects").value  = "Communication,"+ctx.field("syncWith").value
      _cls_info.typedSet(ctx.field("syncWith").value,LAY_VIEWER_PROPERTIES,INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS,"TRUE",INFOTYPE_BOOL)
      _cls_info.notify()
   else:
      ctx.field("info.acceptedObjects").value  = "Communication"
   return


def switchShapeHints(args):
   if (ctx.field("shapeHints").value == True):
      ctx.field("shapeHintsSwitch.whichChild").value = -3 #All
   else:
      ctx.field("shapeHintsSwitch.whichChild").value = -1 #None
   return


def saveScreenshot(field = 0):
   ctx.field("ready").value = False
   ctx.field("viewer.doSnapshot").touch()
   ctx.field("IdleTrigger.on").value = True
   pass
   

def finishingScreenshot(field = 0):
   ctx.field("IdleTrigger.on").value = False
   fileManager = MLABFileManager
   fileName = ctx.field("screenshotFile").value;
   if fileName[-4:] != ".tif":
      fileName = fileName + ".tif"
   if fileManager.exists(fileManager.normalizePath(fileName)):
      if ctx.field("saveThumbNail").value == True or _cls_info.getBool(ctx.field("instanceName").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTTHUMB) == True:
         standardFileName = fileManager.normalizePath(fileName);
         pathComponents = fileManager.splitPath(standardFileName);
         dirName = pathComponents["dir"];
         fileBaseName = pathComponents["name"];
         baseName = pathComponents["base"];
         extension = pathComponents["ext"];
         thumbName = dirName + baseName + ".thumb.tif"
         if not fileManager.exists(fileManager.normalizePath(thumbName)):
            #MLABGraphic.createThumbNail(ctx.field("screenshotFile").value, thumbName, ctx.field("thumbSize").value)
            MLABGraphic.createScaledImage(ctx.field("screenshotFile").value, thumbName, ctx.field("thumbSize").value)
            ctx.field("IdleTrigger.on").value = True
         else:
            _cls_info.typedSet(ctx.field("instanceName").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG, False, INFOTYPE_BOOL)
            ctx.field("ready").value = True
            _cls_info.notify()
      else:
         _cls_info.typedSet(ctx.field("instanceName").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG, False, INFOTYPE_BOOL)
         ctx.field("ready").value = True
         _cls_info.notify()
   pass
   

   
def visLogo (field=0):
   if (ctx.field("enableVisLogo").value == True):
      ctx.field("logoSwitch.whichChild").value = -3 #All
   else:
      ctx.field("logoSwitch.whichChild").value = -1 #None
   return
   
   
   
   
   
   
   
   
   
# Stuff for METKViewerExt   

_windowCtrl = None
_freeFloatFrameCtrl = None
_renderAreaFrameCtrl = None
_inlinePanelFramePropCtrl = None
_inlinePanelFramePickCtrl = None

def initViewerExt( control ):   
   global _windowCtrl
   _windowCtrl = control
  
   # FreeFloat where to display in render area + its panel
   global _freeFloatFrameCtrl
   _freeFloatFrameCtrl = _windowCtrl.control( "freeFloatFrame" )

   # Render area
   global _renderAreaFrameCtrl
   _renderAreaFrameCtrl = _windowCtrl.control( "renderAreaFrame" )

   # Prop panel
   global _inlinePanelFramePropCtrl
   _inlinePanelFramePropCtrl = ctx.control( "inlinePanelFrameProp" )

   # Pick panel
   global _inlinePanelFramePickCtrl
   _inlinePanelFramePickCtrl = ctx.control( "inlinePanelFramePick" )
   
   ctx.field("enablePropExtension").value = True
   ctx.field("enablePickExtension").value = True

   return


  
def viewerExtResizeEvent( event ):   
   updateGeometry()
   return


def updateGeometry(extType=0):
   if not _windowCtrl:
      return
      
   xOffset=50
   yOffset=5
   
   # Current frame size of enclosing frame
   viewerExtSize = _windowCtrl.control( "viewerExt" ).size()  

   # Place render area within FreeFloat frame
   _freeFloatFrameCtrl.setControlPos ( _renderAreaFrameCtrl, 0, 0 )
   _freeFloatFrameCtrl.setControlSize( _renderAreaFrameCtrl, viewerExtSize )

   # Place prop panel in FreeFloat frame
   if (extType==0 or extType==1):      
      _inlinePanelFramePropCtrl.setVisible( ctx.field("propExtVisible").value  )           
      if (ctx.field("propExtVisible").value==True):         
         x = ctx.field("mousePropExt.absX").value + xOffset
         y = viewerExtSize.height() - ctx.field("mousePropExt.absY").value + yOffset         
         _freeFloatFrameCtrl.setControlPos ( _inlinePanelFramePropCtrl, x, y )
   
   
   # Place pick panel in FreeFloat frame
   if (extType==0 or extType==2):   
      _inlinePanelFramePickCtrl.setVisible( ctx.field("pickExtVisible").value )         
      if (ctx.field("pickExtVisible").value==True):         
         x = ctx.field("mousePickExt.absX").value + xOffset
         y = viewerExtSize.height() - ctx.field("mousePickExt.absY").value + yOffset
         _freeFloatFrameCtrl.setControlPos ( _inlinePanelFramePickCtrl, x, y )
         #get window and update list
         count = len(ctx.field("METKPicking.objList").value.split(","))
         print "Count="+str(count)
         newHeight = count*48+6
         print newHeight
         _freeFloatFrameCtrl.setControlSize (_inlinePanelFramePickCtrl, 225, int(newHeight))
         
      
   return
  

 
def showPropExt(fieldP=0):      
   if (ctx.field("enablePropExtension").value==True and ctx.field("mousePropExt.button3Pressed").value==False):
      if (ctx.field("METKPicking.objName").value!=""):
         ctx.field("propExtVisible").value = True
      else:
         ctx.field("propExtVisible").value = False
      updateGeometry(1)
   return

def hidePropExt(fieldP=0):
   if (ctx.field("enablePropExtension").value==True):
      ctx.field("propExtVisible").value = False
      updateGeometry(1)   
   return



def showPickExt(fieldP=0):
   if (ctx.field("enablePickExtension").value==True and ctx.field("mousePickExt.button1Pressed").value==False):
      if (ctx.field("METKPicking.objName").value==""):
         ctx.field("pickExtVisible").value = False
      else:
         ctx.field("pickExtVisible").value = True        
                 
      updateGeometry(2)         
   return
   
   
def hidePickExt(fieldP=0):
   if (ctx.field("enablePickExtension").value==True):
      ctx.field("pickExtVisible").value = False
      updateGeometry(2)   
   return




def initPickWin(args=0):
   print "initPickWin"
   #global _pickWin
   #_pickWin = ctx
   #print _pickWin
   #_pickWin = ctx.control("pickWinPanel")
   #print _pickWin
   return
   
   
   
def sendCamera(fieldP=0):
   if (ctx.field("enableUndo").value==True):
      if (ctx.field("singleMove.button1Pressed").value==False):
         ctx.field("UMDViewerCommunicator.Refresh").touch()
   return