# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Global class instances
_cls_info = None
_cls_iter = None
_containerList = []

def init():
   global _cls_info
   global _cls_iter

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_FILES, "Convert", "TRUE")
   return


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
   pass


def handleLoadedEvent():
   pass

   
def handleSaveEvent():
   pass
   
   
def handleCleanupEvent():
   pass


def save(field = 0):
   path = ctx.field("path").value
   if path == "":
      path = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      ctx.field("path").value = path
   
   _cls_iter.searchFirst()
   while _cls_iter.success():
      ctx.field("UMDPointerToInventor.objectID").value = _cls_iter.object()
      print _cls_iter.object()
      
      ctx.field("SoSceneWriter.filename").value = MLABFileManager.cleanupPath(path + "/" + _cls_info.get(_cls_iter.object(), LAY_FILES, INF_IVFILE)[:-3])
      ctx.field("WEMSave.filename").value =       MLABFileManager.cleanupPath(path + "/" + _cls_info.get(_cls_iter.object(), LAY_FILES, INF_IVFILE)[:-3])
      print ctx.field("SoSceneWriter.filename").value
      
      if ctx.field("mode").value == "Inventor ASCII":
         ctx.field("SoSceneWriter.format").value = "INVENTOR_ASCII"
         ctx.field("SoSceneWriter.save").touch()
      elif ctx.field("mode").value == "Inventor Binary":
         ctx.field("SoSceneWriter.format").value = "INVENTOR_BINARY"
         ctx.field("SoSceneWriter.save").touch()
      elif ctx.field("mode").value == "STL Binary":
         ctx.field("SoSceneWriter.format").value = "STL_BINARY"
         ctx.field("SoSceneWriter.save").touch()
      elif ctx.field("mode").value == "VRML 97":
         ctx.field("SoSceneWriter.format").value = "VRML_97"
         ctx.field("SoSceneWriter.save").touch()
         
      elif ctx.field("mode").value == "WEM":
         ctx.field("SoWEMConvertInventor.apply").touch()
         ctx.field("WEMSave.fileType").value = "Winged Edge Mesh File Format (.wem)"
         ctx.field("WEMSave.save").touch()
      elif ctx.field("mode").value == "Wavefront":
         ctx.field("SoWEMConvertInventor.apply").touch()
         ctx.field("WEMSave.fileType").value = "Wavefront (.obj)"
         ctx.field("WEMSave.save").touch()
      elif ctx.field("mode").value == "Object File Format":
         ctx.field("SoWEMConvertInventor.apply").touch()
         ctx.field("WEMSave.fileType").value = "Object File Format (.off)"
         ctx.field("WEMSave.save").touch()
      elif ctx.field("mode").value == "Geom View":
         ctx.field("SoWEMConvertInventor.apply").touch()
         ctx.field("WEMSave.fileType").value = "Geomview File Format (.geom)"
         ctx.field("WEMSave.save").touch()
      elif ctx.field("mode").value == "Stanford Polygon":
         ctx.field("SoWEMConvertInventor.apply").touch()
         ctx.field("WEMSave.fileType").value = "ASCII Stanford Polygon File Format (.ply)"
         ctx.field("WEMSave.save").touch()
      elif ctx.field("mode").value == "STL ASCII":
         ctx.field("WEMSave.fileType").value = "ASCII Standard Tessellation Language (.stl)"
         ctx.field("WEMSave.save").touch()

      MLAB.processInventorQueue()
      MLAB.processEvents()
      
      _cls_iter.searchNext()
   pass   