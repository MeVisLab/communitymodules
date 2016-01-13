# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *
from NPRShadingParameter import *

# Frequently used controls
# ...

# Frequently used fields
# ...

# Global class instances
_cls_info = None
_cls_iter = None
_containerList = []
_activeIDList = []
_counter = 0.0

_dynamicFrameControl = None

def init():
   global _cls_info
   global _cls_iter
   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   return

def initWindow(field = 0):
   global _dynamicFrameControl
   _dynamicFrameControl = ctx.control("DynamicFrameControl")


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
   #global _containerList, _activeIDList
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()

   if layer == LAY_NPRSHADING:
      if object == OBJ_NPRSHADING:
         # global NPR settings
         print value
         if info == INF_BUFFERTYPE:
            ctx.field("SoShadingMap.bufferType").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_BUFFERTYPE))
         elif info == INF_GLOBALLIGHT:
            ctx.field("SoShadingMap.lightDir").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GLOBALLIGHT))
         elif info == INF_NORMALIZATION:
            ctx.field("SoShadingMap.normalizationType").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_NORMALIZATION))
         elif info == INF_MAXDIST:
            ctx.field("SoShadingMap.maxDistance").value = _cls_info.getDouble(OBJ_NPRSHADING, LAY_NPRSHADING, INF_MAXDIST)
         elif info == INF_RIMLIGHTFILTER:
            ctx.field("SoShadingMap.gaussKernelWidthLight").value = _cls_info.getInt(OBJ_NPRSHADING, LAY_NPRSHADING, INF_RIMLIGHTFILTER)
         elif info == INF_SHADOWFILTER:
            ctx.field("SoShadingMap.gaussKernelWidthShadow").value = _cls_info.getInt(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SHADOWFILTER)
         elif info == INF_GAMMA:
            ctx.field("SoShadingMap.gamma").value = _cls_info.getDouble(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GAMMA)
         elif info == INF_SCENEINFO:
            ctx.field("SoShadingMap.sceneInformation").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SCENEINFO))
         else:
            ctx.log("You've found the forgotten parameter! (handleAttributeModifiedEvent): " + object + " " + info)
      else:
         # per object NPR settings
         if info == INF_COSSMOOTH:
            setFieldValueFromInfo(object, INF_COSSMOOTH, "SoShadingMap.cosineSmoothing")
         elif info == INF_RAKEXAGGERATION:
            setFieldValueFromInfo(object, INF_RAKEXAGGERATION, "SoShadingMap.rakingExaggeration")
         elif info == INF_RAKFREQUENCY:
            setFieldValueFromInfo(object, INF_RAKFREQUENCY, "SoShadingMap.rakingFrequency")
         elif info == INF_WEIGHTCOSINE:
            setFieldValueFromInfo(object, INF_WEIGHTCOSINE, "SoShadingMap.weightCosineShading")
         elif info == INF_WEIGHTRAKING:
            setFieldValueFromInfo(object, INF_WEIGHTRAKING, "SoShadingMap.weightRaking")
         elif info == INF_WEIGHTPLATEAU:
            setFieldValueFromInfo(object, INF_WEIGHTPLATEAU, "SoShadingMap.weightPlateau")
         elif info == INF_WEIGHTMAXCURV:
            setFieldValueFromInfo(object, INF_WEIGHTMAXCURV, "SoShadingMap.weightMaxCurvature")
         elif info == INF_WEIGHTSHAPE:
            setFieldValueFromInfo(object, INF_WEIGHTSHAPE, "SoShadingMap.weightSurfaceShape")
         elif info == INF_WEIGHTDIST:
            setFieldValueFromInfo(object, INF_WEIGHTDIST, "SoShadingMap.weightDistance")
         elif info == INF_WEIGHTDOI:
            setFieldValueFromInfo(object, INF_WEIGHTDOI, "SoShadingMap.weightDOI")
         elif info == INF_WEIGHTEMPH:
            setFieldValueFromInfo(object, INF_WEIGHTEMPH, "SoShadingMap.weightEmphasis")
         elif info == INF_WEIGHTDEPTH:
            setFieldValueFromInfo(object, INF_WEIGHTDEPTH, "SoShadingMap.weightDepth")
         elif info == INF_WEIGHTRIMLIGHT:
            setFieldValueFromInfo(object, INF_WEIGHTRIMLIGHT, "SoShadingMap.weightRimLight")
         elif info == INF_WEIGHTDEPTHSHAD:
            setFieldValueFromInfo(object, INF_WEIGHTDEPTHSHAD, "SoShadingMap.weightDepthShadow")
         elif info == INF_WEIGHTDEPTHFADING:
            setFieldValueFromInfo(object, INF_WEIGHTDEPTHFADING, "SoShadingMap.weightDepthFading")
         elif info == INF_DFINTENSITY:
            setFieldValueFromInfo(object, INF_DFINTENSITY, "SoShadingMap.depthFadingIntensity")
         elif info == INF_DFMIN:
            setFieldValueFromInfo(object, INF_DFMIN, "SoShadingMap.depthFadingMin")
         elif info == INF_DFMAX:
            setFieldValueFromInfo(object, INF_DFMAX, "SoShadingMap.depthFadingMax")
         elif info == INF_DFEXPONENT:
            setFieldValueFromInfo(object, INF_DFEXPONENT, "SoShadingMap.depthFadingExponent")
         elif info == INF_WEIGHTLINES:
            setFieldValueFromInfo(object, INF_WEIGHTLINES, "SoShadingMap.weightFeatureLines")
         elif info == INF_LINEWIDTH:
            setFieldValueFromInfo(object, INF_LINEWIDTH, "SoShadingMap.featureLinesWidth")
         elif info == INF_BORDERENHANCE:
            setFieldValueFromInfo(object, INF_BORDERENHANCE, "SoShadingMap.borderEnhancement")
         elif info == INF_DETAILAMPLITUDE:
            setFieldValueFromInfo(object, INF_DETAILAMPLITUDE, "SoShadingMap.detailAmplitude")
         elif info == INF_SPATIALEFFECT:
            setFieldValueFromInfo(object, INF_SPATIALEFFECT, "SoShadingMap.spatialEffect")
         elif info == INF_FOREGROUNDEXT:
            setFieldValueFromInfo(object, INF_FOREGROUNDEXT, "SoShadingMap.foregroundExtension")
         elif info == INF_NPRSETACTIVE:
            updateShadingMapIDList(object, _cls_info.getBool())
            ctx.field("SoShadingMap.updatedIDList").value = str(_activeIDList)
         else:
            ctx.log("You've found the forgotten parameter! (handleAttributeModifiedEvent): " + object + " " + info)
   pass


