from ObjMgr import *
from VisDefinitions import *
#from NeckSurgery import *
import urllib

_cls_iter = None
_cls_info = None

_previousSlice3D = -1
_prevSelectedLNSize = 1

_dynamicFrameControl = None

_startUp = None
_helpWindow = None


# Content strings for the main tab view
_dynFrame3D = "												\
   Grid {												\
      name = dynFrameGrid										\
      expandY = yes											\
      Panel {												\
         name = Big3DView										\
         x=0 y=0 x2=4 y2=1										\
         name = Big3DViewer										\
         module  = METKViewer3D										\
         window  = METKViewer										\
      }													\
      Vertical {											\
         x=5 y=0 x2=6 y2=1										\
         expandX = Yes											\
         expandY = Yes											\
         Viewer = AxialPreview.view2d    { type = SoRenderArea border = false name = SmallAxialView }	\
         Viewer = CoronalPreview.view2d  { type = SoRenderArea border = false name = SmallCoronalView }	\
         Viewer = SagittalPreview.view2d { type = SoRenderArea border = false name = SmallSagittalView }\
      }													\
   }"

_dynFrameAxial = "											\
   Grid {												\
      name = dynFrameGrid										\
      expandY = yes											\
      Viewer = AxialPreview.view2d {									\
         name = BigAxialView										\
         x=0 y=0 x2=4 y2=1										\
         type = SoRenderArea							       \
         border = false								       \
      }										       \
      Vertical {								       \
         x=5 y=0 x2=6 y2=1							       \
         expandX = Yes								       \
         expandY = Yes								       \
         Panel {								       \
            name = Small3DView                                                           \
            module  = METKViewer3D							       \
            window  = METKViewer						       \
         }									       \
         Viewer = CoronalPreview.view2d  { type = SoRenderArea border = false name = SmallCoronalView }	       \
         Viewer = SagittalPreview.view2d { type = SoRenderArea border = false name = SmallSagittalView }	       \
      } 									       \
   }"

_dynFrameCoronal = "								       \
   Grid {									       \
      name = dynFrameGrid										\
      expandY = yes								       \
      Viewer = CoronalPreview.view2d {						       \
         name = BigCoronalView                                                         \
         x=0 y=0 x2=4 y2=1							       \
         type = SoRenderArea							       \
         border = false								       \
      }										       \
      Vertical {								       \
         x=5 y=0 x2=6 y2=1							       \
         expandX = Yes								       \
         expandY = Yes								       \
         Viewer = AxialPreview.view2d    { type = SoRenderArea border = false name = SmallAxialView }	       \
         Panel {								       \
            name = Small3DView                                                           \
            module  = METKViewer3D							       \
            window  = METKViewer						       \
         }									       \
         Viewer = SagittalPreview.view2d { type = SoRenderArea border = false name = SmallSagittalView }	       \
      }           								       \
   }"

_dynFrameSagittal = "								       \
   Grid {									       \
      name = dynFrameGrid										\
      expandY = yes								       \
      Viewer = SagittalPreview.view2d {						       \
         name = BigSagittalView                                                        \
         x=0 y=0 x2=4 y2=1							       \
         type = SoRenderArea							       \
         border = false								       \
      }										       \
      Vertical{									       \
         x=5 y=0 x2=6 y2=1							       \
         expandX = Yes								       \
         expandY = Yes								       \
         Viewer = AxialPreview.view2d   { type = SoRenderArea border = false name = SmallAxialView }	       \
         Viewer = CoronalPreview.view2d { type = SoRenderArea border = false name = SmallCoronalView }	       \
         Panel {								       \
            name = Small3DView                                                           \
            module  = METKViewer3D							       \
            window  = METKViewer						       \
         }									       \
      }										       \
   }"

_dynFrameOrtho = "									       \
   Grid {										       \
      name = dynFrameGrid										\
      name = OrthoViewLayout                                                                   \
      tabInitial = Yes									       \
      expandY = yes									       \
      Viewer = SagittalPreview.view2d { x=0 y=0 x2=1 y2=1 type = SoRenderArea border = false name = OrthoSagittalView } \
      Viewer = CoronalPreview.view2d  { x=2 y=0 x2=3 y2=1 type = SoRenderArea border = false name = OrthoCoronalView } \
      Panel { x=0 y=2 x2=1 y2=3							       	       \
         name = Ortho3DView                                                        \
         module  = METKViewer3D							  	       \
         window  = METKViewer							               \
      }										               \
      Viewer = AxialPreview.view2d    { x=2 y=2 x2=3 y2=3 type = SoRenderArea border = false name = OrthoAxialView } \
   }"


def init():
   global _cls_iter
   global _cls_info
   
   #button für 3d aktivieren
   ctx.field("METKViewer3D.viewAll").touch()
   #ctx.field("METKViewer3D.decoration").value = True
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, "METKViewer3D", LAY_VIEWER_CAMERA)

   ctx.field("AxialPreview.annotations").value = False
   ctx.field("CoronalPreview.annotations").value = False
   ctx.field("SagittalPreview.annotations").value = False
   return

