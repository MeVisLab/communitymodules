# Python module import
from VisDefinitions import *

LOAD = 1
SAVE = 2
REMOVE = 3
CANCEL = 0

# Frequently used controls
_objectInclude = ctx.field("objectTypeInclude")
_layerExclude = ctx.field("layerExclude")
_infoExclude = ctx.field("infoExclude")

# Global class instances
_cls_info = None
_cls_iter = None
_cls_exchange = None
_listView = None

# Global variables
_countViewer = 0
_waitForExchange = False
_waitForScriptBuilding = False
_renderVideo = False
_preConfiguredCollection = False
_saveCollection = False
_activeCollection = ""
_caseDir = ""
_unsavedCollections = []

def init():
   global _cls_info
   global _cls_iter
   global _cls_exchange
   global _countColl
   global _waitForExchange

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_exchange = ObjInfo(ctx.module("exchange"))
   
   # Only receive events which occur in Object OBJ_COLLECTIONS
   # Additionally get events from OBJ_COMMUNICATION to be up to date on load or cleanup
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_COLLECTIONS + "," + OBJ_COMMUNICATION)
   _cls_exchange.setEventReceiveMode(ERM_ALL)
   
   _cls_iter = ObjIterator(ctx.module("iterator"))

   _waitForExchange = False
   return


###################################################################################
#
#  ObjMgr event handling
#
###################################################################################


def handleObjectCreatedEvent():
   pass


def handleObjectRemovedEvent():
   pass
   

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   global _countViewer, _waitForExchange, _saveCollection
   object   = _cls_info.activeObject()
   layer   = _cls_info.activeLayer()
   info   = _cls_info.activeInfo() 
   value  = _cls_info.get()

   if info == INF_GLOBAL_REFRESH and (_cls_info.getBool() == False or _cls_info.get()=="0" ) and _saveCollection:
      _countViewer = _countViewer - 1
      if _countViewer == 0:
         updateViewerList()
         _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_COLLECTIONS + "," + OBJ_COMMUNICATION + "," + OBJ_ANIMATION + "," + ctx.field("capturedViewer").value)
         makeCollection(1)

   elif object == ctx.field("capturedViewer").value and info == INF_SCREENSHOTFLAG and _cls_info.getBool() == False and _saveCollection:
      makeCollection(2)
      
   elif object == OBJ_ANIMATION and layer == LAY_ANIMATION_SCRIPT and info == INF_SCRIPT_STATUS:
      if (value == "ready" and _waitForExchange == True):
         _waitForExchange = False
         ctx.field("exchange.firstEvent").touch()
         while ctx.field("exchange.operationSuccessful").value:
            _cls_info.typedSet(_cls_exchange.activeObject(), _cls_exchange.activeLayer(), _cls_exchange.activeInfo(), _cls_exchange.get(), _cls_exchange.getType())
            ctx.field("exchange.nextEvent").touch()
         _cls_info.notify()
   pass


def handleLoadedEvent():
   global _caseDir
   global _activeCollection
   global _unsavedCollections

   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
   _activeCollection = ""
   _unsavedCollections = []

   MLABFileManager.remove (_caseDir + "collections/temp.tif")
   MLABFileManager.remove (_caseDir + "collections/temp.thumb.tif")
   updateListView()


def handleSaveEvent():
   global _unsavedCollections
   _unsavedCollections = []
   pass


def handleCleanupEvent():
   global _activeCollection
   global _unsavedCollections
   global _caseDir
   
   _activeCollection = ""
   MLABFileManager.remove (_caseDir + "collections/temp.tif")
   MLABFileManager.remove (_caseDir + "collections/temp.thumb.tif")
   
   # clear all collection files, which are established after the last SAVE event
   for file in _unsavedCollections:
      MLABFileManager.remove(_caseDir + "collections/" + file)
   
   _unsavedCollections = []
   _caseDir = ""
   updateListView()


###################################################################################
#
#  ListView handling
#
###################################################################################


def initListView():
   global _listView
   _listView = ctx.control("collectionList")
   updateListView()
   return


def closeListView():
   global _listView
   _listView = None
   return