def updateShadingMapIDList(object, add):
   global _activeIDList
   container = ctx.module(_cls_info.adjustObjectID(object))
   idNr = container.field("idNr").value
   if (add == True):
      if (idNr not in _activeIDList):
         _activeIDList.append(idNr)
   else:
      if (idNr in _activeIDList):
         _activeIDList.remove(idNr)
   pass


def setFieldValueFromInfo(object, info, field):
   global _activeIDList
   container = ctx.module(_cls_info.adjustObjectID(object))
   idNr = container.field("idNr").value
   ctx.field("SoShadingMap.updatedIDList").value = idNr
   ctx.field(field).value = _cls_info.getDouble(object, LAY_NPRSHADING, info)
   ctx.field("SoShadingMap.updatedIDList").value = str(_activeIDList)
   pass


def handleCleanupEvent():
   _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Cleaning up METKNPRShading...", INFOTYPE_MESSAGE)
   _cls_info.notify()
   
   ctx.log("clean METKNPRShading")
   
   ctx.disconnectField("SoShadingMap.input")
   ctx.module("ShadingMapGroup").removeChildren()
   ctx.field("SoShadingMap.numIDFld").value = 1
   ctx.field("SoShadingMap.updatedIDList").value = "[]"
   
   ctx.callLater(0, "deleteModules")
   pass


