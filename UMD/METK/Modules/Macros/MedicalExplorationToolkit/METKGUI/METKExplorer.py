# Python module import
from ObjMgr import *
from VisDefinitions import *
from os import *
from shutil import *
import string

# Global class instances
_cls_info= None
_cls_iter      = None
_comboObjNames = None

_objNamesEnum_Liver = "Tumor,Liver,Territory1,Territory2,Territory3,Territory4,Territory5,Territory6,Territory7,OwnName"
_objNamesEnum_HNO = "Bones,Lymphnode1,Lymphnode2,Lymphnode3,OwnName"

_globalReturnValue = None # igitt!!! weil wir keine Funktionen der Fenster direkt schreiben und aufrufen können, die uns eigene Werte zurückliefern, müssen wir die Rückgabe über globale Variablen machen ... bäh
_waitForVessel = ""
_vesselList = ""

_fileManager   = None

def init():
   global _cls_info, _cls_iter, _fileManager
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))
   ctx.field("objNamesEnum").value = _objNamesEnum_Liver
   
   _cls_iter = ObjIterator(ctx.module("deleteObjIterator"))
   _cls_iter.setSearchCriteria(LAY_FILES, INF_IVFILE)
   
   
   global _waitForVessel, _vesselList
   _waitForVessel = ""
   _vesselList = ""
   
   _fileManager = MLABFileManager
   
   return


def updateObject(fieldP=0):
   global _comboObjNames
   objId = ctx.field("objID").value
   
   _comboObjNames.setCurrentText(_cls_info.get(objId, LAY_DESCRIPTION, INF_NAME))
   #ctx.field("objName").setStringValue(objName)
   return


def renameObject():
   global _comboObjNames
   objId = ctx.field("objID").value
   objName = _comboObjNames.currentText()
   objName = objName.replace(" ","")
   #objName = objName.replace(".","_")
   
   # rename obj itself (description->name)
   _cls_info.typedSet(objId, LAY_DESCRIPTION, INF_NAME, objName, 'omAttribute_std_string')
   _cls_info.notify()   
   
   # if result, then new objID
   if (_cls_info.get(objId, LAY_GLOBAL, INF_OBJTYPE)=='Result'):
      newId = "Structure_"+objName
      #newId = newId.replace(".","_")
      _cls_info.renameObject(objId,newId)
      _cls_info.notify()
      renameChildsAndParents(objId,newId)
      resetNonP(newId)
      
      #rename iv-file
      ivFile = str(_cls_info.get(newId, LAY_FILES, INF_IVFILE))
      if (ivFile.find(objId)!=-1):
         #print "replace Inventor File Name"
         ivFileNew = ivFile.replace(objId, newId)
         _cls_info.set(newId, LAY_FILES, INF_IVFILE, ivFileNew)
         #print "copy file: "+_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+ivFile+"  to: "+_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+ivFileNew
         #rename ersetzt copy # neu 2006-07-26
         rename(_cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+ivFile, _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR)+ivFileNew)
   
   ctx.field("METKHierarchyBrowser.updateView").touch()
   
   return


def renameChildsAndParents(oldId, newId):
   currentId = _cls_info.firstObject()
   while _cls_info.success():
      if (_cls_info.get(currentId, LAY_GLOBAL, INF_PARENT)==oldId):
         #print "Parent = "+ newId +"  for " + currentId
         _cls_info.set(currentId, LAY_GLOBAL, INF_PARENT,newId)
      childs=_cls_info.get(currentId, LAY_GLOBAL, INF_CHILDS)
      #print childs + "aus "+currentId+" "+LAY_GLOBAL+" "+INF_CHILDS
      #childsChanged=False
      if (childs.find(oldId)!=-1):
         singleChilds = childs.split(";")
         #print "ChildsOld = "+ childs +"  for " + currentId
         childs = ""
         for child in singleChilds:
            if (child==oldId):
               childs = childs + newId+";"
            elif child!="":
               childs = childs + child+";"
         #print "ChildsNew = "+ childs +"  for " + currentId
         _cls_info.set(currentId, LAY_GLOBAL, INF_CHILDS,childs)
      currentId = _cls_info.nextObject()
   return



def generateVessel():
   objID = ctx.field("objID").value
   ctx.field("GenerateVesselSender.data").value = objID
   ctx.field("GenerateVesselSender.send").touch()
   return


