from os import *
from string import *
from ObjMgr import *
from VisDefinitions import *

_cls_info = None
_cls_iter = None

def init():
   global _cls_info, _cls_iter
     
   _cls_info = METKObjInfo(ctx.module("ObjInfo"))
   _cls_info.setEventReceiveMode(ERM_NONE)   
   
   _cls_iter = ObjIterator(ctx.module("ObjConstrainedIterator"))
   
   return


def handleLoadedEvent():
   pass
   
def handleSaveEvent():
   pass

def handleCleanupEvent():
   pass



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