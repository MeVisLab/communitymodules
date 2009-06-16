# Python module import
from ObjMgr import *
from VisDefinitions import *
from os import *
from shutil import *
import string

# Global class instances
_cls_info= None

_objNamesEnum_Liver = "Tumor,Liver,Territory1,Territory2,Territory3,Territory4,Territory5,Territory6,Territory7,OwnName"
_objNamesEnum_HNO = "Bones,Lymphnode1,Lymphnode2,Lymphnode3,OwnName"

_globalReturnValue = None # igitt!!! weil wir keine Funktionen der Fenster direkt schreiben und aufrufen können, die uns eigene Werte zurückliefern, müssen wir die Rückgabe über globale Variablen machen ... bäh

_waitForLoadBase = False
#_waitForPolygonize = False

def init():
   global _cls_info      
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))      
   ctx.field("objNamesEnum").value = _objNamesEnum_Liver   
   
   global _waitForLoadBase#, _waitForPolygonize
   _waitForLoadBase = False
   #_waitForPolygonize = False
   return

   
def updateObject(fieldP=0):
   global _waitForLoadBase
   print "updateObject"
   objId = ctx.field("objID").value
   
   
   _cls_info.set(objId, "Appearance", "Visible","TRUE")
   _cls_info.markInfoNonPersistent(objId, "Appearance", "Visible")
   _cls_info.notify()
   
   sg = _cls_info.get(objId, "Description", "StructureGroup")   
   if (sg == "Vessel"):
      print "It is a vessel"
      caseDir =  _cls_info.get("CaseObject", "Case", "Directory")
      treeFile = caseDir + "/" + _cls_info.get(objId, "Files", "TreeDataFile")
      print "treeFile = " + treeFile
      if (ctx.field("LoadBase.filename").value!=treeFile):
         print "load new treeFile"
         ctx.field("LoadBase.filename").value = treeFile
         _waitForLoadBase = True
         ctx.field("LoadBase.load").touch()
   else:
      print "It is NOT a vessel"
   
   return
   
def treeXMLLoaded(args):
   global _waitForLoadBase#, _waitForPolygonize   
   if (_waitForLoadBase==True and ctx.field("LoadBase.upToDate").value == True):
      print "treeXML loaded"
      
      # ziemlicher Hack aber ich wollte nicht extra den Parameter aus dem METKViewer3D rausführen
      print "*"+ctx.field("ViewerTotalVessel.view3d.button1events").value+"*"
      ctx.field("ViewerTotalVessel.view3d.button1events").setValue("TO_VIEWER")
      
      _waitForLoadBase = False
   return
      
   
def caseTypeChanged(fieldP=0):
   if (ctx.field("selectedCaseType").value=="Liver"):
      ctx.field("objNamesEnum").setStringValue(_objNamesEnum_Liver)
   elif (ctx.field("selectedCaseType").value=="HNO"):
      ctx.field("objNamesEnum").setStringValue(_objNamesEnum_HNO)
   return
       
   
         
def openCase(fieldP=0):
   ctx.showWindow("OpenCase")
   return
   
def saveCase(fieldP=0):
   ctx.showWindow("SaveCase")
   ctx.field("METKObjXMLWriter.filename").value = "ObjectNames.xml"
   ctx.field("METKObjXMLWriter.run").touch()
   return
   
   
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
   
   
def showOldInNew(args):
   if (ctx.field("showOldInNew").value==True):
      ctx.field("switchOldVisible.whichChild").value = -3 #All
   else:
      ctx.field("switchOldVisible.whichChild").value = -1 #None
   return
   
   
def saveNewVessel(args):
   objId = ctx.field("objID").value
   print "objId="+objId
   if (objId!=""):
      caseDir =  _cls_info.get("CaseObject", "Case", "Directory")
      filename = caseDir + "/" + _cls_info.get(objId, "Files", "InventorFile")
      ctx.field("writeNewIV.filename").value = filename
      ctx.field("writeNewIV.save").touch()
   return
   

def syncViewer(args):
   ctx.field("ViewerNewIV.syncViewer").value = ctx.field("syncViewer").value
   ctx.field("ViewerOldIV.syncViewer").value = ctx.field("syncViewer").value
   ctx.field("ViewerTotalVessel.syncViewer").value = ctx.field("syncViewer").value
   return