def deleteModules():
   global _containerList
   # Alle Module löschen
   moduleCounter = 0.0
   list = ctx.modules()
   for module in _containerList:
      moduleCounter = moduleCounter + 1.0
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS, (moduleCounter / float(len(list))), INFOTYPE_DOUBLE)
      _cls_info.notify()
      if ctx.module(module).type() == "NPRShadingContainer":
         ctx.module(module).disconnectField("WEMInfoGenerator.inWEM")
         ctx.module(module).remove()

   _containerList = []
   ctx.field("objectIDs").value = ""
   ctx.log("METKNPRShading is clean!")
   return


def handleLoadedEvent(field = 0):
   global _containerList, _counter, _activeIDList
   _activeIDList = []
   _counter = 0.0
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _counter = _counter + 1.0
      addContainer(_cls_iter.object())
      _cls_iter.searchNext()

   ctx.field("SoShadingMap.updatedIDList").value = "[]"
   ctx.field("SoShadingMap.numIDFld").value = _counter
   ctx.connectField("SoShadingMap.input", "ShadingMapGroup.self")

   # Generating global NPR shading entries for the METK
   _cls_info.set(OBJ_NPRSHADING, LAY_GLOBAL, INF_OBJTYPE, "NPRShading")
   
   # adding global NPR settings
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_BUFFERTYPE):
         ctx.field("SoShadingMap.bufferType").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_BUFFERTYPE))
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GLOBALLIGHT):
         ctx.field("SoShadingMap.lightDir").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GLOBALLIGHT))
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_NORMALIZATION):
         ctx.field("SoShadingMap.normalizationType").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_NORMALIZATION))
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_RIMLIGHTFILTER):
         ctx.field("SoShadingMap.gaussKernelWidthLight").value = _cls_info.getInt(OBJ_NPRSHADING, LAY_NPRSHADING, INF_RIMLIGHTFILTER)
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SHADOWFILTER):
         ctx.field("SoShadingMap.gaussKernelWidthShadow").value = _cls_info.getInt(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SHADOWFILTER)
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_MAXDIST):
         ctx.field("SoShadingMap.maxDistance").value = _cls_info.getDouble(OBJ_NPRSHADING, LAY_NPRSHADING, INF_MAXDIST)
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GAMMA):
         ctx.field("SoShadingMap.gamma").value = _cls_info.getDouble(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GAMMA)
   if _cls_info.existInfo(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SCENEINFO):
         ctx.field("SoShadingMap.sceneInformation").setStringValue(_cls_info.get(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SCENEINFO))

   # adding per object NPR settings
   for module in _containerList:
      object = ctx.module(module).field("objectID").value
      ctx.field("SoShadingMap.updatedIDList").value = ctx.module(module).field("idNr").value
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_COSSMOOTH):
         ctx.field("SoShadingMap.cosineSmoothing").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_COSSMOOTH)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_RAKEXAGGERATION):
         ctx.field("SoShadingMap.rakingExaggeration").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_RAKEXAGGERATION)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_RAKFREQUENCY):
         ctx.field("SoShadingMap.rakingFrequency").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_RAKFREQUENCY)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTCOSINE):
         ctx.field("SoShadingMap.weightCosineShading").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTCOSINE)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTRAKING):
         ctx.field("SoShadingMap.weightRaking").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTRAKING)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTPLATEAU):
         ctx.field("SoShadingMap.weightPlateau").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTPLATEAU)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTMAXCURV):
         ctx.field("SoShadingMap.weightMaxCurvature").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTMAXCURV)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTSHAPE):
         ctx.field("SoShadingMap.weightSurfaceShape").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTSHAPE)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTDIST):
         ctx.field("SoShadingMap.weightDistance").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTDIST)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTDOI):
         ctx.field("SoShadingMap.weightDOI").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTDOI)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTEMPH):
         ctx.field("SoShadingMap.weightEmphasis").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTEMPH)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTDEPTH):
         ctx.field("SoShadingMap.weightDepth").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTDEPTH)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTRIMLIGHT):
         ctx.field("SoShadingMap.weightRimLight").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTRIMLIGHT)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTDEPTHSHAD):
         ctx.field("SoShadingMap.weightDepthShadow").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTDEPTHSHAD)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTDEPTHFADING):
         ctx.field("SoShadingMap.weightDepthFading").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTDEPTHFADING)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_DFINTENSITY):
         ctx.field("SoShadingMap.depthFadingIntensity").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_DFINTENSITY)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_DFMIN):
         ctx.field("SoShadingMap.depthFadingMin").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_DFMIN)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_DFMAX):
         ctx.field("SoShadingMap.depthFadingMax").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_DFMAX)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_DFEXPONENT):
         ctx.field("SoShadingMap.depthFadingExponent").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_DFEXPONENT)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_WEIGHTLINES):
         ctx.field("SoShadingMap.weightFeatureLines").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_WEIGHTLINES)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_LINEWIDTH):
         ctx.field("SoShadingMap.featureLinesWidth").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_LINEWIDTH)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_BORDERENHANCE):
         ctx.field("SoShadingMap.borderEnhancement").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_BORDERENHANCE)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_DETAILAMPLITUDE):
         ctx.field("SoShadingMap.detailAmplitude").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_DETAILAMPLITUDE)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_SPATIALEFFECT):
         ctx.field("SoShadingMap.spatialEffect").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_SPATIALEFFECT)
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_FOREGROUNDEXT):
         ctx.field("SoShadingMap.foregroundExtension").value = _cls_info.getDouble(object, LAY_NPRSHADING, INF_FOREGROUNDEXT)
      # fill activeIDList
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_NPRSETACTIVE):
         updateShadingMapIDList(object, _cls_info.getBool(object, LAY_NPRSHADING, INF_NPRSETACTIVE))

   ctx.field("SoShadingMap.updatedIDList").value = str(_activeIDList)
   pass
   
   
