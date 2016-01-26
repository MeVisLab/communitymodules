from ObjMgr import *
from definitions import *

_cls_info= None
_ObjName = ""


def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))
   
   ctx.field("info.eventReceiveMode").value = ERM_SELECTED
   ctx.field("info.acceptedObjects").value  = ""
   ctx.field("info.acceptedLayers").value   = "*"
   
   global _ObjName
   _ObjName = ""
   
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   if (_cls_info.activeObject()==ctx.field("ObjName").value):
      _cls_info.setString(_cls_info.activeObject(), LAY_GLOBAL, "ObjectType", "Viewer3D")
   return

def handleObjectRemovedEvent():
   return

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return

def handleAttributeRemovedEvent():
   return

def handleAttributeModifiedEvent():
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   if layer == LAY_VIEWER_CAMERA:
      if info == INF_VIEWER_CAMERA_POSITION:
         ctx.field("CamPosition").setStringValue(value)
      elif info == INF_VIEWER_CAMERA_ORIENTATION:
         ctx.field("CamOrientation").setStringValue(value)
      elif info == INF_VIEWER_CAMERA_HEIGHT:
         ctx.field("CamHeight").setStringValue(value)
      elif info == INF_VIEWER_CAMERA_FOCALDISTANCE:
         ctx.field("CamFocalDistance").setStringValue(value)
      elif info == INF_VIEWER_CAMERA_NEWDATA:
         ctx.field("CamPosition").setStringValue(_cls_info.get(object, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION))
         ctx.field("CamOrientation").setStringValue(_cls_info.get(object, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION))
         ctx.field("CamHeight").setStringValue(_cls_info.get(object, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT))
         ctx.field("CamFocalDistance").setStringValue(_cls_info.get(object, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_FOCALDISTANCE))
         ctx.field("newData").touch()
   
   elif layer == LAY_VIEWER_PROPERTIES:
      if info == INF_VIEWER_PROPERTIES_FRAMERATE:
         ctx.field("ViewerFramerate").setStringValue(value)
      elif info == INF_VIEWER_PROPERTIES_CAPTUREFRAME:
         if value == "ping":
            ctx.field("ViewerCaptureFrame").touch()
      elif info == INF_VIEWER_PROPERTIES_CREATEAVI:
         if value == "ping":
            ctx.field("ViewerCreateAVI").touch()
      elif info == INF_VIEWER_PROPERTIES_CANCELRECORDING:
         if value == "ping":
            ctx.field("ViewerCancelRecording").touch()
      elif info == INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT:
         if value == "ping":
            ctx.field("ViewerUpdateMLOutput").touch()
      elif info == INF_VIEWER_PROPERTIES_VIEWERSTATUS:
         ctx.field("ViewerStatus").setStringValue(value)
      elif info == INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS:
         ctx.field("ProtocolViewerStatus").setStringValue(value)
      elif info == INF_VIEWER_PROPERTIES_BGCOLOR:
         ctx.field("ViewerBGColor").setStringValue(value)
      elif info == INF_VIEWER_PROPERTIES_BGGREYCENTER:
         ctx.field("ViewerBGGreyCenter").setStringValue(value)
      elif info == INF_VIEWER_PROPERTIES_BGGREYWIDTH:
         ctx.field("ViewerBGGreyWidth").setStringValue(value)
   elif layer == LAY_GLOBAL:
      if info == INF_GLOBAL_REFRESH:
         refreshAll(0)
         #_cls_info.setString(_ObjName, LAY_GLOBAL, INF_GLOBAL_REFRESH, "0")
   return


def refreshAll(args):
   CamPositionChanged(1)
   CamOrientationChanged(1)
   CamHeightChanged(1)
   CamFocalDistanceChanged(1)
   ViewerFramerateChanged(1)
   ViewerCaptureFrameChanged(1)
   ViewerCreateAVIChanged(1)
   ViewerCancelRecordingChanged(1)
   ViewerUpdateMLOutputChanged(1)
   ViewerStatusChanged(1)
   ProtocolViewerStatusChanged(1)
   ViewerBGColorChanged(1)
   ViewerBGGreyCenterChanged(1)
   ViewerBGGreyWidthChanged(1)
   # Hier wird der Schalter noch auf die altmodische Weise zurückgesetzt, da REFRESH von extern generiert wird und es im moment zuviel Umstände macht dort eine Typ gebundene Erstellung einzuführen
   _cls_info.setString(_ObjName, LAY_GLOBAL, INF_GLOBAL_REFRESH, "0")
   # macht diese Zeile hier Sinn?
   _cls_info.setString(_ObjName, LAY_GLOBAL, "ObjectType", "Viewer3D")
   
   # Zeitverzögerter Aufruf damit möglich notify()-Kollisionen vermieden werden können
   ctx.callLater(0, "_cls_info.notify")
   
   return