def updateListView():
   if _listView:
      _listView.clearItems()
      
      if _cls_info.get(OBJ_COMMUNICATION, LAY_GLOBALEVENTS, INF_CASELOADED) != MSG_CLEANUP:
         _cls_info.activateObject(OBJ_COLLECTIONS)
         _cls_info.firstLayer()
         
         while _cls_info.success():
            if _cls_info.activeLayer() != LAY_GLOBAL:
               layerID = _cls_info.activeLayer()
               
               item = _listView.insertItem()
               if _cls_info.existInfo(OBJ_COLLECTIONS, layerID, INF_THUMBNAME):
                  item.setPixmapFile(0, _caseDir + _cls_info.get(OBJ_COLLECTIONS, layerID, INF_THUMBNAME))
               else:
                  item.setPixmapFile(0, _caseDir + _cls_info.get(OBJ_COLLECTIONS, layerID, INF_SCREENSHOT))
               item.setText(1, _cls_info.get(OBJ_COLLECTIONS, layerID, INF_TITLE))
            _cls_info.nextLayer()
   return


def clicked(item, column):
   if ctx.field("singleClick").value == "Load":
      loadCollection(getActiveCollection(item))
   else:
      showDetails(getActiveCollection(item))
   return


def doubleClicked(item):
   if ctx.field("doubleClick").value == "Load":
      loadCollection(getActiveCollection(item))
   else:
      showDetails(getActiveCollection(item))
   return

   
def showDetails(collection):
   ctx.field("title").value   = _cls_info.get(OBJ_COLLECTIONS, collection, INF_TITLE)
   ctx.field("comment").value = _cls_info.get(OBJ_COLLECTIONS, collection, INF_COMMENT)
            
   # details oben anzeigen
   result = ctx.showModalDialog("METKCollectionsDetailsExist", "Collection Details")
   if result == LOAD:
      loadCollection(_activeCollection)
   elif result == REMOVE:
      removeCollection(_activeCollection)
   pass


def getActiveCollection(item):
   global _activeCollection
   
   title = item.text(1)
   collection = ""
   
   _cls_info.activateObject(OBJ_COLLECTIONS)
   _cls_info.firstLayer()
   
   while _cls_info.success():
      if _cls_info.activeLayer() != LAY_GLOBAL:
         layerID = _cls_info.activeLayer()
 
         if _cls_info.get(OBJ_COLLECTIONS, layerID, INF_TITLE) == title:
            collection = layerID
      _cls_info.nextLayer()
   _activeCollection = collection
   return collection


def setViewer(args=0):
   ctx.field("METKScriptBuilder.viewerName").value = ctx.field("capturedViewer").value
   return


def wakeup(args=0):
   setViewer()
   return

###################################################################################
#
#  Collection management
#
###################################################################################


def saveCollection(field = 0):
   """Generates a temporary collection from the current ObjMgr state. Collection is not saved permanently as long as the user didn't confirm it.
   """
   
   # if there is a case directory, there is also a loaded and valid case
   if _caseDir != "":
      # flag is necessary to prevent this module from reacting on external stimuli
      global _saveCollection
      _saveCollection = True
      makeCollection(0)
   return None


def savePreConfiguredCollection(title, comment):
   """Generates a temporary collection from the current ObjMgr state. Collection is saved permanently.
   """
   global _preConfiguredCollection
   _preConfiguredCollection = True
   ctx.field("title").value = title
   ctx.field("comment").value = comment
   
   makeCollection(0)
   return None


