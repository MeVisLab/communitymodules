# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None
_caseLoaded = False
_availableImages = []

def init():
   global _cls_info, _cls_iter
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_IMAGE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Source")
   pass


def handleObjectCreatedEvent():
   global _availableImages
   object   = _cls_info.activeObject()
   
   if _cls_info.get(object, LAY_GLOBAL, INF_OBJTYPE) == "Source" and _cls_info.get(object, LAY_IMAGE, INF_IMAGETYPE) == "Original":
      _availableImages.append(object)
   
      ctx.field("parentIDs").value = "[choose image]"
      for image in _availableImages:
         ctx.field("parentIDs").value += "," + image
         if _cls_info.get(image, LAY_IMAGE, INF_FILENAME) == MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["name"]:
            ctx.field("parentID").value = image
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
   global _availableImages
   
   _caseLoaded = True
   _availableImages = []
   
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _availableImages.append(_cls_iter.object())
      _cls_iter.searchNext()
   
   ctx.field("parentIDs").value = "[choose image]"
   for image in _availableImages:
      ctx.field("parentIDs").value += "," + image
   
   imageChanged()
   pass


def handleCleanupEvent():
   global _caseLoaded
   global _availableImages
   
   _caseLoaded = False
   _availableImages = []
   
   ctx.field("objectID").value = ""
   ctx.field("filename").value = ""
   ctx.field("parentIDs").value = "[choose image]"
   ctx.field("parentID").value = ""
   pass


def handleSaveEvent():
   pass
   
   
def imageChanged(field = 0):
   ctx.field("objectID").value = "ROI_" + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"]
   ctx.field("filename").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"] + ctx.field("suffix").value + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["ext"]


def create(field = 0):
   if _caseLoaded and ctx.field("image").isValid() and ctx.field("parentID").value != "[choose image]":
      objectID = "ROI_" + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"]
      ctx.field("objectID").value = objectID
      
      ctx.field("ImgSave.save").touch()
      
      # Layer Global
      _cls_info.set(objectID, LAY_GLOBAL, INF_OBJTYPE, "Result")
      _cls_info.set(objectID, LAY_GLOBAL, INF_CHILDS, "")
      _cls_info.set(objectID, LAY_GLOBAL, INF_PARENT, ctx.field("parentID").value)
      _cls_info.typedSet(objectID, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
      
      # Eintrag beim Parent vornehmen
      children = _cls_info.get(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS)
      if children == "":
         _cls_info.set(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS, objectID)
      else:
         _cls_info.set(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS, children + ";" + objectID)
      
      # Layer Image
      if ctx.field("ROIType").value == "OwnImage":
         caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
         _cls_info.set(objectID, LAY_IMAGE, INF_FILENAME, relpath(caseDir, ctx.field("ImgSave.sourceName").value))
      else:
         _cls_info.typedSet(objectID, LAY_IMAGE, INF_STARTVOXEL, ctx.field("startVoxel").stringValue(), INFOTYPE_VEC3)
         _cls_info.typedSet(objectID, LAY_IMAGE, INF_ENDVOXEL,   ctx.field("endVoxel").stringValue(),   INFOTYPE_VEC3)
         
      _cls_info.set(objectID, LAY_IMAGE, INF_IMAGETYPE, "Original")

      _cls_info.typedSet(objectID, LAY_IMAGE, INF_OBJVALUE, -1, INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_WMATRIX, ctx.field("Info.worldMatrix").stringValue(), INFOTYPE_MAT4)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEX,   ctx.field("Info.sizeX").value,       INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEY,   ctx.field("Info.sizeY").value,       INFOTYPE_INT32)
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_SIZEZ,   ctx.field("Info.sizeZ").value,       INFOTYPE_INT32)
      _cls_info.notify()
   pass