# Python module import
from ObjMgr import *
from VisDefinitions import *
from sets import Set

# Global class instances
_cls_converted = None
_cls_info      = None
_cls_iter      = None
_fileManager   = None

# Global variables
_caseDir = ""
_createModules = ","
_generateIV = False
_generateIVOnRequest = False
_caseHasBeenModified = False
_caseLoaded = False

def init():
   global _cls_converted, _cls_iter, _cls_info, _fileManager, _createModules

   # Create an ObjInfo class instance to modify the database and to handle database events
   #_cls_converted = ObjInfo(ctx.module("infoConverted"))
   _cls_converted = METKObjInfo(ctx.module("infoConverted"))
   _cls_info = METKObjInfo(ctx.module("info"))
   _cls_iter = ObjIterator(ctx.module("objCreateIterator"))
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   
   _fileManager = MLABFileManager
   _createModules = ","
   
   return


def new(field = 0):
   global _caseLoaded
   global _caseDir
   global _caseHasBeenModified
   _caseHasBeenModified = False
   
   path = MLABFileDialog.getSaveFileName("","XML files (*.xml)", "New Case File")
   if path != "" and path[-4:] != ".xml":
      path = path + ".xml"
   result = False
   if MLABFileManager.exists(path):
      # overwrite = 0, cancel = 1
      result = MLAB.showInformation("File already exists", "Warning", ["Overwrite","Cancel"],1)
   if not result and path != "":
      ctx.field("fileInfo.path").value = path
      ctx.field("fileName").value = path
      _caseDir = ctx.field("fileInfo.dirname").value
      # Layer CaseObject
      _cls_info.set(OBJ_CASE, LAY_GLOBAL, INF_OBJTYPE, "Case")
      _cls_info.set(OBJ_CASE, LAY_CASE, INF_CASEDIR, _caseDir)
      _cls_info.markInfoNonPersistent(INF_CASEDIR)
      _cls_info.set(OBJ_CASE, LAY_CASE, INF_XMLFILE, ctx.field("fileInfo.filename").value)
      _cls_info.markInfoNonPersistent(INF_XMLFILE)
      
      # Layer ApplicationObject
      _cls_info.set(OBJ_APPLICATION, LAY_GLOBAL,      INF_OBJTYPE, "Application")
      _cls_info.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONNAME,    "MedicalExplorationToolkit")
      _cls_info.set(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONVERSION, "1.0")
      _cls_info.set(OBJ_APPLICATION, LAY_APPLICATION, INF_MEVISLAB,           "1.5.1")
      
      _cls_info.notify()
      
      ctx.field("ObjMgr.eventLogging").value = True
      ctx.field("status").value = "New case openend :o)"
      _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_LOADED,INFOTYPE_MESSAGE)
      _cls_info.markObjectNonPersistent(OBJ_COMMUNICATION)
      _caseLoaded = True
      ctx.field("caseLoaded").value = True
      _cls_info.notify()
   pass


def browse(field = 0):
   ctx.field("fileName").value = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File")
   load()
   pass


def load(againstTError = 0):
   global _caseDir
   global _caseHasBeenModified
   if MLABFileManager.exists(ctx.field("fileName").value) and _cls_info.get(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED) != MSG_LOADED:
      _caseHasBeenModified = False
      ctx.field("ObjLoader.fileName").value = ctx.field("fileName").value
      ctx.field("ObjLoader.employLoaderName").value = ""
      ctx.field("status").value = "Converting XML data..."
      
      ctx.field("ObjLoader.load").touch()
      if not ctx.field("ObjLoader.operationSuccessful").value:
         ctx.field("ObjLoader.employLoaderName").value = "olLoaderHepaVision1"
         ctx.field("ObjLoader.load").touch()
      
      global _generateIV
      _generateIV = False
      
      ctx.log("Converting XML data...")
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Loading case...", INFOTYPE_MESSAGE)
      
      _caseDir = ctx.field("fileInfo.dirname").value
      _cls_info.set(OBJ_CASE, LAY_CASE, INF_CASEDIR, _caseDir)
      _cls_info.markInfoNonPersistent(INF_CASEDIR)
      _cls_info.set(OBJ_CASE, LAY_CASE, INF_XMLFILE, ctx.field("fileInfo.filename").value)
      _cls_info.markInfoNonPersistent(INF_XMLFILE)
      _cls_info.notify()
      MLAB.setWaitCursor()
      ctx.field("ConvertXML.convertFile").touch()
   else:
      ctx.field("status").value = "Please close the current case first or select an existent file."
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Please close the current case first.", INFOTYPE_MESSAGE)
      _cls_info.notify()
   return