def makeCollection(state):
   global _activeCollection
   global _preConfiguredCollection
   global _unsavedCollections
   
   _activeCollection = "temp"
   # update all viewer parameters
   if state == 0:
      #print "makeCollection 0"
      updateViewers()
      
   # make a screenshot
   elif state == 1:
      #print "makeCollection 1"
      MLABFileManager.mkdir(_caseDir + "collections/")
      MLABFileManager.remove (_caseDir + "collections/temp.tif")
      MLABFileManager.remove (_caseDir + "collections/temp.thumb.tif")
      filename = _caseDir + "collections/temp"
      _cls_info.typedSet(ctx.field("capturedViewer").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFILE,  filename + ".tif", INFOTYPE_STRING)
      _cls_info.typedSet(ctx.field("capturedViewer").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTTHUMB, True, INFOTYPE_BOOL)
      _cls_info.typedSet(ctx.field("capturedViewer").value, LAY_VIEWER_SCREENSHOT, INF_SCREENSHOTFLAG,  True, INFOTYPE_BOOL)
      ctx.callLater(0, "_cls_info.notify")
      
   # screenshot is made, lets ask for details
   elif state == 2:
      if _preConfiguredCollection == False:
         ctx.field("title").value = ""
         ctx.field("comment").value = ""
         result = ctx.showModalDialog("METKCollectionsDetailsNew", "Collection Details")
      else:
         result = SAVE
      _preConfiguredCollection = False
      if result == SAVE:
         # create new (unexistent) collection entry
         if not _cls_info.existObject(OBJ_COLLECTIONS):
            _cls_info.createObject(OBJ_COLLECTIONS)
         _cls_info.activateObject(OBJ_COLLECTIONS)
         _cls_info.firstLayer()
         _cls_info.fld_genLayerUIDPrefix.value = "Collection_"
         _cls_info.fld_genLayerUID.touch()
         _activeCollection = _cls_info.activeLayer()
         
         # rename temporary files
         standardFileName = MLABFileManager.normalizePath(_cls_info.get(OBJ_CASE, LAY_CASE, INF_XMLFILE))
         pathComponents = MLABFileManager.splitPath(standardFileName)
         baseName = pathComponents["base"]

         shotName  = MLABFileManager.splitPath(MLABFileManager.getUniqueFilename(_caseDir + "collections/", baseName + _activeCollection, ".tif"))["name"]
         thumbName = MLABFileManager.splitPath(MLABFileManager.getUniqueFilename(_caseDir + "collections/", baseName + _activeCollection, ".thumb.tif"))["name"]
         collName  = MLABFileManager.splitPath(MLABFileManager.getUniqueFilename(_caseDir + "collections/", baseName + _activeCollection, ".col"))["name"]
         
         _unsavedCollections.append(shotName)
         _unsavedCollections.append(thumbName)
         _unsavedCollections.append(collName)
         
         MLABFileManager.rename (_caseDir + "collections/temp.tif", _caseDir + "collections/" + shotName)
         MLABFileManager.rename (_caseDir + "collections/temp.thumb.tif", _caseDir + "collections/" + thumbName)
         

         # Einträge in OBJ_COLLECTIONS machen
         _cls_info.set(OBJ_COLLECTIONS, _activeCollection, INF_TITLE,      ctx.field("title").value)
         _cls_info.set(OBJ_COLLECTIONS, _activeCollection, INF_COMMENT,    ctx.field("comment").value)
         _cls_info.set(OBJ_COLLECTIONS, _activeCollection, INF_SCREENSHOT, "collections/" + shotName)
         _cls_info.set(OBJ_COLLECTIONS, _activeCollection, INF_FILENAME,   "collections/" + collName)
         _cls_info.set(OBJ_COLLECTIONS, _activeCollection, INF_THUMBNAME,  "collections/" + thumbName)
         

         ctx.field("ObjDump.fileName").value = _caseDir + "collections/" + collName
         ctx.field("ObjMgr.clearObjectContainer").touch()
         
         # Datenbank nach relevanten Einträgen durchsuchen
         _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE)
         _cls_iter.searchFirst()
         while _cls_iter.success():
            if _objectInclude.value.count(_cls_info.get(_cls_iter.object(), LAY_GLOBAL, INF_OBJTYPE)):
               # Kopiere rekursiv alle Einträge dieses Objects nach exchange
               _cls_info.activateObject(_cls_iter.object())
               _cls_info.firstLayer()
               while _cls_info.success():
                  layerID = _cls_info.activeLayer()
                  if not _layerExclude.value.count(layerID):
                     _cls_info.firstInfo()
                     while _cls_info.success():
                        infoID = _cls_info.activeInfo()
                        if not _infoExclude.value.count(infoID):
                           _cls_exchange.typedSet(_cls_iter.object(), layerID, infoID, _cls_info.get(), _cls_info.getType())
                        _cls_info.nextInfo()
                  _cls_info.nextLayer()
            _cls_iter.searchNext()
            
         _cls_exchange.notify()
         ctx.field("ObjDump.save").touch()
         _cls_info.notify()
         updateListView()
      else:
         _activeCollection = ""
         MLABFileManager.remove (_caseDir + "collections/temp.tif")
         MLABFileManager.remove (_caseDir + "collections/temp.thumb.tif")
      global _saveCollection
      _saveCollection = False
   return None
   

