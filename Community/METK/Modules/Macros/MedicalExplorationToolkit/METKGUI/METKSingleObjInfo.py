# Python module import
from ObjMgr import *

# Global class instances
_cls_info = None
_objectsList = None
_globalReturnValue = None # igitt!!! weil wir keine Funktionen der Fenster direkt schreiben und aufrufen können, die uns eigene Werte zurückliefern, müssen wir die Rückgabe über globale Variablen machen ... bäh
_globalReturnValue2 = None # igitt!!! weil wir keine Funktionen der Fenster direkt schreiben und aufrufen können, die uns eigene Werte zurückliefern, müssen wir die Rückgabe über globale Variablen machen ... bäh
_comboNewEntryString = ""

def init():
   global _cls_info   
   global _objectsList
   _objectsList = None
   _cls_info = ObjInfo(ctx.module("info"))      
   return


def closeWindow():
   global _objectsList
   _objectsList = None
   return


def openWindow():
   global _objectsList
   _objectsList = ctx.control("objects")
   updateListView(0)
   return


def updateListView(fieldP=0):
   global _objectsList
   
   objID = ctx.field("objID").value
   if (objID==""):
      return
   layerID = ctx.field("layerID").value
   _cls_info.activateObject(objID)
   if (layerID!=""):
      _cls_info.activateLayer(layerID)
      _cls_info.module.field("firstInfo").touch()
   
   if _objectsList:
      _objectsList.clearItems()
      if (layerID==""):
         root = _objectsList.insertItem()
         root.setText(0, objID)            
         layerIDiter = _cls_info.firstLayer()
      else:
         layerIDiter = layerID      
      
      while _cls_info.success():
         if (layerID==""):
            layerItem = _objectsList.insertItem(root, 0)
            layerItem.setText(0, layerIDiter)
            _cls_info.module.field("firstInfo").touch()
            
         while _cls_info.success():
            infoID = _cls_info.activeInfo()
            infoValue = _cls_info.get(objID, layerIDiter, infoID)
            
            if (layerID==""):
               infoItem = _objectsList.insertItem(layerItem, 0)
            else:
               infoItem = _objectsList.insertItem()

            infoItem.setText(0,infoID)
            infoItem.setText(1,infoValue)
            _cls_info.module.field("nextInfo").touch()
               
            
         if (layerID==""):
            layerIDiter = _cls_info.nextLayer()
      if (layerID==""):
         root.setOpen(True)
   return
   

def doubleClicked(fieldP=0):
   global _objectsList
   #ctx.field("doubleClickedEntry").value = _objectsList.selectedItem().text(0)
   global _globalReturnValue
   if (_objectsList.selectedItem().text(0)==""):
      return
   if (ctx.showModalDialog("winEditValue")==1):
      #print _globalReturnValue
      _cls_info.set(ctx.field("objID").value,ctx.field("layerID").value,_objectsList.selectedItem().text(0),_globalReturnValue)
      #print "set "+ctx.field("objID").value+","+ctx.field("layerID").value+","+_objectsList.selectedItem().text(0)+","+_globalReturnValue
      ctx.field("update").touch()
   #else:
      #print "Cancel"
   return
   
   
def openWinEditValue():
   global _objectsList
   ctx.control("editValue").setText(_cls_info.get(ctx.field("objID").value,ctx.field("layerID").value,_objectsList.selectedItem().text(0)))   
   return
   
def closeWinEditValueCancel():
   global _globalReturnValue
   _globalReturnValue = ""
   ctx.window().reject();
   return
   
   
def closeWinEditValueYes():
   global _globalReturnValue
   _globalReturnValue = ctx.control("editValue").text()
   ctx.window().accept();
   return
   



def addNewEntry(fieldP=0):
   global _globalReturnValue
   if (ctx.showModalDialog("winNewEntry")==1):
      #print _globalReturnValue + " -> " + _globalReturnValue2
      _cls_info.set(ctx.field("objID").value,ctx.field("layerID").value,_globalReturnValue,_globalReturnValue2)
      ctx.field("update").touch()
   #else:
      #print "Cancel"   
   return

def openWinNewEntry():
   #global _objectsList
   #ctx.control("newValue").setText(_cls_info.get(ctx.field("objID").value,ctx.field("layerID").value,_objectsList.selectedItem().text(0)))   
   return
   
def closeWinNewEntryCancel():
   global _globalReturnValue
   global _globalReturnValue2
   _globalReturnValue = ""
   _globalReturnValue2 = ""
   ctx.window().reject();
   return
   
   
def closeWinNewEntryYes():
   global _globalReturnValue
   global _globalReturnValue2
   _globalReturnValue = ctx.control("newEntry").text()
   _globalReturnValue2 = ctx.control("comboNewEntryValue").currentText()
   ctx.window().accept();
   return
   
   
def getUsedValues(fieldP=0):
   global _cls_info
   global _comboNewEntryString
   _comboNewEntryString = ""
   infoId = ctx.control("newEntry").text()
   _cls_info.activateLayer(ctx.field("layerID").value)
   
   currentId = _cls_info.firstObject()
   while _cls_info.success():
      if (_cls_info.existInfo(infoId)):
         if (_comboNewEntryString.find(_cls_info.get(infoId)+",")==-1):
            _comboNewEntryString = _comboNewEntryString + _cls_info.get(infoId) + ","
            #print _comboNewEntryString
            
      _cls_info.nextObject()
   ctx.field("comboNewEntryEnum").setStringValue(_comboNewEntryString)
   return