def convertSuccessful(field):   
   # es muss noch gehandlet werden und es gibt erstmal nichts zu tun   
   if ctx.field("ConvertXML.successful").value == True:
      ctx.field("status").value = "Checking file integrity..."
      ctx.field("ObjMgr.eventLogging").value = False
      _cls_converted.handleEvents()
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleObjectCreatedEvent():
   pass

def handleObjectRemovedEvent():
   pass

def handleAttributeCreatedEvent():
   global _generateIVOnRequest
   
   _generateIVOnRequest = False
   pipeThrough = True
   
   object = _cls_converted.activeObject()
   layer  = _cls_converted.activeLayer()
   info   = _cls_converted.activeInfo()
   value  = _cls_converted.get()
   valueType = _cls_converted.getType(object,layer,info)
   if layer == LAY_GLOBAL:
      # Doppelte ChildIDs/ParentIDs löschen und auf Existenz überprüfen
      if info == INF_CHILDS:
         pipeThrough = False
         childIDs = value.split(";")
         uniqueChildIDs = Set()
         for child in childIDs:
            if child != "":
               if child in uniqueChildIDs:
                  MLAB.logWarning("Multiple defined ChildID removed: " + child)
               else:
                  uniqueChildIDs.add(child)
         
         checkedChildIDs = ""
         for uniqueChild in uniqueChildIDs:
            if not _cls_converted.existObject(uniqueChild):
               MLAB.logWarning("ChildID references to non-existent object entry: " + uniqueChild)
            if checkedChildIDs == "":
               checkedChildIDs = uniqueChild
            else:
               checkedChildIDs = checkedChildIDs + ";" + uniqueChild
         _cls_info.set(object, layer, info, checkedChildIDs)
      elif info == INF_PARENT:
         if not _cls_converted.existObject(value):
            MLAB.logWarning("ParentID references to non-existent object entry: " + value)
   
   elif layer == LAY_FILES:
      if info == INF_IVFILE:
         # IV Datei bereits vorhanden?
         #fileNameIV = _caseDir + value
         fileNameIV = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + value
         
         if (not _fileManager.exists(_fileManager.normalizePath(fileNameIV)) and (ctx.field("computeSurfaces").value==True)):
            
            # wenn nicht, Image überhaupt vorhanden? Sonst wars das...
            #objValue = _cls_converted.getImageInfo(object, INF_OBJVALUE, INFOTYPE_INT32)
            #^^ bug objectvalue is of string type
            objValue = _cls_converted.getImageInfo(object, INF_OBJVALUE, INFOTYPE_STRING)
            
            # Es kann ein ObjValue von -1 zurückgegeben werden, weil getImageInfo zu den Parents aufsteigt
            # und dort vielleicht so einen Wert findet, der natürlich nichts mit dem eigentlichen Objekt mehr zu tun hat
            if (objValue):               
               if (int(objValue)<0): #das mit dem int-casten ist nicht ganz sauber, weil es ja auch sonstwas sein könnte
                  objValue = None
            
            fileNameIMAGE = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _cls_converted.getImageInfo(object, INF_FILENAME) #hier ist es besser, den filename aus converted zu holen, da er im info noch nicht zwingend da sein kann (bei HEPA-Fällen aufgetreten)
            
            if not _fileManager.exists(_fileManager.normalizePath(fileNameIMAGE)):
               MLAB.logWarning("Unable to create " + info + " for " + object + ", because Image " + fileNameIMAGE + " does not exist")
            else:
               ctx.field("GenerateIVFile.filenameOutput").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + value
               _cls_info.set(object,layer,info,value)
               if (objValue):
                  Debug(ctx,"NO AUTO!  objValue="+str(objValue))
                  ctx.field("GenerateIVFile.imageValue").setStringValue(objValue)
                  ctx.field("GenerateIVFile.imageValueAuto").setBoolValue(False)
               else:
                  ctx.field("GenerateIVFile.imageValueAuto").setBoolValue(True)
               
               # der filename muss als letztes zugewiesen werden, weil dessen Änderung ein addFile in die Queue auslöst.
               ctx.field("GenerateIVFile.filename").value = fileNameIMAGE
               
               global _generateIV
               _generateIV = True
               pipeThrough = False
   
   elif layer == LAY_APPEARANCE:
      if not _cls_converted.existInfo(object, LAY_APPEARANCE, INF_COLOR):
         _cls_info.typedSet(object, layer, INF_COLOR, "0.8 0.8 0.8", INFOTYPE_VEC3)
      if not _cls_converted.existInfo(object, LAY_APPEARANCE, INF_TRANSPARENCY):
         _cls_info.typedSet(object, layer, INF_TRANSPARENCY, "0.0", INFOTYPE_DOUBLE)
      if not _cls_converted.existInfo(object, LAY_APPEARANCE, INF_VISIBLE):
         _cls_info.typedSet(object, layer, INF_VISIBLE, False, INFOTYPE_BOOL)
      if not _cls_converted.existInfo(object, LAY_APPEARANCE, INF_FLATISO):
         _cls_info.typedSet(object, layer, INF_FLATISO, False, INFOTYPE_BOOL)
      else: #rewrite string-type to bool-type
         if _cls_converted.get(object, LAY_APPEARANCE, INF_VISIBLE) == "True":
            _cls_info.typedSet(object, layer, INF_VISIBLE, True, INFOTYPE_BOOL)
         else:
            _cls_info.typedSet(object, layer, INF_VISIBLE, False, INFOTYPE_BOOL)
   
   elif layer == LAY_IMAGE and info == INF_FILENAME:
      imageType = _cls_converted.getImageInfo(object, INF_IMAGETYPE)
      if imageType == "Segmentation":
         if not _cls_converted.existInfo(object, LAY_IMAGE, INF_OBJVALUE):
            ctx.field("ImgLoad.filename").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + value
            _cls_info.typedSet(object, LAY_IMAGE, INF_OBJVALUE, int(ctx.field("ImgInfo.maxValue").value), INFOTYPE_INT32)
      elif imageType == "Original":
         ctx.field("ImgLoad.filename").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + value
         _cls_info.typedSet(object, LAY_IMAGE, INF_OBJVALUE, -1, INFOTYPE_INT32)
         _cls_info.typedSet(object, LAY_IMAGE, INF_WMATRIX,    ctx.field("ImgInfo.worldMatrix").value,INFOTYPE_MAT4)
         _cls_info.typedSet(object, LAY_IMAGE, INF_VOXELSIZEX, ctx.field("ImgInfo.voxelSizeX").value, INFOTYPE_DOUBLE)
         _cls_info.typedSet(object, LAY_IMAGE, INF_VOXELSIZEY, ctx.field("ImgInfo.voxelSizeY").value, INFOTYPE_DOUBLE)
         _cls_info.typedSet(object, LAY_IMAGE, INF_VOXELSIZEZ, ctx.field("ImgInfo.voxelSizeZ").value, INFOTYPE_DOUBLE)
         _cls_info.typedSet(object, LAY_IMAGE, INF_SIZEX,      ctx.field("ImgInfo.sizeX").value,      INFOTYPE_INT32)
         _cls_info.typedSet(object, LAY_IMAGE, INF_SIZEY,      ctx.field("ImgInfo.sizeY").value,      INFOTYPE_INT32)
         _cls_info.typedSet(object, LAY_IMAGE, INF_SIZEZ,      ctx.field("ImgInfo.sizeZ").value,      INFOTYPE_INT32)
   
   elif layer == LAY_GLOBALEVENTS and info == INF_CASELOADED and value == MSG_CONVERTED:
      pipeThrough = False
      ctx.log("Converted")
      if _generateIV:
         ctx.field("ObjMgr.eventLogging").value = True
         ctx.field("status").value = "Generating Inventor files..."
         ctx.field("GenerateIVFile.queueing").value = True
         ctx.field("GenerateIVFile.start").touch()
      else:
         finalize()
   
   if pipeThrough:
      _cls_info.typedSet(object, layer, info, value, valueType)
   
   return

