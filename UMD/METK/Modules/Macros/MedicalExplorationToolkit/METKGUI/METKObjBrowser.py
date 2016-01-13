# Python module import
from ObjMgr import *
from VisDefinitions import *

# Global class instances
_cls_info= None
_ignoreNextChange = None

def init():
   global _cls_info   
   # Create an ObjInfo class instance to modify the database and to handle database events
   _cls_info= ObjInfo(ctx.module("info"))      
   
   global _ignoreNextChange
   _ignoreNextChange = {'visible':False,'color':False,'transparency':False,'silhouette':False,'silhouetteColor':False,'silhouetteWidth':False,'labelOn':False,'labelText':False,'labelBgColor':False,'flat':False}
   return


def changedColor():
   ctx.log(ctx.control("fieldColor").value)
   return
   
   
def updateObject(fieldP=0):   
   if (ctx.field("objID").value!=""):
      objId = ctx.field("objID").value.split(",")[0]
   else:
      return
      
   global _ignoreNextChange
   _ignoreNextChange = {'visible':True,'color':True,'transparency':True,'silhouette':True,'silhouetteColor':True,'silhouetteWidth':True,'labelOn':True,'labelText':True,'labelBgColor':True,'flat':True}
  
   
   #Visibility
   if _cls_info.existInfo(objId, LAY_APPEARANCE, INF_VISIBLE):
      visible = _cls_info.get(objId, LAY_APPEARANCE, INF_VISIBLE)
   else:
      visible = False
   ctx.field("visible").setStringValue(visible)
   ctx.field("visibleGUI").setStringValue(visible)
   
   #Color
   color = _cls_info.get(objId, LAY_APPEARANCE, INF_COLOR)     
   ctx.field("color").setStringValue(color)
   ctx.field("colorGUI").setStringValue(color)
   
   #Transparency
   transp = _cls_info.get(objId, LAY_APPEARANCE, INF_TRANSPARENCY)  
   ctx.field("transparency").setStringValue(transp)
   ctx.field("transparencyGUI").setStringValue(transp)
   
   #Silhouette
   if _cls_info.existInfo(objId, LAY_APPEARANCE, INF_SILHOUETTE):
      silh = _cls_info.get(objId, LAY_APPEARANCE, INF_SILHOUETTE)
   else:
      silh = False   
   ctx.field("silhouette").setStringValue(silh)
   ctx.field("silhouetteGUI").setStringValue(silh)

   #SilhouetteColor
   silhC = _cls_info.get(objId, LAY_APPEARANCE, INF_SILHOUETTECOLOR)
   ctx.field("silhouetteColor").setStringValue(silhC)
   ctx.field("silhouetteColorGUI").setStringValue(silhC)
   
   
   #SilhouetteWidth
   silhW = _cls_info.get(objId, LAY_APPEARANCE, INF_SILHOUETTEWIDTH)
   ctx.field("silhouetteWidth").setStringValue(silhW)
   ctx.field("silhouetteWidthGUI").setStringValue(silhW)
   

   #FlatShading
   if _cls_info.existInfo(objId, LAY_APPEARANCE, INF_FLATISO):
      flat = _cls_info.get(objId, LAY_APPEARANCE, INF_FLATISO)
   else:
      flat = False
   ctx.field("flat").setStringValue(flat)
   ctx.field("flatGUI").setStringValue(flat)

   
   
   #LabelOn
   labelOn = _cls_info.get(objId, LAY_LABEL, INF_DRAWLABEL)     
   ctx.field("labelOn").setStringValue(labelOn)
   ctx.field("labelOnGUI").setStringValue(labelOn)
   
   #LabelText
   labelText = _cls_info.get(objId, LAY_LABEL, INF_LABELTEXT)
   ctx.field("labelText").setStringValue(labelText)
   ctx.field("labelTextGUI").setStringValue(labelText)
   
   labelBgColor = _cls_info.get(objId, LAY_LABEL, INF_LABELBG)
   ctx.field("labelBgColor").setStringValue(labelBgColor)
   ctx.field("labelBgColorGUI").setStringValue(labelBgColor)
   
   
   objName = _cls_info.get(objId, LAY_DESCRIPTION, INF_NAME)
   ctx.field("objName").setStringValue(objName)
   ivPointer = _cls_info.get(objId, LAY_FILES, INF_IVPOINTER)
   ctx.field("ivPointer").setStringValue(ivPointer)
   fullPointer = _cls_info.get(objId, LAY_FILES, INF_IVFULLPOINTER)
   ctx.field("fullPointer").setStringValue(fullPointer)
   
   ctx.field("UMDPointerToInventor.objectID").setStringValue(objId)
   ctx.field("UMDPointerToInventor.layerID").setStringValue(LAY_FILES)
   ctx.field("UMDPointerToInventor.infoID").setStringValue(INF_IVFULLPOINTER)
   
   ctx.field("maxDiameter").setStringValue(_cls_info.get(objId, LAY_MEASURES, INF_MAX_DIAMETER))
   return
   
   
   
