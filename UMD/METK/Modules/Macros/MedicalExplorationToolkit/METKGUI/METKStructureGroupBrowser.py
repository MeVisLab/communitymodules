# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info = None
_cls_iter = None
_objectsList = None
_selectedItem = None
_recursion = 0
_dictNameOID = dict()
_updateInPrep = False

def init():
   global _cls_info   
   global _cls_iter
   global _objectsList
   global _selectedItem
   _objectsList = None
   _selectedItem = None
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info = ObjInfo(ctx.module("info"))      
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_SELECTEDOBJ + "," + LAY_GLOBALEVENTS + "," + ctx.field("checkBoxLayer").value)
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   return
   

def changedCheckBoxProps(field = 0):   
   _cls_info.setEventReceiveMode(ERM_SELECTED, "*", LAY_SELECTEDOBJ + "," + LAY_GLOBALEVENTS + "," + ctx.field("checkBoxLayer").value)
   updateListView()
   pass


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
   updateListView()
   return


def handleEvent(fieldP=0):
   global _updateInPrep   
   if (not _updateInPrep):
      _updateInPrep=True
      ctx.callLater(0.1,"updateListView")
   return

def updateListView(fieldP=0):   
   global _updateInPrep
   _updateInPrep=False
   global _selectedItem
   global _recursion
   global _dictNameOID
   _recursion = _recursion + 1   
   _dictNameOID = dict()
   hasCheckBox = ctx.field("showCeckBox").value
   disableGrouping = ctx.field("disableGrouping").value
   if _objectsList:      
      # aktuelles Item ermitteln
      _currentItem = _objectsList.currentItem()
      curid = 0
      if _currentItem:
         curid = _currentItem.id()
         #ctx.log("id:"+str(curid))
                           
      # alle Einträge löschen
      _objectsList.clearItems()
      _objectsList.setRootIsDecorated(True)
      if hasCheckBox:
         # oberstes Item mit Checkbox einfügen
         root = _objectsList.insertCheckBoxItem()
      else:
         root = _objectsList.insertItem()
      root.setText(0, ctx.field("root").value)
      root.setOpen(True)
      
      cso = _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID)
      
      # nur Strukturen einfügen, die Suchkriterium entsprechen
      _cls_iter.setSearchCriteria(LAY_DESCRIPTION, ctx.field("groupByInfo").value)
      _cls_iter.searchFirst()
      while _cls_iter.success():
         
         objectID = _cls_iter.object()
         
         # StructureGroup-Eintrag auslesen
         strGrp  = _cls_info.get(objectID, LAY_DESCRIPTION, ctx.field("groupByInfo").value)
         # Structure-Eintrag auslesen
         structure = _cls_info.get(objectID, LAY_DESCRIPTION, INF_STRUCTURE)
         
         # Dictionary aufbauen füre objectID / Name Korrespondenz
         _dictNameOID[_cls_info.get(objectID, LAY_DESCRIPTION, INF_NAME)] = objectID
         
         if ctx.field("setValuesByCheckedObjIdString").value==False:
            if _cls_info.existInfo(objectID, ctx.field("checkBoxLayer").value, ctx.field("checkBoxInfo").value) and _cls_info.getBool(objectID, ctx.field("checkBoxLayer").value, ctx.field("checkBoxInfo").value) == True:
               checked = True
            else:
               checked = False
         else:
            if ctx.field("checkedObjIds").value.find(","+objectID+",")!=-1:
               checked = True
            else:
               checked = False
         
         if strGrp:
            if not disableGrouping:
               itemGrp = getGroupItem(strGrp)
               if not itemGrp:
                  if hasCheckBox:
                     itemGrp = _objectsList.insertCheckBoxItem(root, 0)
                  else:
                     itemGrp = _objectsList.insertItem(root, 0)
                  itemGrp.setOn(True)
                  itemGrp.setText(0, strGrp)
                  itemGrp.setText(1, "GROUP")
               if not checked and hasCheckBox:
                  itemGrp.setOn(False)
                  root.setOn(False)
               
            if hasCheckBox:
               if not disableGrouping:
                  item = _objectsList.insertCheckBoxItem(itemGrp, 0)
               else:
                  item = _objectsList.insertCheckBoxItem(root, 0)
               item.setOn(checked)               
            else:
               if not disableGrouping:
                  item = _objectsList.insertItem(itemGrp, 0)
               else:
                  item = _objectsList.insertItem(root, 0)
            item.setText(0, setEntryName(objectID))
            item.setText(1, objectID)
            setCheckedObjIds(item)
            
            if objectID == cso:
               _selectedItem = item
        
         _cls_iter.searchNext()
      
      #ctx.log("setItem:"+str(curid))
      selItem = _objectsList.itemForId(curid)
      if selItem:
         #_objectsList.setCurrentItem(selItem)
         _objectsList.ensureItemVisible(selItem)
   
   _recursion = _recursion - 1   
   return