def handleAttributeRemovedEvent():
   pass

def handleAttributeModifiedEvent():
   pass



def ivFilesGenerated(field = 0):
   global _generateIVOnRequest
   if ctx.field("GenerateIVFile.ready").value == True:
      if _generateIVOnRequest:
         if (not _cls_info.existInfo(ctx.field("createIVFile.data").value, LAY_FILES, INF_IVFILE)):
            _cls_info.set(ctx.field("createIVFile.data").value, LAY_FILES, INF_IVFILE, ctx.field("createIVFile.data").value + ".iv")
            _cls_info.notify()
         ctx.field("createIVFile.done").touch()
      else:
         Debug(ctx,"GenerateIVFile.ready")
         ctx.field("ObjMgr.eventLogging").value = False # War nur an um Fortschritt des GenerateIVFIle zu protokollieren über den ObjMgr
         finalize()
   return


def finalize():
   global _caseLoaded
   createObjectsByVisible()
   _caseLoaded = True
   ctx.field("ObjMgr.eventLogging").value = True
   
   ctx.field("status").value = "Loaded :o)"
   ctx.log("loaded :o)")
   _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "XML file loaded.", INFOTYPE_MESSAGE)
   _cls_info.notify()
   
   ctx.field("LoadRequest.done").touch()
   _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_LOADED,INFOTYPE_MESSAGE)
   _cls_info.markObjectNonPersistent(OBJ_COMMUNICATION)
   _cls_info.notify()
   MLAB.removeWaitCursor()
   ctx.field("caseLoaded").value = True
   _cls_info.notify()
   return