def handleSaveEvent(field = 0):
   pass


def addContainer(objectID):
   global _containerList, _counter

   if _containerList.count(_cls_info.adjustObjectID(objectID)) == 0:
      # Container erstellen und benennen
      container = ctx.addModule("NPRShadingContainer")
      container.field("instanceName").setStringValue(_cls_info.adjustObjectID(objectID))
      container.field("idNr").setFloatValue(_counter-1)
      _containerList.append(_cls_info.adjustObjectID(objectID))
         
      ctx.module("ShadingMapGroup").addChild(container.field("outAttribITSS"))
      ctx.module("ShadingDisplayGroup").addChild(container.field("outShadingDisplay"))
      ctx.module("HatchingGroup").addChild(container.field("outHatching"))
      infoout = ctx.field("info.outObjectContainer")
      ocin = container.field("inObjectContainer")
      ocin.connectFrom(infoout)

      container.field("objectID").value = objectID
      container.field("initialize").touch()

      count = len(_containerList)
      container.setFrameTopLeft(300*(count%3),75*round((count/3.0)-0.5,0)+350)
   pass


def changedFocus(field = 0):
   if field.value == "":
      contentString = " Vertical { } "
   else:
      contentString = " Panel { module  = " + _cls_info.adjustObjectID(field.value) + " window  = Settings expandY = yes } "
   _dynamicFrameControl.setContentString(contentString)
   pass


def opacification(field = 0):
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _cls_info.typedSet(_cls_iter.object(), LAY_APPEARANCE, INF_TRANSPARENCY, 0.0, INFOTYPE_DOUBLE)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass


def allShading(field = 0):
   global _containerList
   for module in _containerList:
      displayType = ctx.module(module).field("displayType").value
      if displayType != "Invisible":
         ctx.module(module).field("displayType").value = "ShadingMap"
   pass


def allStippling(field = 0):
   global _containerList
   for module in _containerList:
      displayType = ctx.module(module).field("displayType").value
      if displayType != "Invisible":
         ctx.module(module).field("displayType").value = "Stippling"
   pass


def overrideStipplingGamma(field = 0):
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _cls_info.typedSet(_cls_iter.object(), LAY_APPEARANCE, INF_STIPPLINGGAMMA, ctx.field("stipplingGamma").value, INFOTYPE_DOUBLE)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass


def globalPropChanged(field = 0):
   global _containerList
   # global NPR settings
   if field == ctx.field("SoShadingMap.bufferType"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_BUFFERTYPE, field.stringValue(), INFOTYPE_STRING)
   elif field == ctx.field("SoShadingMap.lightDir"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GLOBALLIGHT, field.stringValue(), INFOTYPE_VEC3)
   elif field == ctx.field("SoShadingMap.normalizationType"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_NORMALIZATION, field.stringValue(), INFOTYPE_STRING)
   elif field == ctx.field("SoShadingMap.maxDistance"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_MAXDIST, field.value, INFOTYPE_DOUBLE)
   elif field == ctx.field("SoShadingMap.gaussKernelWidthLight"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_RIMLIGHTFILTER, field.value, INFOTYPE_INT32)
   elif field == ctx.field("SoShadingMap.gaussKernelWidthShadow"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SHADOWFILTER, field.value, INFOTYPE_INT32)
   elif field == ctx.field("SoShadingMap.gamma"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_GAMMA, field.value, INFOTYPE_DOUBLE)
   elif field == ctx.field("SoShadingMap.sceneInformation"):
      _cls_info.typedSet(OBJ_NPRSHADING, LAY_NPRSHADING, INF_SCENEINFO, field.stringValue(), INFOTYPE_STRING)

   # per object NPR settings
   elif field == ctx.field("SoShadingMap.cosineSmoothing"):
      setObjectNPRInfo(INF_COSSMOOTH, field.value)
   elif field == ctx.field("SoShadingMap.rakingExaggeration"):
      setObjectNPRInfo(INF_RAKEXAGGERATION, field.value)
   elif field == ctx.field("SoShadingMap.rakingFrequency"):
      setObjectNPRInfo(INF_RAKFREQUENCY, field.value)
   elif field == ctx.field("SoShadingMap.weightCosineShading"):
      setObjectNPRInfo(INF_WEIGHTCOSINE, field.value)
   elif field == ctx.field("SoShadingMap.weightRaking"):
      setObjectNPRInfo(INF_WEIGHTRAKING, field.value)
   elif field == ctx.field("SoShadingMap.weightPlateau"):
      setObjectNPRInfo(INF_WEIGHTPLATEAU, field.value)
   elif field == ctx.field("SoShadingMap.weightMaxCurvature"):
      setObjectNPRInfo(INF_WEIGHTMAXCURV, field.value)
   elif field == ctx.field("SoShadingMap.weightSurfaceShape"):
      setObjectNPRInfo(INF_WEIGHTSHAPE, field.value)
   elif field == ctx.field("SoShadingMap.weightDistance"):
      setObjectNPRInfo(INF_WEIGHTDIST, field.value)
   elif field == ctx.field("SoShadingMap.weightDOI"):
      setObjectNPRInfo(INF_WEIGHTDOI, field.value)
   elif field == ctx.field("SoShadingMap.weightEmphasis"):
      setObjectNPRInfo(INF_WEIGHTEMPH, field.value)
   elif field == ctx.field("SoShadingMap.weightDepth"):
      setObjectNPRInfo(INF_WEIGHTDEPTH, field.value)
   elif field == ctx.field("SoShadingMap.weightRimLight"):
      setObjectNPRInfo(INF_WEIGHTRIMLIGHT, field.value)
   elif field == ctx.field("SoShadingMap.weightDepthShadow"):
      setObjectNPRInfo(INF_WEIGHTDEPTHSHAD, field.value)
   elif field == ctx.field("SoShadingMap.weightDepthFading"):
      setObjectNPRInfo(INF_WEIGHTDEPTHFADING, field.value)
   elif field == ctx.field("SoShadingMap.depthFadingIntensity"):
      setObjectNPRInfo(INF_DFINTENSITY, field.value)
   elif field == ctx.field("SoShadingMap.depthFadingMin"):
      setObjectNPRInfo(INF_DFMIN, field.value)
   elif field == ctx.field("SoShadingMap.depthFadingMax"):
      setObjectNPRInfo(INF_DFMAX, field.value)
   elif field == ctx.field("SoShadingMap.depthFadingExponent"):
      setObjectNPRInfo(INF_DFEXPONENT, field.value)
   elif field == ctx.field("SoShadingMap.weightFeatureLines"):
      setObjectNPRInfo(INF_WEIGHTLINES, field.value)
   elif field == ctx.field("SoShadingMap.featureLinesWidth"):
      setObjectNPRInfo(INF_LINEWIDTH, field.value)
   elif field == ctx.field("SoShadingMap.borderEnhancement"):
      setObjectNPRInfo(INF_BORDERENHANCE, field.value)
   elif field == ctx.field("SoShadingMap.detailAmplitude"):
      setObjectNPRInfo(INF_DETAILAMPLITUDE, field.value)
   elif field == ctx.field("SoShadingMap.spatialEffect"):
      setObjectNPRInfo(INF_SPATIALEFFECT, field.value)
   elif field == ctx.field("SoShadingMap.foregroundExtension"):
      setObjectNPRInfo(INF_FOREGROUNDEXT, field.value)
   else:
      ctx.log("You've found the forgotten parameter! (globalPropChanged): " + field.fullName())

   _cls_info.notify()
   pass


