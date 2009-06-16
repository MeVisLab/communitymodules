# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None
_objectsList = None
_selectedItem = None

def init():
   global _cls_info   
   global _objectsList
   global _selectedItem
   _objectsList = None
   _selectedItem = None
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = ObjInfo(ctx.module("info"))      
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO + "," + OBJ_COMMUNICATION, LAY_SELECTEDOBJ + "," + LAY_GLOBALEVENTS)
   return


def closeWindow():
   global _objectsList
   global _selectedItem
   _objectsList = None
   _selectedItem = None
   return


def openWindow():
   global _objectsList
   global _selectedItem
   _objectsList = ctx.control("objects")
   _selectedItem = None
   updateListView(0)
   return

def updateListView(fieldP=0):
   global _selectedItem
   global _objectsList   
   if _objectsList:
      
      # aktuelles Item ermitteln
      _currentItem = _objectsList.currentItem()
      curid = 0
      if _currentItem:
         curid = _currentItem.id()

   
      _objectsList.clearItems()
      root = _objectsList.insertItem()
      root.setText(0, ctx.field("root").value)
      
      cso = _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID)
      filter = ctx.field("rootObjectTypeFilter").value
      
      objectID = _cls_info.firstObject()      
      while _cls_info.success():
      
         parentExist = False         
         if _cls_info.get(objectID, LAY_GLOBAL, INF_PARENT):
            parentExist = _cls_info.existObject(_cls_info.get(objectID, LAY_GLOBAL, INF_PARENT))
      
         # nur Objekte einfügen, die selber keine Eltern haben oder deren parent nicht existiert         
         if not _cls_info.get(objectID, LAY_GLOBAL, INF_PARENT) or not parentExist:
            # nur Objekte einfügen, die in rootObjectTypeFilter aufgeführt sind                                    
            if (_cls_info.get(objectID, LAY_GLOBAL, INF_OBJTYPE) and filter.find(_cls_info.get(objectID, LAY_GLOBAL, INF_OBJTYPE)) != -1) or (ctx.field("allObjects").value==True):
               item = _objectsList.insertItem(root, 0)
               item.setText(0, objectID)               
               if ((objectID == cso) and (ctx.field("CSOon").value == True)):
                  _selectedItem = item
               
               # browse recursively through all children               
               addChildren(objectID, _objectsList, item)
         
         objectID = _cls_info.nextObject()         
      
      root.setOpen(True)                  
      if (_selectedItem and ctx.field("CSOon").value == True):         
         _objectsList.ensureItemVisible(_selectedItem)         
         _selectedItem.setSelected(True)
         
      selItem = _objectsList.itemForId(curid)
      if selItem:      
         _objectsList.ensureItemVisible(selItem)	         
         
      return root
   return


def addChildren(objectID, list, item):
   global _selectedItem
   cso = _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID)
   childIDs = _cls_info.get(objectID, LAY_GLOBAL, INF_CHILDS)      
   if childIDs:
      while len(childIDs) > 1:
         nextSep = childIDs.find(";")
         if nextSep!=-1:
            child = childIDs[:nextSep]
            nextSep += 1            
            childIDs = childIDs[nextSep:]            
         else:
            child = childIDs
            childIDs = ""
         if (child!=""):   
            childItem = list.insertItem(item, 0)
            childItem.setText(0, child)
            if child == cso:
               _selectedItem = childItem         
            addChildren(child, list, childItem)
            
   # objectID zurücksetzen
   _cls_info.activateObject(objectID)
   return


def clickedObjects(item, column):
   selectObject(item)
   return


def itemCollapsed(item):
   selectObject(item)
   return
   
   
def selectObject(item):
   objectID = item.text(0)
   if (ctx.field("CSOon").value == True):
      # nur Results sollen auswählbar sein
      if _cls_info.get(objectID, LAY_GLOBAL, INF_OBJTYPE) == "Result" and not _cls_info.get(objectID, LAY_GLOBAL, INF_CHILDS):
         _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, objectID)
         _cls_info.notify()
   # aber für jede selktiertes etwas wird auch die ID über ein Feld nach draußen gegeben
   ctx.field("selectedObjId").setStringValue(objectID)
   
   #selectedMultiObjIds
   multiIDs = getLeafs(item,0)
   multiIDs = multiIDs.rstrip(",")
   multiIDs = multiIDs.lstrip(",")
   ctx.field("selectedMultiObjIds").value=multiIDs
   return



def getLeafs(item,recursion):   
   leafs = ""
   recursion = recursion + 1
   if item:
      if (item.childCount()>0):
         leafs = leafs + getLeafs(item.firstChild(),recursion)
      else:
         leafs = leafs + item.text(0) + ","
      if recursion>1:
         leafs = leafs + getLeafs(item.nextSibling(),recursion)
            
   return leafs
   
   

def handleObjectCreatedEvent():
   if _objectsList:
      updateListView(0)
   return


def handleObjectRemovedEvent():
   pass
   

def handleAttributeCreatedEvent():
   handleAttributeModifiedEvent()
   return


def handleAttributeRemovedEvent():
   pass


def handleAttributeModifiedEvent():
   object = _cls_info.activeObject()
   layer  = _cls_info.activeLayer()
   info   = _cls_info.activeInfo()
   value  = _cls_info.get()
   
   if info == INF_OBJID or (info == INF_CASELOADED and value == MSG_LOADED) and _objectsList:
      updateListView(0)
   elif info == INF_CASELOADED and value == MSG_CLEANUP and _objectsList:
      _objectsList.clearItems()
   return