def detectedFormat(args):
   if ctx.field("ConvertXML.detectedFormat").value == "could not convert":
      ctx.field("status").value = "Unknown file format."
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Unable to load case...", INFOTYPE_MESSAGE)
      _cls_info.notify()
      MLAB.removeWaitCursor()
   return



def cleanup(field):
   global _caseDir
   global _createModules
   global _caseLoaded
   global _caseHasBeenModified, _caseLoaded

   if _cls_info.existInfo(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED) and _cls_info.get(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED) != MSG_CLEANUP:

      # Ask the user whether (s)he wants to save the changes
      answer = 1 # Discard changes
      #print "_caseHasBeenModified " + str(_caseHasBeenModified)
      
      if ctx.field("autoSave").value:
         save()
      
      if _caseHasBeenModified:
         answer = MLAB.showWarning("Do you want to save the changes in this case?", _cls_info.get(OBJ_APPLICATION, LAY_APPLICATION, INF_APPLICATIONNAME), ["Save As...","Discard","Cancel"],2)
         if answer == 0:
            saveAs()
      
      # If Cleanup was not cancelled...
      if answer != 2:
         _caseHasBeenModified = False
         _caseLoaded = False
         ctx.field("changeDetected").value = False
         
         #Clear the cache to get some space
         MLAB.clearMLCache()
         
         # Telling the other modules to clean up
         _cls_info.typedSet(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED, MSG_CLEANUP, INFOTYPE_MESSAGE)
         _cls_info.notify()
         
         _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Cleaning up METKManager...", INFOTYPE_MESSAGE)
         _cls_info.notify()
         _caseDir = ""
         
         # hier müssen noch alle METKInventorObject-Module gelöscht werden
         ctx.log("Clearing METKManager himself...")
         list = ctx.modules()
         moduleCounter = 0.0
         for module in list:
            moduleCounter = moduleCounter + 1.0
            _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_PROGRESS, (moduleCounter / float(len(list))), INFOTYPE_DOUBLE)
            _cls_info.notify()
            if ctx.module(module).type() == "METKInventorObject":
               ctx.module(module).remove()
         _createModules = ","
         ctx.log("Container are removed.")
         
         MLAB.processEvents()
         ctx.field("ObjMgr.clearObjectContainer").touch()
         ctx.field("source.clearObjectContainer").touch()
         ctx.field("converted.clearObjectContainer").touch()
         
         ctx.log("METKManager is cleared.")
         
         ctx.field("status").value = "Case closed."
         _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Case closed.", INFOTYPE_MESSAGE)
         _cls_info.notify()
         _caseLoaded = False
         ctx.field("caseLoaded").value = False
         ctx.field("ConvertXML.detectedFormat").value = ""
   return



