# Python module import
from ObjMgr import *
from VisDefinitions import *
from linear import *


# Global class instances
_cls_info= None


def init():
   global _cls_info
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= METKObjInfo(ctx.module("info"))   
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO + "," + OBJ_COMMUNICATION, LAY_SELECTEDOBJ+","+LAY_MOUSEOVEROBJ)
   
   global _oldID, _oldTransp, _oldColor, _oldSilhouetteWidth, _oldSilhouetteColor, _oldSilhouetteOn
   _oldID = ""
   _oldTransp = -1
   _oldColor = "1,0,0"   
   _oldSilhouetteWidth = 0
   _oldSilhouetteColor = "0,0,0"
   _oldSilhouetteOn = False
   
   return


#--------------------------------------------
# Event handling
#--------------------------------------------

def handleLoadedEvent():
   _cls_info.typedSet(OBJ_CSO, LAY_GLOBAL, INF_IGNOREPICK, ctx.field("ignorePickable").value, INFOTYPE_BOOL)
   _cls_info.notify()
   pass

def handleSaveEvent():
   pass

def handleCleanupEvent():
   pass
   
def handleObjectCreatedEvent():
   _cls_info.typedSet(OBJ_CSO, LAY_GLOBAL, INF_IGNOREPICK, True, INFOTYPE_BOOL)
   _cls_info.setAndNotify(OBJ_CSO, LAY_GLOBAL, INF_OBJTYPE, "CSO")
   return

def handleObjectRemovedEvent():
   _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO, LAY_SELECTEDOBJ+","+LAY_MOUSEOVEROBJ)
   deselect()
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
   
   # we have a new currently selected object
   if object == OBJ_CSO and layer == LAY_SELECTEDOBJ and (info == INF_SELECTION or info == INF_OBJID):      
      if _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID) != "":
         if (info==INF_SELECTION):
            select(value)
         else:
            select("")
      else:
         _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO, LAY_SELECTEDOBJ+","+LAY_MOUSEOVEROBJ)
         deselect()

   # the CSO itself has been changed
   elif info == INF_VISIBLE:
      if _cls_info.getBool() == False:
         deselect()
      else:
         select("") 
   # we have a new currently mouse over object
   elif (object == OBJ_CSO and layer == LAY_MOUSEOVEROBJ and info == INF_OBJID):
      setMouseOver(value)
         
   return


def select(value):
   cso = _cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_OBJID)
   if _cls_info.getBool(cso, LAY_APPEARANCE, INF_VISIBLE) == True or ctx.field("objectMustBeVisible").value==False:
      if not ctx.field("filterDoubleEvents").value or ctx.field("cso").value != cso:
         ctx.field("cso").value = cso
      ctx.field("selection").setStringValue(_cls_info.get(OBJ_CSO, LAY_SELECTEDOBJ, INF_SELECTION))
      ctx.field("UMDPointerToInventor.objectID").value = cso
      ctx.field("FullPointerToInventor.objectID").value = cso
      if value != "":
         ctx.field("roi").value = _cls_info.get(cso, LAY_GLOBAL, INF_PARENT)
      else:
         ctx.field("roi").value = ""
      _cls_info.setEventReceiveMode(ERM_SELECTED, OBJ_CSO + "," + cso, LAY_SELECTEDOBJ + "," + LAY_APPEARANCE + "," + LAY_MOUSEOVEROBJ)
   else:
      deselect() #Wenn eine Struktur als CSO gesetzt wurde, die nicht sichtbar ist wird keine als gewählt markiert
   pass
   
def deselect():
   if not ctx.field("filterDoubleEvents").value or ctx.field("cso").value != "":
      ctx.field("cso").value = ""
   ctx.field("roi").value = ""
   ctx.field("selection").value = "0 0 0"
   ctx.field("UMDPointerToInventor.objectID").value = ""
   ctx.field("FullPointerToInventor.objectID").value = ""
   pass
   
   
def ignorePickable(field = 0):
   _cls_info.typedSet(OBJ_CSO, LAY_GLOBAL, INF_IGNOREPICK, field.value, INFOTYPE_BOOL)
   _cls_info.notify()
   
   
def setMouseOver(value):
   global _oldID, _oldTransp, _oldColor, _oldSilhouetteWidth, _oldSilhouetteColor, _oldSilhouetteOn
   #objectID = _cls_info.get(OBJ_CSO, LAY_MOUSEOVEROBJ, INF_OBJID)
   objectID = value
   
   ctx.field("moObject").value = objectID
   
   if (_oldID == objectID): return
   
   if (_oldID!=""):
      if (ctx.field("useTransparency").value):
         _cls_info.typedSet(_oldID, LAY_APPEARANCE, INF_TRANSPARENCY, _oldTransp, INFOTYPE_DOUBLE)
      if (ctx.field("useColor").value):
         _cls_info.typedSet(_oldID, LAY_APPEARANCE, INF_COLOR, _oldColor, INFOTYPE_VEC3)
      if (ctx.field("useSilhouette").value):
         _cls_info.typedSet(_oldID, LAY_APPEARANCE, INF_SILHOUETTECOLOR, _oldSilhouetteColor, INFOTYPE_VEC3)
         _cls_info.typedSet(_oldID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, _oldSilhouetteWidth, INFOTYPE_DOUBLE)
         _cls_info.typedSet(_oldID, LAY_APPEARANCE, INF_SILHOUETTE, _oldSilhouetteOn, INFOTYPE_BOOL)
   
   _oldID = objectID
   if (_oldID!=""):
      if (ctx.field("useTransparency").value):
         _oldTransp = _cls_info.get(objectID, LAY_APPEARANCE, INF_TRANSPARENCY)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_TRANSPARENCY, ctx.field("transparency").value, INFOTYPE_DOUBLE)
      if (ctx.field("useColor").value):
         _oldColor = _cls_info.get(objectID, LAY_APPEARANCE, INF_COLOR)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_COLOR, ctx.field("color").stringValue(), INFOTYPE_VEC3)
      if (ctx.field("useSilhouette").value):
         _oldSilhouetteOn = _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTE)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_SILHOUETTE, ctx.field("silhouetteOnOff").value, INFOTYPE_BOOL)
         _oldSilhouetteWidth = _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, ctx.field("silhouetteWidth").value, INFOTYPE_DOUBLE)
         _oldSilhouetteColor = _cls_info.get(objectID, LAY_APPEARANCE, INF_SILHOUETTECOLOR)
         _cls_info.typedSet(objectID, LAY_APPEARANCE, INF_SILHOUETTECOLOR, ctx.field("silhouetteColor").stringValue(), INFOTYPE_VEC3)
      
   _cls_info.notify()
   
   pass   