def loadCollection(collection):
   global _countViewer
   
   # causes the viewers to change their values in the ObjMgr. This is necessary to enforce the adoption of the camera parameters on loading
   updateViewers()
   _countViewer = 0
   # loads the Collection actually stored in field collection

   # alles aufräumen
   ctx.field("ObjMgr.clearObjectContainer").touch()
   ctx.field("exchange.clearEventQueue").touch()
   
   # Collection laden
   ctx.field("ObjLoader.fileName").value = _caseDir + _cls_info.get(OBJ_COLLECTIONS, collection, INF_FILENAME)
   ctx.field("ObjLoader.load").touch()
   
   # remove non-existent viewers from the loaded collection
   iter = ObjIterator(ctx.module("viewerRemoving"))
   iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer3D")
   iter.searchFirst()
   while iter.success():
      if not _cls_info.existObject(iter.object()):
         _cls_exchange.removeObject(iter.object())
      iter.searchNext()
   iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer2D")
   iter.searchFirst()
   while iter.success():
      if not _cls_info.existObject(iter.object()):
         _cls_exchange.removeObject(iter.object())
      iter.searchNext()
   _cls_exchange.notify()
   
   if ctx.field("animated").value:
      #run script   
      #print "buildNewScript"
      ctx.field("METKScriptBuilder.useColor").value = False
      ctx.field("METKScriptBuilder.useTransparency").value = False
      ctx.field("METKScriptBuilder.useVisibility").value = False
      ctx.field("METKScriptBuilder.realtime").value = True
      ctx.field("METKScriptBuilder.length").value = 2
      ctx.field("METKScriptBuilder.buildNewScript").touch()      
      global _waitForExchange, _waitForScriptBuilding
      _waitForExchange = True
      _waitForScriptBuilding = True
   else:
      ctx.field("exchange.firstEvent").touch()
      while ctx.field("exchange.operationSuccessful").value and _cls_exchange.existObject(_cls_exchange.activeObject()):
         _cls_info.set(_cls_exchange.activeObject(), _cls_exchange.activeLayer(), _cls_exchange.activeInfo(), _cls_exchange.get())
         ctx.field("exchange.nextEvent").touch()
      _cls_info.notify()
   
   
   pass

def removeCollection(collectionName):
   global _activeCollection
   _activeCollection = ""
   
   # Dateien löschen
   MLABFileManager.remove(_caseDir + _cls_info.get(OBJ_COLLECTIONS, collectionName, INF_FILENAME))
   MLABFileManager.remove(_caseDir + _cls_info.get(OBJ_COLLECTIONS, collectionName, INF_SCREENSHOT))

   # remove entry
   _cls_info.removeLayer(OBJ_COLLECTIONS, collectionName)
   _cls_info.notify()
   updateListView()   
   return


###################################################################################
#
#  Detail window handling
#
###################################################################################


def initDetailView():
   if _activeCollection == "temp":
      ctx.control("imageView").setPixmapFile(_caseDir + "collections/temp.tif")
   else:
      ctx.control("imageView").setPixmapFile(_caseDir + _cls_info.get(OBJ_COLLECTIONS, _activeCollection, INF_SCREENSHOT))


def modalDialogLoad():
   ctx.window().done(LOAD)
   pass


def modalDialogSave():
   ctx.window().done(SAVE)
   pass


def modalDialogRemove():
   ctx.window().done(REMOVE)
   pass


def modalDialogCancel():
   ctx.window().done(CANCEL)
   pass


###################################################################################
#
#  video rendering / animations
#
###################################################################################


def initRenderVideo(field = 0):
   global _renderVideo
   
   ctx.field("METKScriptBuilder.useColor").value = True
   ctx.field("METKScriptBuilder.useTransparency").value = True
   ctx.field("METKScriptBuilder.useVisibility").value = True
   ctx.field("METKScriptBuilder.realtime").value = False
   ctx.field("METKScriptBuilder.length").value = 0
   
   ctx.field("METKScriptBuilder.clear").touch()
   
   _renderVideo = True
   
   # get first collection
   _cls_info.activateObject(OBJ_COLLECTIONS)
   _cls_info.firstLayer()
   appendScript()
   pass   
   
   
