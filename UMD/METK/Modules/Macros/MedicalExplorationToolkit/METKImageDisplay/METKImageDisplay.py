# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *

# Global class instances
_cls_info = None
_cls_iter = None
_caseDir = ""
_currentROI = ""
_availableROIs = ""
_previousROI = ""


def init():
   global _cls_info
   global _cls_iter

   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = METKObjInfo(ctx.module("info"))
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_IMAGE+","+LAY_ROIS+","+LAY_APPEARANCE)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Original")
   
   #was soll das denn hier???
   ctx.field("roiSelect").touch()

   #gives all modules the opportunity to have their own init
   try: init2()
   except: blub=False #do nothing
   

   return


def ignoreEmptyString(field = 0):
   #ctx.log("ignoreEmptyString")
   #ctx.log("_currentROI " + _currentROI)
   if ctx.field("ignoreEmptyString").value == True:
      ctx.field("rois").value = _availableROIs      
      if (ctx.field("roiSelect").value != _currentROI):         
         ctx.field("roiSelect").value = _currentROI
   else:
      if _availableROIs == "":
         ctx.field("rois").value = "[none]"         
         ctx.field("roiSelect").value = ""
      else:
         ctx.field("rois").value = "[none]," + _availableROIs         
         ctx.field("roiSelect").value = _currentROI
   return


def setCurrentROI(field = 0):
   #ctx.log("setCurrentROI to "+ctx.field("roiSelect").value)
   global _currentROI
   global _previousROI
   
   if (_currentROI == ctx.field("roiSelect").value):
      return
   
   
   # verhindern, dass [none] im Eintrag steht
   if ctx.field("roiSelect").value == "[none]":
      ctx.field("roiSelect").value = ""
      
   # Eintrag vorhanden?
   if ctx.field("roiSelect").value != "":
      _currentROI = ctx.field("roiSelect").value
      if _currentROI == "[none]":
         _currentROI = ""
      ctx.field("roiSelect").value = _currentROI
      if _currentROI == "":
         closeImage()
      else:
         # Aufruf der spezialisierten Funktion                  
         getImage(_currentROI)
      _previousROI = _currentROI      
   else: # nicht? Dann testen ob das das sein darf
      if ctx.field("ignoreEmptyString").value == True:         
         ctx.field("roiSelect").value = _previousROI
         _currentROI = _previousROI
      else:
         _currentROI = ""
         _previousROI = ctx.field("roiSelect").value
         closeImage()
   #communicate currentROI
   if (ctx.field("listenToROI").value!=""):
      _cls_info.typedSet(OBJ_COMMUNICATION, LAY_ROIS, ctx.field("listenToROI").value, _currentROI, INFOTYPE_MESSAGE)
      _cls_info.notify()
   
   return
   
   
def handleLoadedEvent(field = 0):
   global _caseDir   
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)   
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Original")
   _cls_iter.searchFirst()
   setROI = True #Nur die erste gefunden ROI soll aus roiSelect gesetzt und damit geladen werden
   while _cls_iter.success():
      if _cls_info.get(_cls_iter.object(), LAY_GLOBAL, INF_OBJTYPE) == "Result":             
         newEntry(_cls_iter.object(),setROI)
         setROI=False
      _cls_iter.searchNext()

   #gives all modules the opporunity to have their own handleLoadedEvent
   try: handleLoadedEvent2()
   except: blub=False #do nothing
      
   pass

   
def handleSaveEvent():
   pass


def handleCleanupEvent(field = 0):
   #ctx.log("handleCleanupEvent")
   global _caseDir
   global _currentROI
   global _availableROIs
   global _previousROI
   
   _caseDir = ""
   _currentROI = ""
   _availableROIs = ""
   _previousROI = ""
   
   ctx.field("rois").value = "[none]"
   ctx.field("roiSelect").value = ""
   closeImage()
   
   #gives all modules the opporunity to have their own handleCleanupEvent
   try: handleCleanupEvent2()
   except: blub=False #do nothing
   
   pass
   

def newEntry(object,setROI=True):
   global _caseDir
   global _currentROI
   global _availableROIs
   
   _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)

   # Komma am Anfang verhindern
   if _availableROIs == "":
      _availableROIs = object
   else:
      _availableROIs = _availableROIs + "," + object

   if ctx.field("ignoreEmptyString").value == True and setROI:
      if ctx.field("roiSelect").value == "" and object != "":
         ctx.field("roiSelect").value = object
         _currentROI = object
         
   ignoreEmptyString()
   pass
   

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
   global _caseDir
   global _currentROI
   global _availableROIs
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   # if created attribute is a ROI
   if layer == LAY_IMAGE and info == INF_IMAGETYPE and value == "Original" and _cls_info.get(object, LAY_GLOBAL, INF_OBJTYPE) == "Result":      
      newEntry(object)
   elif object == OBJ_CASE and layer == LAY_CASE and info == INF_CASEDIR:
      _caseDir = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)
      ctx.log("set _caseDir in modifiedEvent in CodSegmentation ="+_caseDir)
   elif object == OBJ_COMMUNICATION and layer == LAY_ROIS and info == ctx.field("listenToROI").value:
      ctx.field("roiSelect").value=value
   pass
