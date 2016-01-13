# Python module import
from VisDefinitions import *
from KeystateDefinitions import *

# Global class instances
_cls_info = None
_cls_iter = None
_listView = None
_listViewHorz = None

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
   global _countColl
   global _waitForExchange
   
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))   
   _cls_info.setEventReceiveMode(ERM_ALL)
   _cls_iter = ObjIterator(ctx.module("iterator"))
   _cls_iter.setSearchCriteria(LAY_GLOBAL, INF_ISKEYSTATE, "TRUE")

   return


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
   global _countViewer, _waitForExchange
   object   = _cls_info.activeObject()
   layer   = _cls_info.activeLayer()
   info   = _cls_info.activeInfo() 
   value  = _cls_info.get()

   if info == INF_ISKEYSTATE:
      if _cls_info.existInfo(OBJ_GLOBAL,LAY_KEYSTATES,INF_IMAGEPATH):
         updateListView()
   pass




def initListView():
   global _listView
   _listView = ctx.control("keystateList")
   updateListView()
   return

def initListViewHorz():
   global _listViewHorz
   _listViewHorz = ctx.control("keystateListHorz")
   updateListViewHorz()
   return


def closeListView():
   global _listView
   _listView = None
   return


def closeListViewHorz():
   global _listViewHorz
   _listViewHorz = None
   return


def updateListView(args=0):
   updateListViewHorz() #update also horizontal list
   if _listView:
      _listView.clearItems()
   else:
      return
      
   imagePath = _cls_info.get(OBJ_GLOBAL,LAY_KEYSTATES,INF_IMAGEPATH)+"/"
   
   _cls_iter.searchFirst()
   while _cls_iter.success():
      print _cls_iter.object()
      item = _listView.insertItem()
      item.setPixmapFile(0, imagePath + _cls_info.get(_cls_iter.object(), LAY_INFORMATION, INF_SCREENSHOT))
      item.setText(1, _cls_iter.object())
      _cls_iter.searchNext()
   return
   
   
def updateListViewHorz(args=0):
   print "updateListViewHorz"
   if _listViewHorz:
      _listViewHorz.clearItems()
   else:
      print "ERROR"
      return
      
   #itemNames = _listViewHorz.insertItem()   
   #itemImages = _listViewHorz.insertItem()
   
      
   imagePath = _cls_info.get(OBJ_GLOBAL,LAY_KEYSTATES,INF_IMAGEPATH)+"/"
   
   #count = 0
   _cls_iter.searchFirst()
   while _cls_iter.success():
      #print _cls_iter.object()      
      #_listViewHorz.addColumn("XX")
      #itemImages.setPixmapFile(count, imagePath + _cls_info.get(_cls_iter.object(), LAY_INFORMATION, INF_SCREENSHOT))
      #itemNames.setText(count, _cls_iter.object())
      _listViewHorz.insertItem(imagePath + _cls_info.get(_cls_iter.object(), LAY_INFORMATION, INF_SCREENSHOT),_cls_iter.object())
      
      #count=count+1
      #print count
      _cls_iter.searchNext()
   return


def keystateSelected(index):
   #print "keystateSelected"
   if not _listViewHorz:
      return
   keystate = _listViewHorz.itemText(index)
   #print keystate
   ctx.field("currentKeystate").value = keystate
   return