def applyVisible(fieldP=0):
   ctx.field("visible").value = ctx.field("visibleGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['visible']==True):
      _ignoreNextChange['visible']=False
      return
   
   if (ctx.field("objID").value!=""):
      applyVisible2(ctx.field("objID").value)
   return
   
   
   
def applyColor(fieldP=0):   
   ctx.field("color").value = ctx.field("colorGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['color']==True):
      _ignoreNextChange['color']=False
      return
      
   if (ctx.field("objID").value!=""):
      applyColor2(ctx.field("objID").value)
   return
   
   
   
def applyTransp(fieldP=0):
   ctx.field("transparency").value = ctx.field("transparencyGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['transparency']==True):
      _ignoreNextChange['transparency']=False
      return

   if (ctx.field("objID").value!=""):
      applyTransp2(ctx.field("objID").value)
   return
      
      
      
def applySilhouette(fieldP=0):
   ctx.field("silhouette").value = ctx.field("silhouetteGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['silhouette']==True):
      _ignoreNextChange['silhouette']=False
      return
   
   if (ctx.field("objID").value!=""):
      applySilhouette2(ctx.field("objID").value)
   return   


def applySilhouetteColor(fieldP=0):
   ctx.field("silhouetteColor").value = ctx.field("silhouetteColorGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['silhouetteColor']==True):
      _ignoreNextChange['silhouetteColor']=False
      return

   if (ctx.field("objID").value!=""):
      applySilhouetteColor2(ctx.field("objID").value)
   return
   

def applySilhouetteWidth(fieldP=0):
   ctx.field("silhouetteWidth").value = ctx.field("silhouetteWidthGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['silhouetteWidth']==True):
      _ignoreNextChange['silhouetteWidth']=False
      return

   if (ctx.field("objID").value!=""):
      applySilhouetteWidth2(ctx.field("objID").value)
   return  
   
   
def applyFlat(fieldP=0):
   ctx.field("flat").value = ctx.field("flatGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['flat']==True):
      _ignoreNextChange['flat']=False
      return
   
   if (ctx.field("objID").value!=""):
      applyFlat2(ctx.field("objID").value)
   return   


def applyLabelOn(fieldP=0):
   ctx.field("labelOn").value = ctx.field("labelOnGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['labelOn']==True):
      _ignoreNextChange['labelOn']=False
      return

   if (ctx.field("objID").value!=""):
      applyLabelOn2(ctx.field("objID").value)
   return   


def applyLabelText(fieldP=0):
   ctx.field("labelText").value = ctx.field("labelTextGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['labelText']==True):
      _ignoreNextChange['labelText']=False
      return

   if (ctx.field("objID").value!=""):
      applyLabelText2(ctx.field("objID").value)
   return   


def applyLabelBgColor(fieldP=0):
   ctx.field("labelBgColor").value = ctx.field("labelBgColorGUI").value

   global _ignoreNextChange
   if (_ignoreNextChange['labelBgColor']==True):
      _ignoreNextChange['labelBgColor']=False
      return

   if (ctx.field("objID").value!=""):
      applyLabelBgColor2(ctx.field("objID").value)
   return   


   
def applyVisible2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyVisible2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_VISIBLE, ctx.field("visible").stringValue(), 'omAttribute_bool')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_VISIBLE)
      _cls_info.notify()
   return



def applyColor2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyColor2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_COLOR, ctx.field("color").stringValue(), 'omAttribute_vec3')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_COLOR)
      _cls_info.notify()
   return



def applyTransp2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyTransp2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_TRANSPARENCY, ctx.field("transparency").stringValue(), 'omAttribute_double')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_TRANSPARENCY)
      _cls_info.notify()
   return



def applySilhouette2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applySilhouette2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_SILHOUETTE, ctx.field("silhouette").stringValue(), 'omAttribute_bool')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_SILHOUETTE)
      _cls_info.notify()
   return



def applySilhouetteColor2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applySilhouetteColor2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_SILHOUETTECOLOR, ctx.field("silhouetteColor").stringValue(), 'omAttribute_vec3')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_SILHOUETTECOLOR)
      _cls_info.notify()
   return




def applySilhouetteWidth2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applySilhouetteWidth2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH, ctx.field("silhouetteWidth").stringValue(), 'omAttribute_double')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_SILHOUETTEWIDTH)
      _cls_info.notify()
   return


def applyFlat2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyFlat2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_APPEARANCE, INF_FLATISO, ctx.field("flat").stringValue(), 'omAttribute_bool')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_APPEARANCE, INF_FLATISO)
      _cls_info.notify()
   return
   
   

def applyLabelOn2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyLabelOn2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_LABEL, INF_DRAWLABEL, ctx.field("labelOn").stringValue(), 'omAttribute_bool')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_LABEL, INF_DRAWLABEL)
      _cls_info.notify()
   return
 
 
def applyLabelText2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyLabelText2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_LABEL, INF_LABELTEXT, ctx.field("labelText").stringValue(), 'omAttribute_std_string')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_LABEL, INF_LABELTEXT)
      _cls_info.notify()
   return 


def applyLabelBgColor2(objID):
   if (objID==""): return
   objects = objID.split(",")
   if len(objects)>1:
      for objID in objects:
         applyLabelBgColor2(objID)
   else:
      objID = objects[0]
      _cls_info.typedSet(objID, LAY_LABEL, INF_LABELBG, ctx.field("labelBgColor").stringValue(), 'omAttribute_vec3')
      if (ctx.field("markNonPersistent").boolValue()):
         _cls_info.markInfoNonPersistent(objID, LAY_LABEL, INF_LABELBG)
      _cls_info.notify()
   return    