def gotMeasureRequest(fieldP=0):
   objectID = ctx.field("MeasureRequest.data").value
   if (not createObject(objectID)):
      ctx.field("MeasureRequest.done").touch()
      ctx.log("Object ("+objectID+") existiert nicht im ObjMgr oder wurde schon erstellt!")
   return


def measuresValid(value):   
   if (value==ctx.field("MeasureRequest.data").value):
      ctx.field("MeasureRequest.done").touch()
   return


def gotLoadRequest(fieldP=0):
   ctx.field("fileName").value = ctx.field("LoadRequest.data").value
   Debug(ctx,"gotLoadRequest  filename="+ctx.field("fileName").value)
   ctx.field("load").touch()   
   return


def infoChanged(fieldP=0):
   global _caseHasBeenModified
   ctx.field("info.firstEvent").touch()
   while (ctx.field("info.operationSuccessful").value == True):
      object = _cls_info.activeObject()
      layer  = _cls_info.activeLayer()
      info   = _cls_info.activeInfo()
      value  = _cls_info.get()
      
      # look for events for creating new IV files
      if layer == LAY_APPEARANCE:
         if info == INF_VISIBLE:
            if value == "TRUE":
               createObject(object)
      
      # look for events which should suggest the user to save the XML file
      eventType = ctx.field("info.eventType").value
      if eventType == "ObjectCreated" or eventType == "ObjectRemoved":
         level = 1
      elif eventType == "AttributeCreated" or eventType == "AttributeRemoved":
         level = 2
      elif layer != LAY_FILES or info != INF_IVFILE:
         # do not care for non-persistent entries...
         level = 3
      else: #default level value added by konrad to prevent an error in the warnLevel>=level question
         level = 4
      
      if not _caseHasBeenModified and _caseLoaded and object != OBJ_COMMUNICATION:
         if ctx.field("warnLevel").value >= level:
            _caseHasBeenModified = True
            ctx.field("changeDetected").value = True
      
      ctx.field("info.nextEvent").touch()
   ctx.field("info.clearEventQueue").touch()
   return


def createObject(objectID):
   global _createModules
   if (_cls_info.existObject(objectID) and _createModules.find(","+objectID+",")==-1):
      inventorObject = ctx.addModule("METKInventorObject")
      inventorObject.field("instanceName").setStringValue(_cls_info.adjustObjectID(objectID))
      inventorObject.field("objectID").setStringValue(objectID)
      inventorObject.field("inObjectContainer").connectFrom(ctx.field("info.outObjectContainer"))
      inventorObject.field("initialize").touch();
      _createModules = _createModules+objectID+","
      
      count = len(_createModules.split(","))
      inventorObject.setFrameTopLeft(200*(count%3)-300,75*round((count/3.0)-0.5,0)+100)

      return True
   return False


def createObjectsByVisible():
   Debug(ctx,"load existing visible objects")
   _cls_info.typedSet(OBJ_COMMUNICATION, LAY_STATUS, INF_STATUSMSG, "Load existing visible objects...", INFOTYPE_MESSAGE)
   _cls_info.notify()

   _cls_iter.searchFirst()
   while _cls_iter.success():
      if (_cls_info.get(_cls_iter.object(), LAY_APPEARANCE, INF_VISIBLE)=="TRUE"):
         createObject(_cls_iter.object())
      _cls_iter.searchNext()
   return


