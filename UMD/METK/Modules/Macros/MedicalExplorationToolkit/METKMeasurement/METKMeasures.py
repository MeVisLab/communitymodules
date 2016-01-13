# Python module import
from ObjMgr import *
from VisDefinitions import *


# Global class instances
_cls_info = None
_cls_iter = None

def init():
   global _cls_info
   global _cls_iter

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   pass

def start(field = 0):
   ctx.log("Starting exact size computation: " + ctx.field("structure").value)
   ctx.field("done").value = False
   if ctx.field("descriptionTag").value == "Structure":
      _cls_iter.setSearchCriteria(LAY_DESCRIPTION, INF_STRUCTURE, ctx.field("structure").value)
   if ctx.field("descriptionTag").value == "StructureGroup":
      _cls_iter.setSearchCriteria(LAY_DESCRIPTION, INF_STRUCTUREGROUP, ctx.field("structure").value)
   if ctx.field("descriptionTag").value == "Name":
      _cls_iter.setSearchCriteria(LAY_DESCRIPTION, INF_NAME, ctx.field("structure").value)
   _cls_iter.searchFirst()
   compute()


def compute():
   if _cls_iter.success():
      if not _cls_info.existInfo(_cls_iter.object(), LAY_MEASURES, INF_EXACT) or not _cls_info.getBool(_cls_iter.object(), LAY_MEASURES, INF_EXACT):
         
         objValue = _cls_info.getImageInfo(_cls_iter.object(), INF_OBJVALUE, INFOTYPE_INT32)
         imageName = _cls_info.getImageInfo(_cls_iter.object(), INF_FILENAME)
         
         ctx.field("ImgLoad.filename").setStringValue(_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + imageName)
         
         ctx.field("BoundingBox.objectMin").value = objValue
         ctx.field("BoundingBox.objectMax").value = objValue
         ctx.field("UMDSoThresholdToPointSet.minValue").value = objValue - 1
         ctx.field("UMDSoThresholdToPointSet.maxValue").value = objValue + 1
         ctx.field("BoundingBox.update").touch()
         ctx.field("boundary.update").touch()
         if ctx.field("BoundingBox.objectVoxels").value == 0:
            MLAB.logWarning("Structure " + _cls_iter.object() + " has no valid mask!")
      else:
         _cls_iter.searchNext()
         MLAB.processEvents()
         compute()
         
   else:
      _cls_info.notify()
      ctx.field("ImgLoad.close").touch()
      ctx.log("Exact size computation finished.")
      ctx.field("done").value = True
   pass
   

def saveResults(field = 0):
   if field.value and _cls_iter.object() != "":
      ctx.log("Registering exact values for " + _cls_iter.object())
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_EXACT, True,  INFOTYPE_BOOL)
      
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_BARYCENTER, str(ctx.field("SoMainAxis.baryCenter").value[0]) + " " + str(ctx.field("SoMainAxis.baryCenter").value[1]) + " " + str(ctx.field("SoMainAxis.baryCenter").value[2]), INFOTYPE_VEC3)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_MIDPOINT,   str(ctx.field("SoMainAxis.midPoint").value[0])   + " " + str(ctx.field("SoMainAxis.midPoint").value[1])   + " " + str(ctx.field("SoMainAxis.midPoint").value[2]),   INFOTYPE_VEC3)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_X_AXIS,     str(ctx.field("SoMainAxis.xAxis").value[0])      + " " + str(ctx.field("SoMainAxis.xAxis").value[1])      + " " + str(ctx.field("SoMainAxis.xAxis").value[2]),      INFOTYPE_VEC3)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_Y_AXIS,     str(ctx.field("SoMainAxis.yAxis").value[0])      + " " + str(ctx.field("SoMainAxis.yAxis").value[1])      + " " + str(ctx.field("SoMainAxis.yAxis").value[2]),      INFOTYPE_VEC3)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_Z_AXIS,     str(ctx.field("SoMainAxis.zAxis").value[0])      + " " + str(ctx.field("SoMainAxis.zAxis").value[1])      + " " + str(ctx.field("SoMainAxis.zAxis").value[2]),      INFOTYPE_VEC3)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_X_DIAMETER, ctx.field("SoMainAxis.xDiameter").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_Y_DIAMETER, ctx.field("SoMainAxis.yDiameter").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_Z_DIAMETER, ctx.field("SoMainAxis.zDiameter").value,  INFOTYPE_DOUBLE)

      xd = ctx.field("SoMainAxis.xDiameter").value
      yd = ctx.field("SoMainAxis.yDiameter").value
      zd = ctx.field("SoMainAxis.zDiameter").value
      
      if xd > yd and xd > zd:
         maxd = xd
      elif yd > xd and yd > zd:
         maxd = yd
      else:
         maxd = zd

      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_MAX_DIAMETER, maxd,  INFOTYPE_DOUBLE)

      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_VOLUMEDIGITS, 0,    INFOTYPE_INT32)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_VOLUME,       ctx.field("BoundingBox.objectVolume").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(_cls_iter.object(), LAY_MEASURES, INF_BOUNDVOL,     ctx.field("boundary.objectVolume").value,  INFOTYPE_DOUBLE)
      _cls_info.set(     _cls_iter.object(), LAY_MEASURES, INF_VOLUMEUNIT, "ml")
      
      _cls_iter.searchNext()
      compute()
   pass


def handleLoadedEvent():
   pass

   
def handleSaveEvent():
   pass


def handleCleanupEvent():
   ctx.field("ImgLoad.close").touch()
   pass