def generateVesselAll():
   global _waitForVessel, _vesselList
   if (_waitForVessel==""):
      #build list
      _vesselList = ""
      currentId = _cls_info.firstObject()
      while _cls_info.success():
         sg=_cls_info.get(currentId, LAY_DESCRIPTION, INF_STRUCTUREGROUP)
         if (sg=="Vessel"):
            _vesselList = _vesselList + "," + currentId
            _waitForVessel = currentId
         currentId = _cls_info.nextObject()

   #print _vesselList
   
   if (_waitForVessel!=""):
      if (len(_vesselList)>1):
         object = _vesselList.split(",")
         #print "Structure:" + object[1]
         #print "Vorher: " + _vesselList
         _vesselList = _vesselList.replace(","+object[1],"",1)
         #print "Nachher: " + _vesselList
         ctx.field("GenerateVesselSender.data").value = object[1]
         ctx.field("GenerateVesselSender.send").touch()
      else:
         _waitForVessel = ""
         #print "Ready ... All vessels created!"
   
   return


def nextVessel(args):
   global _waitForVessel
   if (_waitForVessel!=""):
      reloadObject(_waitForVessel)
      generateVesselAll()
   else:
      reloadObject()
   return


def reloadObject(args=0):
   if (args==0):
      objID = ctx.field("objID").value
   else:
      objID = args
   filename = _cls_info.get(objID, LAY_FILES, INF_IVFILE)
   _cls_info.set(objID, LAY_FILES, INF_IVFILE, "")
   _cls_info.notify()
   _cls_info.set(objID, LAY_FILES, INF_IVFILE, filename)
   _cls_info.notify()
   return




def caseTypeChanged(fieldP=0):
   if (ctx.field("selectedCaseType").value=="Liver"):
      ctx.field("objNamesEnum").setStringValue(_objNamesEnum_Liver)
   elif (ctx.field("selectedCaseType").value=="HNO"):
      ctx.field("objNamesEnum").setStringValue(_objNamesEnum_HNO)
   return

def objNamesCombo(fieldP=0):
   global _comboObjNames
   if (_comboObjNames):
      if (_comboObjNames.currentText()=="OwnName"):
         #ctx.field("objName").value = ""
         _comboObjNames.setCurrentText("")
   return



def openWindow():
   global _comboObjNames
   _comboObjNames = ctx.control("comboObjName")
   return

def closeWindow():
   global _comboObjNames
   _comboObjNames = None
   return


def openCase(field = 0):
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
      ctx.field("METKManager.cleanup").touch()
      ctx.field("METKManager.fileName").value = caseFile
      ctx.field("METKManager.load").touch()
      ctx.window().setTitle("METKExplorer | " + caseFile)
   return


def saveCaseAs(field = 0):
   ctx.field("METKManager.saveAs").touch()
   #ctx.field("METKObjXMLWriter.filename").value = "ObjectNames.xml"
   #ctx.field("METKObjXMLWriter.run").touch()
   pass


def saveCase(field = 0):
   ctx.field("METKManager.save").touch()
   #ctx.field("METKObjXMLWriter.filename").value = "ObjectNames.xml"
   #ctx.field("METKObjXMLWriter.run").touch()
   pass


def exit(fieldP=0):
   ctx.window().close()
   return


def changeObjMgr(fieldP=0):
   ctx.field("internalObjMgr.outObjectContainer").disconnectOutputs()
   ctx.field("externalObjMgr.outObjectContainer").disconnectOutputs()
   if (ctx.field("internalObjMgr").boolValue() == True):
      ctx.field("info.inObjectContainer").connectFrom(ctx.field("internalObjMgr.outObjectContainer"))
   else:
      ctx.field("info.inObjectContainer").connectFrom(ctx.field("externalObjMgr.outObjectContainer"))
   
   return


def resetNonP(objId):
   print "set non-persistent infos for "+objId
   _cls_info.markInfoNonPersistent(objId, LAY_FILES, INF_IVPOINTER)
   _cls_info.markInfoNonPersistent(objId, LAY_FILES, INF_IVFULLPOINTER)
   _cls_info.markInfoNonPersistent(objId, LAY_FILES, INF_ISOPOINTER)
   _cls_info.markInfoNonPersistent(objId, LAY_FILES, INF_STIPPLEPOINTER)
   _cls_info.markInfoNonPersistent(objId, LAY_FILES, INF_SILHOUETTEPOINTER)
   if (ctx.field("METKObjBrowser.markNonPersistent").boolValue()):
      _cls_info.markInfoNonPersistent(objId, LAY_APPEARANCE, INF_VISIBLE)
   return



def deleteObject(fieldP=0):   
   objId = ctx.field("objID").value
   #print "objId: " + objId
   removeAsChild(objId)
   #print "removeObject: "+objId
   _cls_info.removeObject(objId)
   ctx.field("METKHierarchyBrowser.updateView").touch()
   return