def setObjectNPRInfo(info, value):
   global _containerList
   for module in _containerList:
      object = ctx.module(module).field("objectID").value
      if _cls_info.existInfo(object, LAY_NPRSHADING, INF_NPRSETACTIVE):
         if _cls_info.getBool(object, LAY_NPRSHADING, INF_NPRSETACTIVE):
            _cls_info.typedSet(object, LAY_NPRSHADING, info, value, INFOTYPE_DOUBLE)
   pass


def renderCSOChanged(field = 0):
   global _containerList
   cso = ctx.field("METKCSO.cso").value
   csoModule = ctx.module(_cls_info.adjustObjectID(cso))

   for module in _containerList:
      if cso == ctx.module(module).field("objectID").value:
         # connecting module
         if ctx.field("renderCSO").value != "Emphasis": # Distance or Both
            ctx.module(module).field("distanceObject").disconnect()
            ctx.module(module).field("WEMInfoGenerator.updateDistance").touch()
         if ctx.field("renderCSO").value != "Distance": # Emphasis or Both
            ctx.module(module).field("emphasis").value = True
      else:
         # disconnecting module
         if ctx.field("renderCSO").value != "Emphasis": # Distance or Both
            # Connecting module to distance reference structure
            # print cso.replace(".", "_") + " " + module
            if csoModule != None:
               ctx.module(module).field("distanceObject").connectFrom(csoModule.field("outWEM"))
            else:
               ctx.module(module).field("distanceObject").disconnect()
            ctx.module(module).field("WEMInfoGenerator.updateDistance").touch()
         if ctx.field("renderCSO").value != "Distance": # Emphasis or Both
            ctx.module(module).field("emphasis").value = False
   pass