def ObjNameChanged(againstTError):
   ctx.field("info.acceptedObjects").value  = ctx.field("ObjName").value
   
   global _ObjName
   _ObjName = ctx.field("ObjName").value
   
   _cls_info.setString(_ObjName, LAY_GLOBAL, "ObjectType", "Viewer3D")
   ctx.field("ProtocolViewerStatus").setStringValue(_cls_info.get(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS))
   return


def CamPositionChanged(againstTError):
   if (ctx.field("ProtocolViewerStatus").value==True) or (againstTError==1):
      if _cls_info.existInfo(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION) == False:
         _cls_info.typedSet(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION, "" , INFOTYPE_VEC3)
      _cls_info.setVec3(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_POSITION, ctx.field("CamPosition").vectorValue())
      if (againstTError!=1):
         _cls_info.notify()
   return

def CamOrientationChanged(againstTError):
   if (ctx.field("ProtocolViewerStatus").value==True) or (againstTError==1):
      if _cls_info.existInfo(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION) == False:
         _cls_info.typedSet(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION, "", INFOTYPE_VEC4)
      _cls_info.setVec4(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_ORIENTATION, ctx.field("CamOrientation").vectorValue())
      if (againstTError!=1):
         _cls_info.notify()
   return

def CamHeightChanged(againstTError):
   if (ctx.field("ProtocolViewerStatus").value==True) or (againstTError==1):
      if _cls_info.existInfo(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT) == False:
         _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT, "", INFOTYPE_DOUBLE)
      _cls_info.setDouble(ctx.field("ObjName").stringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_HEIGHT, ctx.field("CamHeight").floatValue())
      if (againstTError!=1):
         _cls_info.notify()
   return

def CamFocalDistanceChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_FOCALDISTANCE) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_FOCALDISTANCE, "", INFOTYPE_DOUBLE)
   _cls_info.setDouble(ctx.field("ObjName").stringValue(), LAY_VIEWER_CAMERA, INF_VIEWER_CAMERA_FOCALDISTANCE, ctx.field("CamFocalDistance").floatValue())
   #ctx.module("info").field("notify").touch()
   return

def ViewerFramerateChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_FRAMERATE) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_FRAMERATE, "", INFOTYPE_INT32)
   _cls_info.setInt(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_FRAMERATE, ctx.field("ViewerFramerate").intValue())
   #ctx.module("info").field("notify").touch()
   return

def ViewerCaptureFrameChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CAPTUREFRAME) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CAPTUREFRAME, "", INFOTYPE_MESSAGE)
   #ctx.module("info").field("notify").touch()
   return

def ViewerCreateAVIChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CREATEAVI) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CREATEAVI, "", INFOTYPE_MESSAGE)   
   #ctx.module("info").field("notify").touch()
   return

def ViewerCancelRecordingChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CANCELRECORDING) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_CANCELRECORDING, "", INFOTYPE_MESSAGE)
   #ctx.module("info").field("notify").touch()
   return

def ViewerUpdateMLOutputChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_UPDATEMLOUTPUT, "", INFOTYPE_MESSAGE)
   #ctx.module("info").field("notify").touch()
   return

def ViewerStatusChanged(againstTError):
   if (ctx.field("ProtocolViewerStatus").stringValue()=="TRUE" and ctx.field("ViewerStatus").stringValue()!="x"):
      if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_VIEWERSTATUS) == False:
         _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_VIEWERSTATUS, "", INFOTYPE_MESSAGE)
      _cls_info.setString(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_VIEWERSTATUS, ctx.field("ViewerStatus").stringValue())
      #ctx.module("info").field("notify").touch()
      #ctx.log("callLater von notify bei viewerStatus: " + ctx.field("ViewerStatus").stringValue())
      ctx.callLater(0, "_cls_info.notify")
      ctx.field("ViewerStatus").value = "x"
   return

def ProtocolViewerStatusChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS, "", INFOTYPE_BOOL)
   _cls_info.setBool(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_PROTOCOLVIEWERSTATUS, ctx.field("ProtocolViewerStatus").boolValue())
   #ctx.module("info").field("notify").touch()
   return

def ViewerBGColorChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGCOLOR) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGCOLOR, "", INFOTYPE_VEC3)
   _cls_info.setVec3(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGCOLOR, ctx.field("ViewerBGColor").vectorValue())
   #ctx.module("info").field("notify").touch()
   return

def ViewerBGGreyCenterChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYCENTER) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYCENTER, "", INFOTYPE_DOUBLE)
   _cls_info.setDouble(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYCENTER, ctx.field("ViewerBGGreyCenter").doubleValue())
   #ctx.module("info").field("notify").touch()
   return

def ViewerBGGreyWidthChanged(againstTError):
   if _cls_info.existInfo(_ObjName, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYWIDTH) == False:
      _cls_info.typedSet(ctx.field("ObjName").value, LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYWIDTH, "", "omAttribute_double")
   _cls_info.setDouble(ctx.field("ObjName").stringValue(), LAY_VIEWER_PROPERTIES, INF_VIEWER_PROPERTIES_BGGREYWIDTH, ctx.field("ViewerBGGreyWidth").doubleValue())
   #ctx.module("info").field("notify").touch()
   return