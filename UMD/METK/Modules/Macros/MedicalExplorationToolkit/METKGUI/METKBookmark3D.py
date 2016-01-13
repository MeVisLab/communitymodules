# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info= None
_bmList = None


def init():
   global _cls_info      
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))      
   return


   
   
def openWindow():
   global _bmList
   _bmList = ctx.control("bookmarklist")
   return
   
def closeWindow():
   global _bmlList   
   _bmList = None
   return


def saveBookmark(fieldP=0):
   _cls_info.typedSet(ctx.field("viewerName").value,LAY_GLOBAL,"Refresh","1","omAttribute_std_string")
   _cls_info.notify()
   
   countStr = _cls_info.get(OBJ_BOOKMARKS,LAY_GLOBAL,INF_COUNT3D)
   if (countStr==""):
      count = 1
   else:
      count = int(countStr)+1
      
   _cls_info.typedSet( OBJ_BOOKMARKS, "Bookmark"+str(count), INF_CAMPOSITION, _cls_info.get(ctx.field("viewerName").value, "Camera", "Position"),'omAttribute_vec3')
   _cls_info.typedSet( OBJ_BOOKMARKS, "Bookmark"+str(count), INF_CAMORIENTATION, _cls_info.get(ctx.field("viewerName").value, "Camera", "Orientation"),'omAttribute_vec4')
   _cls_info.typedSet( OBJ_BOOKMARKS, "Bookmark"+str(count), INF_CAMHEIGHT, _cls_info.get(ctx.field("viewerName").value, "Camera", "Height"),'omAttribute_double')
   _cls_info.typedSet( OBJ_BOOKMARKS, "Bookmark"+str(count), INF_CAMFOCALDISTANCE, _cls_info.get(ctx.field("viewerName").value, "Camera", "FocalDistance"),'omAttribute_double')
   _cls_info.typedSet( OBJ_BOOKMARKS, LAY_GLOBAL, INF_COUNT3D, str(count),'omAttribute_std_string')
   _cls_info.notify()
   
   updateListView(0)
   
   return
   
      
def updateListView(fieldP=0):
   global _selectedItem
   global _bmList
   if _bmList:
      _bmList.clearItems()

      _cls_info.activateObject(OBJ_BOOKMARKS)
      layerID = _cls_info.firstLayer()
      #print layerID
      while _cls_info.success():      
         if (layerID.find("Bookmark")!=-1):
            #print "insert "+layerID
            item = _bmList.insertItem()
            item.setText(0, layerID)
         layerID = _cls_info.nextLayer()         
   return
   
   
def selectBookmark(item):
   #print item.text(0)
   gotoBookmark(item.text(0))
   return


def gotoBookmark(name):
   scriptFile = ""
   scriptFile = scriptFile + "[Ini]\nLengthTimeUnit=2\nLengthSeconds=2\nRealTime=yes\n\n[Script]\n"
   scriptFile = scriptFile + "[0,2] 'Cam' moveCoord "+_cls_info.get(OBJ_BOOKMARKS,name,INF_CAMPOSITION).replace(" ",",")+" "+_cls_info.get(OBJ_BOOKMARKS,name,INF_CAMORIENTATION).replace(" ",",")+" "+_cls_info.get(OBJ_BOOKMARKS,name,INF_CAMHEIGHT).replace(" ",",")+" "+_cls_info.get(OBJ_BOOKMARKS,name,INF_CAMFOCALDISTANCE).replace(" ",",")
   #print scriptFile
   
   _cls_info.typedSet(OBJ_ANIMATION, LAY_ANIMATION_SCRIPT, INF_SCRIPT_FULLSCRIPT,scriptFile,INFOTYPE_MESSAGE)
   _cls_info.notify()
   return