def createIVFile(field):
   #global _caseDir
   global _generateIVOnRequest
   field = ctx.field("createIVFile.data")
   if _cls_info.existObject(field.value):
      _generateIVOnRequest = True
      if _cls_info.existInfo(field.value, LAY_FILES, INF_IVFILE):
         fileNameIV = _cls_info.get(field.value, LAY_FILES, INF_IVFILE)
      else:
         fileNameIV = field.value + ".iv"
      
      # wenn nicht, Image überhaupt vorhanden? Sonst wars das...
      #fileNameIMAGE = _caseDir + _cls_converted.getImageInfo(field.value, INF_FILENAME)
      fileNameIMAGE = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + "/" + _cls_info.getImageInfo(field.value, INF_FILENAME)      
      if not _fileManager.exists(_fileManager.normalizePath(fileNameIMAGE)):
         MLAB.logWarning("Unable to create " + INF_FILENAME + " for " + field.value + ", because Image " + fileNameIMAGE + " does not exist.")
      else:
         ctx.field("GenerateIVFile.queueing").value = False
         
         #objValue = _cls_converted.getImageInfo(field.value, INF_OBJVALUE, INFOTYPE_STRING)
         objValue = _cls_info.getImageInfo(field.value, INF_OBJVALUE, INFOTYPE_STRING)
   # Es kann ein ObjValue von -1 zurückgegeben werden, weil getImageInfo zu den Parents aufsteigt
   # und dort vielleicht so einen Wert findet, der natürlich nichts mit dem eigentlichen Objekt mehr zu tun hat
   if (objValue):
      if (int(objValue)<0): #das mit dem int-casten ist nicht ganz sauber, weil es ja auch sonstwas sein könnte
         objValue = None
         
         if (objValue):
            ctx.field("GenerateIVFile.imageValue").setStringValue(objValue)
            ctx.field("GenerateIVFile.imageValueAuto").setBoolValue(False)
         else:
            ctx.field("GenerateIVFile.imageValueAuto").setBoolValue(True)
         
         ctx.field("GenerateIVFile.filenameOutput").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + "/" + fileNameIV
         # der filename muss als letztes zugewiesen werden, weil dessen Änderung ein addFile in die Queue auslöst.
         ctx.field("GenerateIVFile.filename").value = fileNameIMAGE
         
         # der filename muss als letztes zugewiesen werden, weil dessen Änderung ein addFile in die Queue auslöst.
         ctx.field("GenerateIVFile.start").touch()
   else:
      MLAB.logWarning("Unable to create IV File for " + field.value + ", because Object ID " + field.value + " does not exist.")
      ctx.field("createIVFile.done").touch()
   pass



def save(field = 0):
   global _caseHasBeenModified
   _caseHasBeenModified = False
   _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_SAVE,INFOTYPE_MESSAGE)
   _cls_info.notify()
   ctx.field("ObjDump.fileName").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE)
   ctx.field("ObjDump.save").touch()
   pass


def saveAs(field = 0):
   global _caseHasBeenModified
   global _caseDir
   ctx.field("saveAsFilename").value = _cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE)
   result = ctx.showModalDialog("SaveAs", "Save Case File")
   if result:
      caseFile = ctx.field("saveAsFilename").value
      if caseFile != "":
         if caseFile[-4:] != ".xml":
            caseFile = caseFile + ".xml"
         _cls_info.typedSet(OBJ_COMMUNICATION,LAY_GLOBALEVENTS,INF_CASELOADED,MSG_SAVE,INFOTYPE_MESSAGE)
         _cls_info.set(OBJ_CASE, LAY_CASE, INF_XMLFILE, caseFile)
         _cls_info.notify()
         ctx.field("ObjDump.fileName").value = _caseDir + caseFile
         ctx.field("ObjDump.save").touch()
         ctx.field("fileName").value = _caseDir + caseFile
         _caseHasBeenModified = False
         #ctx.field("fileName").value = ctx.field("saveAsFilename").value
   pass


def modalDialogSave(field = 0):
   if ctx.field("saveAsFilename").value[-4:] != ".xml":
      ctx.field("saveAsFilename").value = ctx.field("saveAsFilename").value + ".xml"
   if MLABFileManager.exists(_caseDir + ctx.field("saveAsFilename").value):
      result = MLAB.showInformation("File already exists", "Warning", ["Overwrite","Cancel"],1)
      if not result:
         ctx.window().done(1)
   else:
      ctx.window().done(1)
   pass

def modalDialogCancel(field = 0):
   ctx.window().done(0)
   pass