def getGroupItem(group):
   grpItem = None
   item = _objectsList.firstItem().firstChild()
   while item:
      if item.text(0) == group:
         grpItem = item
      item = item.nextSibling()
   return grpItem


def clickedObjects(item, column):
   selectObject(item)
   return


def itemCollapsed(item):
   selectObject(item)
   return
   
   
def selectObject(item):   
   # nur Strukturen sollen auswählbar sein
   if item.text(1) != "" and item.text(1) != "GROUP":
      if ctx.field("useCSO").value==True:         
         _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, item.text(1))      
         _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_MULTIOBJID, "")
      ctx.field("selectedObjId").setStringValue(item.text(1))
      ctx.field("selectedMultiObjIds").setStringValue(item.text(1))
   else:
      multiObj = getLeafs(item,0)
      multiObj = multiObj.rstrip(",")
      multiObj = multiObj.lstrip(",")
      
      if ctx.field("useCSO").value==True:
         _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_MULTIOBJID, multiObj)            
         _cls_info.set(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID, "")
      ctx.field("selectedMultiObjIds").setStringValue(multiObj)
      ctx.field("selectedObjId").setStringValue("")

   _cls_info.notify()   
   return


def checkListItemChanged(item):   
   if item.text(0) != "" and item.childCount() == 0 and item.text(1) != "GROUP":
      if ctx.field("writeToObjMgr").value==True:
         _cls_info.typedSet(getEntryName(item.text(0)), ctx.field("checkBoxLayer").value, ctx.field("checkBoxInfo").value, item.isOn(), INFOTYPE_BOOL)
      setCheckedObjIds(item)
   if _recursion == 0:
      setParameter(item.firstChild(), item.isOn())
      _cls_info.notify()
   return
   

def getLeafs(item,recursion):   
   leafs = ""
   recursion = recursion + 1
   if item:
      if (item.childCount()>0):
         leafs = leafs + getLeafs(item.firstChild(),recursion)
      else:
         leafs = leafs + item.text(1) + ","
      if recursion>1:
         leafs = leafs + getLeafs(item.nextSibling(),recursion)
            
   return leafs
   
def setParameter(child, status):   
   global _recursion
   _recursion = _recursion + 1
   if child:
      child.setOn(status)
      if child.text(0) != "" and child.childCount() == 0 and child.text(1) != "GROUP":
         if ctx.field("writeToObjMgr").value==True:
            _cls_info.typedSet(getEntryName(child.text(0)), ctx.field("checkBoxLayer").value, ctx.field("checkBoxInfo").value, status, INFOTYPE_BOOL)
         setCheckedObjIds(child)         
      else:
         setParameter(child.firstChild(), status)
      setParameter(child.nextSibling(), status)
   _recursion = _recursion - 1   
   return


def getEntryName(entry):   
   if ctx.field("valueEntry").value == "objectID":
      return entry
   else:
      if _dictNameOID.has_key(entry):
         return _dictNameOID[entry]
      else:
         return entry
   pass 


def setEntryName(objectID):   
   if ctx.field("valueEntry").value == "objectID":
      return objectID
   else:
      return _cls_info.get(objectID, LAY_DESCRIPTION, INF_NAME)



def setCheckedObjIds(item):      
   if (item.isOn()):
      if (ctx.field("checkedObjIds").value.find(","+item.text(0)+",")==-1):
         ctx.field("checkedObjIds").value=ctx.field("checkedObjIds").value+item.text(0)+","
   else:
      ctx.field("checkedObjIds").value=ctx.field("checkedObjIds").value.replace(item.text(0)+",","")
   ctx.field("checkedObjIds").value.replace(",,",",")      
   return