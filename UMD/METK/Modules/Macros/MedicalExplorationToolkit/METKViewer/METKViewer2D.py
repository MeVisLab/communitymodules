# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *


# Global class instances
_cls_info= None
_oldSelection = None
_viewerName = ""
_updatedByMyself = False
_voxelPosChangedIntern = False

def init():
   global _cls_info
   global _oldSelection
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))   
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO, LAY_SELECTEDOBJ)
   _oldSelection = ctx.field("selectedPosition").value
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
   global _updatedByMyself
   global _voxelPosChangedIntern
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   if layer == LAY_SELECTEDOBJ and _cls_info.getVec3(OBJ_CSO, LAY_SELECTEDOBJ, INF_SELECTION) != (0.0, 0.0, 0.0):
      _voxelPosChangedIntern = True
      ctx.field("selectedPosition").setStringValue(_cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_SELECTION))
   elif layer == LAY_GLOBAL and info == INF_GLOBAL_REFRESH and _cls_info.getBool() == True:
      refresh()
   elif layer == LAY_VIEWER_PROPERTIES and _updatedByMyself == False and object == _viewerName:
      if info == INF_WINDOWCENTER:
         ctx.field("windowCenter").value     = _cls_info.getDouble(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWCENTER)
      elif info == INF_WINDOWWIDTH:
         ctx.field("windowWidth").value      = _cls_info.getDouble(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWWIDTH)
      elif info == INF_LAYOUT:
         ctx.field("layoutMode").value       = _cls_info.get(_viewerName, LAY_VIEWER_PROPERTIES, INF_LAYOUT)
      elif info == INF_ANNOTATIONS:
         ctx.field("annotations").value      = _cls_info.getBool(_viewerName, LAY_VIEWER_PROPERTIES, INF_ANNOTATIONS)
      elif info == INF_WPOSITION:
         _voxelPosChangedIntern = True
         ctx.field("worldPosition").value = _cls_info.getVec3(_viewerName, LAY_VIEWER_PROPERTIES, INF_WPOSITION)
   elif layer == LAY_VIEWER_SCREENSHOT and info == INF_SCREENSHOTFLAG and _cls_info.getBool():
      ctx.field("screenshotFile").value = _cls_info.get(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFILE)
      saveScreenshot()
   pass
   

def handleLoadedEvent():
   global _viewerName
   global _voxelPosChangedIntern
   viewerNameChanged(0)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWCENTER):
      ctx.field("windowCenter").value     = _cls_info.getDouble(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWCENTER)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWWIDTH):
      ctx.field("windowWidth").value      = _cls_info.getDouble(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWWIDTH)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_PROPERTIES, INF_LAYOUT):
      ctx.field("layoutMode").value       = _cls_info.get(_viewerName, LAY_VIEWER_PROPERTIES, INF_LAYOUT)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_PROPERTIES, INF_ANNOTATIONS):
      ctx.field("annotations").value      = _cls_info.getBool(_viewerName, LAY_VIEWER_PROPERTIES, INF_ANNOTATIONS)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_PROPERTIES, INF_WPOSITION):
      _voxelPosChangedIntern = True
      ctx.field("worldPosition").value = _cls_info.getVec3(_viewerName, LAY_VIEWER_PROPERTIES, INF_WPOSITION)
   if _cls_info.existInfo(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFILE):
      ctx.field("screenshotFile").value = _cls_info.get(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFILE)
   _cls_info.markObjectNonPersistent(_viewerName)
   pass
   
   
def handleSaveEvent():
   pass


def handleCleanupEvent():
   pass
   
   
def selectedPosition(field):
   global _oldSelection
   if field.name == "selectedPosition" and field.value != _oldSelection:
      ctx.field("SoView2DPosition.worldPosition").value = field.value
      ctx.field("worldPosition").value = field.value
      _oldSelection = field.value
   elif ctx.field("SoView2DMenu.hitItem").value == -1:
      ctx.field("selectedPosition").value = field.value
      ctx.field("worldPosition").value = field.value
      _oldSelection = field.value
   pass
   
   