def initWindow(field = 0):
   global _dynamicFrameControl

   _dynamicFrameControl = ctx.control("DynamicFrameControl")
   _dynamicFrameControl.setContentString(_dynFrame3D)
   pass
      

def changeTabView(field):
   
   #Großansicht
   if field.value == 0:
      changeMainViewer(ctx.field("MainViewer"))
      ctx.field("AxialPreview.annotations").value = False
      ctx.field("CoronalPreview.annotations").value = False
      ctx.field("SagittalPreview.annotations").value = False
      ctx.field("axialLiftChart.verticalOffset").value = 110
   #Ortho View
   elif field.value == 4:
      _dynamicFrameControl.setContentString(_dynFrameOrtho)
      ctx.field("AxialPreview.annotations").value = True
      ctx.field("CoronalPreview.annotations").value = True
      ctx.field("SagittalPreview.annotations").value = True
      ctx.field("axialLiftChart.verticalOffset").value = 110
      ctx.field("axialLiftChart.active").value = True
      ctx.field("coronalLiftChart.active").value = True
      ctx.field("sagittalLiftChart.active").value = True

   return

	
def changeMainViewer(field):
   global _dynamicFrameControl
   
   #große 3D Ansicht
   if field.value == 0:
      _dynamicFrameControl.setContentString(_dynFrame3D)
      ctx.field("AxialPreview.annotations").value = False
      ctx.field("CoronalPreview.annotations").value = False
      ctx.field("SagittalPreview.annotations").value = False
      ctx.field("axialLiftChart.active").value = False
      ctx.field("coronalLiftChart.active").value = False
      ctx.field("sagittalLiftChart.active").value = False
      wfeNSPSendState("large_view", "3d_view")
      
   #Axiale Ansicht mit Default-Einteilung
   if field.value == 1:
      _dynamicFrameControl.setContentString(_dynFrameAxial)
      ctx.field("AxialPreview.annotations").value = True
      ctx.field("CoronalPreview.annotations").value = False
      ctx.field("SagittalPreview.annotations").value = False			
      ctx.field("axialLiftChart.active").value = True
      ctx.field("coronalLiftChart.active").value = False
      ctx.field("sagittalLiftChart.active").value = False
      wfeNSPSendState("large_view", "axial_view")
      
   #Coronal Ansicht mit Default-Einteilung
   if field.value == 2:
      _dynamicFrameControl.setContentString(_dynFrameCoronal)
      ctx.field("AxialPreview.annotations").value = False
      ctx.field("CoronalPreview.annotations").value = True
      ctx.field("SagittalPreview.annotations").value = False		
      ctx.field("axialLiftChart.active").value = False
      ctx.field("coronalLiftChart.active").value = True
      ctx.field("sagittalLiftChart.active").value = False
      wfeNSPSendState("large_view", "coronal_view")
	   	
   #Sagittal Ansicht mit Default-Einteilung
   if field.value == 3:
      _dynamicFrameControl.setContentString(_dynFrameSagittal)
      ctx.field("AxialPreview.annotations").value = False
      ctx.field("CoronalPreview.annotations").value = False
      ctx.field("SagittalPreview.annotations").value = True
      ctx.field("axialLiftChart.active").value = False
      ctx.field("coronalLiftChart.active").value = False
      ctx.field("sagittalLiftChart.active").value = True
      wfeNSPSendState("large_view", "sagittal_view")
   pass
   
   
def showSliceIn3D(field):
   global _previousSlice3D

   if field.value == _previousSlice3D:
      ctx.field("3DWithSlice.whichChild").value = -1
      field.value = -1
      wfeNSPSendState("slice_in_3d", "off")

   else:
      ctx.field("3DWithSlice.whichChild").value = field.value
      _previousSlice3D = field.value
      if _previousSlice3D == 0:
         wfeNSPSendState("slice_in_3d", "axial")
      elif _previousSlice3D == 1:
         wfeNSPSendState("slice_in_3d", "coronal")
      elif _previousSlice3D == 2:
         wfeNSPSendState("slice_in_3d", "sagittal")
   return


def showDecoration(field = 0):
   ctx.field("METKViewer3D.decoration").value = not ctx.field("METKViewer3D.decoration").value
   pass
	
	 
def openCase(field = 0):
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.cleanup").touch()
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      ctx.window().setTitle("METKExplorer | " + caseFile)
   return
   
def closeCase(field = 0):
   ctx.field("METKManager.cleanup").touch()
   return
   	
def saveCase(field = 0):
   ctx.field("METKManager.save").touch()
   return   	
   	
def saveCaseAs(field = 0):
   ctx.field("METKManager.saveAs").touch()
   return
   


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
   pass

def handleLoadedEvent():
   pass

   
def handleSaveEvent():
   pass

def handleCleanupEvent():
   pass

