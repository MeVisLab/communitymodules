# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None
_caseLoaded = False

def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   pass


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
   pass


def handleLoadedEvent():
   global _caseLoaded
   _caseLoaded = True
   pass


def handleCleanupEvent():
   global _caseLoaded
   _caseLoaded = False
   ctx.field("objectID").value = ""
   pass


def handleSaveEvent():
   pass
   
   
def imageChanged(field = 0):
   ctx.field("objectID").value = "Image_" + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"]


def create(field = 0):
   if _caseLoaded and ctx.field("image").isValid():
      objectID = "Image_" + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"]
      ctx.field("objectID").value = objectID
      caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      
      # Layer Global
      _cls_info.set(objectID, LAY_GLOBAL, INF_OBJTYPE, "Source")
      _cls_info.set(objectID, LAY_GLOBAL, INF_CHILDS, "")
      _cls_info.typedSet(objectID, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
      
      # Layer Image
      _cls_info.set(objectID, LAY_IMAGE, INF_FILENAME, relpath(caseDir, ctx.field("ImgSave.sourceName").value))
      _cls_info.set(objectID, LAY_IMAGE, INF_IMAGETYPE, "Original")
      
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_OBJVALUE, -1, INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_WMATRIX, ctx.field("Info.worldMatrix").stringValue(), INFOTYPE_MAT4)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_VOXELSIZEX,   ctx.field("Info.voxelSizeX").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_VOXELSIZEY,   ctx.field("Info.voxelSizeY").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_VOXELSIZEZ,   ctx.field("Info.voxelSizeZ").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEX,   ctx.field("Info.sizeX").value,       INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEY,   ctx.field("Info.sizeY").value,       INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEZ,   ctx.field("Info.sizeZ").value,       INFOTYPE_INT32)

      _cls_info.set(objectID, LAY_IMAGE, INF_MODALITY,     ctx.field("DicomTagViewer.tagValue0").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_PROTOCOL,     ctx.field("DicomTagViewer.tagValue1").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_STUDYDATE,    ctx.field("DicomTagViewer.tagValue2").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_STUDYTIME,    ctx.field("DicomTagViewer.tagValue3").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_INSTITUTION,  ctx.field("DicomTagViewer.tagValue4").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_MANUFACTURER, ctx.field("DicomTagViewer.tagValue5").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_MMODELNAME,   ctx.field("DicomTagViewer.tagValue6").value)
      _cls_info.set(objectID, LAY_IMAGE, INF_BPEX,         ctx.field("DicomTagViewer.tagValue7").value)
      
      _cls_info.notify()
   pass