def removeAsChild(oldId):
   #print "remove as Child"+oldId
   currentId = _cls_info.firstObject()
   while _cls_info.success():
      childs=_cls_info.get(currentId, LAY_GLOBAL, INF_CHILDS)
      if (childs!=""):
         if (childs.find(oldId)!=-1):
            singleChilds = childs.split(";")
            #print "ChildsOld = "+ childs +"  for " + currentId
            childs = ""
            for child in singleChilds:
               if (child!=oldId and child!=""):
                  childs = childs + child+";"
            #print "ChildsNew = "+ childs +"  for " + currentId
            _cls_info.set(currentId, LAY_GLOBAL, INF_CHILDS,childs)
      currentId = _cls_info.nextObject()
   return


def changeParent(fieldP=0):
   global _globalReturnValue
   objId = ctx.field("objID").value
   if (ctx.showModalDialog("winChangeParent")==1):
      #print _globalReturnValue
      newParent = _globalReturnValue
      oldParent = _cls_info.get(objId, LAY_GLOBAL, INF_PARENT)
      #print "oldParent: "+oldParent+"   newParent:"+newParent
      
      # change childs for old parent
      childs=_cls_info.get(oldParent, LAY_GLOBAL, INF_CHILDS)
      if (childs.find(objId)!=-1):
         singleChilds = childs.split(";")
         #print "ChildsOld = "+ childs +"  for " + objId
         childs = ""
         for child in singleChilds:
            if (child!=objId and child!=""):
               childs = childs + child+";"
         #print "ChildsNew = "+ childs +"  for " + objId
         _cls_info.set(oldParent, LAY_GLOBAL, INF_CHILDS,childs)
      # change childs for new parent
      childs=_cls_info.get(newParent, LAY_GLOBAL, INF_CHILDS)
      if (childs!=""):
         childs=childs+";"
      childs = childs + objId
      _cls_info.set(newParent, LAY_GLOBAL, INF_CHILDS,childs)
      
      # change parent itself
      _cls_info.set(objId, LAY_GLOBAL, INF_PARENT, newParent)
      
      ctx.field("updateList").touch()
   #else:
      #print "Cancel"
   
   return

def openWinChangeParent():
   #global _objectsList
   #ctx.control("newValue").setText(_cls_info.get(ctx.field("objID").value,ctx.field("layerID").value,_objectsList.selectedItem().text(0)))
   parentEnum = ""
   currentId = _cls_info.firstObject()
   while _cls_info.success():
      parentEnum = parentEnum + currentId + ","
      currentId = _cls_info.nextObject()
   ctx.field("parentsEnum").setStringValue(parentEnum)

   return

def closeWinChangeParentCancel():
   global _globalReturnValue   
   _globalReturnValue = ""
   ctx.window().reject()
   return


def closeWinChangeParentYes():
   global _globalReturnValue
   _globalReturnValue = ctx.control("comboNewParent").currentText()
   ctx.window().accept()
   return




def deleteNonIVObjects(args=0):
   global _cls_iter
   fileManager = MLABFileManager
   deleteCounter = 0
   _cls_iter.setSearchCriteria(LAY_FILES, INF_IVFILE)
   _cls_iter.searchFirst()
   toDelete = ""
   while _cls_iter.success():
      fileNameIV = _cls_info.get(OBJ_CASE, LAY_CASE, INF_CASEDIR) + _cls_info.get(_cls_iter.object(), LAY_FILES, INF_IVFILE)
      if not fileManager.exists(fileManager.normalizePath(fileNameIV)):
         deleteCounter = deleteCounter + 1
         toDelete = toDelete + _cls_iter.object() + ","
         #print _cls_iter.object()
         #_cls_info.removeObject(_cls_iter.object())
      _cls_iter.searchNext()
   
   singles = toDelete.split(",")
   for single in singles:
      _cls_info.removeObject(single)
   _cls_info.notify()
   
   #Clean up childIds
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_CHILDS)
   _cls_iter.searchFirst()
   while _cls_iter.success():
      childsstr = _cls_info.get(_cls_iter.object(), LAY_GLOBAL, INF_CHILDS)
      #print "old childs: " + childsstr
      childs = childsstr.split(";")
      newchilds = ""
      for singlechild in childs:
         if (_cls_info.existObject(singlechild)):
            newchilds = newchilds + singlechild + ";"
         _cls_info.set(_cls_iter.object(), LAY_GLOBAL, INF_CHILDS, newchilds)
         #print "new childs: " + newchilds
      _cls_iter.searchNext()
   
   #print "deleteCounter: " + str(deleteCounter)
   return