def appendScript():
   print "APPEND SCRIPT......................................"
   collection = ""
   
   collection = _cls_info.activeLayer()

   #print "appendScript for " + collection
   if collection != LAY_GLOBAL:
      #print ".  load collection"
      # alles aufräumen
      ctx.field("ObjMgr.clearObjectContainer").touch()
      ctx.field("exchange.clearEventQueue").touch()
      
      # Collection laden
      ctx.field("ObjLoader.fileName").value = _caseDir + _cls_info.get(OBJ_COLLECTIONS, collection, INF_FILENAME)
      ctx.field("ObjLoader.load").touch()
      
      #run script   
      #print ".  appendToScript"
      ctx.field("METKScriptBuilder.appendToScript").touch()

   else:
      _cls_info.nextLayer()
      if  _cls_info.success():
         appendScript()
      else:
         scriptAppended()
   pass
   

def scriptAppended(field = 0):
   global _renderVideo, _waitForScriptBuilding
   
   if _waitForScriptBuilding:
      _waitForScriptBuilding = False
      
      #print "set to OBJMGR:"+ctx.field("METKScriptBuilder.script").value
      _cls_info.typedSet( OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_FULLSCRIPT, ctx.field("METKScriptBuilder.script").value,INFOTYPE_MESSAGE)
      _cls_info.notify()      
   
   elif _renderVideo and ctx.field("METKScriptBuilder.script").value != "":
      _cls_info.nextLayer()
      if _cls_info.success():
         ctx.field("METKScriptBuilder.length").value = ctx.field("videoStepLength").value
         appendScript()
      else:
         _renderVideo = False
         #print "set SCRIPT 2 in OBJMGR:" + ctx.field("METKScriptBuilder.script").value
         _cls_info.typedSet( OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_FULLSCRIPT, ctx.field("METKScriptBuilder.script").value, INFOTYPE_MESSAGE)
         _cls_info.notify()      
   pass


###################################################################################
#
#  updating
#
###################################################################################


def updateViewers():
   global _countViewer
   
   # Sicherstellen, dass das Abarbeiten sich nicht selbst überholt
   _countViewer = 0
   
   # Damit handleAttributeModifiedEvent die Nachrichten von allen Viewern erhält, ERM_ALL setzen
   _cls_info.setEventReceiveMode(ERM_ALL)

   # Alle Viewer ihre aktuellen Parameter in den ObjMgr schreiben lassen
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer3D")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _countViewer = _countViewer + 1
      _cls_info.typedSet(_cls_iter.object(), LAY_GLOBAL, INF_GLOBAL_REFRESH, True, INFOTYPE_BOOL)
      _cls_iter.searchNext()
      
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer2D")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _countViewer = _countViewer + 1
      _cls_info.typedSet(_cls_iter.object(), LAY_GLOBAL, INF_GLOBAL_REFRESH, True, INFOTYPE_BOOL)
      _cls_iter.searchNext()
      
   _cls_info.notify()
   pass


def updateViewerList(field = 0):
   capturedViewerF = ""
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer3D")
   _cls_iter.searchFirst()
   # ersten gefundenen Viewer einsetzen
   if _cls_iter.success() and ctx.field("capturedViewer").value == "[any viewer]":
      ctx.field("capturedViewer").value = _cls_iter.object()
      
   while _cls_iter.success():
      ctx.log("Found viewer: " + _cls_iter.object())
      if capturedViewerF == "":
         capturedViewerF = _cls_iter.object()
      else:
         capturedViewerF += "," + _cls_iter.object()
      _cls_iter.searchNext()
      
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_OBJTYPE, "Viewer2D")
   _cls_iter.searchFirst()
   
   # ersten gefundenen Viewer einsetzen
   if _cls_iter.success() and ctx.field("capturedViewer").value == "[any viewer]":
      ctx.field("capturedViewer").value = _cls_iter.object()
      
   while _cls_iter.success():
      ctx.log("Found viewer: " + _cls_iter.object())
      if capturedViewerF == "":
         capturedViewerF = _cls_iter.object()
      else:
         capturedViewerF += "," + _cls_iter.object()
      _cls_iter.searchNext()

   if capturedViewerF == "":
      capturedViewerF = "[any viewer]"
   ctx.field("capturedViewerF").value = capturedViewerF
   if ctx.field("capturedViewer").value == "":
      ctx.field("capturedViewer").value = "[any viewer]"
   pass


def notify():
   _cls_info.notify()