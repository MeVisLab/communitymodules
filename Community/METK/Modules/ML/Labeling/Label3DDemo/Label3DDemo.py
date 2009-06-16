from mevis import *
from os import *
from os.path import *
from urlparse import *
from string import *
from ObjMgr import *
from VisDefinitions import *

_maxInt = 16777215

_cls_info = None
_cls_iter = None
_mainWin = None

def init():
   global _cls_info, _cls_iter
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)
   ctx.field("METKViewer3D.viewAll").touch()
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   return

def openCase(field = 0):   
   #ctx.field("METKScriptInit.scriptFile").value = ctx.field("baseScriptPath").value + "/" + ctx.field("caseType").value + "/init-script.txt"
   
   ctx.field("METKManager.cleanup").touch()
   caseFile = MLABFileDialog.getOpenFileName("","XML files (*.xml)", "Open Case File");
   if caseFile != "":
     ctx.field("LastXMLFile").value = caseFile
     ctx.field("METKManager.fileName").value = caseFile
     ctx.field("METKManager.load").touch()
     _mainWin.setTitle("Label3DDemo | " + caseFile)
   ctx.field("METKViewer3D.viewAll").touch();
   return


# frag mich mal keiner was dieser mist hier mit der url soll ... 
# aber das war die einzige möglichkeit, aus einem absoluten pfad (c:/aaa/bbb/ccc/) und einer relativen Angabe wie "../" etwas zu zaubern   
def relPath(orgpath, relative):
   (scheme, netloc, path, query, fragment) = urlsplit(urljoin("http://netloc/"+orgpath, relative),"http://",0)   
   return lstrip(path,"/")

def refresh(field = 0):
  ctx.field("METKViewer3D.viewAll").touch()
  return

def loadUsedXMLFile(field = 0):
  ctx.field("METKManager.fileName").value=ctx.field("LastXMLFile").value
  ctx.field("METKManager.load").touch()
  return


def initWindow(args=0):
   global _mainWin
   _mainWin = ctx.window() 
   _mainWin.setTitle("Label3DDemo")
   return
   

def startAnimation(args=0):
   ctx.field("UMDAnimation2.ScriptAreaExecute").touch()
   return


def setLabelsNames():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      nameStr = _cls_info.get(_cls_iter.object(), LAY_DESCRIPTION, INF_NAME)
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELTEXT, nameStr, INFOTYPE_STRING)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass
   
   
def setLabelsStructureGroup():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      nameStr = _cls_info.get(_cls_iter.object(), LAY_DESCRIPTION, INF_STRUCTUREGROUP)
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELTEXT, nameStr, INFOTYPE_STRING)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass
   
   
def setLabelsMeasures():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():      
      nameStr = _cls_info.get(_cls_iter.object(), LAY_DESCRIPTION, INF_NAME)
      
      maxDiameter = _cls_info.get(_cls_iter.object(), LAY_MEASURES, INF_MAX_DIAMETER)
      if (maxDiameter!=""):
         nameStr = nameStr+"\nmax Diameter: %.2f mm" % (float(maxDiameter))
         
      volume = _cls_info.get(_cls_iter.object(), LAY_MEASURES, INF_VOLUME)
      if (volume!=""):
         volumeUnit = _cls_info.get(_cls_iter.object(), LAY_MEASURES, INF_VOLUMEUNIT)
         nameStr = nameStr+"\nVolume: %.2f %s" % (float(volume),volumeUnit)
         
      
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELTEXT, nameStr, INFOTYPE_STRING)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass   
   

def setLabelsLiver():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():      
      nameStr = _cls_info.get(_cls_iter.object(), LAY_DESCRIPTION, INF_NAME)
      
      volume = _cls_info.get(_cls_iter.object(), LAY_MEASURES, INF_VOLUME)
      if (volume!=""):         
         nameStr = nameStr+"\nVolume: "+volume
         
      
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELTEXT, nameStr, INFOTYPE_STRING)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass


def setLymphLarge():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      
      strGrp = _cls_info.get(_cls_iter.object(), LAY_DESCRIPTION, INF_STRUCTUREGROUP)
      if (strGrp=="Lymphnode"):      
         maxDiaStr = _cls_info.get(_cls_iter.object(), LAY_MEASURES, INF_MAX_DIAMETER)
         if (maxDiaStr!=""):
            maxDiameter = (float)(maxDiaStr)
            if (maxDiameter>ctx.field("lymphSize").value):
               _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_DRAWLABEL, True, INFOTYPE_BOOL)
            else:
               _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_DRAWLABEL, False, INFOTYPE_BOOL)
      _cls_iter.searchNext()
   _cls_info.notify()  
   pass 

def setAutoBG():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      color = _cls_info.get(_cls_iter.object(), LAY_APPEARANCE, INF_COLOR)
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELBG, color, INFOTYPE_VEC3)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass

def setDefaultBG():
   _cls_iter.setSearchCriteria(LAY_IMAGE, INF_IMAGETYPE, "Segmentation")
   _cls_iter.searchFirst()
   while _cls_iter.success():
      _cls_info.typedSet(_cls_iter.object(), LAY_LABEL, INF_LABELBG, ctx.field("METKLabels.defaultLabelBgColor").stringValue(), INFOTYPE_VEC3)
      _cls_iter.searchNext()
   _cls_info.notify()
   pass



# MeVis signature v1
# key: MFowDQYJKoZIhvcNAQEBBQADSQAwRgJBAKyWeOZdMOT9Sj40ZS32A8i8GAjHkcHh3QT05EJBoHkqE3S0/FOcZM6SFkMLVcjiDmzBdoc9REZUIl5061137BkCARE=:mbUARszcrJxTM2e3H2YskVHojl15mYg4OBjlYsSeJfjTYEVwkgvJJ0eFJB4r7/u64QtiMUxLsor5/MgAGO4iCA==
# owner: MeVis Internal Developer
# date: 2008-01-30T03:14:08
# hash: J07FZP7/dcyEDqdn1UaEY59P+BkNJHcXRegtWMUdij9hj9oI9kz9OYDT9Kr/k0yMSYmZj4wbqdz6CaUOgd3vow==
# MeVis end