def viewerNameChanged(field = 0):
   global _viewerName
   oldName = _viewerName
   _viewerName = ctx.field("instanceName").value
   
   #Dieser Error wird nicht geworfen, wenn gleiche Viewernamen auftreten,
   #indem die sie durch Macronetze wie ViewSlices hinzugefügt werden.
   #Die Verbinsung zum ObjMgr erfolgt dann erst nach der Setzung des InstanceNames
   #Somit geht die Überprüfung schief
   if _cls_info.existObject(_viewerName) and field:
      MLAB.logError("At least two viewers share the same name (" + _viewerName + ")!")
      
   if oldName == "":
      _cls_info.set(_viewerName, LAY_GLOBAL, INF_OBJTYPE, "Viewer2D")
   else:
      _cls_info.renameObject(oldName, _viewerName)
      
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO + "," + _viewerName, LAY_SELECTEDOBJ + "," + LAY_VIEWER_PROPERTIES + "," + LAY_VIEWER_SCREENSHOT + "," + LAY_GLOBAL)
   _cls_info.notify()
   return


def refresh(field = 0):
   global _viewerName
   global _updatedByMyself
   _updatedByMyself = True
   viewerNameChanged(0)
   
   _cls_info.typedSet(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWCENTER, ctx.field("windowCenter").value,     INFOTYPE_DOUBLE)
   _cls_info.typedSet(_viewerName, LAY_VIEWER_PROPERTIES, INF_WINDOWWIDTH,  ctx.field("windowWidth").value,      INFOTYPE_DOUBLE)

   _cls_info.typedSet(_viewerName, LAY_VIEWER_PROPERTIES, INF_LAYOUT,       ctx.field("layoutMode").value,       INFOTYPE_STRING)
   _cls_info.typedSet(_viewerName, LAY_VIEWER_PROPERTIES, INF_ANNOTATIONS,  ctx.field("annotations").value,      INFOTYPE_BOOL)
   _cls_info.typedSet(_viewerName, LAY_VIEWER_PROPERTIES, INF_WPOSITION,    ctx.field("worldPosition").stringValue(), INFOTYPE_VEC3)

   _cls_info.typedSet(_viewerName, LAY_GLOBAL, INF_GLOBAL_REFRESH, False, INFOTYPE_BOOL)
   ctx.callLater(0, "_cls_info.notify")
   _updatedByMyself = False
   pass


def changedVoxelPos(field = 0):
   global _voxelPosChangedIntern
   if _voxelPosChangedIntern == False:
      if field.name == "voxelPos":
         _voxelPosChangedIntern = True
         ctx.field("voxelPosition").value = field.value
   
      else:
         _voxelPosChangedIntern = True
         ctx.field("voxel2world.voxelPos").value = ctx.field("voxelPosition").value
   _voxelPosChangedIntern = False
   pass


def saveScreenshot(field = 0):
   ctx.field("ready").value = False
   ctx.field("viewer.doSnapshot").touch()
   ctx.field("IdleTrigger.on").value = True
   pass
   

def finishingScreenshot(field = 0):
   ctx.field("IdleTrigger.on").value = False
   fileManager = MLABFileManager
   fileName = ctx.field("screenshotFile").value;
   if fileManager.exists(fileManager.normalizePath(fileName)):
      if ctx.field("saveThumbNail").value == True or _cls_info.getBool(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTTHUMB) == True:
         standardFileName = fileManager.normalizePath(fileName);
         pathComponents = fileManager.splitPath(standardFileName);
         dirName = pathComponents["dir"];
         fileBaseName = pathComponents["name"];
         baseName = pathComponents["base"];
         extension = pathComponents["ext"];
         thumbName = dirName + baseName + ".thumb.tif"
         if not fileManager.exists(fileManager.normalizePath(thumbName)):
            MLABGraphic.createThumbNail(ctx.field("screenshotFile").value, thumbName, ctx.field("thumbSize").value)
            ctx.field("IdleTrigger.on").value = True
         else:
            _cls_info.typedSet(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG, False, INFOTYPE_BOOL)
            ctx.field("ready").value = True
            _cls_info.notify()
      else:
         _cls_info.typedSet(_viewerName, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG, False, INFOTYPE_BOOL)
         ctx.field("ready").value = True
         _cls_info.notify()
   pass
   
   
def autoRefreshObjMgr(args=0):
   if ctx.field("autoRefreshObjMgr").value == True:
      refresh()
   return