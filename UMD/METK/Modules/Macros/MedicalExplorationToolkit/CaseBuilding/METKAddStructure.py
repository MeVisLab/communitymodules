# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None
_caseLoaded = False
_availableROIs = []

def init():
   global _cls_info, _cls_iter
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_IMAGE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Result")
   pass


def handleObjectCreatedEvent():
   global _availableROIs
   object   = _cls_info.activeObject()
   
   if _cls_info.get(object, LAY_GLOBAL, INF_OBJTYPE) == "Result" and _cls_info.get(object, LAY_IMAGE, INF_IMAGETYPE) == "Original":
      _availableROIs.append(object)
   
      ctx.field("parentIDs").value = "[choose ROI]"
      for image in _availableROIs:
         ctx.field("parentIDs").value += "," + image
         if _cls_info.getImageInfo(image, INF_FILENAME) == MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["name"]:
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
   ctx.log("handleLoadedEvent")
   global _caseLoaded
   global _availableROIs
   
   _caseLoaded = True
   _availableROIs = []
   
   _cls_iter.searchFirst()
   while _cls_iter.success():
      if _cls_info.get(_cls_iter.object(), LAY_IMAGE, INF_IMAGETYPE) == "Original":
         _availableROIs.append(_cls_iter.object())
      _cls_iter.searchNext()
   
   ctx.field("parentIDs").value = "[choose ROI]"
   for image in _availableROIs:
      ctx.field("parentIDs").value += "," + image
   
   imageChanged()
   pass


def handleCleanupEvent():
   global _caseLoaded, _availableImages
   global _availableImages
   
   _caseLoaded = False
   _availableImages = []
   
   ctx.field("objectID").value = ""
   ctx.field("parentIDs").value = "[choose image]"
   ctx.field("parentID").value = ""

   ctx.field("name").value = ""
   ctx.field("structure").value = ""
   ctx.field("strGroup").value = ""
   pass


def handleSaveEvent():
   pass
   
   
def imageChanged(field = 0):
   ctx.field("objectID").value = "Structure_" + ctx.field("name").value.replace(" ", "")
   if ctx.field("createImageFile").value:
      ctx.field("filename").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["base"] + "." + ctx.field("name").value.replace(" ", "") + MLABFileManager.splitPath(ctx.field("ImgSave.sourceName").value)["ext"]
   else:
      ctx.field("filename").value = ctx.field("ImgSave.sourceName").value


def create(field = 0):
   if _caseLoaded and ctx.field("image").isValid() and ctx.field("parentID").value != "[choose ROI]" and ctx.field("objectID").value != "":
      objectID = "Structure_" + ctx.field("name").value.replace(" ", "").replace("-", "_")
      
      if ctx.field("createImageFile").value:
         ctx.field("ImgSave.save").touch()
      
      # Layer Global
      _cls_info.set(objectID, LAY_GLOBAL, INF_OBJTYPE, "Result")
      _cls_info.set(objectID, LAY_GLOBAL, INF_PARENT, ctx.field("parentID").value)
      _cls_info.typedSet(objectID, LAY_GLOBAL, INF_VALID, True, INFOTYPE_BOOL)
      
      # Eintrag beim Parent vornehmen
      children = _cls_info.get(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS)
      if children == "":
         _cls_info.set(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS, objectID)
      else:
         _cls_info.set(ctx.field("parentID").value, LAY_GLOBAL, INF_CHILDS, children + ";" + objectID)
      
      # Layer Image
      caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      _cls_info.set(objectID, LAY_IMAGE, INF_FILENAME, relpath(caseDir, ctx.field("filename").value))
      _cls_info.set(objectID, LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
      _cls_info.typedSet(objectID, LAY_IMAGE, INF_OBJVALUE, ctx.field("Info.maxValue").value, INFOTYPE_INT32)
      
      # Layer Description
      _cls_info.set(objectID, LAY_DESCRIPTION, INF_NAME,           ctx.field("name").value)
      _cls_info.set(objectID, LAY_DESCRIPTION, INF_STRUCTURE,      ctx.field("structure").value)
      _cls_info.set(objectID, LAY_DESCRIPTION, INF_STRUCTUREGROUP, ctx.field("strGroup").value)
      
      _cls_info.notify()
      
      if ctx.field("createIVFile").value == True:
         ctx.field("ready").value = False
         ctx.field("METKMsgSender.data").value = objectID
         ctx.field("METKMsgSender.send").touch()
   pass


def ivfileCreated(field = 0):
   if field.value == True and ctx.field("METKMsgSender.successful").value == True:
      objectID = ctx.field("METKMsgSender.data").value
      # Layer Files
      _cls_info.set(objectID, LAY_FILES, INF_IVFILE, ctx.field("METKMsgSender.data").value + ".iv")
      
      # Layer Appearance
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_COLOR,        ctx.field("color").stringValue(), INFOTYPE_VEC3)
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TRANSPARENCY, ctx.field("transparency").value,  INFOTYPE_DOUBLE)
      _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_VISIBLE,      ctx.field("visible").value,       INFOTYPE_BOOL)
      _cls_info.notify()

      ctx.field("ready").value = True
   pass
   

def loadSetting(field = 0):
   if ctx.field("settingsPath").value != "" and ctx.field("structure").value != "":
      ctx.field("SettingsManager.filename").value = ctx.field("settingsPath").value + "/"  + ctx.field("structure").value + ".txt"
   pass